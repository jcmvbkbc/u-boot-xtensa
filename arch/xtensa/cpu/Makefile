#
# (C) Copyright 2007 - 2013 Tensilica, Inc.
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-y = cpu.o exceptions.o
extra-y = start.o

$(obj).depend.%:	%.lds.S
	$(CC) -M $(CPPFLAGS) $(EXTRA_CPPFLAGS_DEP) \
	-MQ $(obj)$(basename $<) $< >$@

