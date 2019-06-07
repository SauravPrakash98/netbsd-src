#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx8_ccm_fixed.c $");

#include <sys/param.h>
#include <sys/bus.h>

#include <dev/clk/clk_backend.h>

#include <arm/nxp/imx8_ccm.h>

u_int
imx8_clk_fixed_get_rate(struct imx8_clk_softc *sc, struct imx8_clk_clk *clk)
{
	struct imx8_clk_fixed *fixed = &clk->u.fixed;

	KASSERT(clk->type == IMX8_CLK_FIXED);

	return fixed->rate;
}
