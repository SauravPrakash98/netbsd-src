#include "opt_soc.h"
#include "opt_multiprocessor.h"
#include "opt_console.h"

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx8_ccm.c $");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/cpu.h>
#include <sys/device.h>

#include <dev/fdt/fdtvar.h>

#include <dev/clk/clk_backend.h>

#include <arm/nxp/imx8_ccm.h>

static void*
imx8_clk_reset_acquire(device_t dev, const void *data, size_t len)
{
	struct imx8_clk_softc * const sc = device_private(dev);
	struct imx8_clk_reset * reset;

	if (len != 4)
		return NULL;
	const u_int reset_id = be32dec(data);

	if (reset_id >= sc->sc_nresets)
		return NULL;
	reset = &sc->sc_resets[reset_id];
	if(reset->mask == 0)
		return NULL;
	
	return reset;
}

static void
imx8_clk_reset_release(device_t dev, void *priv)
{
}

static int
imx8_clk_reset_assert(device_t dev, void *priv)
{
	struct imx8_clk_softc * const sc = device_private(dev);
	struct imx8_clk_reset * const reset = priv;

	CLK_LOCK(sc);
	const uint32_t val = CLK_READ(sc, reset->reg);
	CLK_WRITE(sc, reset->reg, val | reset->mask);
	CLK_UNLOCK(sc);

	return 0;
}

static int
imx8_clk_reset_deassert(device_t dev, void *priv)
{
	struct imx8_clk_softc * const sc = device_private(dev);
	struct imx8_clk_reset * const reset = priv;

	CLK_LOCK(sc);
	const uint32_t val = CLK_READ(sc, reset->reg);
	CLK_WRITE(sc, reset->reg, val & ~reset->mask);
	CLK_UNLOCK(sc);

	return 0;
}

static const struct fdtbus_reset_controller_func imx8_clk_fdtreset_funcs = {
	.acquire = imx8_clk_reset_acquire,
	.release = imx8_clk_reset_release,
	.reset_assert = imx8_clk_reset_assert,
	.reset_deassert = imx8_clk_reset_deassert,
};

static struct clk *
imx8_clk_clock_decode(device_t dev, int cc_phandle, const void *data,
		size_t len)
{
	struct imx8_clk_softc * const sc = device_private(dev);
	struct imx8_clk_clk * clk;
	
	if (len != 4)
		return NULL;
	const u_int clock_id = be32dec(data);
	if(clock_id >= sc->sc_nclks)
		return NULL;

	clk = &sc->sc_clks[clock_id];
	if (clk->type == IMX8_CLK_UNKNOWN)
		return NULL;

	return &clk->base;
}

static const struct fdtbus_clock_controller_func imx8_clk_fdtclock_funcs = {
	.decode = imx8_clk_clock_decode,
};

static struct clk *
imx8_clk_clock_get(void *priv, const char *name)
{
	struct imx8_clk_softc * const sc = priv;
	struct imx8_clk_clk *clk;

	clk = imx8_clk_clock_find(sc, name);
	if(clk == NULL)
		return NULL;
	return &clk->base;
}

static void
imx8_clk_clock_put(void *priv, struct clk *clk)
{
}

static u_int
imx8_clk_clock_get_rate(void *priv, struct clk *clkp)
{
	struct imx8_clk_softc * const sc = priv;
	struct imx8_clk_clk *clk = (struct imx8_clk_clk *)clkp;
	struct clk *clkp_parent;

	if (clk->get_rate)
		return clk->get_rate(sc, clk);

	clkp_parent = clk_get_parent(clkp);
	if(clkp_parent == NULL) {
		aprint_debug("%s: no parent for %s\n", __func__, clk->base.name);
		return 0;
	}

	return clk_get_rate(clkp_parent);
}

static int
imx8_clk_clock_set_rate(void *priv, struct clk *clkp, u_int rate)
{
	struct imx8_clk_softc * const sc = priv;
	struct imx8_clk_clk *clk = (struct imx8_clk_clk *)clkp;
	struct clk *clkp_parent;

	if (clkp->flags & CLK_SET_RATE_PARENT) {
		clkp_parent = clk_get_parent(clkp);
		if (clkp_parent == NULL) {
			aprint_debug("%s: no parent for %s\n", __func__, clk->base.name);
			return ENXIO;
		}
		return clk_set_rate(clkp_parent, rate);
	}

	if(clk->set_rate)
		return clk->set_rate(sc, clk, rate);
	return ENXIO;
}

static u_int
imx8_clk_clock_round_rate(void *priv, struct clk *clkp, u_int rate)
{
	struct imx8_clk_softc * const sc = priv;
	struct imx8_clk_clk *clk = (struct imx8_clk_clk *)clkp;
	struct clk *clkp_parent;

	if (clkp->flags & CLK_SET_RATE_PARENT) {
		clkp_parent = clk_get_parent(clkp);
		if (clkp_parent == NULL) {
			aprint_debug("%s: no parent for %s\n", __func__, clk->base.name);
			return 0;
		}
		return clk_round_rate(clkp_parent, rate);
	}

	if (clk->round_rate)
		return clk->round_rate(sc, clk, rate);

	return 0;
}

static int
imx8_clk_clock_enable(void *priv, struct clk *clkp)
{
	struct imx8_clk_softc * const sc = priv;
	struct imx8_clk_clk *clk = (struct imx8_clk_clk *)clkp;
	struct clk *clkp_parent;
	int error = 0;

	clkp_parent = clk_get_parent(clkp);
	if (clkp_parent != NULL) {
		error = clk_enable(clkp_parent);
		if (error != 0)
			return error;
	}

	if (clk->enable)
		error = clk->enable(sc, clk, 1);

	return error;
}

static int
imx8_clk_clock_disable(void *priv, struct clk *clkp)
{
	struct imx8_clk_softc * const sc = priv;
	struct imx8_clk_clk *clk = (struct imx8_clk_clk *)clkp;
	int error = EINVAL;

	if (clk->enable)
		error = clk->enable(sc, clk, 0);

	return error;
}

static int
imx8_clk_clock_set_parent(void *priv, struct clk *clkp,
    struct clk *clkp_parent)
{
	struct imx8_clk_softc * const sc = priv;
	struct imx8_clk_clk *clk = (struct imx8_clk_clk *)clkp;

	if (clk->set_parent == NULL)
		return EINVAL;

	return clk->set_parent(sc, clk, clkp_parent->name);
}

static struct clk *
imx8_clk_clock_get_parent(void *priv, struct clk *clkp)
{
	struct imx8_clk_softc * const sc = priv;
	struct imx8_clk_clk *clk = (struct imx8_clk_clk *)clkp;
	struct imx8_clk_clk *clk_parent;
	const char *parent;

	if (clk->get_parent == NULL)
		return NULL;

	parent = clk->get_parent(sc, clk);
	if (parent == NULL)
		return NULL;

	clk_parent = imx8_clk_clock_find(sc, parent);
	if (clk_parent != NULL)
		return &clk_parent->base;

	/* No parent in this domain, try FDT */
	return fdtbus_clock_get(sc->sc_phandle, parent);
}

static const struct clk_funcs imx8_clk_clock_funcs = {
	.get = imx8_clk_clock_get,
	.put = imx8_clk_clock_put,
	.get_rate = imx8_clk_clock_get_rate,
	.set_rate = imx8_clk_clock_set_rate,
	.round_rate = imx8_clk_clock_round_rate,
	.enable = imx8_clk_clock_enable,
	.disable = imx8_clk_clock_disable,
	.set_parent = imx8_clk_clock_set_parent,
	.get_parent = imx8_clk_clock_get_parent,
};

struct imx8_clk_clk *
imx8_clk_clock_find(struct imx8_clk_softc *sc, const char *name)
{
	for (int i = 0; i < sc->sc_nclks; i++) {
		if (sc->sc_clks[i].base.name == NULL)
			continue;
		if (strcmp(sc->sc_clks[i].base.name, name) == 0)
			return &sc->sc_clks[i];
	}

	return NULL;
}

void
imx8_clk_attach(struct imx8_clk_softc *sc)
{
	int i;

	sc->sc_clkdom.name = device_xname(sc->sc_dev);
	sc->sc_clkdom.funcs = &imx8_clk_clock_funcs;
	sc->sc_clkdom.priv = sc;
	for (i = 0; i < sc->sc_nclks; i++) {
		sc->sc_clks[i].base.domain = &sc->sc_clkdom;
		clk_attach(&sc->sc_clks[i].base);
	}

	if (sc->sc_nclks > 0)
		fdtbus_register_clock_controller(sc->sc_dev, sc->sc_phandle,
		    &imx8_clk_fdtclock_funcs);

	if (sc->sc_nresets > 0)
		fdtbus_register_reset_controller(sc->sc_dev, sc->sc_phandle,
		    &imx8_clk_fdtreset_funcs);
}

void
imx8_clk_print(struct imx8_clk_softc *sc)
{
	struct imx8_clk_clk *clk;
	struct clk *clkp_parent;
	const char *type;
	int i;

	for (i = 0; i < sc->sc_nclks; i++) {
		clk = &sc->sc_clks[i];
		if (clk->type == IMX8_CLK_UNKNOWN)
			continue;

		clkp_parent = clk_get_parent(&clk->base);

		switch (clk->type) {
		case IMX8_CLK_FIXED:		type = "fixed"; break;
		case IMX8_CLK_GATE:		type = "gate"; break;
//		case IMX8_CLK_MPLL:		type = "mpll"; break;
		case IMX8_CLK_PLL:		type = "pll"; break;
		case IMX8_CLK_DIV:		type = "div"; break;
		case IMX8_CLK_FIXED_FACTOR:	type = "fixed-factor"; break;
		case IMX8_CLK_MUX:		type = "mux"; break;
		default:			type = "???"; break;
		}

        	aprint_debug_dev(sc->sc_dev,
		    "%3d %-12s %2s %-12s %-7s ",
		    i,
        	    clk->base.name,
        	    clkp_parent ? "<-" : "",
        	    clkp_parent ? clkp_parent->name : "",
        	    type);
		aprint_debug("%10u Hz\n", clk_get_rate(&clk->base));
	}
}

void
imx8_clk_lock(struct imx8_clk_softc *sc)
{
	if (sc->sc_syscon != NULL)
		syscon_lock(sc->sc_syscon);
}

void
imx8_clk_unlock(struct imx8_clk_softc *sc)
{
	if (sc->sc_syscon != NULL)
		syscon_unlock(sc->sc_syscon);
}

uint32_t
imx8_clk_read(struct imx8_clk_softc *sc, bus_size_t reg)
{
	if (sc->sc_syscon != NULL)
		return syscon_read_4(sc->sc_syscon, reg);
	else
		return bus_space_read_4(sc->sc_bst, sc->sc_bsh, reg);
}

void
imx8_clk_write(struct imx8_clk_softc *sc, bus_size_t reg, uint32_t val)
{
	if (sc->sc_syscon != NULL)
		syscon_write_4(sc->sc_syscon, reg, val);
	else
		bus_space_write_4(sc->sc_bst, sc->sc_bsh, reg, val);
}
