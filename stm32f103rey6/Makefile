MODULE =cpu
#include $(MAKEBASE)/Makefile.base

DIRS =
ifneq (,$(findstring cpu_drivers,$(USEMODULE)))
    DIRS += drivers
endif

DIRS += $(RIOTCPU)/STM32F10x_StdPeriph_Lib_V3.5.0
#
all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;
#
include $(MAKEBASE)/Makefile.base
#
#clean:
#	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
