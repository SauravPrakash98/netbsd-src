#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx8_clk_fixed_factor.c$");

#include <sys/param.h>
#include <sys/bus.h>

#include <dev/clk/clk_backend.h>

#include <arm/nxp/imx8_ccm.h>

static u_int
imx8_clk_fixed_factor_get_parent_rate(struct clk *clkp)
{
	struct clk *clkp_parent;

	clkp_parent = clk_get_parent(clkp);
	if (clkp_parent == NULL)
		return 0;

	return clk_get_rate(clkp_parent);
}

u_int
imx8_clk_fixed_factor_get_rate(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk)
{
	struct imx8_clk_fixed_factor *fixed_factor = &clk->u.fixed_factor;
	struct clk *clkp = &clk->base;

	KASSERT(clk->type == IMX8_CLK_FIXED_FACTOR);

	const u_int p_rate = imx8_clk_fixed_factor_get_parent_rate(clkp);
	if (p_rate == 0)
		return 0;

	return (u_int)(((uint64_t)p_rate * fixed_factor->mult) / fixed_factor->div);
}

static int
imx8_clk_fixed_factor_set_parent_rate(struct clk *clkp, u_int rate)
{
	struct clk *clkp_parent;

	clkp_parent = clk_get_parent(clkp);
	if (clkp_parent == NULL)
		return ENXIO;

	return clk_set_rate(clkp_parent, rate);
}

int
imx8_clk_fixed_factor_set_rate(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk, u_int rate)
{
	struct imx8_clk_fixed_factor *fixed_factor = &clk->u.fixed_factor;
	struct clk *clkp = &clk->base;

	KASSERT(clk->type == IMX8_CLK_FIXED_FACTOR);

	rate *= fixed_factor->div;
	rate /= fixed_factor->mult;

	return imx8_clk_fixed_factor_set_parent_rate(clkp, rate);
}

const char *
imx8_clk_fixed_factor_get_parent(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk)
{
	struct imx8_clk_fixed_factor *fixed_factor = &clk->u.fixed_factor;

	KASSERT(clk->type == IMX8_CLK_FIXED_FACTOR);

	return fixed_factor->parent;
}
