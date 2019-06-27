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
#define IMX8MQ_CLKC_REG_INDEX		1
#define CLK_IS_CRITICAL			1

static int imx8mq_clkc_match(device_t, cfdata_t, void *);
static void imx8mq_clkc_attach(device_t, device_t, void *);

static const char * const compatible[] = {
	"fsl,imx8mq-clkc",
	NULL
};

CFATTACH_DECL_NEW(imx8mq_clkc, sizeof(struct imx8_clk_softc),
		imx8mq_clkc_match, imx8mq_clkc_attach, NULL, NULL);

static const char *pll_ref_parents[] = { "osc_25m", "osc_27m", "dummy", "dummy", };
//static const char *arm_pll_bypass_parents[] = {"arm_pll", "arm_pll_ref_sel", };
//static const char *gpu_pll_bypass_parents[] = {"gpu_pll", "gpu_pll_ref_sel", };
//static const char *vpu_pll_bypass_parents[] = {"vpu_pll", "vpu_pll_ref_sel", };
static const char *audio_pll1_bypass_parents[] = {"audio_pll1", "audio_pll1_ref_sel", };
static const char *audio_pll2_bypass_parents[] = {"audio_pll2", "audio_pll2_ref_sel", };
//static const char *video_pll1_bypass_parents[] = {"video_pll1", "video_pll1_ref_sel", };

//static const char *sys1_pll_out_parents[] = {"sys1_pll1_ref_sel", };
//static const char *sys2_pll_out_parents[] = {"sys1_pll1_ref_sel", "sys2_pll1_ref_sel", };
//static const char *sys3_pll_out_parents[] = {"sys3_pll1_ref_sel", "sys2_pll1_ref_sel", };
//static const char * dram_pll_out_parents[] = {"dram_pll1_ref_sel", };

/*
static const char * imx8mq_a53_parents[] = {"osc_25m", "arm_pll_out", "sys2_pll_500m", "sys2_pll_1000m",
					"sys1_pll_800m", "sys1_pll_400m", "audio_pll1_out", "sys3_pll2_out", };

static const char * imx8mq_arm_m4_parents[] = {"osc_25m", "sys2_pll_200m", "sys2_pll_250m", "sys1_pll_266m",
					"sys1_pll_800m", "audio_pll1_out", "video_pll1_out", "sys3_pll2_out", };

static const char * imx8mq_vpu_parents[] = {"osc_25m", "arm_pll_out", "sys2_pll_500m", "sys2_pll_1000m",
					"sys1_pll_800m", "sys1_pll_400m", "audio_pll1_out", "vpu_pll_out", };

static const char * imx8mq_gpu_core_parents[] = {"osc_25m", "gpu_pll_out", "sys1_pll_800m", "sys3_pll2_out",
					     "sys2_pll_1000m", "audio_pll1_out", "video_pll1_out", "audio_pll2_out", };

static const char * imx8mq_gpu_shader_parents[] = {"osc_25m", "gpu_pll_out", "sys1_pll_800m", "sys3_pll2_out",
					       "sys2_pll_1000m", "audio_pll1_out", "video_pll1_out", "audio_pll2_out", };

static const char * imx8mq_main_axi_parents[] = {"osc_25m", "sys2_pll_333m", "sys1_pll_800m", "sys2_pll_250m",
					     "sys2_pll_1000m", "audio_pll1_out", "video_pll1_out", "sys1_pll_100m",};

static const char * imx8mq_enet_axi_parents[] = {"osc_25m", "sys1_pll_266m", "sys1_pll_800m", "sys2_pll_250m",
					     "sys2_pll_200m", "audio_pll1_out", "video_pll1_out", "sys3_pll2_out", };

static const char * imx8mq_nand_usdhc_parents[] = {"osc_25m", "sys1_pll_266m", "sys1_pll_800m", "sys2_pll_200m",
					       "sys1_pll_133m", "sys3_pll2_out", "sys2_pll_250m", "audio_pll1_out", };

static const char * imx8mq_vpu_bus_parents[] = {"osc_25m", "sys1_pll_800m", "vpu_pll_out", "audio_pll2_out", "sys3_pll2_out", "sys2_pll_1000m", "sys2_pll_200m", "sys1_pll_100m", };

static const char * imx8mq_disp_axi_parents[] = {"osc_25m", "sys2_pll_125m", "sys1_pll_800m", "sys3_pll2_out", "sys1_pll_400m", "audio_pll2_out", "clk_ext1", "clk_ext4", };

static const char * imx8mq_disp_apb_parents[] = {"osc_25m", "sys2_pll_125m", "sys1_pll_800m", "sys3_pll2_out",
					     "sys1_pll_40m", "audio_pll2_out", "clk_ext1", "clk_ext3", };

static const char * imx8mq_disp_rtrm_parents[] = {"osc_25m", "sys1_pll_800m", "sys2_pll_200m", "sys1_pll_400m",
					      "audio_pll1_out", "video_pll1_out", "clk_ext2", "clk_ext3", };

static const char * imx8mq_usb_bus_parents[] = {"osc_25m", "sys2_pll_500m", "sys1_pll_800m", "sys2_pll_100m",
					    "sys2_pll_200m", "clk_ext2", "clk_ext4", "audio_pll2_out", };

static const char * imx8mq_gpu_axi_parents[] = {"osc_25m", "sys1_pll_800m", "gpu_pll_out", "sys3_pll2_out", "sys2_pll_1000m",
					    "audio_pll1_out", "video_pll1_out", "audio_pll2_out", };

static const char * imx8mq_gpu_ahb_parents[] = {"osc_25m", "sys1_pll_800m", "gpu_pll_out", "sys3_pll2_out", "sys2_pll_1000m",
					    "audio_pll1_out", "video_pll1_out", "audio_pll2_out", };

static const char * imx8mq_noc_parents[] = {"osc_25m", "sys1_pll_800m", "sys3_pll2_out", "sys2_pll_1000m", "sys2_pll_500m",
					"audio_pll1_out", "video_pll1_out", "audio_pll2_out", };

static const char * imx8mq_noc_apb_parents[] = {"osc_25m", "sys1_pll_400m", "sys3_pll2_out", "sys2_pll_333m", "sys2_pll_200m",
					    "sys1_pll_800m", "audio_pll1_out", "video_pll1_out", };

static const char * imx8mq_ahb_parents[] = {"osc_25m", "sys1_pll_133m", "sys1_pll_800m", "sys1_pll_400m",
					"sys2_pll_125m", "sys3_pll2_out", "audio_pll1_out", "video_pll1_out", };

static const char * imx8mq_audio_ahb_parents[] = {"osc_25m", "sys2_pll_500m", "sys1_pll_800m", "sys2_pll_1000m",
						  "sys2_pll_166m", "sys3_pll2_out", "audio_pll1_out", "video_pll1_out", };

static const char * imx8mq_dsi_ahb_parents[] = {"osc_25m", "sys2_pll_100m", "sys1_pll_80m", "sys1_pll_800m",
						"sys2_pll_1000m", "sys3_pll2_out", "clk_ext3", "audio_pll2_out"};

static const char * imx8mq_dram_alt_parents[] = {"osc_25m", "sys1_pll_800m", "sys1_pll_100m", "sys2_pll_500m",
						"sys2_pll_250m", "sys1_pll_400m", "audio_pll1_out", "sys1_pll_266m", };

static const char * imx8mq_dram_apb_parents[] = {"osc_25m", "sys2_pll_200m", "sys1_pll_40m", "sys1_pll_160m",
						"sys1_pll_800m", "sys3_pll2_out", "sys2_pll_250m", "audio_pll2_out", };

static const char * imx8mq_vpu_g1_parents[] = {"osc_25m", "vpu_pll_out", "sys1_pll_800m", "sys2_pll_1000m", "sys1_pll_100m", "sys2_pll_125m", "sys3_pll2_out", "audio_pll1_out", };

static const char * imx8mq_vpu_g2_parents[] = {"osc_25m", "vpu_pll_out", "sys1_pll_800m", "sys2_pll_1000m", "sys1_pll_100m", "sys2_pll_125m", "sys3_pll2_out", "audio_pll1_out", };

static const char * imx8mq_disp_dtrc_parents[] = {"osc_25m", "vpu_pll_out", "sys1_pll_800m", "sys2_pll_1000m", "sys1_pll_160m", "sys2_pll_100m", "sys3_pll2_out", "audio_pll2_out", };

static const char * imx8mq_disp_dc8000_parents[] = {"osc_25m", "vpu_pll_out", "sys1_pll_800m", "sys2_pll_1000m", "sys1_pll_160m", "sys2_pll_100m", "sys3_pll2_out", "audio_pll2_out", };

static const char * imx8mq_pcie1_ctrl_parents[] = {"osc_25m", "sys2_pll_250m", "sys2_pll_200m", "sys1_pll_266m",
					       "sys1_pll_800m", "sys2_pll_500m", "sys2_pll_250m", "sys3_pll2_out", };

static const char * imx8mq_pcie1_phy_parents[] = {"osc_25m", "sys2_pll_100m", "sys2_pll_500m", "clk_ext1", "clk_ext2",
					      "clk_ext3", "clk_ext4", };

static const char * imx8mq_pcie1_aux_parents[] = {"osc_25m", "sys2_pll_200m", "sys2_pll_500m", "sys3_pll2_out",
					      "sys2_pll_100m", "sys1_pll_80m", "sys1_pll_160m", "sys1_pll_200m", };

static const char * imx8mq_dc_pixel_parents[] = {"osc_25m", "video_pll1_out", "audio_pll2_out", "audio_pll1_out", "sys1_pll_800m", "sys2_pll_1000m", "sys3_pll2_out", "clk_ext4", };

static const char * imx8mq_lcdif_pixel_parents[] = {"osc_25m", "video_pll1_out", "audio_pll2_out", "audio_pll1_out", "sys1_pll_800m", "sys2_pll_1000m", "sys3_pll2_out", "clk_ext4", };

static const char * imx8mq_sai1_parents[] = {"osc_25m", "audio_pll1_out", "audio_pll2_out", "video_pll1_out", "sys1_pll_133m", "osc_27m", "clk_ext1", "clk_ext2", };

static const char * imx8mq_sai2_parents[] = {"osc_25m", "audio_pll1_out", "audio_pll2_out", "video_pll1_out", "sys1_pll_133m", "osc_27m", "clk_ext2", "clk_ext3", };

static const char * imx8mq_sai3_parents[] = {"osc_25m", "audio_pll1_out", "audio_pll2_out", "video_pll1_out", "sys1_pll_133m", "osc_27m", "clk_ext3", "clk_ext4", };

static const char * imx8mq_sai4_parents[] = {"osc_25m", "audio_pll1_out", "audio_pll2_out", "video_pll1_out", "sys1_pll_133m", "osc_27m", "clk_ext1", "clk_ext2", };

static const char * imx8mq_sai5_parents[] = {"osc_25m", "audio_pll1_out", "audio_pll2_out", "video_pll1_out", "sys1_pll_133m", "osc_27m", "clk_ext2", "clk_ext3", };

static const char * imx8mq_sai6_parents[] = {"osc_25m", "audio_pll1_out", "audio_pll2_out", "video_pll1_out", "sys1_pll_133m", "osc_27m", "clk_ext3", "clk_ext4", };

static const char * imx8mq_spdif1_parents[] = {"osc_25m", "audio_pll1_out", "audio_pll2_out", "video_pll1_out", "sys1_pll_133m", "osc_27m", "clk_ext2", "clk_ext3", };

static const char * imx8mq_spdif2_parents[] = {"osc_25m", "audio_pll1_out", "audio_pll2_out", "video_pll1_out", "sys1_pll_133m", "osc_27m", "clk_ext3", "clk_ext4", };

static const char * imx8mq_enet_ref_parents[] = {"osc_25m", "sys2_pll_125m", "sys2_pll_500m", "sys2_pll_100m",
					     "sys1_pll_160m", "audio_pll1_out", "video_pll1_out", "clk_ext4", };

static const char * imx8mq_enet_timer_parents[] = {"osc_25m", "sys2_pll_100m", "audio_pll1_out", "clk_ext1", "clk_ext2",
					       "clk_ext3", "clk_ext4", "video_pll1_out", };

static const char * imx8mq_enet_phy_parents[] = {"osc_25m", "sys2_pll_50m", "sys2_pll_125m", "sys2_pll_500m",
					     "audio_pll1_out", "video_pll1_out", "audio_pll2_out", };

static const char * imx8mq_nand_parents[] = {"osc_25m", "sys2_pll_500m", "audio_pll1_out", "sys1_pll_400m",
					 "audio_pll2_out", "sys3_pll2_out", "sys2_pll_250m", "video_pll1_out", };

static const char * imx8mq_qspi_parents[] = {"osc_25m", "sys1_pll_400m", "sys1_pll_800m", "sys2_pll_500m",
					 "audio_pll2_out", "sys1_pll_266m", "sys3_pll2_out", "sys1_pll_100m", };

static const char * imx8mq_usdhc1_parents[] = {"osc_25m", "sys1_pll_400m", "sys1_pll_800m", "sys2_pll_500m",
					 "audio_pll2_out", "sys1_pll_266m", "sys3_pll2_out", "sys1_pll_100m", };

static const char * imx8mq_usdhc2_parents[] = {"osc_25m", "sys1_pll_400m", "sys1_pll_800m", "sys2_pll_500m",
					 "audio_pll2_out", "sys1_pll_266m", "sys3_pll2_out", "sys1_pll_100m", };

static const char * imx8mq_i2c1_parents[] = {"osc_25m", "sys1_pll_160m", "sys2_pll_50m", "sys3_pll2_out", "audio_pll1_out",
					 "video_pll1_out", "audio_pll2_out", "sys1_pll_133m", };

static const char * imx8mq_i2c2_parents[] = {"osc_25m", "sys1_pll_160m", "sys2_pll_50m", "sys3_pll2_out", "audio_pll1_out",
					 "video_pll1_out", "audio_pll2_out", "sys1_pll_133m", };

static const char * imx8mq_i2c3_parents[] = {"osc_25m", "sys1_pll_160m", "sys2_pll_50m", "sys3_pll2_out", "audio_pll1_out",
					 "video_pll1_out", "audio_pll2_out", "sys1_pll_133m", };

static const char * imx8mq_i2c4_parents[] = {"osc_25m", "sys1_pll_160m", "sys2_pll_50m", "sys3_pll2_out", "audio_pll1_out",
					 "video_pll1_out", "audio_pll2_out", "sys1_pll_133m", };

static const char * imx8mq_uart1_parents[] = {"osc_25m", "sys1_pll_80m", "sys2_pll_200m", "sys2_pll_100m",
					  "sys3_pll2_out", "clk_ext2", "clk_ext4", "audio_pll2_out", };
	
static const char * imx8mq_uart2_parents[] = {"osc_25m", "sys1_pll_80m", "sys2_pll_200m", "sys2_pll_100m",
					  "sys3_pll2_out", "clk_ext2", "clk_ext3", "audio_pll2_out", };

static const char * imx8mq_uart3_parents[] = {"osc_25m", "sys1_pll_80m", "sys2_pll_200m", "sys2_pll_100m",
					  "sys3_pll2_out", "clk_ext2", "clk_ext4", "audio_pll2_out", };

static const char * imx8mq_uart4_parents[] = {"osc_25m", "sys1_pll_80m", "sys2_pll_200m", "sys2_pll_100m",
					  "sys3_pll2_out", "clk_ext2", "clk_ext3", "audio_pll2_out", };
*//*
static const char * imx8mq_usb_core_parents[] = {"osc_25m", "sys1_pll_100m", "sys1_pll_40m", "sys2_pll_100m",
					     "sys2_pll_200m", "clk_ext2", "clk_ext3", "audio_pll2_out", };

static const char * imx8mq_usb_phy_parents[] = {"osc_25m", "sys1_pll_100m", "sys1_pll_40m", "sys2_pll_100m",
					     "sys2_pll_200m", "clk_ext2", "clk_ext3", "audio_pll2_out", };

static const char * imx8mq_ecspi1_parents[] = {"osc_25m", "sys2_pll_200m", "sys1_pll_40m", "sys1_pll_160m",
					   "sys1_pll_800m", "sys3_pll2_out", "sys2_pll_250m", "audio_pll2_out", };

static const char * imx8mq_ecspi2_parents[] = {"osc_25m", "sys2_pll_200m", "sys1_pll_40m", "sys1_pll_160m",
					   "sys1_pll_800m", "sys3_pll2_out", "sys2_pll_250m", "audio_pll2_out", };

static const char * imx8mq_pwm1_parents[] = {"osc_25m", "sys2_pll_100m", "sys1_pll_160m", "sys1_pll_40m",
					 "sys3_pll2_out", "clk_ext1", "sys1_pll_80m", "video_pll1_out", };

static const char * imx8mq_pwm2_parents[] = {"osc_25m", "sys2_pll_100m", "sys1_pll_160m", "sys1_pll_40m",
					 "sys3_pll2_out", "clk_ext1", "sys1_pll_80m", "video_pll1_out", };

static const char * imx8mq_pwm3_parents[] = {"osc_25m", "sys2_pll_100m", "sys1_pll_160m", "sys1_pll_40m",
					 "sys3_pll2_out", "clk_ext2", "sys1_pll_80m", "video_pll1_out", };

static const char * imx8mq_pwm4_parents[] = {"osc_25m", "sys2_pll_100m", "sys1_pll_160m", "sys1_pll_40m",
					 "sys3_pll2_out", "clk_ext2", "sys1_pll_80m", "video_pll1_out", };

static const char * imx8mq_gpt1_parents[] = {"osc_25m", "sys2_pll_100m", "sys1_pll_400m", "sys1_pll_40m",
					 "sys1_pll_80m", "audio_pll1_out", "clk_ext1", };

static const char * imx8mq_wdog_parents[] = {"osc_25m", "sys1_pll_133m", "sys1_pll_160m", "vpu_pll_out",
					 "sys2_pll_125m", "sys3_pll2_out", "sys1_pll_80m", "sys2_pll_166m", };

static const char * imx8mq_wrclk_parents[] = {"osc_25m", "sys1_pll_40m", "vpu_pll_out", "sys3_pll2_out", "sys2_pll_200m",
					  "sys1_pll_266m", "sys2_pll_500m", "sys1_pll_100m", };

static const char * imx8mq_dsi_core_parents[] = {"osc_25m", "sys1_pll_266m", "sys2_pll_250m", "sys1_pll_800m",
					     "sys2_pll_1000m", "sys3_pll2_out", "audio_pll2_out", "video_pll1_out", };

static const char * imx8mq_dsi_phy_parents[] = {"osc_25m", "sys2_pll_125m", "sys2_pll_100m", "sys1_pll_800m",
					    "sys2_pll_1000m", "clk_ext2", "audio_pll2_out", "video_pll1_out", };

static const char * imx8mq_dsi_dbi_parents[] = {"osc_25m", "sys1_pll_266m", "sys2_pll_100m", "sys1_pll_800m",
					    "sys2_pll_1000m", "sys3_pll2_out", "audio_pll2_out", "video_pll1_out", };

static const char * imx8mq_dsi_esc_parents[] = {"osc_25m", "sys2_pll_100m", "sys1_pll_80m", "sys1_pll_800m",
					    "sys2_pll_1000m", "sys3_pll2_out", "clk_ext3", "audio_pll2_out", };

static const char * imx8mq_csi1_core_parents[] = {"osc_25m", "sys1_pll_266m", "sys2_pll_250m", "sys1_pll_800m",
					      "sys2_pll_1000m", "sys3_pll2_out", "audio_pll2_out", "video_pll1_out", };

static const char * imx8mq_csi1_phy_parents[] = {"osc_25m", "sys2_pll_125m", "sys2_pll_100m", "sys1_pll_800m",
					     "sys2_pll_1000m", "clk_ext2", "audio_pll2_out", "video_pll1_out", };

static const char * imx8mq_csi1_esc_parents[] = {"osc_25m", "sys2_pll_100m", "sys1_pll_80m", "sys1_pll_800m",
					     "sys2_pll_1000m", "sys3_pll2_out", "clk_ext3", "audio_pll2_out", };

static const char * imx8mq_csi2_core_parents[] = {"osc_25m", "sys1_pll_266m", "sys2_pll_250m", "sys1_pll_800m",
					      "sys2_pll_1000m", "sys3_pll2_out", "audio_pll2_out", "video_pll1_out", };

static const char * imx8mq_csi2_phy_parents[] = {"osc_25m", "sys2_pll_125m", "sys2_pll_100m", "sys1_pll_800m",
					     "sys2_pll_1000m", "clk_ext2", "audio_pll2_out", "video_pll1_out", };

static const char * imx8mq_csi2_esc_parents[] = {"osc_25m", "sys2_pll_100m", "sys1_pll_80m", "sys1_pll_800m",
					     "sys2_pll_1000m", "sys3_pll2_out", "clk_ext3", "audio_pll2_out", };

static const char * imx8mq_pcie2_ctrl_parents[] = {"osc_25m", "sys2_pll_250m", "sys2_pll_200m", "sys1_pll_266m",
					       "sys1_pll_800m", "sys2_pll_500m", "sys2_pll_333m", "sys3_pll2_out", };

static const char * imx8mq_pcie2_phy_parents[] = {"osc_25m", "sys2_pll_100m", "sys2_pll_500m", "clk_ext1",
					      "clk_ext2", "clk_ext3", "clk_ext4", "sys1_pll_400m", };

static const char * imx8mq_pcie2_aux_parents[] = {"osc_25m", "sys2_pll_200m", "sys2_pll_50m", "sys3_pll2_out",
					      "sys2_pll_100m", "sys1_pll_80m", "sys1_pll_160m", "sys1_pll_200m", };

static const char * imx8mq_ecspi3_parents[] = {"osc_25m", "sys2_pll_200m", "sys1_pll_40m", "sys1_pll_160m",
					   "sys1_pll_800m", "sys3_pll2_out", "sys2_pll_250m", "audio_pll2_out", };
static const char * imx8mq_dram_core_parents[] = {"dram_pll_out", "dram_alt_root", };

static const char * imx8mq_clko1_parents[] = {"osc_25m", "sys1_pll_800m", "osc_27m", "sys1_pll_200m",
					  "audio_pll2_out", "sys2_pll_500m", "vpu_pll_out", "sys1_pll_80m", };
static const char * imx8mq_clko2_parents[] = {"osc_25m", "sys2_pll_200m", "sys1_pll_400m", "sys2_pll_166m",
					  "sys3_pll2_out", "audio_pll1_out", "video_pll1_out", "ckil", };
*/

static struct imx8_clk_clk imx8mq_clkc_clks[] = {
	IMX8_CLK_FIXED(IMX8MQ_CLK_32K,"osc_32k",32000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_25M,"osc_25m",24000000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_27M,"osc_27m",27000000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_EXT1,"ext_clk_1",133000000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_EXT2,"ext_clk_2",133000000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_EXT3,"ext_clk_3",133000000),
	IMX8_CLK_FIXED(IMX8MQ_CLK_EXT4,"ext_clk_4",133000000),
	
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_40M,"sys1_pll_40m","sys1_pll_out",20,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_80M,"sys1_pll_80m","sys1_pll_out",10,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_100M,"sys1_pll_100m","sys1_pll_out",8,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_133M,"sys1_pll_133m","sys1_pll_out",6,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_160M,"sys1_pll_160m","sys1_pll_out",5,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_200M,"sys1_pll_200m","sys1_pll_out",4,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_266M,"sys1_pll_266m","sys1_pll_out",3,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_400M,"sys1_pll_400m","sys1_pll_out",2,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS1_PLL_800M,"sys1_pll_800m","sys1_pll_out",1,1),

	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_50M,"sys2_pll_50m","sys2_pll_out",20,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_100M,"sys2_pll_100m","sys2_pll_out",10,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_125M,"sys2_pll_125m","sys2_pll_out",8,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_166M,"sys2_pll_166m","sys2_pll_out",6,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_200M,"sys2_pll_200m","sys2_pll_out",5,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_250M,"sys2_pll_250m","sys2_pll_out",4,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_333M,"sys2_pll_333m","sys2_pll_out",3,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_500M,"sys2_pll_500m","sys2_pll_out",2,1),
	IMX8_CLK_FIXED_FACTOR(IMX8MQ_SYS2_PLL_1000M,"sys2_pll_1000m","sys2_pll_out",1,1),

	IMX8_CLK_GATE2(IMX8MQ_CLK_UART1_ROOT,"uart1_root_clk","uart1",0x4490, 0),	// base +
	IMX8_CLK_GATE2(IMX8MQ_CLK_UART2_ROOT,"uart2_root_clk","uart2",0x44a0, 0),	// base + 
	IMX8_CLK_GATE2(IMX8MQ_CLK_UART3_ROOT,"uart3_root_clk","uart3",0x44b0, 0),	// base +
	IMX8_CLK_GATE2(IMX8MQ_CLK_UART4_ROOT,"uart4_root_clk","uart4",0x44c0, 0),	// base +

	IMX8_CLK_GATE(IMX8MQ_AUDIO_PLL1_OUT,"audio_pll1_out","audio_pll1_bypass", 0x0, 21),	// base +
	IMX8_CLK_GATE(IMX8MQ_AUDIO_PLL2_OUT,"audio_pll2_out","audio_pll2_bypass", 0x8, 21), 	// base +


//	IMX8_CLK_COMPOSITE(IMX8MQ_CLK_UART1,"uart1",imx8mq_uart1_parents, base + 0xaf00, , , ),
//	IMX8_CLK_COMPOSITE(IMX8MQ_CLK_UART2,"uart2",imx8mq_uart2_parents, base + 0xaf80,),
//	IMX8_CLK_COMPOSITE(IMX8MQ_CLK_UART3,"uart3",imx8mq_uart3_parents, base + 0xb000),
//	IMX8_CLK_COMPOSITE(IMX8MQ_CLK_UART4,"uart4",imx8mq_uart4_parents, base + 0xb080),

// 	SCCG_PLL Driver not complete

//	IMX8_CLK_SCCG_PLL(IMX8MQ_SYS1_PLL_OUT,"sys1_pll_out", sys1_pll_out_parents, 1, 0, 0, 0x30, CLK_IS_CRITICAL)
//	IMX8_CLK_SCCG_PLL(IMX8MQ_SYS2_PLL_OUT,"sys2_pll_out", sys2_pll_out_parents, 2, 0, 0, 0x3c, CLK_IS_CRITICAL),
//	IMX8_CLK_SCCG_PLL(IMX8MQ_SYS3_PLL_OUT,"sys3_pll_out", sys3_pll_out_parents, 2, 0, 0, 0x40, CLK_IS_CRITICAL),
	//DRAM_PLL1_REF_OUT

	IMX8_CLK_MUX(IMX8MQ_SYS1_PLL1_REF_SEL,"sys1_pll1_ref_sel", pll_ref_parents, 
			0x30, 			// reg
			__BITS(1,0), 		// sel
			0),
	IMX8_CLK_MUX(IMX8MQ_SYS2_PLL1_REF_SEL,"sys2_pll1_ref_sel", pll_ref_parents, 
			0x3c, 			// reg
			__BITS(1,0),		// sel
		       	0),
	IMX8_CLK_MUX(IMX8MQ_SYS3_PLL1_REF_SEL,"sys3_pll1_ref_sel", pll_ref_parents, 
			0x48, 			// reg
			__BITS(1,0), 		// sel
			0),
	//DRAM_PLL1_REF_SEL
	
	IMX8_CLK_MUX(IMX8MQ_AUDIO_PLL1_BYPASS,"audio_pll1_bypass", audio_pll1_bypass_parents,
			0x0,			// reg 
			__BIT(14), 		//sel
			0), 
	IMX8_CLK_MUX(IMX8MQ_AUDIO_PLL2_BYPASS,"audio_pll2_bypass", audio_pll2_bypass_parents,
			0x8, 			// reg
			__BIT(14), 		// sel
			0), 
	IMX8_CLK_MUX(IMX8MQ_AUDIO_PLL1_REF_SEL,"audio_pll1_ref_sel", pll_ref_parents,
			0x0, 			// reg
			__BITS(17,16),		// sel
		       	0),
	IMX8_CLK_MUX(IMX8MQ_AUDIO_PLL2_REF_SEL,"audio_pll2_ref_sel", pll_ref_parents, 
		       	0x8, 			// reg
			__BITS(17,16),		// sel
		       	0),


//	IMX8_CLK_FRAC_PLL(IMX8MQ_AUDIO_PLL1,"audio_pll1","audio_pll1_ref_div", base + 0x0),
//	IMX8_CLK_FRAC_PLL(IMX8MQ_AUDIO_PLL2,"audio_pll2","audio_pll2_ref_div", base + 0x8),


	IMX8_CLK_DIV(IMX8MQ_AUDIO_PLL1_REF_DIV,"audio_pll1_ref_div","audio_pll1_ref_sel", 
			0x0,			// reg
		       	__BITS(10,5), 		// div
			0), 			// base +
	IMX8_CLK_DIV(IMX8MQ_AUDIO_PLL2_REF_DIV,"audio_pll2_ref_div","audio_pll1_ref_sel", 
			0x8, 			// reg
			__BITS(10,5), 		// div
			0), 			// base +
	
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

//	sc->sc_resets = imx8mq_clkc_resets;
//	sc->sc_nresets = __arraycount(imx8mq_clkc_resets);

	sc->sc_clks = imx8mq_clkc_clks;
	sc->sc_nclks = __arraycount(imx8mq_clkc_clks);

	imx8_clk_attach(sc);

	aprint_naive("\n");
	aprint_normal(": IMX8MQ clock controller");

	imx8_clk_print(sc);
}
