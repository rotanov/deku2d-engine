TEMPLATE = lib

win32 {
	TARGET =
	DESTDIR = ./Engine/lib
}

CONFIG += staticlib

OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp

win32 {
	QMAKE_CXXFLAGS_RELEASE -= /Zc:wchar_t
	QMAKE_CXXFLAGS_DEBUG -= /Zc:wchar_t
	QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:libci \
		/NODEFAULTLIB:libcmt
	QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:libci \
		/NODEFAULTLIB:libcmt
	DEFINES -= UNICODE
}

INCLUDEPATH += \	# is it essential on unix platform?
	./Engine/inc

HEADERS += \
	Engine/inc/2de_Vector3.h \
	Engine/inc/2de_Vector2.h \
	Engine/inc/2de_TimerComponent.h \
	Engine/inc/2de_Text.h \
	Engine/inc/2de_SpatialManager.h \
	Engine/inc/2de_Sound.h \
	Engine/inc/2de_RTTI.h \
	Engine/inc/2de_RGBA.h \
	Engine/inc/2de_Resource.h \
	Engine/inc/2de_RenderableComponent.h \
	Engine/inc/2de_Prototype.h \
	Engine/inc/2de_PlaceableComponent.h \
	Engine/inc/2de_Physics.h \
	Engine/inc/2de_Matrix2.h \
	Engine/inc/2de_Math.h \
	Engine/inc/2de_LuaUtils.h \
	Engine/inc/2de_Log.h \
	Engine/inc/2de_Input.h \
	Engine/inc/2de_ImageUtils.h \
	Engine/inc/2de_GraphicsLow.h \
	Engine/inc/2de_GraphicsHigh.h \
	Engine/inc/2de_Geometry.h \
	Engine/inc/2de_GeometricComponent.h \
	Engine/inc/2de_GameUtils.h \
	Engine/inc/2de_GameObject.h \
	Engine/inc/2de_Factory.h \
	Engine/inc/2de_Event.h \
	Engine/inc/2de_Engine.h \
	Engine/inc/2de_DebugBox.h \
	Engine/inc/2de_Core.h \
	Engine/inc/2de_Config.h \
	Engine/inc/2de_Box.h \
	Engine/inc/2de_Xml.h

SOURCES += \
	Engine/src/2de_Xml.cpp \
	Engine/src/2de_Vector3.cpp \
	Engine/src/2de_TimerComponent.cpp \
	Engine/src/2de_Text.cpp \
	Engine/src/2de_SpatialManager.cpp \
	Engine/src/2de_Sound.cpp \
	Engine/src/2de_ShroomTitle.cpp \
	Engine/src/2de_RTTI.cpp \
	Engine/src/2de_Resource.cpp \
	Engine/src/2de_RenderableComponent.cpp \
	Engine/src/2de_Prototype.cpp \
	Engine/src/2de_PlaceableComponent.cpp \
	Engine/src/2de_Physics.cpp \
	Engine/src/2de_Matrix2.cpp \
	Engine/src/2de_Math.cpp \
	Engine/src/2de_LuaUtils.cpp \
	Engine/src/2de_Log.cpp \
	Engine/src/2de_Input.cpp \
	Engine/src/2de_IncludeAllTU.cpp \
	Engine/src/2de_ImageUtils.cpp \
	Engine/src/2de_GraphicsLow.cpp \
	Engine/src/2de_GraphicsHigh.cpp \
	Engine/src/2de_Geometry.cpp \
	Engine/src/2de_GeometricComponent.cpp \
	Engine/src/2de_GameUtils.cpp \
	Engine/src/2de_GameObject.cpp \
	Engine/src/2de_Factory.cpp \
	Engine/src/2de_Event.cpp \
	Engine/src/2de_Engine.cpp \
	Engine/src/2de_DefaultFont.cpp \
	Engine/src/2de_DebugBox.cpp \
	Engine/src/2de_Core.cpp \
	Engine/src/2de_Config.cpp \
	Engine/src/2de_Box.cpp

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
