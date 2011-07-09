SOURCES		=	src/MainTanks.cpp \
			src/Tanks.cpp

HEADERS		=	inc/Tanks.h

TARGET		=	bin/Tanks
ENGINE_LIB	=	../../Engine/lib/libDeku2D.a

LUA_CFLAGS	:=	$(shell pkg-config --cflags lua5.1)

SDL_LDFLAGS	:=	$(shell sdl-config --libs)
LUA_LDFLAGS	:=	$(shell pkg-config --libs lua5.1)

CFLAGS		:= 	${CFLAGS} ${SDL_CFLAGS} ${LUA_CFLAGS} -Iinc -I../../Engine/inc
LDFLAGS		:=	${LDFLAGS} ${SDL_LDFLAGS} ${LUA_LDFLAGS} -lGLU -lGLEW -lSDL_mixer -lIL ${ENGINE_LIB}

all: ${TARGET}

debug: MAKETARGET = debug
debug: CFLAGS := -g3 -D_DEBUG ${CFLAGS}
debug: all

strict: MAKETARGET = strict
strict: CFLAGS := -Wall ${CFLAGS}
strict: all

distclean:
	rm -f ${TARGET}
	rm -f bin/Logs/*
	rm -f bin/Config/Resources.xml

${TARGET}: ${ENGINE_LIB} ${SOURCES} ${HEADERS}
	g++ -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}

${ENGINE_LIB}:
	$(MAKE) -C ../../Engine/ $(MAKETARGET)
