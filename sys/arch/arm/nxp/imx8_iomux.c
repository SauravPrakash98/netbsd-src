#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: imx7_iomux.c$");

#include <sys/bus.h>
#include <sys/device.h>
#include <sys/param.h>

#include <dev/fdt/fdtvar.h>
#include <dev/fdt/syscon.h>

struct imx8iomux_softc {
	device_t sc_dev;
	bus_addr_t sc_addr;
	bus_space_tag_t sc_iot;
	bus_space_handle_t sc_ioh_iomuxc;	/* IOMUXC */
};

static struct imx8iomux_softc *iomux_softc;

static int imx8iomux_match(device_t, struct cfdata *, void *);
static void imx8iomux_attach(device_t, device_t, void *);

uint32_t iomux_read(uint32_t);
void iomux_write(uint32_t, uint32_t);

CFATTACH_DECL_NEW(imx8iomux, sizeof(struct imx8iomux_softc),
	imx8iomux_match, imx8iomux_attach, NULL, NULL);

static const char * compatible[] = {
	"fsl,imx8mq-iomuxc",
	NULL
};

static int
imx8iomux_match(device_t parent __unused, struct cfdata *match __unused,
		void *aux)
{
	struct fdt_attach_args * const faa = aux;
	
	if (iomux_softc != NULL)
		return 0;

	return of_match_compatible(faa->faa_phandle, compatible);
}

static void
imx8iomux_attach(device_t parent __unused, device_t self, void *aux)
{
	struct imx8iomux_softc *sc;
	struct fdt_attach_args * faa = aux;
	const int phandle = faa->faa_phandle;
	
	sc = device_private(self);
	sc->sc_dev = self;
	sc->sc_iot = faa->faa_bst;
	
	bus_addr_t addr;
	bus_size_t size;

	if (fdtbus_get_reg(phandle, 0, &addr, &size) != 0){
		aprint_error(": couldn't get registers\n");
		return;
	}

	sc->sc_addr = addr;

	aprint_naive("\n");
	aprint_normal(": IOMUX Controller\n");

	if (bus_space_map(sc->sc_iot, sc->sc_addr, size, 0,
	    &sc->sc_ioh_iomuxc)) {
		aprint_error_dev(self, "Cannot map IOMUXC registers\n");
		return;
	}

	iomux_softc = sc;
	return;
}


uint32_t
iomux_read(uint32_t reg)
{
	if(iomux_softc == NULL)
		return 0;

	return bus_space_read_4(iomux_softc->sc_iot, iomux_softc->sc_ioh_iomuxc, reg);
}

void
iomux_write(uint32_t reg, uint32_t val)
{
	if (iomux_softc == NULL)
		return;

	bus_space_write_4(iomux_softc->sc_iot, iomux_softc->sc_ioh_iomuxc, reg, val);
}
