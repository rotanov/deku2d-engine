SOURCES		=	MainTanks.cpp \
			Tanks.cpp \
			TanksNetwork.cpp

INCPATH		=	-iquote../inc

LIBS		=	-lGLU -lz ../../lib/Deku2d.a ../../lib/libtnl.a ../../lib/libtomcrypt.a

SDL_CFLAGS	:=	$(shell sdl-config --cflags)
LUA_CFLAGS	:=	$(shell pkg-config --cflags lua5.1)
PNG_CFLAGS	:=	$(shell pkg-config --cflags libpng)

SDL_LDFLAGS	:=	$(shell sdl-config --libs)
LUA_LDFLAGS	:=	$(shell pkg-config --libs lua5.1)
PNG_LDFLAGS	:=	$(shell pkg-config --libs libpng)

CFLAGS		:= 	${CFLAGS} ${SDL_CFLAGS} ${LUA_CFLAGS} ${PNG_CFLAGS} ${INCPATH}
LDFLAGS		:=	${LDFLAGS} ${SDL_LDFLAGS} ${LUA_LDFLAGS} ${PNG_LDFLAGS} ${LIBS}

TARGET		=	../../bin/Tanks

all:
	g++ -fpermissive -w -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}

debug:
	g++ -g3 -fpermissive -w -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}

strict:
	g++ -Wall -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}