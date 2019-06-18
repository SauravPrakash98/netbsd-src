#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx8_ccm_gate2.c$");

#include <sys/param.h>
#include <sys/bus.h>

#include <dev/clk/clk_backend.h>

#include <arm/nxp/imx8_ccm.h>

int
imx8_clk_gate2_enable(struct imx8_clk_softc *sc, struct imx8_clk_clk *clk,
    int enable)
{
	struct imx8_clk_gate2 *gate2 = &clk->u.gate2;
	uint32_t val;
//	int set;

	KASSERT(clk->type == IMX8_CLK_GATE2);

//	set = (gate->flags & IMX8_CLK_GATE_SET_TO_DISABLE) ? !enable : enable;

	CLK_LOCK(sc);

	val = CLK_READ(sc, gate2->reg);
//	if (set)
//		val |= gate->mask;
//	else
//		val &= ~gate->mask;
	
	if (gate2->share_count && (*gate2->share_count)++ > 0)
		goto out;

	val &= ~(3 << gate2->bit_idx);
	val |= gate2->cgr_val << gate2->bit_idx;
	CLK_WRITE(sc, gate2->reg, val);

out:
	CLK_UNLOCK(sc);

	return 0;
}

const char *
imx8_clk_gate2_get_parent(struct imx8_clk_softc *sc, struct imx8_clk_clk *clk)
{
	struct imx8_clk_gate2 *gate2 = &clk->u.gate2;

	KASSERT(clk->type == IMX8_CLK_GATE2);

	return gate2->parent;
}
