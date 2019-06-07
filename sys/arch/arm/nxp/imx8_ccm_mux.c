#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx8_clk_mux.c$");

#include <sys/param.h>
#include <sys/bus.h>

#include <dev/clk/clk_backend.h>

#include <arm/nxp/imx8_ccm.h>

const char *
imx8_clk_mux_get_parent(struct imx8_clk_softc *sc, struct imx8_clk_clk *clk)
{
	struct imx8_clk_mux *mux = &clk->u.mux;
	uint32_t val;
	u_int sel;

	KASSERT(clk->type == IMX8_CLK_MUX);

	CLK_LOCK(sc);
	val = CLK_READ(sc, mux->reg);
	CLK_UNLOCK(sc);

	sel = __SHIFTOUT(val, mux->sel);
	if (sel >= mux->nparents)
		return NULL;

	return mux->parents[sel];
}
