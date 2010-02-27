SOURCES		=	src/main.cpp \
			src/Font\ editor.cpp

LUA_CFLAGS	:=	$(shell pkg-config --cflags lua5.1)

SDL_LDFLAGS	:=	$(shell sdl-config --libs)
LUA_LDFLAGS	:=	$(shell pkg-config --libs lua5.1)

CFLAGS		:= 	${CFLAGS} $(LUA_CFLAGS) -iquoteinc -iquote../../Engine/inc
LDFLAGS		:=	${LDFLAGS} ${SDL_LDFLAGS} ${LUA_LDFLAGS} -lGLU -lSDL_mixer -lIL ../../Engine/lib/libDeku2d.a

TARGET		=	bin/Font\ editor

all:
	g++ -fpermissive -w -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}

debug:
	g++ -g3 -fpermissive -w -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}

strict:
	g++ -Wall -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}

distclean:
	rm -f ${TARGET}
	rm -f bin/Logs/*
