/*
 * (C) Copyright 2008 - 2013 Tensilica Inc.
 * (C) Copyright 2014 Cadence Design Systems Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Exception handling.
 *  We currently don't handle any exception and force a reset.
 *  (Note that alloca is a special case and handled in start.S)
 */

#include <common.h>
#include <command.h>
#include <asm/xtensa.h>
#include <asm/string.h>
#include <asm/regs.h>

typedef void (*handler_t)(struct pt_regs *);

void xtensa_mem_exc_dummy(struct pt_regs *);

void unhandled_exception(struct pt_regs *regs)
{
	display_printf("!! EXCCAUSE = %2ld", regs->exccause);
	printf("Unhandled Exception: EXCCAUSE = %ld (pc %lx)\n",
	       regs->exccause, regs->pc);
	udelay(10000000);	/* 10s to read display message */
	panic("*** PANIC\n");
}

handler_t exc_table[EXCCAUSE_LAST] = {
	[0 ... EXCCAUSE_LAST-1]			= unhandled_exception,
	[EXCCAUSE_LOAD_STORE_ERROR]		= xtensa_mem_exc_dummy,
	[EXCCAUSE_UNALIGNED]			= xtensa_mem_exc_dummy,
	[EXCCAUSE_LOAD_STORE_DATA_ERROR]	= xtensa_mem_exc_dummy,
	[EXCCAUSE_LOAD_STORE_ADDR_ERROR]	= xtensa_mem_exc_dummy,
	[EXCCAUSE_FETCH_CACHE_ATTRIBUTE]	= xtensa_mem_exc_dummy,
	[EXCCAUSE_DTLB_MISS]			= xtensa_mem_exc_dummy,
	[EXCCAUSE_DTLB_MULTIHIT]		= xtensa_mem_exc_dummy,
	[EXCCAUSE_DTLB_PRIVILEGE]		= xtensa_mem_exc_dummy,
	[EXCCAUSE_DTLB_SIZE_RESTRICTION]	= xtensa_mem_exc_dummy,
	[EXCCAUSE_LOAD_CACHE_ATTRIBUTE]		= xtensa_mem_exc_dummy,
	[EXCCAUSE_STORE_CACHE_ATTRIBUTE]	= xtensa_mem_exc_dummy,
};

#ifdef CONFIG_USE_IRQ
#error "Use of interrupts is not supported in Xtensa port"
#else
int interrupt_init(void)
{
	return 0;
}

void enable_interrupts(void)
{
}

int disable_interrupts(void)
{
	return 0;
}
#endif
