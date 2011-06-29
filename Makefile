all: engine demos

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

tags:
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .

.PHONY: clean distclean docs tags
