SOURCES		=	src/2de_Core.cpp \
			src/2de_Engine.cpp \
			src/2de_GameUtils.cpp \
			src/2de_GraphicsHigh.cpp \
			src/2de_GraphicsLow.cpp \
			src/2de_Gui.cpp \
			src/2de_ImageUtils.cpp \
			src/2de_LuaUtils.cpp \
			src/2de_MathUtils.cpp \
			src/2de_Physics.cpp \
			src/2de_Resource.cpp \
			src/2de_Sound.cpp \
			src/2de_Xml.cpp

OBJFILES	=	obj/2de_Core.o \
			obj/2de_Engine.o \
			obj/2de_GameUtils.o \
			obj/2de_GraphicsHigh.o \
			obj/2de_GraphicsLow.o \
			obj/2de_Gui.o \
			obj/2de_ImageUtils.o \
			obj/2de_LuaUtils.o \
			obj/2de_MathUtils.o \
			obj/2de_Physics.o \
			obj/2de_Resource.o \
			obj/2de_Sound.o \
			obj/2de_Xml.o

INCPATH		=	-iquoteinc

LIBS		=	-lGLU -lz -lSDL_mixer

SDL_CFLAGS	:=	$(shell sdl-config --cflags)
LUA_CFLAGS	:=	$(shell pkg-config --cflags lua5.1)
PNG_CFLAGS	:=	$(shell pkg-config --cflags libpng)

SDL_LDFLAGS	:=	$(shell sdl-config --libs)
LUA_LDFLAGS	:=	$(shell pkg-config --libs lua5.1)
PNG_LDFLAGS	:=	$(shell pkg-config --libs libpng)

CFLAGS		:= 	${CFLAGS} ${SDL_CFLAGS} ${LUA_CFLAGS} ${PNG_CFLAGS} ${INCPATH}
LDFLAGS		:=	${LDFLAGS} ${SDL_LDFLAGS} ${LUA_LDFLAGS} ${PNG_LDFLAGS} ${LIBS}

obj/%.o: src/%.cpp
	g++ $(DEBUGFLAG) $(STRICTFLAG) -c -o $@ $(CFLAGS) $<

lib/Deku2d.a: lib/Deku2d.a($(OBJFILES))

STRICTFLAG = -w

all: lib/Deku2d.a

shared: $(OBJFILES)
	g++ $(STRICTFLAG) -shared -fPIC -o lib/Deku2d.so $(OBJFILES)

debug: DEBUGFLAG = -g3 -D_DEBUG
debug: lib/Deku2d.a

strict: STRICTFLAG = -Wall
strict: lib/Deku2d.a

docs: doc/Doxyfile
	if [ -e /usr/bin/doxygen ]; then \
		doxygen doc/Doxyfile; \
	fi

clean:
	rm -f obj/*.o

dist-clean: clean
	rm -f lib/Deku2d.a
	rm -f lib/Deku2d.so
	rm -rf doc/html/

.PHONY: clean dist-clean docs
