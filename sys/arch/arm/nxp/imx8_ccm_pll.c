#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx8_ccm_pll.c $");

#include <sys/param.h>
#include <sys/bus.h>

#include <dev/clk/clk_backend.h>

#include <arm/nxp/imx8_ccm.h>

u_int
imx8_clk_pll_get_rate(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk)
{
	struct imx8_clk_pll *pll = &clk->u.pll;
	struct clk *clkp, *clkp_parent;
	u_int n, m, frac;
	uint64_t parent_rate, rate;
	uint32_t val;

	KASSERT(clk->type == IMX8_CLK_PLL);

	clkp = &clk->base;
	clkp_parent = clk_get_parent(clkp);
	if (clkp_parent == NULL)
		return 0;

	parent_rate = clk_get_rate(clkp_parent);
	if (parent_rate == 0)
		return 0;

	CLK_LOCK(sc);

	val = CLK_READ(sc, pll->n.reg);
	n = __SHIFTOUT(val, pll->n.mask);

	val = CLK_READ(sc, pll->m.reg);
	m = __SHIFTOUT(val, pll->m.mask);

	if (pll->frac.mask) {
		val = CLK_READ(sc, pll->frac.reg);
		frac = __SHIFTOUT(val, pll->frac.mask);
	} else {
		frac = 0;
	}

	CLK_UNLOCK(sc);

	rate = parent_rate * m;
	if (frac) {
		uint64_t frac_rate = parent_rate * frac;
		rate += howmany(frac_rate, __SHIFTOUT_MASK(pll->frac.mask) + 1);
	}

	return (u_int)howmany(rate, n);
}

const char *
imx8_clk_pll_get_parent(struct imx8_clk_softc *sc,
    struct imx8_clk_clk *clk)
{
	struct imx8_clk_pll *pll = &clk->u.pll;

	KASSERT(clk->type == IMX8_CLK_PLL);

	return pll->parent;
}
