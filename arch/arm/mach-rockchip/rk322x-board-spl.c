// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2017 Rockchip Electronics Co., Ltd
 */

#include <common.h>
#include <debug_uart.h>
#include <dm.h>
#include <ram.h>
#include <spl.h>
#include <asm/io.h>
#include <asm/arch-rockchip/bootrom.h>
#include <asm/arch-rockchip/hardware.h>
#include <asm/arch-rockchip/timer.h>

u32 spl_boot_device(void)
{
	return BOOT_DEVICE_MMC1;
}

#define SGRF_DDR_CON0 0x10150000
void board_init_f(ulong dummy)
{
	struct udevice *dev;
	int ret;

#ifdef CONFIG_DEBUG_UART
	/*
	 * Debug UART can be used from here if required:
	 *
	 * debug_uart_init();
	 * printch('a');
	 * printhex8(0x1234);
	 * printascii("string");
	 */
	debug_uart_init();
	printascii("SPL Init");
#endif
	ret = spl_early_init();
	if (ret) {
		debug("spl_early_init() failed: %d\n", ret);
		hang();
	}

	rockchip_timer_init();
	printf("timer init done\n");
	ret = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (ret) {
		printf("DRAM init failed: %d\n", ret);
		return;
	}

	/* Disable the ddr secure region setting to make it non-secure */
	rk_clrreg(SGRF_DDR_CON0, 0x4000);
#if defined(CONFIG_SPL_ROCKCHIP_BACK_TO_BROM) && !defined(CONFIG_SPL_BOARD_INIT)
	back_to_bootrom(BROM_BOOT_NEXTSTAGE);
#endif
}
