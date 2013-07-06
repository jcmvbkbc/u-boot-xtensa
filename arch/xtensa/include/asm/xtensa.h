/*
 * Copyright (C) 2007 Tensilica, Inc.
 *
 * A place for global definitions specific to Xtensa-based ports.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _XTENSA_H_
#define _XTENSA_H_

#include <stdarg.h>
#include <config.h>
#include <asm/u-boot.h>

/* Much of this is not specific to Xtensa and should move to a common header. */

/* Quote a macro value as a string constant. */
#define QUOTE1(s) #s
#define QUOTE(s) QUOTE1(s)

/* Align a value up to nearest n-byte boundary, where n is a power of 2. */
#define ALIGNUP(n, val) (((val) + (n)-1) & -(n))

#ifdef CONFIG_SYS_ASCDISP
/*
 * Print a formatted string to the board's ASCII character display.
 * String may have embedded newlines. Starts at top left and wraps long lines.
 */
void display_printf(const char *fmt, ...);
void lcd_disp_at_pos(char *, unsigned char);
#else
#define display_printf(fmt, args)
#endif

#endif /* _XTENSA_H_ */
