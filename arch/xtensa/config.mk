#
# (C) Copyright 2007 - 2014 Tensilica, Inc.
#
# SPDX-License-Identifier:	GPL-2.0+
#

CROSS_COMPILE ?= xtensa-linux-
PLATFORM_CPPFLAGS += -D__XTENSA__ -mlongcalls

CPUDIR = arch/$(ARCH)/cpu

__HAVE_ARCH_GENERIC_BOARD := y

