#include <sys/cdefs.h>
__KERNEL_RCSID(1, "$NetBSD: imx8mq_ccm.c $");

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/device.h>
#include <sys/systm.h>

#include <dev/fdt/fdtvar.h>

#include <arm/nxp/imx8_ccm.h>
#include <arm/nxp/imx8mq_ccm.h>

// some definitions
#define IMX8MQ_CLKC_REG_INDEX 	1

static int imx8mq_clkc_match(device_t, cfdata_t, void *);
static void imx8mq_clkc_attach(device_t, device_t, void *);

static const char * const compatible[] = {
	"fsl,imx8mq-clkc",
	NULL
};

CFATTACH_DECL_NEW(imx8mq_clkc, sizeof(struct imx8_clk_softc),
		imx8mq_clkc_match, imx8mq_clkc_attach, NULL, NULL);

static struct imx8mq_clk_reset imx8mq_clkc_resets[] = {
	IMX8_CLK_RESET(),  //insert some resets
};

static const char * parents[] = {};

static struct imx8_clk_clk imx8mq_clkc_clks[] = {
	IMX8_CLK_FIXED(IMX8MQ_CLK_32K,"osc_32k",32000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_25M,"osc_25m",24000000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_27M,"osc_27m",27000000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_EXT1,"ext_clk_1",133000000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_EXT2,"ext_clk_2",133000000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_EXT3,"ext_clk_3",133000000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_EXT4,"ext_clk_4",133000000),
	
	//IMX8_CLK_PLL(IMX8MQ_SYS1_PLL1_OUT, "sys1_pll1_out" )
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_40M,"sys1_pll_40m","sys1_pll",20,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_80M,"sys1_pll_80m","sys1_pll",10,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_100M,"sys1_pll_100m","sys1_pll",8,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_133M,"sys1_pll_133m","sys1_pll",6,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_160M,"sys1_pll_160m","sys1_pll",5,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_200M,"sys1_pll_200m","sys1_pll",4,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_266M,"sys1_pll_266m","sys1_pll",3,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_400M,"sys1_pll_400m","sys1_pll",2,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_800M,"sys1_pll_800m","sys1_pll",1,1),

	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_50M,"sys2_pll_50m","sys2_pll",20,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_100M,"sys2_pll_100m","sys2_pll",10,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_125M,"sys2_pll_125m","sys2_pll",8,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_166M,"sys2_pll_166m","sys2_pll",6,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_200M,"sys2_pll_200m","sys2_pll",5,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_250M,"sys2_pll_250m","sys2_pll",4,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_333M,"sys2_pll_333m","sys2_pll",3,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_500M,"sys2_pll_500m","sys2_pll",2,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_1000M,"sys2_pll_1000m","sys2_pll",1,1),
};

static int
imx8mq_clkc_match(device_t parent, cfdata_t cf, void *aux)
{
	struct fdt_attach_args * const faa = aux;
	return of_match_compatible(faa->faa_phandle, compatible);
}

static void
imx8mq_clkc_attach(device_t parent, device_t self, void *aux)
{
	struct imx8_clk_softc * const sc = device_private(self);
	struct fdt_attach_args * const faa = aux;
	bus_addr_t addr;
	bus_size_t size;

	sc->sc_dev = self;
	sc->sc_phandle = faa->faa_phandle;
	sc->sc_bst = faa->faa_bst;
	if (fdtbus_get_reg(sc->sc_phandle, IMX8MQ_CLKC_REG_INDEX, &addr, &size) != 0) {
		aprint_error(": couldn't get registers\n");
		return;
	}
	if (bus_space_map(sc->sc_bst, addr, size, 0 , &sc->sc_bsh) != 0) {
		aprint_error(": couldn't map registers\n");
		return;
	}

	sc->sc_resets = imx8mq_clkc_resets;
	sc->sc_nresets = __arraycount(imx8mq_clkc_resets);

	sc->sc_clks = imx8mq_clkc_clks;
	sc->sc_nclks = __arraycount(imx8mq_clkc_clks);

	imx8_clk_attach(sc);

	aprint_naive("\n");
	aprint_normal(": IMX8MQ clock controller");

	imx8_clk_print(sc);
}
