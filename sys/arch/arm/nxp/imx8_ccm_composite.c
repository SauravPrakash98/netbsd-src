#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx8_ccm_composite.c $");

#include <sys/param.h>
#include <sys/bus.h>

#include <dev/clk/clk_backend.h>

#include <arm/nxp/imx8_ccm.h>

#include <dev/fdt/fdtvar.h>

int
imx8_clk_composite_enable(struct imx8_clk_softc *sc, struct imx8_clk_clk *clk,
    int enable)
{
	struct imx8_clk_composite *composite = &clk->u.composite;

	KASSERT(clk->type == IMX8_CLK_COMPOSITE);

	if (composite->gate_mask == 0)
		return enable ? 0 : ENXIO;

	const uint32_t write_mask = composite->gate_mask << 16;
	const uint32_t write_val = enable ? 0 : composite->gate_mask;

	CLK_WRITE(sc, composite->gate_reg, write_mask | write_val);

	return 0;
}

u_int
imx8_clk_composite_get_rate(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk)
{
	struct imx8_clk_composite *composite = &clk->u.composite;
	struct clk *clkp, *clkp_parent;

	KASSERT(clk->type == IMX8_CLK_COMPOSITE);

	clkp = &clk->base;
	clkp_parent = clk_get_parent(clkp);
	if (clkp_parent == NULL)
		return 0;

	const u_int prate = clk_get_rate(clkp_parent);
	if (prate == 0)
		return 0;

	const uint32_t val = CLK_READ(sc, composite->muxdiv_reg);
	const u_int div = __SHIFTOUT(val, composite->div_mask) + 1;

	return prate / div;
}

int
imx8_clk_composite_set_rate(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk, u_int rate)
{
	struct imx8_clk_composite *composite = &clk->u.composite;
	u_int best_div, best_mux, best_diff;
	struct imx8_clk_clk *rclk_parent; //change rclk to something else and everywhere it appears
	struct clk *clk_parent;

	KASSERT(clk->type == IMX8_CLK_COMPOSITE);

	best_div = 0;
	best_mux = 0;
	best_diff = INT_MAX;
	for (u_int mux = 0; mux < composite->nparents; mux++) {
		rclk_parent = imx8_clk_clock_find(sc, composite->parents[mux]);  // replace here also
		if (rclk_parent != NULL)  // here too 
			clk_parent = &rclk_parent->base;  // here 
		else
			clk_parent = fdtbus_clock_byname(composite->parents[mux]);
		if (clk_parent == NULL)
			continue;

		const u_int prate = clk_get_rate(clk_parent);
		if (prate == 0)
			continue;

		for (u_int div = 1; div <= __SHIFTOUT_MASK(composite->div_mask) + 1; div++) {
			const u_int cur_rate = prate / div;
			const int diff = (int)rate - (int)cur_rate;
			if (composite->flags & IMX8_COMPOSITE_ROUND_DOWN) {
				if (diff >= 0 && diff < best_diff) {
					best_diff = diff;
					best_mux = mux;
					best_div = div;
				}
			} else {
				if (abs(diff) < best_diff) {
					best_diff = abs(diff);
					best_mux = mux;
					best_div = div;
				}
			}
		}
	}
	if (best_diff == INT_MAX)
		return ERANGE;

	uint32_t write_mask = composite->div_mask << 16;
	uint32_t write_val = __SHIFTIN(best_div - 1, composite->div_mask);
	if (composite->mux_mask) {
		write_mask |= composite->mux_mask << 16;
		write_val |= __SHIFTIN(best_mux, composite->mux_mask);
	}

	CLK_WRITE(sc, composite->muxdiv_reg, write_mask | write_val);

	return 0;
}

const char *
imx8_clk_composite_get_parent(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk)
{
	struct imx8_clk_composite *composite = &clk->u.composite;
	uint32_t val;
	u_int mux;

	KASSERT(clk->type == IMX8_CLK_COMPOSITE);

	if (composite->mux_mask) {
		val = CLK_READ(sc, composite->muxdiv_reg);
		mux = __SHIFTOUT(val, composite->mux_mask);
	} else {
		mux = 0;
	}

	return composite->parents[mux];
}

int
imx8_clk_composite_set_parent(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk, const char *parent)
{
	struct imx8_clk_composite *composite = &clk->u.composite;

	KASSERT(clk->type == IMX8_CLK_COMPOSITE);

	if (!composite->mux_mask)
		return EINVAL;

	for (u_int mux = 0; mux < composite->nparents; mux++) {
		if (strcmp(composite->parents[mux], parent) == 0) {
			const uint32_t write_mask = composite->mux_mask << 16;
			const uint32_t write_val = __SHIFTIN(mux, composite->mux_mask);

			CLK_WRITE(sc, composite->muxdiv_reg, write_mask | write_val);
			return 0;
		}
	}

	return EINVAL;
}
