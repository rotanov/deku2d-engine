# #####################################################################
# Automatically generated by qmake (2.01a) ?? ???? 9 20:15:22 2011
# #####################################################################
TEMPLATE = app
TARGET = 
DESTDIR = bin
DEPENDPATH += . \
    inc \
    src \
    ui
INCLUDEPATH += . \
    inc \
    ../Engine/inc
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp
unix { 
    LIBS += $$system(sdl-config --libs)
    LIBS += $$system(pkg-config --libs lua5.1)
}
LIBS += -lGLU \
    -lGLEW \
    -lSDL_mixer \
    -lIL \
    -lqscintilla2 \
    -L../Engine/lib \
    -lDeku2D
QMAKE_CXXFLAGS += $$system(pkg-config --cflags lua5.1)

# Input
HEADERS += inc/cmainwindow.h \
    inc/cluasyntaxhighlighter.h \
    inc/CCodeEditorWidget.h \
    inc/CEngineLuaConsole.h \
    inc/CLogViewer.h
FORMS += ui/cmainwindow.ui
SOURCES += src/cmainwindow.cpp \
    src/main.cpp \
    src/cluasyntaxhighlighter.cpp \
    src/CCodeEditorWidget.cpp \
    src/CEngineLuaConsole.cpp \
    src/CLogViewer.cpp
RESOURCES += ui/resources.qrc
