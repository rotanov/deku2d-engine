SOURCES		=	src/src/2de_Core.cpp \
			src/src/2de_Engine.cpp \
			src/src/2de_GameUtils.cpp \
			src/src/2de_GraphicsHigh.cpp \
			src/src/2de_GraphicsLow.cpp \
			src/src/2de_Gui.cpp \
			src/src/2de_ImageUtils.cpp \
			src/src/2de_LuaUtils.cpp \
			src/src/2de_MathUtils.cpp \
			src/src/2de_Physics.cpp \
			src/src/2de_Resource.cpp \
			src/src/2de_Xml.cpp \
			src/Tanks/MainTanks.cpp \
			src/Tanks/Tanks.cpp

INCPATH		=	-iquotesrc/inc

LIBS		=	-lGLU -lGL -lz

SDL_CFLAGS	:=	$(shell sdl-config --cflags)
LUA_CFLAGS	:=	$(shell pkg-config --cflags lua5.1)
PNG_CFLAGS	:=	$(shell pkg-config --cflags libpng)

SDL_LDFLAGS	:=	$(shell sdl-config --libs)
LUA_LDFLAGS	:=	$(shell pkg-config --libs lua5.1)
PNG_LDFLAGS	:=	$(shell pkg-config --libs libpng)

CFLAGS		:= 	${CFLAGS} ${SDL_CFLAGS} ${LUA_CFLAGS} ${PNG_CFLAGS} ${INCPATH}
LDFLAGS		:=	${LDFLAGS} ${SDL_LDFLAGS} ${LUA_LDFLAGS} ${PNG_LDFLAGS} ${LIBS}

TARGET		=	bin/tanks

all:
	g++ -w -o ${TARGET} ${CFLAGS} ${SOURCES} ${LDFLAGS}
maketest:
	echo ${CFLAGS}
