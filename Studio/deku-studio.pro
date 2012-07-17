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
    LIBS += -lGLU \
        -lGLEW \
        -lSDL_mixer \
        -lIL \
        -lqscintilla2 \
        -L../Engine/lib \
        -lDeku2D
    QMAKE_CXXFLAGS += $$system(pkg-config --cflags lua5.1)
}

#message( $$INCLUDE )

win32 {
    # INCLUDEPATH += D:\Devel\inc
    # INCLUDEPATH += D:\Devel\Lua\5.1\include
    # LIBS += -LD:\Devel\lib
    # LIBS += -L"C:\Program Files\Microsoft SDKs\Windows\v7.0A\Lib"
    # LIBS += -lGLU32 -lSDL_mixer -lDevIL -L../Engine/lib -lDeku2d.Release -lSDL -lSDLmain -llua5.1 -lopengl32 -lGLEW32s -lUser32
    LIBS += -lOpenGl32 \
	-lGlu32 \
	-lSdl \
	-lSDLMain \
	-lLua5.1 \
	#-lDeku2d.Release \
	-lDeku2d \
	-lSDL_mixer \
	-lDevIL \
	-lGLEW32s \
	-lUser32 \
	-lqscintilla2 \

DESTDIR = ../Studio/bin
}

# Input
HEADERS += inc/cmainwindow.h \
    inc/CCodeEditorWidget.h \
    inc/CEngineLuaConsole.h \
    inc/CLogViewer.h \
    inc/CEngineViewport.h \
    inc/CComponentTreeWidget.h \
    inc/CPropertyEditWidget.h \
    inc/Editors/CAbstractEditor.h \
    inc/CEditorTabWidget.h \
    inc/Editors/CCodeEditor.h \
    inc/CEditorRegistry.h \
    inc/CEditorWidget.h \
    inc/Dialogs/CFileSaveDialog.h \
    inc/Dialogs/CSelectEditorDialog.h
FORMS += ui/cmainwindow.ui \
    ui/CSelectEditorDialog.ui
SOURCES += src/cmainwindow.cpp \
    src/main.cpp \
    src/CCodeEditorWidget.cpp \
    src/CEngineLuaConsole.cpp \
    src/CLogViewer.cpp \
    src/CEngineViewport.cpp \
    src/CComponentTreeWidget.cpp \
    src/CPropertyEditWidget.cpp \
    src/Editors/CAbstractEditor.cpp \
    src/CEditorTabWidget.cpp \
    src/Editors/CCodeEditor.cpp \
    src/CEditorRegistry.cpp \
    src/CEditorWidget.cpp \
    src/Dialogs/CFileSaveDialog.cpp \
    src/Dialogs/CSelectEditorDialog.cpp
RESOURCES += ui/resources.qrc
QMAKE_CLEAN += bin/Logs/*.log \
    bin/Config/Resources.xml
