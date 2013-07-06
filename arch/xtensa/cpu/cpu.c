/*
 * (C) Copyright 2008 - 2013 Tensilica Inc.
 * (C) Copyright 2014 - 2016 Cadence Design Systems Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * CPU specific code
 */

#include <common.h>
#include <command.h>
#include <linux/stringify.h>
#include <asm/global_data.h>
#include <asm/cache.h>
#include <asm/string.h>
#include <asm/misc.h>

DECLARE_GLOBAL_DATA_PTR;
gd_t *gd;

#if defined(CONFIG_DISPLAY_CPUINFO)
/*
 * Print information about the CPU.
 */

int print_cpuinfo(void)
{
	char buf[120], mhz[8];
	uint32_t id0, id1;

	asm volatile ("rsr %0, 176\n"
		      "rsr %1, 208\n"
		      : "=r"(id0), "=r"(id1));

	sprintf(buf, "CPU:   Xtensa %s (id: %08x:%08x) at %s MHz\n",
		XCHAL_CORE_ID, id0, id1, strmhz(mhz, gd->cpu_clk));
	puts(buf);
	return 0;
}
#endif

/*
 * Implement the "reset" command.
 * We need support from the board, though.
 */

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	board_reset();

	/* Shouldn't come back! */
	printf("****** RESET FAILED ******\n");
	return 0;
}

/*
 * We currently run always with caches enabled when running for mmemory.
 * Xtensa version D or later will support changing cache behavior, so
 * we could implement it if necessary.
 */

int dcache_status(void)
{
	return 1;
}

void dcache_enable(void)
{
}

void dcache_disable(void)
{
}

void flush_cache(ulong start_addr, ulong size)
{
	__flush_invalidate_dcache_range(start_addr, size);
	__invalidate_icache_range(start_addr, size);
}

void flush_dcache_range(ulong start_addr, ulong end_addr)
{
	__flush_invalidate_dcache_range(start_addr, end_addr - start_addr);
}

int arch_cpu_init(void)
{
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;
	return 0;
}
