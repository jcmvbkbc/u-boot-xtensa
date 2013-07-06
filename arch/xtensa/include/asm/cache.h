/*
 * Copyright (C) 2009 Tensilica Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef _XTENSA_CACHE_H
#define _XTENSA_CACHE_H

#include <asm/arch/core.h>

#define ARCH_DMA_MINALIGN	XCHAL_DCACHE_LINESIZE

#ifndef __ASSEMBLY__

void __flush_invalidate_dcache_range(unsigned long addr, unsigned long size);
void __invalidate_icache_range(unsigned long addr, unsigned long size);

#endif

#endif	/* _XTENSA_CACHE_H */
