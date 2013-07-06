/*
 * Copyright (C) 2007-2013 Tensilica, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/addrspace.h>

/*
 * The 'xtfpga' board describes a set of very similar boards with only minimal
 * differences. Define one of the following in boards.cfg
 *  XTFPGA_LX60
 *  XTFPGA_LX110
 *  XTFPGA_LX200
 *  XTFPGA_ML605
 *  XTFPGA_KC705
 */

/*=====================*/
/* Board and Processor */
/*=====================*/

#define CONFIG_XTFPGA

/* FPGA CPU freq after init */
#define CONFIG_SYS_CLK_FREQ		(gd->cpu_clk)

/* Which (core) timer to use */
#define CONFIG_TIMER_INDEX		0
#define CONFIG_SYS_HZ			1000		/* timer-ticks/second */

/*===================*/
/* RAM Memory Layout */
/*===================*/

/* Lx60 can only map 128kb memory (instead of 256kb) when running under OCD */
#ifdef CONFIG_XTFPGA_LX60
# define CONFIG_SYS_MONITOR_LEN		0x00020000	/* 128 << 10 */
#else
# define CONFIG_SYS_MONITOR_LEN		0x00040000	/* 256 << 10 */
#endif

#define CONFIG_SYS_STACKSIZE		(512 << 10)	/* stack 128KB */
#define CONFIG_SYS_MALLOC_LEN		(256 << 10)	/* heap  256KB */

/* Linux boot param area in RAM (used only when booting linux) */
#define CONFIG_SYS_BOOTPARAMS_LEN	(64  << 10)

/* U-Boot initial RAM area before it unpacks itself (not currently used) */

/* Memory test is destructive so default must not overlap vectors or U-Boot*/
#define CONFIG_SYS_MEMTEST_START	MEMADDR(0x01000000)
#define CONFIG_SYS_MEMTEST_END		MEMADDR(0x02000000)

/* Load address for stand-alone applications.
 * MEMADDR cannot be used here, because the definition needs to be
 * a plain number as it's used as -Ttext argument for ld in standalone
 * example makefile. Handle MMUv2 vs MMUv3 distinction here manually.
 */
#if XCHAL_VECBASE_RESET_VADDR == XCHAL_VECBASE_RESET_PADDR
#define CONFIG_STANDALONE_LOAD_ADDR	0x00800000
#else
#define CONFIG_STANDALONE_LOAD_ADDR	0xd0800000
#endif

/* SDRAM sizes: */
/* LX60		0x04000000		  64 MB  */
/* LX110	0x03000000		  48 MB  */
/* LX200	0x06000000		  96 MB  */
/* ML605	0x20000000		 512 MB  */
/* KC705	0x40000000		   1 GB  */

#define CONFIG_SYS_SDRAM_PHYS		0x00000000
#define CONFIG_SYS_SDRAM_VADDR		MEMADDR(0x00000000)
#ifndef CONFIG_SYS_MEMORY_SIZE
# define CONFIG_SYS_MEMORY_SIZE		CONFIG_SYS_SDRAM_SIZE
#endif

#define CONFIG_SYS_MEMORY_TOP		\
	(CONFIG_SYS_MEMORY_BASE + CONFIG_SYS_MEMORY_SIZE)

#define CONFIG_SYS_BOOTPARAMS_ADDR	\
	(CONFIG_SYS_MEMORY_TOP - CONFIG_SYS_BOOTPARAMS_LEN)
#define CONFIG_SYS_SP_TOP		\
	(CONFIG_SYS_BOOTPARAMS_ADDR - GD_SIZE)
#define CONFIG_SYS_TEXT_ADDR		\
	(CONFIG_SYS_SP_TOP - CONFIG_SYS_STACKSIZE - CONFIG_SYS_MONITOR_LEN)

#define CONFIG_SYS_GD_ADDR		CONFIG_SYS_SP_TOP

/* Used by tftpboot; env var 'loadaddr' */
#define CONFIG_SYS_LOAD_ADDR		MEMADDR(0x02000000)

/*==============================*/
/* U-Boot general configuration */
/*==============================*/

#undef	CONFIG_USE_IRQ			/* Keep it simple, poll only */
#define CONFIG_SYS_GENERIC_BOARD
#define CONFIG_BOARD_POSTCLK_INIT
#define CONFIG_DISPLAY_BOARDINFO
#define CONFIG_MISC_INIT_R
#define CONFIG_MISC_INIT_F

#define CONFIG_BOOTFILE			"uImage"
#define CONFIG_SYS_PROMPT		"U-Boot> "
	/* Console I/O Buffer Size  */
#define CONFIG_SYS_CBSIZE		1024
	/* Prt buf */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					 sizeof(CONFIG_SYS_PROMPT) + 16)
	/* max number of command args */
#define CONFIG_SYS_MAXARGS		16
	/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE


/* Enable device tree support */
#define CONFIG_OF_LIBFDT		1
/*#define CONFIG_FIT*/

/*=================*/
/* U-Boot commands */
/*=================*/

#include <config_cmd_default.h>
#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_DIAG
#define CONFIG_CMD_PING
#define CONFIG_CMD_SAVES
#define CONFIG_CMD_SAVEENV
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_SETGETDCR
#undef CONFIG_CMD_XIMG

/*==============================*/
/* U-Boot autoboot configuration */
/*==============================*/

#define CONFIG_BOOTDELAY		10	/* autoboot after 10 seconds */
#define CONFIG_BOOT_RETRY_TIME		60	/* retry after 60 secs */
#define CONFIG_BOOT_RETRY_MIN		 1	/* at least 1 second timeout */
/*
 * Be selective on what keys can delay or stop the autoboot process
 *      To stop use: " "
 */
#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_PROMPT          "Autobooting in %d seconds, " \
					"press <SPACE> to stop\n", bootdelay
#define CONFIG_AUTOBOOT_STOP_STR        " "
#undef CONFIG_AUTOBOOT_DELAY_STR
#define DEBUG_BOOTKEYS			0

#define CONFIG_VERSION_VARIABLE
#define CONFIG_AUTO_COMPLETE			/* Support tab autocompletion */
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LONGHELP
#define CONFIG_CRC32_VERIFY
#define CONFIG_MX_CYCLIC
#define CONFIG_SHOW_BOOT_PROGRESS

#ifdef DEBUG
#define CONFIG_PANIC_HANG		1	/* Require manual reboot */
#endif


/*=========================================*/
/* FPGA Registers (board info and control) */
/*=========================================*/

/*
 * These assume FPGA bitstreams from Tensilica release RB and up. Earlier
 * releases may not provide any/all of these registers or at these offsets.
 * Some of the FPGA registers are broken down into bitfields described by
 * SHIFT left amount and field WIDTH (bits), and also by a bitMASK.
 */

/* Date of FPGA bitstream build in binary coded decimal (BCD) */
#define CONFIG_SYS_FPGAREG_DATE		IOADDR(0x0D020000)
#define FPGAREG_MTH_SHIFT		24		/* BCD month 1..12 */
#define FPGAREG_MTH_WIDTH		8
#define FPGAREG_MTH_MASK		0xFF000000
#define FPGAREG_DAY_SHIFT		16		/* BCD day 1..31 */
#define FPGAREG_DAY_WIDTH		8
#define FPGAREG_DAY_MASK		0x00FF0000
#define FPGAREG_YEAR_SHIFT		0		/* BCD year 2001..9999*/
#define FPGAREG_YEAR_WIDTH		16
#define FPGAREG_YEAR_MASK		0x0000FFFF

/* FPGA core clock frequency in Hz (also input to UART) */
#define CONFIG_SYS_FPGAREG_FREQ	IOADDR(0x0D020004)	/* CPU clock frequency*/

/*
 * DIP switch (left=sw1=lsb=bit0, right=sw8=msb=bit7; off=0, on=1):
 *   Bits 0..5 set the lower 6 bits of the default ethernet MAC.
 *   Bit 6 is reserved for future use by Tensilica.
 *   Bit 7 maps the first 128KB of ROM address space at CONFIG_SYS_ROM_BASE to
 *   the base of flash * (when on/1) or to the base of RAM (when off/0).
 */
#define CONFIG_SYS_FPGAREG_DIPSW	IOADDR(0x0D02000C)
#define FPGAREG_MAC_SHIFT		0	/* Ethernet MAC bits 0..5 */
#define FPGAREG_MAC_WIDTH		6
#define FPGAREG_MAC_MASK		0x3f
#define FPGAREG_BOOT_SHIFT		7	/* Boot ROM addr mapping */
#define FPGAREG_BOOT_WIDTH		1
#define FPGAREG_BOOT_MASK		0x80
#define FPGAREG_BOOT_RAM		0
#define FPGAREG_BOOT_FLASH		(1<<FPGAREG_BOOT_SHIFT)

/* Force hard reset of board by writing a code to this register */
#define CONFIG_SYS_FPGAREG_RESET	IOADDR(0x0D020010) /* Reset board .. */
#define CONFIG_SYS_FPGAREG_RESET_CODE	0x0000DEAD   /*  by writing this code */

/*====================*/
/* ASCII Display Info */
/*====================*/

#ifndef CONFIG_XTFPGA_LX200		/* LX200 has no ascii display */
# ifdef CONFIG_XTFPGA_LX60
#  define CONFIG_SYS_ASCDISP_ADDR	IOADDR(0x0D040000)
# else
#  define CONFIG_SYS_ASCDISP_ADDR	IOADDR(0x0D0C0000)
# endif
#define CONFIG_SYS_ASCDISP_LINES	2
#define CONFIG_SYS_ASCDISP_CHARS	16
#define CONFIG_SYS_ASCDISP_BUFSZ	64	/* Display printf buffer size */
#endif

/*====================*/
/* Serial Driver Info */
/*====================*/

#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_COM1		IOADDR(0x0D050020) /* Base address */

/* Input clk to NS16550 (in Hz; the SYS_CLK_FREQ is in kHz) */
#define CONFIG_SYS_NS16550_CLK		CONFIG_SYS_CLK_FREQ
#define CONFIG_CONS_INDEX		1	/* use UART0 for console */
#define CONFIG_BAUDRATE			115200	/* Default baud rate */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_SYS_NS16550_BROKEN_TEMT

/*======================*/
/* Ethernet Driver Info */
/*======================*/

#define CONFIG_ETHOC
#define CONFIG_ETHBASE			00:50:C2:13:6f:00
#define CONFIG_SYS_ETHOC_BASE		IOADDR(0x0d030000)
#define CONFIG_SYS_ETHOC_BUFFER_ADDR	IOADDR(0x0D800000)
#define CONFIG_SYS_ETHOC_SETUP_PHY

/*=====================*/
/* Flash & Environment */
/*=====================*/

#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER			/* use generic CFI driver */
#undef  CONFIG_FLASH_CFI_LEGACY			/* only use CFI for flash */
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#ifdef CONFIG_XTFPGA_LX60
# define CONFIG_SYS_FLASH_SIZE		0x0040000	/* 4MB */
# define CONFIG_SYS_FLASH_SECT_SZ	0x10000		/* block size 64KB */
# define CONFIG_SYS_FLASH_PARMSECT_SZ	0x2000		/* param size  8KB */
# define CONFIG_SYS_FLASH_BASE		IOADDR(0x08000000)
#elif defined(CONFIG_XTFPGA_KC705)
# define CONFIG_SYS_FLASH_SIZE		0x8000000	/* 128MB */
# define CONFIG_SYS_FLASH_SECT_SZ	0x20000		/* block size 128KB */
# define CONFIG_SYS_FLASH_PARMSECT_SZ	0x8000		/* param size 32KB */
# define CONFIG_SYS_FLASH_BASE		IOADDR(0x00000000)
#else
# define CONFIG_SYS_FLASH_SIZE		0x0200000	/* 32MB */
# define CONFIG_SYS_FLASH_SECT_SZ	0x20000		/* block size 128KB */
# define CONFIG_SYS_FLASH_PARMSECT_SZ	0x8000		/* param size 32KB */
# define CONFIG_SYS_FLASH_BASE		IOADDR(0x08000000)
#endif
#define CONFIG_SYS_MAX_FLASH_SECT	\
	(CONFIG_SYS_FLASH_SECT_SZ/CONFIG_SYS_FLASH_PARMSECT_SZ + \
	 CONFIG_SYS_FLASH_SIZE/CONFIG_SYS_FLASH_SECT_SZ - 1)
#define CONFIG_SYS_FLASH_PROTECTION		/* hw flash protection */

#define CONFIG_SYS_FLASH_TOP	(CONFIG_SYS_FLASH_BASE + CONFIG_SYS_FLASH_SIZE)

/* U-Boot monitor lives at the base of flash where it's mapped to ROM area */
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_FLASH_BASE

/*
 * Put environment in top block (64kB)
 * Another option would be to put env. in 2nd param block offs 8KB, size 8KB
 */
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_OFFSET    (CONFIG_SYS_FLASH_SIZE - CONFIG_SYS_FLASH_SECT_SZ)
#define CONFIG_ENV_SIZE	     CONFIG_SYS_FLASH_SECT_SZ

/* print 'E' for empty sector on flinfo */
#define CONFIG_SYS_FLASH_EMPTY_INFO

#endif /* __CONFIG_H */
