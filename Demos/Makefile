DISABLED_DEMOS	=	Font\ editor \
			Level\ editor \
			Pac\ man \
			Tanks \
			Tileset\ editor

DEMOS		=	Benchmark \
			Color\ lines \
			Demo\ template \
			LuaConsole \
			Ninja\ story \
			Pong \
			Sandbox \
			SoundCheck

all: $(DEMOS)

debug: MAKETARGET = debug
debug: all

strict: MAKETARGET = strict
strict: all

distclean: MAKETARGET = distclean
distclean: all

$(DEMOS):
	-@$(MAKE) -C "$@" $(MAKETARGET)

$(DISABLED_DEMOS):
	@echo "Demo $@ is disabled"


.PHONY: $(DEMOS) $(DISABLED_DEMOS)
