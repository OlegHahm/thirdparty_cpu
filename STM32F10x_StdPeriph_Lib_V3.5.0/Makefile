DIRS = Libraries/STM32F10x_StdPeriph_Driver

.PHONY: StdDriver
.PHONY: $(DIRS)

StdDriver: $(DIRS)

$(DIRS): 
	$(MAKE) -C $@

clean:
	$(MAKE) -C Libraries/STM32F10x_StdPeriph_Driver clean
