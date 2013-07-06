/*
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __ASM_XTENSA_SECTIONS_H
#define __ASM_XTENSA_SECTIONS_H

#include <asm-generic/sections.h>

extern void *_text_start;
#define CONFIG_SYS_TEXT_BASE ((unsigned int)&_text_start)

#endif
