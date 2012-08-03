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
	CONFIG += windows

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

# TODO: adapt this for unix
# TODO: put common stuff for win and unix outside
unix {
	LIBS += $$system(sdl-config --libs)
	LIBS += $$system(pkg-config --libs lua5.1)
	LIBS += -lGLU \
		-lGLEW \
		-lSDL_mixer \
		-lIL \
		-L../Engine/lib \
		-lDeku2D
	QMAKE_CXXFLAGS += $$system(pkg-config --cflags lua5.1)
}

CONFIG(debug, debug|release) {
	win32 {
		LIBS += -lDeku2d.debug

		DEFINES +=	\
			_DEBUG \
			WIN32 \
			_WINDOWS \

		QMAKE_CXXFLAGS_DEBUG -= /Zc:wchar_t

		QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:libci \
			/NODEFAULTLIB:libcmt

	}

} else {
	win32 {
		LIBS += -lDeku2d.release

		DEFINES +=	\
			WIN32 \
			_WINDOWS \

		QMAKE_CXXFLAGS_RELEASE -= /Zc:wchar_t

		QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:libci \
			/NODEFAULTLIB:libcmt

	}

}
