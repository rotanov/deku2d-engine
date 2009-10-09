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
			src/Tanks/Tanks.cpp \

INCPATH		=	-I/usr/include -I/usr/include/SDL -I/usr/include/libpng -I/usr/include/lua5.1 -Isrc/inc

TARGET		=	bin/tanks

LIBS		=	-lpng -lz -lGLU -lSDL -lSDLmain -llua5.1

all:
	g++ -w -o ${TARGET} ${INCPATH} ${SOURCES} ${LIBS}
