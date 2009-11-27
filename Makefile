all:
	$(MAKE) -C Engine/ $(MAKETARGET)
	$(MAKE) -C Demos/Sandbox/ $(MAKETARGET)
	$(MAKE) -C Demos/Tanks/ $(MAKETARGET)
	$(MAKE) -C "Demos/Font editor/" $(MAKETARGET)
	$(MAKE) -C "Demos/Level editor/" $(MAKETARGET)
	$(MAKE) -C Demos/Pong/ $(MAKETARGET)

debug: MAKETARGET = debug
debug: all

strict: MAKETARGET = strict
strict: all

clean:
	make -C Engine/ clean

dist-clean: MAKETARGET = dist-clean
dist-clean: all
