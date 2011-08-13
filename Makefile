all: engine demos studio

debug: MAKETARGET = debug
debug: all

strict: MAKETARGET = strict
strict: all

docs: MAKETARGET = docs
docs: engine

clean: MAKETARGET = clean
clean: engine

distclean: MAKETARGET = distclean
distclean: all

engine:
	@$(MAKE) -C Engine/ $(MAKETARGET)

demos:
	@$(MAKE) -C Demos/ $(MAKETARGET)

studio: Studio/Makefile
	@$(MAKE) -C Studio/ $(MAKETARGET)

tags:
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .

Studio/Makefile:
	cd Studio && qmake deku-studio.pro

.PHONY: clean distclean docs tags
