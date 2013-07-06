#
# (C) Copyright 2007 - 2013 Tensilica, Inc.
#
# SPDX-License-Identifier:	GPL-2.0+
#

# The linker script is pre-processed by CPP to adapt to the Xtensa core.
LDSCRIPT	:= $(srctree)/$(CPUDIR)/u-boot.lds.S
LDPPFLAGS	:= -I$(srctree)/include
