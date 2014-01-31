MKBASE = ../$(BINDIR)
MODULE = stm32f10xstdperi

SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=$(BINDIR)%.o)
DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES = -I../inc
INCLUDES += -I$(RIOTCPU)/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/Include
INCLUDES += -I$(RIOTCPU)/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
INCLUDES += -I$(RIOTCPU)/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc
INCLUDES += -I$(RIOTCPU)/$(CPU)/include

$(BINDIR)$(MODULE).a: $(OBJ) $(ASMOBJ)
	$(AR) -rc $(BINDIR)$(MODULE).a $(OBJ) $(ASMOBJ)

# pull in dependency info for *existing* .o files
-include $(OBJ:.o=.d)

# compile and generate dependency info
$(BINDIR)%.o: %.c
	@$(CC) $(CFLAGS) $(PROJECTINCLUDE) $(BOARDINCLUDE) $(INCLUDES) -c $*.c -o $(BINDIR)$*.o
	@$(CC) $(CFLAGS) $(PROJECTINCLUDE) $(BOARDINCLUDE) $(INCLUDES) -MM $*.c > $(BINDIR)$*.d
	@printf "$(BINDIR)"|cat - $(BINDIR)$*.d > /tmp/fw_out && mv /tmp/fw_out $(BINDIR)$*.d

$(BINDIR)%.o: %.s
	@$(AS) $(ASFLAGS) $*.s -o $(BINDIR)$*.o

# remove compilation products
clean::
	rm -f $(BINDIR)$(MODULE).a $(OBJ) $(DEP) $(ASMOBJ)
