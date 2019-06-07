#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx8_ccm_div.c $");

#include <sys/param.h>
#include <sys/bus.h>

#include <dev/clk/clk_backend.h>

#include <arm/nxp/imx8_ccm.h>

u_int
imx8_clk_div_get_rate(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk)
{
	struct imx8_clk_div *div = &clk->u.div;
	struct clk *clkp, *clkp_parent;
	u_int rate, ratio;
	uint32_t val;

	KASSERT(clk->type == IMX8_CLK_DIV);

	clkp = &clk->base;
	clkp_parent = clk_get_parent(clkp);
	if (clkp_parent == NULL)
		return 0;

	rate = clk_get_rate(clkp_parent);
	if (rate == 0)
		return 0;

	CLK_LOCK(sc);
	val = CLK_READ(sc, div->reg);
	CLK_UNLOCK(sc);

	if (div->div)
		ratio = __SHIFTOUT(val, div->div);
	else
		ratio = 0;

	if (div->flags & IMX8_CLK_DIV_POWER_OF_TWO) {
		return rate >> ratio;
	} else if (div->flags & IMX8_CLK_DIV_CPU_SCALE_TABLE) {
		if (ratio < 1 || ratio > 8)
			return 0;
		return rate / ((ratio + 1) * 2);
	} else {
		return rate / (ratio + 1);
	}
}

int
imx8_clk_div_set_rate(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk, u_int new_rate)
{
	struct imx8_clk_div *div = &clk->u.div;
	struct clk *clkp, *clkp_parent;
	int parent_rate;
	uint32_t val, raw_div;
	int ratio, error;

	KASSERT(clk->type == IMX8_CLK_DIV);

	clkp = &clk->base;
	clkp_parent = clk_get_parent(clkp);
	if (clkp_parent == NULL)
		return ENXIO;

	if ((div->flags & IMX8_CLK_DIV_SET_RATE_PARENT) != 0)
		return clk_set_rate(clkp_parent, new_rate);

	if (div->div == 0)
		return ENXIO;

	CLK_LOCK(sc);

	val = CLK_READ(sc, div->reg);

	parent_rate = clk_get_rate(clkp_parent);
	if (parent_rate == 0) {
		error = (new_rate == 0) ? 0 : ERANGE;
		goto done;
	}

	ratio = howmany(parent_rate, new_rate);
	if ((div->flags & IMX8_CLK_DIV_POWER_OF_TWO) != 0) {
		error = EINVAL;
		goto done;
	} else if ((div->flags & IMX8_CLK_DIV_CPU_SCALE_TABLE) != 0) {
		error = EINVAL;
		goto done;
	} else {
		raw_div = (ratio > 0) ? ratio - 1 : 0;
	}
	if (raw_div > __SHIFTOUT_MASK(div->div)) {
		error = ERANGE;
		goto done;
	}

	val &= ~div->div;
	val |= __SHIFTIN(raw_div, div->div);
	CLK_WRITE(sc, div->reg, val);

	error = 0;

done:
	CLK_UNLOCK(sc);

	return error;
}

const char *
imx8_clk_div_get_parent(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk)
{
	struct imx8_clk_div *div = &clk->u.div;

	KASSERT(clk->type == IMX8_CLK_DIV);

	return div->parent;
}
