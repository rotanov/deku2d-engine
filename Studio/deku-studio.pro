!include(../Engine/SharedSettings.pri) {
	error("Can not include shared settings.")

}

CONFIG += qt

DEPENDPATH += . \
	inc \
	src \
	ui

INCLUDEPATH += . \
	inc

unix {
	LIBS += -lqscintilla2

}

win32 {
	CONFIG( debug, debug|release ) {
		LIBS += -lqscintilla2d

	} else {
		LIBS += -lqscintilla2

	}

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
