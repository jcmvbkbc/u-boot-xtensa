/*
 * (C) Copyright 2007 - 2013 Tensilica Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/stringify.h>
#include <asm/xtensa.h>
#include <asm/global_data.h>
#include <netdev.h>

#include "lcd.h"

DECLARE_GLOBAL_DATA_PTR;

/*
 * Check board idendity.
 * (Print information about the board to stdout.)
 */


#if defined(CONFIG_XTFPGA_LX60)
const char *board = "XT_AV60";
const char *description = "Avnet Xilinx LX60 FPGA Evaluation Board / ";
#elif defined(CONFIG_XTFPGA_LX110)
const char *board = "XT_AV110";
const char *description = "Avnet Xilinx Virtex-5 LX110 Evaluation Kit / ";
#elif defined(CONFIG_XTFPGA_LX200)
const char *board = "XT_AV200";
const char *description = "Avnet Xilinx Virtex-4 LX200 Evaluation Kit / ";
#elif defined(CONFIG_XTFPGA_ML605)
const char *board = "XT_ML605";
const char *description = "Xilinx Virtex-6 FPGA ML605 Evaluation Kit / ";
#elif defined(CONFIG_XTFPGA_KC705)
const char *board = "XT_KC705";
const char *description = "Xilinx Kintex-7 FPGA KC705 Evaluation Kit / ";
#else
const char *board = "<unknown>";
const char *description = "";
#endif

int checkboard(void)
{
	printf("Board: %s: %sTensilica bitstream\n", board, description);
	return 0;
}

int dram_init(void)
{
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;
	return 0;
}

int board_postclk_init(void)
{
	/*
	 * Obtain CPU clock frequency from board and cache in global
	 * data structure (Hz). Return 0 on success (OK to continue),
	 * else non-zero (hang).
	 */

#ifdef CONFIG_SYS_FPGAREG_FREQ
	gd->cpu_clk = (*(volatile unsigned long *)CONFIG_SYS_FPGAREG_FREQ);
#else
	/* early Tensilica bitstreams lack this reg, but most run at 50 MHz */
	gd->cpu_clk = 50000000UL;
#endif
	return 0;
}

/*
 * Miscellaneous early initializations.
 * We use this hook to retrieve the processor frequency
 * and to initialize the LCD display.
 */

int misc_init_f(void)
{
#ifdef CONFIG_SYS_ASCDISP
	/* Initialize the LCD. */

	lcd_init();
#endif
	display_printf("U-Boot starting", NULL);

	return 0;
}

/*
 *  Miscellaneous late initializations.
 *  The environment has been set up, so we can set the Ethernet address.
 */

int misc_init_r(void)
{
#ifdef CONFIG_CMD_NET
	/*
	 * Initialize ethernet environment variables and board info.
	 * Default MAC address comes from CONFIG_ETHADDR + DIP switches 1-6.
	 */

	char *s = getenv("ethaddr");
	if (s == 0) {
		unsigned int x;
		s = __stringify(CONFIG_ETHBASE);
		x = (*(volatile u32 *)CONFIG_SYS_FPGAREG_DIPSW)
			& FPGAREG_MAC_MASK;
		sprintf(&s[15], "%02x", x);
		setenv("ethaddr", s);
	}
#endif /* CONFIG_CMD_NET */

	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	return ethoc_initialize(0, CONFIG_SYS_ETHOC_BASE);
}
#endif


/*
 * Print a formatted string to the board's ASCII character display.
 * String may have embedded newlines. Starts at top left and wraps long lines.
 */

#ifdef CONFIG_SYS_ASCDISP
void display_printf(const char *fmt, ...)
{
	va_list args;

	/* Warning: sprintf() can overflow this buffer if too small! */
	char buf[CONFIG_SYS_ASCDISP_BUFSZ];
	int i;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);

	/* Truncate to one line and pad line with blanks. */
	for (i = 0;
	     i < CONFIG_SYS_ASCDISP_BUFSZ-1 && buf[i] != '\0' && buf[i] != '\n';
	     ++i)
		;
	for (; i < CONFIG_SYS_ASCDISP_CHARS; ++i)
		buf[i] = ' ';
	buf[CONFIG_SYS_ASCDISP_CHARS] = 0;

	lcd_disp_at_pos(buf, 0);

	va_end(args);
}
#endif

#ifdef CONFIG_SHOW_BOOT_PROGRESS
/* Display a boot progress number on the LCD display. */
void show_boot_progress(int val)
{
	display_printf("Progress = %d", val);
}
#endif

/* Implement the "reset" command. */
void board_reset(void)
{
	*(vu_long *)CONFIG_SYS_FPGAREG_RESET = CONFIG_SYS_FPGAREG_RESET_CODE;
	/* Shouldn't reach here. */
}
