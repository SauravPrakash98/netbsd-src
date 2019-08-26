# Google Summer of Code: Final Evaluation Report
### Porting NetBSD to HummingBoard Pulse
```<SauravPrakash98/netbsd-src> (branch -> build_1)``` is the result of the work done during the period of GSoC.
The following commits list out my contribution to ```netbsd-src```.

|Commit|Link|
|---|---|
| Created imx8_platform.c and imx8_platform.h, and enabled its compilation with GENERIC64 kernel|https://github.com/SauravPrakash98/netbsd-src/commit/1a11c5e494f62b418eff7937bb1ad461a3b352c7|
|Adding a temporary clock controller modules for imx8 | https://github.com/SauravPrakash98/netbsd-src/commit/4e8ddc777b03578925e45975dc1d8e4d6e494c1a |
| Added composite clock driver code and incomplete driver code for imx8mq ccm|https://github.com/SauravPrakash98/netbsd-src/commit/1aac7b016646ff19dac170500a6870947a3a4c00|
|Added uart clocks and its parents in imx8mq ccm|https://github.com/SauravPrakash98/netbsd-src/commit/fd1b6fc03fac6f2bb785058950b8947b747efff6|
|Added gate and gate2 driver and created the struct for sccg pll clocks |https://github.com/SauravPrakash98/netbsd-src/commit/637f336422e971f4bf425f3b1e157daf7a545b98|
| Added fixed-factor clock driver and corrected the mux and div clock drivers|https://github.com/SauravPrakash98/netbsd-src/commit/118c90458b771eee8691c9683e5164ae164d7de4|
|Added fdt based bus glue for uart driver of imx8|https://github.com/SauravPrakash98/netbsd-src/commit/5ff42a50b7cba5acc78b88e2e2c59b8a370a0ef6|
|Added fdt based iomuxc driver of imx8mq|https://github.com/SauravPrakash98/netbsd-src/commit/15d88f799cd5e4b3d4be10f7d1db95de1e1adf95|
| Enabled earlycons for hummingboard, the board boots till root device|https://github.com/SauravPrakash98/netbsd-src/commit/fa9befb8af15a7d1d3827f6d174a398b61c7f574|
|Enabled the GPC driver for IMX8MQ, uart is now interrupting|https://github.com/SauravPrakash98/netbsd-src/commit/932405c2dcc7a33f82aabfd66e47fbf94ea4289f|



Blog report for Phase 1:(http://blog.netbsd.org/tnf/entry/porting_netbsd_to_hummingboard_pulse)

This Blog has a detailed description of the work involving.
- Device Tree files
- Board Platform Code
- Clock Driver
- UART Driver

Starting from where the Blog left off:

- Adding the IOMUX Driver for IMX8MQ: This driver currently matches the compatible node from the .dts file and attaches the peripheral. Read and Write functions are implemented for the register addresses from the .dts file.
- Adding the GPC driver for IMX8MQ: The existing fdt based code of soc imx6 closely resembles its imx8 counterpart. So a modification in imx6 compatible string enables us to use the imx6 code for imx8. This interrupt driver then feeds the UART.
- Modifying the early_putchar function so that earlycons options of netbsd kernel can work. This prints early output to the serial port before the kernel takes over.
- The board boots to the root device.

##### Creating a bootable image for HummingBoard Pulse :


###### 1.Building NetBSD evbarm64 release:


    git clone https://github.com/SauravPrakash98/netbsd-src.git
    ./build.sh -U -m evbarm64 -O <destination folder> tools
    ./build.sh -U -u -m evbarm64 -O <destination folder> kernel=GENERIC64
    ./build.sh -U -u -m evbarm64 -O <destination folder> release


NetBSD evbarm64 release has a bootable image in ```<destination folder>/releasedir/evbarm/binary/gzimg/arm64.img.gz```

###### 2.Building the ATF and U-boot (Boot loader)
Documention present on this website to build ```flash.bin```         (https://developer.solid-run.com/knowledge-base/i-mx8m-atf-u-boot-and-linux-kernel/)

Follow steps till U-boot.

Resulting flash.bin is renamed as imx8mq-flash.bin 
###### 3.Installing U-boot to netbsd image
Writing ```imx8mq-flash.bin``` to ```arm64.bin```

    dd if=imx8mq-flash.bin of=arm64.img bs=1024 seek=33 conv=notrunc

Writing the ```arm64.img``` to sd-card after unmounting the card

    dd if=arm64.img of=/dev/sdb bs=4k conv=fsync 
    
Bootable sd-card is ready !!

##### Testing:
Output can be testing through its serial port output. We use Kermit to connect to the serial port of the board

Create a kermit-usb0 file:

    set line /dev/ttyUSB0
    set speed 115200
    set carrier-watch off
    set flow-control none
    set handshake none
    set prefixing all
    set streaming off
    set parity none
    connect

```/dev/ttyUSB0``` should be changed to the ```/dev/ttyUSB*``` device file your machine detects as the board.

Use ```dmesg | tail -15``` to find through which device the board connects to the machine.

    dmesg | tail -15
    [ 7324.391402] usb 1-2: new full-speed USB device number 6 using xhci_hcd
    [ 7324.538843] usb 1-2: New USB device found, idVendor=0403, idProduct=6015
    [ 7324.538850] usb 1-2: New USB device strings: Mfr=1, Product=2, SerialNumber=3
    [ 7324.538854] usb 1-2: Product: FT230X Basic UART
    [ 7324.538858] usb 1-2: Manufacturer: FTDI
    [ 7324.538862] usb 1-2: SerialNumber: DM02XSL8
    [ 7324.702956] usbcore: registered new interface driver usbserial
    [ 7324.703017] usbcore: registered new interface driver usbserial_generic
    [ 7324.703073] usbserial: USB Serial support registered for generic
    [ 7324.720811] usbcore: registered new interface driver ftdi_sio
    [ 7324.720859] usbserial: USB Serial support registered for FTDI USB Serial Device
    [ 7324.721013] ftdi_sio 1-2:1.0: FTDI USB Serial Device converter detected
    [ 7324.721093] usb 1-2: Detected FT-X
    [ 7324.721548] usb 1-2: FTDI USB Serial Device converter now attached to ttyUSB0


Now, execute the ```kermit-usb0``` file using ```kermit```

    kermit kermit-usb0
    
Connect the board to the power supply to see the output.
##### Result:

    >> NetBSD/evbarm EFI Boot (aarch64), Revision 1.11 (Fri Aug 16 03:16:11 UTC 2019) (from NetBSD 9.99.9)
    Press return to boot now, any other key for boot prompt
    booting netbsd - starting in 0 seconds.     
    6134952+2738520+1994612+1824532 [705552+492849]=0xec9900
    boot NetBSD/aarch64
    Drop to EL1...OK
    Creating VA=PA tables for CONSADDR
    Creating VA=PA tables for kernel image
    Creating VA=PA tables for FDT
    Creating KVA=PA tables
    OK
    MMU Enable...OK
    initarm
    [   1.0000000] FDT<0xfca00000>
    [   1.0000000] cpufunc
    [   1.0000000] devmap
    [   1.0000000] pmap_devmap_bootstrap:
    [   1.0000000]  devmap: pa 30000000-38ffffff = va fffffffff0000000
    [   1.0000000] bootstrap
    [   1.0000000] stdout
    [   1.0000000] consinit ok
    [   1.0000000] uboot: args 0xfca00000, 0, 0, 0
    [   1.0000000] NetBSD/evbarm (fdt) booting ...
    [   1.0000000] FDT /memory [0] @ 0x40020000 size 0x7ed7000
    [   1.0000000] FDT /memory [1] @ 0x47f09000 size 0xb392f000
    [   1.0000000] FDT /memory [2] @ 0xfb838000 size 0x1cce000
    [   1.0000000] FDT /memory [3] @ 0xfd506000 size 0x2f000
    [   1.0000000] FDT /memory [4] @ 0xfd53e000 size 0x2a06000
    [   1.0000000] FDT /memory [5] @ 0xfff45000 size 0xbb000
    [   1.0000000] initarm: memory start 40020000 end 100000000 (len bffe0000)
    [   1.0000000] initarm: fdt_build_bootconfig
    [   1.0000000] Usable memory:
    [   1.0000000]   40020000 - 47ef6fff
    [   1.0000000]   47f09000 - fd534fff
    [   1.0000000]   fd53e000 - fff43fff
    [   1.0000000]   fff45000 - ffffffff
    [   1.0000000] fdt_map_efi_runtime: netbsd,uefi-runtime-code fff44000-fff44fff (ffff80000000a000-ffff80000000afff)
    [   1.0000000] fdt_map_efi_runtime: netbsd,uefi-runtime-data 47efc000-47efffff (ffff800000000000-ffff800000003fff)
    [   1.0000000] fdt_map_efi_runtime: netbsd,uefi-runtime-data 47f05000-47f08fff (ffff800000004000-ffff800000007fff)
    [   1.0000000] fdt_map_efi_runtime: netbsd,uefi-runtime-data fd537000-fd537fff (ffff800000008000-ffff800000008fff)
    [   1.0000000] fdt_map_efi_runtime: netbsd,uefi-runtime-data fd53d000-fd53dfff (ffff800000009000-ffff800000009fff)
    [   1.0000000] Creating KSEG tables for 0x0000000040020000-0x0000000100000000
    [   1.0000000] cpu_kernel_vm_init: kernel phys start fba00000 end fc7a5000+c000
    [   1.0000000] bootargs: console=ttymxc1,115200 earlycon=ec_imx6q,0x30890000,115200
    [   1.0000000] Memory regions:
    [   1.0000000]   40020000 - 47ef6fff
    [   1.0000000]   47f09000 - fb9fffff
    [   1.0000000]   fc7b1000 - fd534fff
    [   1.0000000]   fd53e000 - fff43fff
    [   1.0000000]   fff45000 - ffffffff
    [   1.0000000] ------------------------------------------
    [   1.0000000] kern_vtopdiff         = 0xffffffbf04600000
    [   1.0000000] physical_start        = 0x0000000040020000
    [   1.0000000] kernel_start_phys     = 0x00000000fba00000
    [   1.0000000] kernel_end_phys       = 0x00000000fc7a5000
    [   1.0000000] msgbuf                = 0x0000000047ee7000
    [   1.0000000] physical_end          = 0x0000000100000000
    [   1.0000000] VM_MIN_KERNEL_ADDRESS = 0xffffffc000000000
    [   1.0000000] kernel_start_l2       = 0xffffffc000000000
    [   1.0000000] kernel_start          = 0xffffffc000000000
    [   1.0000000] kernel_end            = 0xffffffc000da5000
    [   1.0000000] pagetables            = 0xffffffc000da5000
    [   1.0000000] pagetables_end        = 0xffffffc000db1000
    [   1.0000000] kernel_end_l2         = 0xffffffc000e00000
    [   1.0000000] module_start          = 0xffffffc000e00000
    [   1.0000000] module_end            = 0xffffffc002e00000
    [   1.0000000] (kernel va area)
    [   1.0000000] (devmap va area)      = 0xfffffffff0000000
    [   1.0000000] VM_MAX_KERNEL_ADDRESS = 0xffffffffffe00000
    [   1.0000000] ------------------------------------------
    [   1.0000000] mpstart
    main
    [   1.0000000] ksyms: checking .text
    [   1.0000000] ksyms: checking .rodata
    [   1.0000000] ksyms: checking link_set_evcnts
    [   1.0000000] ksyms: checking .eh_frame
    [   1.0000000] ksyms: checking link_set_arm_platforms
    [   1.0000000] ksyms: checking link_set_fdt_consoles
    [   1.0000000] ksyms: checking link_set_arm_cpu_methods
    [   1.0000000] ksyms: checking link_set_sysctl_funcs
    [   1.0000000] ksyms: checking link_set_modules
    [   1.0000000] ksyms: checking link_set_ieee80211_funcs
    [   1.0000000] ksyms: checking link_set_domains
    [   1.0000000] ksyms: checking link_set_sdt_argtypes_set
    [   1.0000000] ksyms: checking link_set_sdt_probes_set
    [   1.0000000] ksyms: checking link_set_linux_module_param_desc
    [   1.0000000] ksyms: checking link_set_linux_module_param_info
    [   1.0000000] ksyms: checking link_set_sdt_providers_set
    [   1.0000000] ksyms: checking link_set_dkwedge_methods
    [   1.0000000] ksyms: checking link_set_prop_linkpools
    [   1.0000000] ksyms: checking .data
    [   1.0000000] ksyms: checking .data.cacheline_aligned
    [   1.0000000] ksyms: checking .data.read_mostly
    [   1.0000000] ksyms: checking .bss
    [   1.0000000] ksyms: checking .ident
    [   1.0000000] ksyms: checking .comment
    [   1.0000000] ksyms: checking .note.netbsd.ident
    [   1.0000000] ksyms: checking .symtab
    [   1.0000000] ksyms: checking .strtab
    [   1.0000000] ksyms: checking .shstrtab
    [   1.0000000] Loaded initial symtab at 0xffffffc000a7e360, strtab at 0xffffffc000b2a770, # entries 29398
    [   1.0000000] Copyright (c) 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005,
    [   1.0000000]     2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017,
    [   1.0000000]     2018, 2019 The NetBSD Foundation, Inc.  All rights reserved.
    [   1.0000000] Copyright (c) 1982, 1986, 1989, 1991, 1993
    [   1.0000000]     The Regents of the University of California.  All rights reserved.
    
    [   1.0000000] NetBSD 9.99.10 (GENERIC64) #23: Mon Aug 26 11:42:02 CEST 2019
    [   1.0000000] 	saurav@saurav-Inspiron-5570:/home/saurav/oba/sys/arch/evbarm/compile/GENERIC64
    [   1.0000000] total memory = 3058 MB
    [   1.0000000] avail memory = 2948 MB
    [   1.0000000] armfdt0 (root)
    [   1.0000000] simplebus0 at armfdt0: SolidRun i.MX8MQ HummingBoard Pulse
    [   1.0000000] cpus0 at simplebus0
    [   1.0000000] simplebus1 at simplebus0
    [   1.0000000] simplebus2 at simplebus0
    [   1.0000000] cpu0 at cpus0: Cortex-A53 r0p4 (Cortex V8-A core)
    [   1.0000000] cpu0: package 0, core 0, smt 0
    [   1.0000000] cpu0: IC enabled, DC enabled, EL0/EL1 stack Alignment check enabled
    [   1.0000000] cpu0: Cache Writeback Granule 16B, Exclusives Reservation Granule 16B
    [   1.0000000] cpu0: Dcache line 64, Icache line 64
    [   1.0000000] cpu0: L1 32KB/64B 2-way read-allocate VIPT Instruction cache
    [   1.0000000] cpu0: L1 32KB/64B 4-way write-back read-allocate write-allocate PIPT Data cache
    [   1.0000000] cpu0: L2 1024KB/64B 16-way write-back read-allocate write-allocate PIPT Unified cache
    [   1.0000000] cpu0: revID=0x180, PMCv3, 4k table, 64k table, 16bit ASID
    [   1.0000000] cpu0: auxID=0x11120, GICv3, FP, CRC32, SHA1, SHA256, AES+PMULL, NEON, rounding, NaN propagation,     denormals, 32x64bitRegs, Fused Multiply-Add
    [   1.0000000] cpu1 at cpus0: Cortex-A53 r0p4 (Cortex V8-A core)
    [   1.0000000] cpu1: package 0, core 1, smt 0
    [   1.0000000] cpu2 at cpus0: Cortex-A53 r0p4 (Cortex V8-A core)
    [   1.0000000] cpu2: package 0, core 2, smt 0
    [   1.0000000] cpu3 at cpus0: Cortex-A53 r0p4 (Cortex V8-A core)
    [   1.0000000] cpu3: package 0, core 3, smt 0
    [   1.0000000] imxgpc0 at simplebus0: General Power Controller
    [   1.0000000] gicvthree0 at simplebus0: GICv3
    [   1.0000000] gtmr0 at simplebus0: Generic Timer
    [   1.0000000] gtmr0: interrupting on GICv3 irq 27
    [   1.0000000] armgtmr0 at gtmr0: Generic Timer (8333 kHz, virtual)
    [   1.0000100] syscon0 at simplebus0: System Controller Registers
    [   1.0000100] syscon1 at simplebus0: System Controller Registers
    [   1.0000100] imx8iomux0 at simplebus0: IOMUX Controller
    [   1.0000100] fregulator0 at simplebus1: VSD_3V3
    [   1.0000100] /ccm@30380000 at simplebus0 not configured
    [   1.0000100] /tmu@30260000 at simplebus0 not configured
    [   1.0000100] /sdma@30bd0000 at simplebus0 not configured
    [   1.0000100] /phy@381f0040 at simplebus0 not configured
    [   1.0000100] /power-controller at simplebus0 not configured
    [   1.0000100] /phy@382f0040 at simplebus0 not configured
    [   1.0000100] /gpio@30210000 at simplebus0 not configured
    [   1.0000100] /iomuxc-gpr@30340000 at simplebus0 not configured
    [   1.0000100] /ethernet@30be0000 at simplebus0 not configured
    [   1.0000100] /gpio@30200000 at simplebus0 not configured
    [   1.0000100] psci0 at simplebus0: PSCI 1.1
    [   1.0000100] /gpio@30220000 at simplebus0 not configured
    [   1.0000100] /gpio@30230000 at simplebus0 not configured
    [   1.0000100] /gpio@30240000 at simplebus0 not configured
    [   1.0000100] imxuart0 at simplebus0imxuart0: interrupting on irq 58
    
    [   1.0000100] imxuart0: intr_establish failed
    [   1.0000100] imxuart1 at simplebus0imxuart1: interrupting on irq 59
    
    [   1.0000100] imxuart1: intr_establish failed
    [   1.0000100] /usb@38100000 at simplebus0 not configured
    [   1.0000100] /usb@38200000 at simplebus0 not configured
    [   1.0000100] /usdhc@30b40000 at simplebus0 not configured
    [   1.0000100] /usdhc@30b50000 at simplebus0 not configured
    [   1.0000100] /sdma@302c0000 at simplebus0 not configured
    [   1.0000100] /imx_ion at simplebus0 not configured
    [   1.0000100] /i2c@30a20000 at simplebus0 not configured
    [   1.0000100] /wdog@30280000 at simplebus0 not configured
    [   1.0000100] /dma_cap at simplebus0 not configured
    [   1.0000100] /qspi@30bb0000 at simplebus0 not configured
    [   1.0000100] cpu_boot_secondary_processors: writing mbox with 0xe
    [   1.0000100] cpu3: IC enabled, DC enabled, EL0/EL1 stack Alignment check enabled
    [   1.1795220] cpu3: Cache Writeback Granule 16B, Exclusives Reservation Granule 16B
    [   1.1795220] cpu3: Dcache line 64, Icache line 64
    [   1.1895135] cpu3: L1 32KB/64B 2-way read-allocate VIPT Instruction cache
    [   1.1895135] cpu3: L1 32KB/64B 4-way write-back read-allocate write-allocate PIPT Data cache
    [   1.1995147] cpu3: L2 1024KB/64B 16-way write-back read-allocate write-allocate PIPT Unified cache
    [   1.2095148] cpu3: revID=0x180, PMCv3, 4k table, 64k table, 16bit ASID
    [   1.2195157] cpu3: auxID=0x11120, GICv3, FP, CRC32, SHA1, SHA256, AES+PMULL, NEON, rounding, NaN propagation,     denormals, 32x64bitRegs, Fused Multiply-Add
    [   1.2295158] cpu2: IC enabled, DC enabled, EL0/EL1 stack Alignment check enabled
    [   1.2395173] cpu2: Cache Writeback Granule 16B, Exclusives Reservation Granule 16B
    [   1.2495166] cpu2: Dcache line 64, Icache line 64
    [   1.2495166] cpu2: L1 32KB/64B 2-way read-allocate VIPT Instruction cache
    [   1.2595176] cpu2: L1 32KB/64B 4-way write-back read-allocate write-allocate PIPT Data cache
    [   1.2695174] cpu2: L2 1024KB/64B 16-way write-back read-allocate write-allocate PIPT Unified cache
    [   1.2795187] cpu2: revID=0x180, PMCv3, 4k table, 64k table, 16bit ASID
    [   1.2795187] cpu2: auxID=0x11120, GICv3, FP, CRC32, SHA1, SHA256, AES+PMULL, NEON, rounding, NaN propagation,     denormals, 32x64bitRegs, Fused Multiply-Add
    [   1.2995197] cpu1: IC enabled, DC enabled, EL0/EL1 stack Alignment check enabled
    [   1.2995197] cpu1: Cache Writeback Granule 16B, Exclusives Reservation Granule 16B
    [   1.3095197] cpu1: Dcache line 64, Icache line 64
    [   1.3095197] cpu1: L1 32KB/64B 2-way read-allocate VIPT Instruction cache
    [   1.3195205] cpu1: L1 32KB/64B 4-way write-back read-allocate write-allocate PIPT Data cache
    [   1.3295204] cpu1: L2 1024KB/64B 16-way write-back read-allocate write-allocate PIPT Unified cache
    [   1.3395215] cpu1: revID=0x180, PMCv3, 4k table, 64k table, 16bit ASID
    [   1.3495214] cpu1: auxID=0x11120, GICv3, FP, CRC32, SHA1, SHA256, AES+PMULL, NEON, rounding, NaN propagation,     denormals, 32x64bitRegs, Fused Multiply-Add
    [   1.3595231] cpu_boot_secondary_processors: secondary processors hatched
    [   1.3895243] WARNING: 2 errors while detecting hardware; check system log.
    [   1.4032764] boot device: <unknown>
    [   1.4032764] root device: 

