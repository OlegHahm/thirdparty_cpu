MODULE =cpu

INCLUDES = -Iinclude -I../cortex-m/CMSIS/include -I../cortex-m/CMSIS/Device/ST/STM32F4xx/include -I../cortex-m/STM32F4xx_StdPeriph_Driver/include
INCLUDES += -I$(RIOTBASE)/core/include -I$(RIOTBASE)/sys/include -I$(RIOTBASE)/sys/lib 

DIRS =
ifneq (,$(findstring cpu_drivers,$(USEMODULE)))
    DIRS += drivers
endif

DIRS += $(RIOTCPU)/cortex_common

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(RIOTBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;

