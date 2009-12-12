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
			src/2de_Xml.cpp

INCPATH		=	-iquoteinc

LIBS		=	-lGLU -lz

SDL_CFLAGS	:=	$(shell sdl-config --cflags)
LUA_CFLAGS	:=	$(shell pkg-config --cflags lua5.1)
PNG_CFLAGS	:=	$(shell pkg-config --cflags libpng)

SDL_LDFLAGS	:=	$(shell sdl-config --libs)
LUA_LDFLAGS	:=	$(shell pkg-config --libs lua5.1)
PNG_LDFLAGS	:=	$(shell pkg-config --libs libpng)

CFLAGS		:= 	${CFLAGS} ${SDL_CFLAGS} ${LUA_CFLAGS} ${PNG_CFLAGS} ${INCPATH}
LDFLAGS		:=	${LDFLAGS} ${SDL_LDFLAGS} ${LUA_LDFLAGS} ${PNG_LDFLAGS} ${LIBS}

TARGET		=	lib/Deku2d.a

all: release-compile makelib

debug: debug-compile makelib

strict: strict-compile makelib

release-compile:
	g++ -w -c ${CFLAGS} ${SOURCES} ${LDFLAGS}

debug-compile:
	g++ -g3 -w -c ${CFLAGS} ${SOURCES} ${LDFLAGS}

strict-compile:
	g++ -Wall -c ${CFLAGS} ${SOURCES} ${LDFLAGS}

makelib:
	ar cr ${TARGET} *.o
	mv *.o obj/

docs: doc/Doxyfile
	if [ -e /usr/bin/doxygen ]; then \
		doxygen doc/Doxyfile; \
	fi

clean:
	rm -f obj/*.o

dist-clean: clean
	rm -f ${TARGET}
	rm -rf doc/html/

