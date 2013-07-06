/*
 * (C) Copyright 2008, Tensilica Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 ********************************************************************
 * NOTE: This header file defines an interface to U-Boot. Including
 * this (unmodified) header file in another file is considered normal
 * use of U-Boot, and does *not* fall under the heading of "derived
 * work".
 ********************************************************************
 */

#ifndef _XTENSA_MISC_H
#define _XTENSA_MISC_H

/* Defined in cpu/xtensa/u-boot.lds.S */
void *__memory_avail_start;
void *__memory_avail_end;

/* Used in cpu/xtensa/cpu.c */
void board_reset(void);

#endif	/* _XTENSA_MISC_H */
