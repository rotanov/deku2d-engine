TEMPLATE = app
CONFIG += windows
CONFIG -= qt

win32 {
	DESTDIR = bin
}

OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp

QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:libci \
	/NODEFAULTLIB:libcmt

QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:libci \
	/NODEFAULTLIB:libcmt

LIBS += -lDeku2d

CONFIG( debug, debug|release ) {

	DEFINES +=	\
		_DEBUG \
		WIN32 \
		_WINDOWS \

} else {

	DEFINES +=	\
		WIN32 \
		_WINDOWS \
}

win32 {
	LIBS +=	-lOpenGl32 \
			-lGlu32 \
			-lSdl \
			-lSDLMain \
			-lLua5.1 \
			-lSDL_mixer \
			-lDevIL \
			-lGLEW32s \
			-lUser32
}

INCLUDEPATH += ../../Engine/inc

SOURCES += \
    src/main.cpp
