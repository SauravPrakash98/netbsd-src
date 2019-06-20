#ifndef _ARM_IMX8_CLK_H
#define _ARM_IMX8_CLK_H

#include <dev/clk/clk_backend.h>
#include <dev/fdt/syscon.h>

struct imx8_clk_softc;
struct imx8_clk_clk;
struct imx8_clk_reset;

/*
 * Reset
 */

struct imx8_clk_reset {
	bus_size_t	reg;
	uint32_t	mask;
};

#define IMX8_CLK_RESET(_id, _reg, _bit)		\
	[_id] = {				\
		.reg = (_reg),			\
		.mask = __BIT(_bit),		\
	}

/*
 * Clocks
 */

enum imx8_clk_clktype {
	IMX8_CLK_UNKNOWN,
	IMX8_CLK_FIXED,
	IMX8_CLK_GATE2,
	IMX8_CLK_GATE,
	IMX8_CLK_PLL,
	IMX8_CLK_DIV,
	IMX8_CLK_FIXED_FACTOR,
	IMX8_CLK_MUX,
	IMX8_CLK_COMPOSITE,
	IMX8_CLK_SCCG_PLL,
};

/*
 * Fixed clocks
 */

struct imx8_clk_fixed {
	u_int		rate;
};

u_int	imx8_clk_fixed_get_rate(struct imx8_clk_softc *, struct imx8_clk_clk *);

#define	IMX8_CLK_FIXED(_id, _name, _rate)			\
	[_id] = {						\
		.type = IMX8_CLK_FIXED,			\
		.base.name = (_name),				\
		.base.flags = 0,				\
		.u.fixed.rate = (_rate),			\
		.get_rate = imx8_clk_fixed_get_rate,		\
	}

/*
 * Gate2 clocks
 */

struct imx8_clk_gate2 {
	bus_size_t	reg;
	//uint32_t	mask;
	const char	*parent;
	//uint32_t	flags;
	uint8_t		bit_idx;
	uint8_t		cgr_val;
	uint8_t		flags;
	unsigned int	*share_count;

//#define	IMX8_CLK_GATE_SET_TO_DISABLE		__BIT(0)
};

int	imx8_clk_gate2_enable(struct imx8_clk_softc *,
			      struct imx8_clk_clk *, int);
const char *imx8_clk_gate2_get_parent(struct imx8_clk_softc *,
				      struct imx8_clk_clk *);

#define	IMX8_CLK_GATE2_FLAGS(_id, _name, _pname, _reg, _bit_idx, _cgr_val , _flags, _scount)	\
	[_id] = {						\
		.type = IMX8_CLK_GATE2,				\
		.base.name = (_name),				\
		.base.flags = CLK_SET_RATE_PARENT,		\
		.u.gate2.parent = (_pname),			\
		.u.gate2.reg = (_reg),				\
		.u.gate2.bit_idx = (_bit_idx),			\
		.u.gate2.cgr_val = (_cgr_val),			\
		.u.gate2.flags = (_flags),			\
		.u.gate2.share_count = (_scount),		\
		.enable = imx8_clk_gate2_enable,			\
		.get_parent = imx8_clk_gate2_get_parent,		\
	}

#define	IMX8_CLK_GATE2(_id, _name, _pname, _reg, _bit_idx)		\
	IMX8_CLK_GATE2_FLAGS(_id, _name, _pname, _reg, _bit_idx, 0x3, 0, NULL)



/*
 * Gate clocks
 */

struct imx8_clk_gate {
	bus_size_t	reg;
	uint32_t	mask;
	const char	*parent;
	uint32_t	flags;
#define	IMX8_CLK_GATE_SET_TO_DISABLE		__BIT(0)
};

int	imx8_clk_gate_enable(struct imx8_clk_softc *,
			      struct imx8_clk_clk *, int);
const char *imx8_clk_gate_get_parent(struct imx8_clk_softc *,
				      struct imx8_clk_clk *);

#define	IMX8_CLK_GATE_FLAGS(_id, _name, _pname, _reg, _bit, _flags)	\
	[_id] = {						\
		.type = IMX8_CLK_GATE,				\
		.base.name = (_name),				\
		.base.flags = CLK_SET_RATE_PARENT,		\
		.u.gate.parent = (_pname),			\
		.u.gate.reg = (_reg),				\
		.u.gate.mask = __BIT(_bit),			\
		.u.gate.flags = (_flags),			\
		.enable = imx8_clk_gate_enable,		\
		.get_parent = imx8_clk_gate_get_parent,	\
	}

#define	IMX8_CLK_GATE(_id, _name, _pname, _reg, _bit)		\
	IMX8_CLK_GATE_FLAGS(_id, _name, _pname, _reg, _bit, 0)



/*
 * Divider clocks
 */

struct imx8_clk_div {
	bus_size_t	reg;
	const char	*parent;
	uint32_t	div;
	uint32_t	flags;
#define	IMX8_CLK_DIV_POWER_OF_TWO	__BIT(0)
#define	IMX8_CLK_DIV_SET_RATE_PARENT	__BIT(1)
#define	IMX8_CLK_DIV_CPU_SCALE_TABLE	__BIT(2)
};

u_int	imx8_clk_div_get_rate(struct imx8_clk_softc *,
			       struct imx8_clk_clk *);
int	imx8_clk_div_set_rate(struct imx8_clk_softc *,
			       struct imx8_clk_clk *, u_int);
const char *imx8_clk_div_get_parent(struct imx8_clk_softc *,
				     struct imx8_clk_clk *);

#define	IMX8_CLK_DIV(_id, _name, _parent, _reg, _div, _flags)	\
	[_id] = {						\
		.type = IMX8_CLK_DIV,				\
		.base.name = (_name),				\
		.u.div.reg = (_reg),				\
		.u.div.parent = (_parent),			\
		.u.div.div = (_div),				\
		.u.div.flags = (_flags),			\
		.get_rate = imx8_clk_div_get_rate,		\
		.set_rate = imx8_clk_div_set_rate,		\
		.get_parent = imx8_clk_div_get_parent,		\
	}

/*
 * Fixed-factor clocks
 */

struct imx8_clk_fixed_factor {
	const char	*parent;
	u_int		div;
	u_int		mult;
};

u_int	imx8_clk_fixed_factor_get_rate(struct imx8_clk_softc *,
					struct imx8_clk_clk *);
int	imx8_clk_fixed_factor_set_rate(struct imx8_clk_softc *,
					struct imx8_clk_clk *, u_int);
const char *imx8_clk_fixed_factor_get_parent(struct imx8_clk_softc *,
					      struct imx8_clk_clk *);

#define	IMX8_CLK_FIXED_FACTOR(_id, _name, _parent, _div, _mult)	\
	[_id] = {							\
		.type = IMX8_CLK_FIXED_FACTOR,				\
		.base.name = (_name),					\
		.u.fixed_factor.parent = (_parent),			\
		.u.fixed_factor.div = (_div),				\
		.u.fixed_factor.mult = (_mult),				\
		.get_rate = imx8_clk_fixed_factor_get_rate,		\
		.get_parent = imx8_clk_fixed_factor_get_parent,	\
		.set_rate = imx8_clk_fixed_factor_set_rate,		\
	}

/*
 * Mux clocks
 */

struct imx8_clk_mux {
	bus_size_t	reg;
	const char	**parents;
	u_int		nparents;
	uint32_t	sel;
	uint32_t	flags;
};

const char *imx8_clk_mux_get_parent(struct imx8_clk_softc *,
			       struct imx8_clk_clk *);

#define	IMX8_CLK_MUX(_id, _name, _parents, _reg, _sel,_flags)		\
	[_id] = {							\
		.type = IMX8_CLK_MUX,					\
		.base.name = (_name),					\
		.base.flags = CLK_SET_RATE_PARENT,			\
		.u.mux.parents = (_parents),				\
		.u.mux.nparents = __arraycount(_parents),		\
		.u.mux.reg = (_reg),					\
		.u.mux.sel = (_sel),				\
		.u.mux.flags = (_flags),				\
		.get_parent = imx8_clk_mux_get_parent,			\
	}

/*
 * PLL clocks
 */

struct imx8_clk_pll_reg {
	bus_size_t	reg;
	uint32_t	mask;
};

#define	IMX8_CLK_PLL_REG(_reg, _mask)					\
	{ .reg = (_reg), .mask = (_mask) }
#define	IMX8_CLK_PLL_REG_INVALID	IMX8_CLK_PLL_REG(0,0)

struct imx8_clk_pll {
	struct imx8_clk_pll_reg		enable;
	struct imx8_clk_pll_reg		m;
	struct imx8_clk_pll_reg		n;
	struct imx8_clk_pll_reg		frac;
	struct imx8_clk_pll_reg		l;
	struct imx8_clk_pll_reg		reset;
	const char			*parent;
	uint32_t			flags;
};

u_int	imx8_clk_pll_get_rate(struct imx8_clk_softc *,
			       struct imx8_clk_clk *);
const char *imx8_clk_pll_get_parent(struct imx8_clk_softc *,
				     struct imx8_clk_clk *);

#define	IMX8_CLK_PLL_RATE(_id, _name, _parent, _enable, _m, _n, _frac, _l,	\
		      _reset, _setratefn, _flags)			\
	[_id] = {							\
		.type = IMX8_CLK_PLL,					\
		.base.name = (_name),					\
		.u.pll.parent = (_parent),				\
		.u.pll.enable = _enable,				\
		.u.pll.m = _m,						\
		.u.pll.n = _n,						\
		.u.pll.frac = _frac,					\
		.u.pll.l = _l,						\
		.u.pll.reset = _reset,					\
		.u.pll.flags = (_flags),				\
		.set_rate = (_setratefn),				\
		.get_rate = imx8_clk_pll_get_rate,			\
		.get_parent = imx8_clk_pll_get_parent,			\
	}

#define	IMX8_CLK_PLL(_id, _name, _parent, _enable, _m, _n, _frac, _l,	\
		      _reset, _flags)					\
	[_id] = {							\
		.type = IMX8_CLK_PLL,					\
		.base.name = (_name),					\
		.u.pll.parent = (_parent),				\
		.u.pll.enable = _enable,				\
		.u.pll.m = _m,						\
		.u.pll.n = _n,						\
		.u.pll.frac = _frac,					\
		.u.pll.l = _l,						\
		.u.pll.reset = _reset,					\
		.u.pll.flags = (_flags),				\
		.get_rate = imx8_clk_pll_get_rate,			\
		.get_parent = imx8_clk_pll_get_parent,			\
	}

/*
 * COMPOSITE Clocks
 */


struct imx8_clk_composite {		//wrong definition of composite clock
	bus_size_t	reg;
	uint32_t	mux_mask;
	uint32_t	div_mask;
//	bus_size_t	gate_reg;
	uint32_t	gate_mask;
	const char	**parents;
	u_int		nparents;
	u_int		flags;
#define	IMX8_COMPOSITE_ROUND_DOWN		0x01
};

int	imx8_clk_composite_enable(struct imx8_clk_softc *, struct imx8_clk_clk *, int);
u_int	imx8_clk_composite_get_rate(struct imx8_clk_softc *, struct imx8_clk_clk *);
int	imx8_clk_composite_set_rate(struct imx8_clk_softc *, struct imx8_clk_clk *, u_int);
const char *imx8_clk_composite_get_parent(struct imx8_clk_softc *, struct imx8_clk_clk *);
int	imx8_clk_composite_set_parent(struct imx8_clk_softc *, struct imx8_clk_clk *, const char *);

#define	IMX8_COMPOSITE(_id, _name, _parents, _reg, _mux_mask, _div_mask, _gate_mask, _flags) \
	[_id] =	{							\
		.type = IMX8_CLK_COMPOSITE,				\
		.base.name = (_name),					\
		.base.flags = 0,					\
		.u.composite.parents = (_parents),			\
		.u.composite.nparents = __arraycount(_parents),		\
		.u.composite.reg = (_reg),				\
		.u.composite.mux_mask = (_mux_mask),			\
		.u.composite.div_mask = (_div_mask),			\
		.u.composite.gate_mask = (_gate_mask),			\
		.u.composite.flags = (_flags),				\
		.enable = imx8_clk_composite_enable,			\
		.get_rate = imx8_clk_composite_get_rate,		\
		.set_rate = imx8_clk_composite_set_rate,		\
		.get_parent = imx8_clk_composite_get_parent,		\
		.set_parent = imx8_clk_composite_set_parent,		\
	}

#define	IMX8_COMPOSITE_NOMUX(_id, _name, _parent, _div_reg, _div_mask, _gate_reg, _gate_mask, _flags) \
	IMX8_COMPOSITE(_id, _name, (const char *[]){ _parent }, _div_reg, 0, _div_mask, _gate_reg, _gate_mask, _flags)

#define	IMX8_COMPOSITE_NOGATE(_id, _name, _parents, _muxdiv_reg, _mux_mask, _div_mask, _flags) \
	IMX8_COMPOSITE(_id, _name, _parents, _muxdiv_reg, _mux_mask, _div_mask, 0, 0, _flags)

#define	IMX8_DIV(_id, _name, _parent, _div_reg, _div_mask, _flags) \
	IMX8_COMPOSITE(_id, _name, (const char *[]){ _parent }, _div_reg, 0, _div_mask, 0, 0, _flags)

/*
 *  SCCG PLL clocks
 */

struct imx8_clk_sccg_pll {
	bus_size_t	reg;
	const char	**parents;
	u_int 		nparents;
	uint8_t	bypass1;
	uint8_t	bypass2;
	unsigned long flags;
};

const char *imx8_clk_sccg_pll_get_parent(struct imx8_clk_softc *,
			struct imx8_clk_clk *);
//insert get rate function here

#define IMX8_CLK_SCCG_PLL(_id, _name, _parents, _nparents, _bypass1, _bypass2, _reg, _flags)	\
	[_id] = {							\
		.type = IMX8_CLK_SCCG_PLL,				\
		.base.name = (_name),					\
		.u.sccg_pll.parents = (_parents),			\
		.u.sccg_pll.nparents = (_nparents),			\
		.u.sccg_pll.bypass1 = (_bypass1),			\
		.u.sccg_pll.bypass2 = (_bypass2),			\
		.u.sccg_pll.reg = (_reg),				\
		.u.sccg_pll.flags = (_flags),				\
		.get_parent = imx8_clk_sccg_pll_get_parent,		\
	}

struct imx8_clk_clk {
	struct clk	base;
	enum imx8_clk_clktype type;
	union {
		struct imx8_clk_fixed fixed;
		struct imx8_clk_gate2 gate2;
		struct imx8_clk_gate gate;
		struct imx8_clk_div div;
		struct imx8_clk_fixed_factor fixed_factor;
		struct imx8_clk_mux mux;
		struct imx8_clk_pll pll;
		struct imx8_clk_sccg_pll sccg_pll;
		struct imx8_clk_composite composite;
	} u;

	int		(*enable)(struct imx8_clk_softc *,
				  struct imx8_clk_clk *, int);
	u_int		(*get_rate)(struct imx8_clk_softc *,
				    struct imx8_clk_clk *);
	int		(*set_rate)(struct imx8_clk_softc *,
				    struct imx8_clk_clk *, u_int);
	u_int		(*round_rate)(struct imx8_clk_softc *,
				    struct imx8_clk_clk *, u_int);
	const char *	(*get_parent)(struct imx8_clk_softc *,
				      struct imx8_clk_clk *);
	int		(*set_parent)(struct imx8_clk_softc *,
				      struct imx8_clk_clk *,
				      const char *);
};

struct imx8_clk_softc {
	device_t		sc_dev;
	int			sc_phandle;

	bus_space_tag_t		sc_bst;
	bus_space_handle_t	sc_bsh;

	struct syscon		*sc_syscon;

	struct clk_domain	sc_clkdom;

	struct imx8_clk_reset *sc_resets;
	u_int			sc_nresets;

	struct imx8_clk_clk	*sc_clks;
	u_int			sc_nclks;
};

void	imx8_clk_attach(struct imx8_clk_softc *);
struct imx8_clk_clk *imx8_clk_clock_find(struct imx8_clk_softc *,
					   const char *);
void	imx8_clk_print(struct imx8_clk_softc *);

void	imx8_clk_lock(struct imx8_clk_softc *);
void	imx8_clk_unlock(struct imx8_clk_softc *);
uint32_t imx8_clk_read(struct imx8_clk_softc *, bus_size_t);
void	imx8_clk_write(struct imx8_clk_softc *, bus_size_t, uint32_t);

#define	CLK_LOCK	imx8_clk_lock
#define	CLK_UNLOCK	imx8_clk_unlock
#define	CLK_READ	imx8_clk_read
#define	CLK_WRITE	imx8_clk_write

#endif /* _ARM_IMX8_CLK_H */
