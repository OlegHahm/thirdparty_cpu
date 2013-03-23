ifeq ($(CPU),stm32f407vgt6)
	DIRS = cortex-m stm32f407vgt6
endif

.PHONY: cpus
.PHONY: $(DIRS)

cpus: $(DIRS)

$(DIRS): 
	@$(MAKE) -C $@

clean:
	@for i in $(DIRS) ; do $(MAKE) -C $$i clean ; done ;
