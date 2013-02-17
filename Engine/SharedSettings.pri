DESTDIR = bin
TEMPLATE = app
CONFIG -= qt

OBJECTS_DIR = tmp/obj
MOC_DIR = tmp/moc
UI_DIR = tmp/ui
RCC_DIR = tmp/rcc

INCLUDEPATH += $$PWD/inc
LIBS += -L$$PWD/lib

win32 {
	#environment variable LIB should be used for these to be found
	LIBS +=	-lOpenGl32 \
			-lGlu32 \
			-lSdl \
			-lSDLMain \
			-lLua5.1 \
			-lSDL_mixer \
			-lDevIL \
			-lGLEW32s \
			-lUser32

	DEFINES -= UNICODE
}

unix {
	LIBS += $$system(sdl-config --libs)
	LIBS += $$system(pkg-config --libs lua5.1)
	LIBS += -lGLU \
		-lGLEW \
		-lSDL_mixer \
		-lIL
	QMAKE_CXXFLAGS += $$system(pkg-config --cflags lua5.1)
	CONFIG -= warn_on
}

CONFIG(debug, debug|release) {
	DEFINES += \
		_DEBUG

	LIBS += -lDeku2d.debug

	win32 {
		CONFIG += console

		DEFINES +=	\
			WIN32 \
			_WINDOWS \

		QMAKE_CXXFLAGS_DEBUG -= /Zc:wchar_t

		QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:libci \
			/NODEFAULTLIB:libcmt
	}

} else {
	LIBS += -lDeku2d.release

	win32 {
		CONFIG += windows

		DEFINES +=	\
			WIN32 \
			_WINDOWS \

		QMAKE_CXXFLAGS_RELEASE -= /Zc:wchar_t

		QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:libci \
			/NODEFAULTLIB:libcmt
	}
}
