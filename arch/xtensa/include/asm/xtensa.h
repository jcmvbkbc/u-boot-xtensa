/*
 * Copyright (C) 2007 Tensilica, Inc.
 * Copyright (C) 2014 - 2016 Cadence Design Systems Inc.
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

#ifdef CONFIG_SYS_ASCDISP
/*
 * Print a formatted string to the board's ASCII character display.
 * String may have embedded newlines. Starts at top left and wraps long lines.
 */
void display_printf(const char *fmt, ...);
#else
static inline void display_printf(const char *fmt, ...)
{
}
#endif

#endif /* _XTENSA_H_ */
