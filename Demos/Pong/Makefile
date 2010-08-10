SOURCES		=	src/main.cpp \
			src/Pong.cpp

SDL_LDFLAGS	:=	$(shell sdl-config --libs)
LUA_LDFLAGS	:=	$(shell pkg-config --libs lua5.1)

CFLAGS		:= 	${CFLAGS} ${SDL_CFLAGS} ${LUA_CFLAGS} -iquoteinc -iquote../../Engine/inc
LDFLAGS		:=	${LDFLAGS} ${SDL_LDFLAGS} ${LUA_LDFLAGS} -lGLU -lSDL_mixer -lIL ../../Engine/lib/libDeku2d.a

TARGET		=	bin/Pong

all:
	g++ -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}

debug:
	g++ -g3 -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}

strict:
	g++ -Wall -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}

distclean:
	rm -f ${TARGET}
	rm -f bin/Logs/*
	rm -f bin/Config/Resources.xml 
