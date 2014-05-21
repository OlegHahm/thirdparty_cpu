MODULE =cpu
#include $(RIOTBASE)/Makefile.base

DIRS =
ifneq (,$(findstring cpu_drivers,$(USEMODULE)))
    DIRS += drivers
endif

DIRS += $(RIOTCPU)/STM32F10x_StdPeriph_Lib_V3.5.0
DIRS += $(RIOTCPU)/cortex_common
DIRS += $(RIOTCPU)/$(CPU)/periph

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;
#
include $(RIOTBASE)/Makefile.base
#
#clean:
#	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;
