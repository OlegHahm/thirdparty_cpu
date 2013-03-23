MKBASE = ../$(BINDIR)
MODULE = stm32f4xxstdperi

SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=$(BINDIR)%.o)
DEP = $(SRC:%.c=$(BINDIR)%.d)

INCLUDES = -I../inc
INCLUDES += -I$(RIOTCPU)/cortex-m/CMSIS/include -I$(RIOTCPU)/cortex-m/CMSIS/Device/ST/STM32F4xx/include -I$(RIOTCPU)/cortex-m/STM32F4xx_StdPeriph_Driver/include
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

