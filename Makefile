all:
	$(MAKE) -C Engine/ $(MAKETARGET)
	SAVEIFS=$$IFS		# IFS hack to make for-loop work with filenames with spaces
	IFS=$$(echo -en "\n\b")
	for i in Demos/*; do \
		$(MAKE) -C "$$i" $(MAKETARGET) ; \
	done
	IFS=$$SAVEIFS

debug: MAKETARGET = debug
debug: all

strict: MAKETARGET = strict
strict: all

docs:
	$(MAKE) -C Engine/ docs

clean:
	$(MAKE) -C Engine/ clean

distclean: MAKETARGET = distclean
distclean: all

.PHONY: clean distclean docs
