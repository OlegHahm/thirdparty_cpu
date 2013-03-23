MODULE =cpu

INCLUDES = -Iinclude -I../cortex-m/CMSIS/include -I../cortex-m/CMSIS/Device/ST/STM32F4xx/include -I../cortex-m/STM32F4xx_StdPeriph_Driver/include
INCLUDES += -I../../core/include -I../../sys/include -I../../sys/lib -I../../hal/include -I../../.. 

DIRS =
ifneq (,$(findstring cpu_drivers,$(USEMODULE)))
    DIRS += drivers
endif

all: $(BINDIR)$(MODULE).a
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i ; done ;

include $(MAKEBASE)/Makefile.base

clean::
	@for i in $(DIRS) ; do "$(MAKE)" -C $$i clean ; done ;

