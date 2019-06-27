#include "opt_imxuart.h"

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/device.h>

#include <arm/nxp/imx8_reg.h>
//#include <arm/nxp/imx8var.h> // not present now

#include <arm/imx/imxuartreg.h>
#include <arm/imx/imxuartvar.h>

#include <dev/fdt/fdtvar.h>

static int imx8_uart_match(device_t, struct cfdata *, void *);
static void imx8_uart_attach(device_t, device_t, void *);

CFATTACH_DECL_NEW(imx8_uart, sizeof(struct imxuart_softc),
	imx8_uart_match, imx8_uart_attach, NULL, NULL);

static const char * compatible[] = {
	"fsl,imx8-uart",
	NULL
};

int
imx8_uart_match(device_t parent, struct cfdata *cf, void *aux)
{
	struct fdt_attach_args * const faa = aux;

	return of_match_compatible(faa->faa_phandle, compatible);
	/*switch (addr) {
		case (IMX8_AIPS3_BASE + AIPS3_UART1_BASE):
		case (IMX8_AIPS3_BASE + AIPS3_UART2_BASE):
		case (IMX8_AIPS3_BASE + AIPS3_UART3_BASE):
		case (IMX8_AIPS3_BASE + AIPS3_UART4_BASE):
			return 1;
	}
	return 0;*/
}

void
imx8_uart_attach(device_t parent, device_t self, void *aux)
{
	struct fdt_attach_args * faa = aux;
	const int phandle = faa->faa_phandle;
	char intrstr[128];
	bus_addr_t addr;
	bus_size_t size;

	if (fdtbus_get_reg(phandle, 0, &addr, &size) != 0){
		aprint_error(": couldn't get registers\n");
		return;
	}

	if (!fdtbus_intr_str(phandle, 0, intrstr, sizeof(intrstr))) {
		aprint_error(": failed to decode interrupt\n");
		return;
	}

	if (fdtbus_intr_establish(phandle, 0, IPL_SERIAL, 0,
		imxuintr, device_private(self)) == NULL) {
		aprint_error_dev(self, "failed to establish interrupt on %s\n", intrstr);
		return;
	}
	aprint_normal_dev(self, "interrupting on %s\n", intrstr);

	imxuart_attach_common(parent, self,
			faa->faa_bst, addr, size,-1, 0);
}
