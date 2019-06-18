/* #include <linux/clk-provider.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/slab.h>
#include <linux/bitfield.h>

#include "clk.h"

*/

//This driver is incomplete at this stage


#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx8_ccm_sccg_pll.c $");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/cpu.h>
#include <sys/device.h>

#include <dev/fdt/fdtvar.h>
#include <dev/fdt/syscon.h>
#include <dev/clk/clk_backend.h>


/* PLL CFGs */
#define PLL_CFG0		0x0
#define PLL_CFG1		0x4
#define PLL_CFG2		0x8

#define PLL_DIVF1_MASK		GENMASK(18, 13)
#define PLL_DIVF2_MASK		GENMASK(12, 7)
#define PLL_DIVR1_MASK		GENMASK(27, 25)
#define PLL_DIVR2_MASK		GENMASK(24, 19)
#define PLL_DIVQ_MASK           GENMASK(6, 1)
#define PLL_REF_MASK		GENMASK(2, 0)

#define PLL_LOCK_MASK		BIT(31)
#define PLL_PD_MASK		BIT(7)

/* These are the specification limits for the SSCG PLL */
#define PLL_REF_MIN_FREQ		25000000UL
#define PLL_REF_MAX_FREQ		235000000UL

#define PLL_STAGE1_MIN_FREQ		1600000000UL
#define PLL_STAGE1_MAX_FREQ		2400000000UL

#define PLL_STAGE1_REF_MIN_FREQ		25000000UL
#define PLL_STAGE1_REF_MAX_FREQ		54000000UL

#define PLL_STAGE2_MIN_FREQ		1200000000UL
#define PLL_STAGE2_MAX_FREQ		2400000000UL

#define PLL_STAGE2_REF_MIN_FREQ		54000000UL
#define PLL_STAGE2_REF_MAX_FREQ		75000000UL

#define PLL_OUT_MIN_FREQ		20000000UL
#define PLL_OUT_MAX_FREQ		1200000000UL

#define PLL_DIVR1_MAX			7
#define PLL_DIVR2_MAX			63
#define PLL_DIVF1_MAX			63
#define PLL_DIVF2_MAX			63
#define PLL_DIVQ_MAX			63

#define PLL_BYPASS_NONE			0x0
#define PLL_BYPASS1			0x2
#define PLL_BYPASS2			0x1

#define SSCG_PLL_BYPASS1_MASK           BIT(5)
#define SSCG_PLL_BYPASS2_MASK           BIT(4)
#define SSCG_PLL_BYPASS_MASK		GENMASK(5, 4)

#define PLL_SCCG_LOCK_TIMEOUT		70

struct clk_sccg_pll_setup {
	int divr1, divf1;
	int divr2, divf2;
	int divq;
	int bypass;

	uint64_t vco1;
	uint64_t vco2;
	uint64_t fout;
	uint64_t ref;
	uint64_t ref_div1;
	uint64_t ref_div2;
	uint64_t fout_request;
	int fout_error;
};

struct clk_sccg_pll {
	struct clk_hw	hw;
	//const struct clk_ops  ops;

	void __iomem *base;

	struct clk_sccg_pll_setup setup;

	uint8_t parent; 
	uint8_t bypass1; 
	uint8_t bypass2;
};

#define to_clk_sccg_pll(_hw) container_of(_hw, struct clk_sccg_pll, hw)
static u8 clk_sccg_pll_get_parent(struct clk_hw *hw)
{
	struct clk_sccg_pll *pll = to_clk_sccg_pll(hw);
	uint32_t val;
	uint8_t ret = pll->parent;

	val = readl(pll->base + PLL_CFG0);
	if (val & SSCG_PLL_BYPASS2_MASK)
		ret = pll->bypass2;
	else if (val & SSCG_PLL_BYPASS1_MASK)
		ret = pll->bypass1;
	return ret;
}

static int __clk_sccg_pll_determine_rate(struct clk_hw *hw,
					struct clk_rate_request *req,
					uint64_t min,
					uint64_t max,
					uint64_t rate,
					int bypass)
{
	struct clk_sccg_pll *pll = to_clk_sccg_pll(hw);
	struct clk_sccg_pll_setup *setup = &pll->setup;
	struct clk_hw *parent_hw = NULL;
	int bypass_parent_index;
	int ret = -EINVAL;

	req->max_rate = max;
	req->min_rate = min;

	switch (bypass) {
	case PLL_BYPASS2:
		bypass_parent_index = pll->bypass2;
		break;
	case PLL_BYPASS1:
		bypass_parent_index = pll->bypass1;
		break;
	default:
		bypass_parent_index = pll->parent;
		break;
	}

	parent_hw = clk_hw_get_parent_by_index(hw, bypass_parent_index);
	ret = __clk_determine_rate(parent_hw, req);
	if (!ret) {
		ret = clk_sccg_pll_find_setup(setup, req->rate,
						rate, bypass);
	}

	req->best_parent_hw = parent_hw;
	req->best_parent_rate = req->rate;
	req->rate = setup->fout;

	return ret;
}

static int clk_sccg_pll_determine_rate(struct clk_hw *hw,
				       struct clk_rate_request *req)
{
	struct clk_sccg_pll *pll = to_clk_sccg_pll(hw);
	struct clk_sccg_pll_setup *setup = &pll->setup;
	uint64_t rate = req->rate;
	uint64_t min = req->min_rate;
	uint64_t max = req->max_rate;
	int ret = -EINVAL;

	if (rate < PLL_OUT_MIN_FREQ || rate > PLL_OUT_MAX_FREQ)
		return ret;

	ret = __clk_sccg_pll_determine_rate(hw, req, req->rate, req->rate,
						rate, PLL_BYPASS2);
	if (!ret)
		return ret;

	ret = __clk_sccg_pll_determine_rate(hw, req, PLL_STAGE1_REF_MIN_FREQ,
						PLL_STAGE1_REF_MAX_FREQ, rate,
						PLL_BYPASS1);
	if (!ret)
		return ret;

	ret = __clk_sccg_pll_determine_rate(hw, req, PLL_REF_MIN_FREQ,
						PLL_REF_MAX_FREQ, rate,
						PLL_BYPASS_NONE);
	if (!ret)
		return ret;

	if (setup->fout >= min && setup->fout <= max)
		ret = 0;

	return ret;
}


/*struct clk *imx_clk_sccg_pll(const char *name,
				const char * const *parent_names,
				u8 num_parents,
				u8 parent, u8 bypass1, u8 bypass2,
				void __iomem *base,
				unsigned long flags)
{
	struct clk_sccg_pll *pll;
	struct clk_init_data init;
	struct clk_hw *hw;
	int ret;

	pll = kzalloc(sizeof(*pll), GFP_KERNEL);
	if (!pll)
		return ERR_PTR(-ENOMEM);

	pll->parent = parent;
	pll->bypass1 = bypass1;
	pll->bypass2 = bypass2;

	pll->base = base;
	init.name = name;
	init.ops = &clk_sccg_pll_ops;

	init.flags = flags;
	init.parent_names = parent_names;
	init.num_parents = num_parents;

	pll->base = base;
	pll->hw.init = &init;

	hw = &pll->hw;

	ret = clk_hw_register(NULL, hw);
	if (ret) {
		kfree(pll);
		return ERR_PTR(ret);
	}

	return hw->clk;
} */
