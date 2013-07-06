/*
 * (C) Copyright 2008 - 2013 Tensilica Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <asm/asmmacro.h>
#include <asm/regs.h>
#include <asm/arch/tie.h>
#include <asm-offsets.h>

/*
 * Offsets into the the pt_regs struture.
 * Make sure these always match with the structure defined in ptrace.h!
 */

#define PT_PC		0
#define PT_PS		4
#define PT_DEPC		8
#define PT_EXCCAUSE	12
#define PT_EXCVADDR	16
#define PT_DEBUGCAUSE	20
#define PT_WMASK	24
#define PT_LBEG		28
#define PT_LEND		32
#define PT_LCOUNT	36
#define PT_SAR		40
#define PT_WINDOWBASE	44
#define PT_WINDOWSTART	48
#define PT_SYSCALL	52
#define PT_ICOUNTLEVEL	56
#define PT_RESERVED	60
#define PT_AREG		64
#define PT_SIZE		(64 + 64)

/*
 * Reset vector.
 * Only a trampoline to jump to _start
 * (Note that we have to mark the section writable as the section contains
 *  a relocatable literal)
 */

	.section .ResetVector.text, "awx"
	.global _ResetVector
_ResetVector:

	j	1f
	.align 4
2:	.long	_start
1:	l32r	a2, 2b
	jx	a2


/*
 * Processor initialization. We still run in rom space.
 *
 * NOTE: Running in ROM
 *  For Xtensa, we currently don't allow to run some code from ROM but
 *  unpack the data immediately to memory. This requires, for example,
 *  that DDR has been set up before running U-Boot. (See also comments
 *  inline for ways to change it)
 */

	.section .reset.text, "ax"
	.global _start
	.align 4
_start:
	/* Keep a0 = 0 for various initializations. */

	movi	a0, 0

	/*
	 * For full MMU cores, put page table at unmapped virtual address.
	 * This ensures that accesses outside the static maps result
	 * in miss exceptions rather than random behaviour.
	 */

#if XCHAL_HAVE_TLBS
	wsr	a0, PTEVADDR
#endif

	/* Disable dbreak debug exceptions. */

#if XCHAL_HAVE_DEBUG && XCHAL_NUM_DBREAK > 0
	.set	_index, 0
	.rept	XCHAL_NUM_DBREAK - 1
	wsr	a0, DBREAKC + _index
	.set	_index, _index + 1
	.endr
#endif

	/* Reset windowbase and windowstart. */

#if XCHAL_HAVE_WINDOWED
	movi	a3, 1
	wsr	a3, windowstart
	wsr	a0, windowbase
	rsync
	movi	a0, 0			/* windowbase might have changed */
#endif

#
# Vecbase in bitstream may differ from header files
# set or check it.
#

#if XCHAL_HAVE_VECBASE
	movi	a3, XCHAL_VECBASE_RESET_VADDR	/* VECBASE reset value */
	wsr	a3, VECBASE
#endif

	/* Disable loops. */

	wsr	a0, LCOUNT

	/* Set PS.WOE = 0, PS.EXCM = 0 (for loop), PS.INTLEVEL = EXCM level */

#if XCHAL_HAVE_XEA1
	movi	a2, 1
#else
	movi	a2, XCHAL_EXCM_LEVEL
#endif
	wsr	a2, PS
	rsync

	/* Unlock and invalidate caches. */

        movi    a2, 0
	__loopi	a2, a3, XCHAL_DCACHE_SIZE, 4 << XCHAL_DCACHE_LINEWIDTH
#if XCHAL_DCACHE_LINE_LOCKABLE
	diu	a2, 0 << XCHAL_DCACHE_LINEWIDTH
	diu	a2, 1 << XCHAL_DCACHE_LINEWIDTH
	diu	a2, 2 << XCHAL_DCACHE_LINEWIDTH
	diu	a2, 3 << XCHAL_DCACHE_LINEWIDTH
#endif
	dii	a2, 0 << XCHAL_DCACHE_LINEWIDTH
	dii	a2, 1 << XCHAL_DCACHE_LINEWIDTH
	dii	a2, 2 << XCHAL_DCACHE_LINEWIDTH
	dii	a2, 3 << XCHAL_DCACHE_LINEWIDTH
	__endla	a2, a3, 4 << XCHAL_DCACHE_LINEWIDTH

        movi    a2, 0
	__loopi	a2, a3, XCHAL_ICACHE_SIZE, 4 << XCHAL_ICACHE_LINEWIDTH
#if XCHAL_ICACHE_LINE_LOCKABLE
	iiu	a2, 0 << XCHAL_ICACHE_LINEWIDTH
	iiu	a2, 1 << XCHAL_ICACHE_LINEWIDTH
	iiu	a2, 2 << XCHAL_ICACHE_LINEWIDTH
	iiu	a2, 3 << XCHAL_ICACHE_LINEWIDTH
#endif
	iii	a2, 0 << XCHAL_ICACHE_LINEWIDTH
	iii	a2, 1 << XCHAL_ICACHE_LINEWIDTH
	iii	a2, 2 << XCHAL_ICACHE_LINEWIDTH
	iii	a2, 3 << XCHAL_ICACHE_LINEWIDTH
	__endla	a2, a3, 4 << XCHAL_ICACHE_LINEWIDTH

	isync

#if XCHAL_HAVE_PTP_MMU && XCHAL_HAVE_SPANNING_WAY
	movi	a2, XCHAL_SPANNING_WAY
	movi	a3, 7
	wdtlb	a3, a2
	witlb	a3, a2
	isync
#endif

	/* Unpack data sections. */

	movi	a2, __reloc_table_start
	movi	a3, __reloc_table_end

1:	beq	a2, a3, 3f	# no more entries?
	l32i	a4, a2, 0	# start destination (in RAM)
	l32i	a5, a2, 4	# end destination (in RAM)
	l32i	a6, a2, 8	# start source (in ROM)
	addi	a2, a2, 12	# next entry
	beq	a4, a5, 1b	# skip, empty entry
	beq	a4, a6, 1b	# skip, source and destination are the same

2:	l32i	a7, a6, 0
	addi	a6, a6, 4
	s32i	a7, a4, 0
	addi	a4, a4, 4
	bltu	a4, a5, 2b
	j	1b

3:	/* All code and initalized data segments have been copied. */

	/* Setup PS, PS.WOE = 1, PS.EXCM = 0, PS.INTLEVEL = EXCM level. */

#if __XTENSA_CALL0_ABI__
	movi	a2, XCHAL_EXCM_LEVEL
#else
	movi	a2, (1<<PS_WOE_BIT) | XCHAL_EXCM_LEVEL
#endif
	wsr	a2, PS
	rsync


	/*
	 * Setup a small stack for some early initialization c-code.
	 * Also, set up a temporary GD.
	 * (reserve 16 bytes for the call4 stack frame)
	 */

	/* Clear BSS */

	movi	a2, _bss_start
	movi	a3, _bss_end

	__loopt	a2, a3, a4, 2
	s32i	a0, a2, 0
	__endla	a2, a4, 4

	/* Writeback */

        movi    a2, 0
	__loopi	a2, a3, XCHAL_DCACHE_SIZE, 4 << XCHAL_DCACHE_LINEWIDTH
	diwb	a2, 0 << XCHAL_DCACHE_LINEWIDTH
	diwb	a2, 1 << XCHAL_DCACHE_LINEWIDTH
	diwb	a2, 2 << XCHAL_DCACHE_LINEWIDTH
	diwb	a2, 3 << XCHAL_DCACHE_LINEWIDTH
	__endla	a2, a3, 4 << XCHAL_DCACHE_LINEWIDTH


        /*
	 * Call board initialization routine (never returns).
	 */

	movi	a6, (CONFIG_SYS_GD_ADDR & -16) & 0xffffffff
	movi	a1, (CONFIG_SYS_SP_TOP & -16) & 0xffffffff

	movi	a4, xtensa_board_init

#ifdef __XTENSA_CALL0_ABI__
	mov	a2, a6
	mov	a3, a1
	callx0	a4
#else
	mov	a7, a1
	addi    a1, a1, -16
	callx4	a4
#endif
	/* Never Returns */
	ill

/*
 * void relocate_code (addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 * a0 = addr_sp
 * a1 = gd
 * a2 = destination address
 */
	.text
	.globl relocate_code
	.align 4
relocate_code:
	entry	a1, 16
	retw

#if XCHAL_HAVE_EXCEPTIONS

/*
 * Exception vectors.
 *
 *  Various notes:
 *   - We currently don't use the user exception vector (PS.UM is always 0),
 *     but do define such a vector, just in case. They both jump to the
 *     same exception handler, though.
 *   - We currently only save the bare minimum number of registers:
 *     a0...a15, sar, loop-registers, exception register (epc1, excvaddr,
 *     exccause, depc)
 *   - WINDOWSTART is only saved to identify if registers have been spilled
 *     to the wrong stack (exception stack) while executing the exception
 *     handler.
 */

	.section .KernelExceptionVector.text, "ax"
	.global _KernelExceptionVector
_KernelExceptionVector:

	wsr	a2, EXCSAVE1
	movi	a2, ExceptionHandler
	jx	a2

	.section .UserExceptionVector.text, "ax"
	.global _UserExceptionVector
_UserExceptionVector:

	wsr	a2, EXCSAVE1
	movi	a2, ExceptionHandler
	jx	a2

#if !XCHAL_HAVE_XEA1
	.section .DoubleExceptionVector.text, "ax"
	.global _DoubleExceptionVector
_DoubleExceptionVector:

#ifdef __XTENSA_CALL0_ABI__
	wsr	a0, EXCSAVE1
	movi    a0, hang                # report and ask user to reset board
	callx0	a0
#else
	wsr	a4, EXCSAVE1
	movi    a4, hang                # report and ask user to reset board
	callx4	a4
#endif
#endif
	/* Does not return here. */


	.text
	.align 4
ExceptionHandler:

	rsr	a2, EXCCAUSE		# find handler

	/* Special case for alloca handler. */

	bnei	a2, 5, 1f		# jump if not alloca exception

	addi	a1, a1, -16 - 4		# create a small stack frame
	s32i	a3, a1, 0		# and save a3 (a2 still in excsave1)
	movi	a2, fast_alloca_exception
	jx	a2			# jump to fast_alloca_exception

	/* All other exceptions go here: */

	/* Create ptrace stack and save a0...a3 */

1:	addi	a2, a1, - PT_SIZE - 16
	s32i	a0, a2, PT_AREG + 0 * 4
	s32i	a1, a2, PT_AREG + 1 * 4
	s32i	a3, a2, PT_AREG + 3 * 4
	rsr	a3, EXCSAVE1
	s32i	a3, a2, PT_AREG + 2 * 4
	mov	a1, a2

	/* Save remaining AR registers. */

	s32i	a4, a1, PT_AREG + 4 * 4
	s32i	a5, a1, PT_AREG + 5 * 4
	s32i	a6, a1, PT_AREG + 6 * 4
	s32i	a7, a1, PT_AREG + 7 * 4
	s32i	a8, a1, PT_AREG + 8 * 4
	s32i	a9, a1, PT_AREG + 9 * 4
	s32i	a10, a1, PT_AREG + 10 * 4
	s32i	a11, a1, PT_AREG + 11 * 4
	s32i	a12, a1, PT_AREG + 12 * 4
	s32i	a13, a1, PT_AREG + 13 * 4
	s32i	a14, a1, PT_AREG + 14 * 4
	s32i	a15, a1, PT_AREG + 15 * 4

	/* Save SRs */

	rsr	a2, WINDOWSTART
	s32i	a2, a1, PT_WINDOWSTART

	rsr	a2, SAR
	rsr	a3, EPC1
	s32i	a2, a1, PT_SAR
	s32i	a3, a1, PT_PC

#if XCHAL_HAVE_LOOPS
	movi	a2, 0
	rsr	a3, LBEG
	xsr	a2, LCOUNT
	s32i	a3, a1, PT_LBEG
	rsr	a3, LEND
	s32i	a2, a1, PT_LCOUNT
	s32i	a3, a1, PT_LEND
#endif

	/* Set up C environment and call registered handler. */
	/* Setup stack, PS.WOE = 1, PS.EXCM = 0, PS.INTLEVEL = EXCM level. */

	rsr	a2, EXCCAUSE
#if XCHAL_HAVE_XEA1
	movi	a3, (1<<PS_WOE_BIT) | 1
#elif __XTENSA_CALL0_ABI__
	movi	a3, XCHAL_EXCM_LEVEL
#else
	movi	a3, (1<<PS_WOE_BIT) | XCHAL_EXCM_LEVEL
#endif
	xsr	a3, PS
	rsync
	s32i	a2, a1, PT_EXCCAUSE
	s32i	a3, a1, PT_PS

	movi	a0, exc_table
	addx4	a0, a2, a0
	l32i	a0, a0, 0
#ifdef __XTENSA_CALL0_ABI__
	mov	a2, a1			# Provide stack frame as only argument
	callx0	a0
	l32i	a3, a1, PT_PS
#else
	mov	a6, a1			# Provide stack frame as only argument
	callx4	a0
#endif

	/* Restore PS and go to exception mode (PS.EXCM=1) */

	wsr	a3, PS

	/* Restore SR registers */

#if XCHAL_HAVE_LOOPS
	l32i	a2, a1, PT_LBEG
	l32i	a3, a1, PT_LEND
	l32i	a4, a1, PT_LCOUNT
	wsr	a2, LBEG
	wsr	a3, LEND
	wsr	a4, LCOUNT
#endif

	l32i	a2, a1, PT_SAR
	l32i	a3, a1, PT_PC
	wsr	a2, SAR
	wsr	a3, EPC1

	/* Do we need to simulate a MOVSP? */

	l32i	a2, a1, PT_WINDOWSTART
	addi	a3, a2, -1
	and	a2, a2, a3
	beqz	a2, 1f			# Skip if regs were spilled before exc.

	rsr	a2, WINDOWSTART
	addi	a3, a2, -1
	and	a2, a2, a3
	bnez	a2, 1f			# Skip if registers aren't spilled now

	addi	a2, a1, -16
	l32i	a4, a2, 0
	l32i	a5, a2, 4
	s32i	a4, a1, PT_SIZE + 0
	s32i	a5, a1, PT_SIZE + 4
	l32i	a4, a2, 8
	l32i	a5, a2, 12
	s32i	a4, a1, PT_SIZE + 8
	s32i	a5, a1, PT_SIZE + 12

	/* Restore address register. */

1:	l32i	a15, a1, PT_AREG + 15 * 4
	l32i	a14, a1, PT_AREG + 14 * 4
	l32i	a13, a1, PT_AREG + 13 * 4
	l32i	a12, a1, PT_AREG + 12 * 4
	l32i	a11, a1, PT_AREG + 11 * 4
	l32i	a10, a1, PT_AREG + 10 * 4
	l32i	a9, a1, PT_AREG + 9 * 4
	l32i	a8, a1, PT_AREG + 8 * 4
	l32i	a7, a1, PT_AREG + 7 * 4
	l32i	a6, a1, PT_AREG + 6 * 4
	l32i	a5, a1, PT_AREG + 5 * 4
	l32i	a4, a1, PT_AREG + 4 * 4
	l32i	a3, a1, PT_AREG + 3 * 4
	l32i	a2, a1, PT_AREG + 2 * 4
	l32i	a0, a1, PT_AREG + 0 * 4

	l32i	a1, a1, PT_AREG + 1 * 4 # Remove ptrace stack frame

	rfe


/*
 * Dummy memory exception handler to avoid crash/hang on load/store of
 * an invalid address (such as might be requested by user "md" command).
 * U-Boot currently does not provide a hook to prevent accessing invalid
 * addresses nor to inform the user, so we have to try to live with it.
 * Simply skip the offending instruction (don't care what load returns).
 * This is ugly (and possibly dangerous in Xtensa FLIX configs), but if we
 * get here we're in trouble anyway, so might as well *try* to recover.
 *	void xtensa_mem_exc_dummy(struct pt_regs*);
 */
        /* Table of instruction sizes based on op0 field. */
	.section .rodata, "a"
	.type	op0_format_lengths, @object
	.align	4
op0_format_lengths:
	.byte	XCHAL_OP0_FORMAT_LENGTHS

	.text
	.global xtensa_mem_exc_dummy
	.type	xtensa_mem_exc_dummy, @function
	.align	4
xtensa_mem_exc_dummy:

#ifndef __XTENSA_CALL0_ABI__
        entry   a1, 16
#endif

	/* Decode the size of the instruction that caused the exception. */
	l32i    a3, a2, PT_PC           /* a3 = PC of exception */
	l8ui	a4, a3, 0		/* a4 = first byte of insn */
	movi	a5, op0_format_lengths	/* a5 = table of instruction sizes */
	#if XCHAL_HAVE_BE
	extui	a4, a4, 4, 4		/* a4 = op0 = big end nibble */
	#else
	extui	a4, a4, 0, 4		/* a4 = op0 = little end nibble */
	#endif
	add	a5, a5, a4		/* index table with op0 */
	l8ui	a4, a5, 0		/* a4 = instruction size */

	/* Increment the PC past the instruction that caused the exception. */
	add     a3, a3, a4              /* PC += size of insn   */
	#if XCHAL_HAVE_LOOPS
	l32i    a4, a2, PT_LEND         /* if (PC == LEND       */
	bne     a3, a4, 1f
	l32i    a4, a2, PT_LCOUNT       /*     && LCOUNT != 0)  */
	beqz    a4, 1f                  /* {                    */
	addi    a4, a4, -1              /*   --LCOUNT           */
	l32i    a3, a2, PT_LBEG         /*   PC = LBEG          */
	s32i    a4, a2, PT_LCOUNT       /* }                    */
	#endif
1:	s32i    a3, a2, PT_PC           /* update PC            */

#ifdef __XTENSA_CALL0_ABI__
        ret
#else
        retw
#endif

#endif /* XCHAL_HAVE_EXCEPTIONS */

#if XCHAL_HAVE_WINDOWED

/*
 * Window overflow and underflow handlers.
 * The handlers must be 64 bytes apart, first starting with the underflow
 * handlers underflow-4 to underflow-12, then the overflow handlers
 * overflow-4 to overflow-12.
 *
 * Note: We rerun the underflow handlers if we hit an exception, so
 *	 we try to access any page that would cause a page fault early.
 */

	.section .WindowVectors.text, "ax"

/* 4-Register Window Overflow Vector (Handler) */

	.align 64
.global _WindowOverflow4
_WindowOverflow4:
	s32e	a0, a5, -16
	s32e	a1, a5, -12
	s32e	a2, a5,  -8
	s32e	a3, a5,  -4
	rfwo


/* 4-Register Window Underflow Vector (Handler) */

	.align 64
.global _WindowUnderflow4
_WindowUnderflow4:
	l32e	a0, a5, -16
	l32e	a1, a5, -12
	l32e	a2, a5,  -8
	l32e	a3, a5,  -4
	rfwu

/*
 * a0:	a0
 * a1:	new stack pointer = a1 - 16 - 4
 * a2:	available, saved in excsave1
 * a3:	available, saved on stack *a1
 */

/* 15*/	.byte	0xff

fast_alloca_exception:	/* must be at _WindowUnderflow4 + 16

/* 16*/	rsr	a2, PS
/* 19*/	rsr	a3, WINDOWBASE
/* 22*/	extui	a2, a2, PS_OWB_SHIFT, PS_OWB_SHIFT
/* 25*/	xor	a2, a2, a3
/* 28*/	rsr	a3, PS
/* 31*/	slli	a2, a2, PS_OWB_SHIFT
/* 34*/	xor	a2, a3, a2
/* 37*/	wsr	a2, PS

/* 40*/	_l32i	a3, a1, 0
/* 43*/	addi	a1, a1, 16 + 4
/* 46*/	rsr	a2, EXCSAVE1

/* 49*/	rotw	-1
/* 52*/	_bbci.l	a4, 31, _WindowUnderflow4	/* 0x: call4 */
/* 55*/	rotw	-1
/* 58*/	_bbci.l	a8, 30, _WindowUnderflow8	/* 10: call8 */
/* 61*/ _j	__WindowUnderflow12		/* 11: call12 */
/* 64*/

/* 8-Register Window Overflow Vector (Handler) */

	.align 64
.global _WindowOverflow8
_WindowOverflow8:
	s32e	a0, a9, -16
	l32e	a0, a1, -12
	s32e	a2, a9,  -8
	s32e	a1, a9, -12
	s32e	a3, a9,  -4
	s32e	a4, a0, -32
	s32e	a5, a0, -28
	s32e	a6, a0, -24
	s32e	a7, a0, -20
	rfwo

/* 8-Register Window Underflow Vector (Handler) */

	.align 64
.global _WindowUnderflow8
_WindowUnderflow8:
	l32e	a1, a9, -12
	l32e	a0, a9, -16
	l32e	a7, a1, -12
	l32e	a2, a9,  -8
	l32e	a4, a7, -32
	l32e	a3, a9,  -4
	l32e	a5, a7, -28
	l32e	a6, a7, -24
	l32e	a7, a7, -20
	rfwu

/* 12-Register Window Overflow Vector (Handler) */

	.align 64
.global _WindowOverflow12
_WindowOverflow12:
	s32e	a0,  a13, -16
	l32e	a0,  a1,  -12
	s32e	a1,  a13, -12
	s32e	a2,  a13,  -8
	s32e	a3,  a13,  -4
	s32e	a4,  a0,  -48
	s32e	a5,  a0,  -44
	s32e	a6,  a0,  -40
	s32e	a7,  a0,  -36
	s32e	a8,  a0,  -32
	s32e	a9,  a0,  -28
	s32e	a10, a0,  -24
	s32e	a11, a0,  -20
	rfwo

/* 12-Register Window Underflow Vector (Handler) */

	.org _WindowOverflow12 + 64 - 3
__WindowUnderflow12:
	rotw	-1
.global _WindowUnderflow12
_WindowUnderflow12:
	l32e	a1,  a13, -12
	l32e	a0,  a13, -16
	l32e	a11, a1,  -12
	l32e	a2,  a13,  -8
	l32e	a4,  a11, -48
	l32e	a8,  a11, -32
	l32e	a3,  a13,  -4
	l32e	a5,  a11, -44
	l32e	a6,  a11, -40
	l32e	a7,  a11, -36
	l32e	a9,  a11, -28
	l32e	a10, a11, -24
	l32e	a11, a11, -20
	rfwu

#endif /* XCHAL_HAVE_WINDOWED */
