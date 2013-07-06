/*
 * Copyright (C) 2008-2013 Tensilica Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _XTENSA_ADDRSPACE_H
#define _XTENSA_ADDRSPACE_H

#include <asm/arch/core.h>

/*
 * MMU Memory Map
 *
 * V2 MMU:
 *   IO (uncached)	f0000000..ffffffff	-> f000000
 *   IO (cached)	e0000000..efffffff	-> f000000
 *   MEM (uncached)	d8000000..dfffffff	-> 0000000
 *   MEM (cached)	d0000000..d7ffffff	-> 0000000
 *
 * V3 MMU:
 *   IO (uncached)	f0000000..ffffffff	-> f000000
 *   MEM (uncached)	00000000..0fffffff	-> 0000000
 *
 */

/* Region typically starting at 0xf0000000 */
#define CONFIG_SYS_IO_BASE	0xf0000000

/*
 * Region typically starting at
 *  0xD0000000 for V2 MMU
 * and
 *  0x00000000 for V3 MMU
 */
#define CONFIG_SYS_MEMORY_BASE \
	(XCHAL_VECBASE_RESET_VADDR - XCHAL_VECBASE_RESET_PADDR)

#define IOADDR(x)		(CONFIG_SYS_IO_BASE + (x))
#define MEMADDR(x)		(CONFIG_SYS_MEMORY_BASE + (x))
#define PHYSADDR(x)		((x) - (CONFIG_SYS_MEMORY_BASE))

#endif	/* _XTENSA_ADDRSPACE_H */
