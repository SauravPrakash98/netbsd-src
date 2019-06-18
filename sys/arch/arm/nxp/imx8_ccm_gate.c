#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx8_ccm_gate.c$");

#include <sys/param.h>
#include <sys/bus.h>

#include <dev/clk/clk_backend.h>

#include <arm/nxp/imx8_ccm.h>

int
imx8_clk_gate_enable(struct imx8_clk_softc *sc, struct imx8_clk_clk *clk,
    int enable)
{
	struct imx8_clk_gate *gate = &clk->u.gate;
	uint32_t val;
	int set;

	KASSERT(clk->type == IMX8_CLK_GATE);

	set = (gate->flags & IMX8_CLK_GATE_SET_TO_DISABLE) ? !enable : enable;

	CLK_LOCK(sc);

	val = CLK_READ(sc, gate->reg);
	if (set)
		val |= gate->mask;
	else
		val &= ~gate->mask;
	
	CLK_WRITE(sc, gate->reg, val);

	CLK_UNLOCK(sc);

	return 0;
}

const char *
imx8_clk_gate_get_parent(struct imx8_clk_softc *sc, struct imx8_clk_clk *clk)
{
	struct imx8_clk_gate *gate = &clk->u.gate;

	KASSERT(clk->type == IMX8_CLK_GATE);

	return gate->parent;
}
