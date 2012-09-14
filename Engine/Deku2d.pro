!include(SharedSettings.pri) {
	error("Can not include shared settings.")

}

TEMPLATE = lib
DESTDIR = lib
CONFIG += staticlib

CONFIG( debug, debug|release ) {
	TARGET = Deku2d.debug
} else {
	TARGET = Deku2d.release
}

HEADERS += \
	inc/2de_Vector3.h \
	inc/2de_Vector2.h \
	inc/2de_TimerComponent.h \
	inc/2de_Text.h \
	inc/2de_SpatialManager.h \
	inc/2de_Sound.h \
	inc/2de_RTTI.h \
	inc/2de_RGBA.h \
	inc/2de_Resource.h \
	inc/2de_RenderableComponent.h \
	inc/2de_Prototype.h \
	inc/2de_PlaceableComponent.h \
	inc/2de_Physics.h \
	inc/2de_Matrix2.h \
	inc/2de_Math.h \
	inc/2de_LuaUtils.h \
	inc/2de_Log.h \
	inc/2de_Input.h \
	inc/2de_ImageUtils.h \
	inc/2de_GraphicsLow.h \
	inc/2de_GraphicsHigh.h \
	inc/2de_Geometry.h \
	inc/2de_GeometricComponent.h \
	inc/2de_GameUtils.h \
	inc/2de_GameObject.h \
	inc/2de_Factory.h \
	inc/2de_Event.h \
	inc/2de_Engine.h \
	inc/2de_DebugBox.h \
	inc/2de_Core.h \
	inc/2de_Config.h \
	inc/2de_Box.h \
	inc/2de_Xml.h \
    inc/2de_Visitor.h \
    inc/2de_TypeInfo.h \
    inc/2de_Storage.h \
    inc/2de_Serialization.h \
    inc/2de_PropertyInfo.h \
    inc/2de_Property.h \
    inc/2de_Object.h \
    inc/2de_Memory.h \
    inc/2de_Define.h

SOURCES += \
	src/2de_Xml.cpp \
	src/2de_Vector3.cpp \
	src/2de_TimerComponent.cpp \
	src/2de_Text.cpp \
	src/2de_SpatialManager.cpp \
	src/2de_Sound.cpp \
	src/2de_ShroomTitle.cpp \
	src/2de_RTTI.cpp \
	src/2de_Resource.cpp \
	src/2de_RenderableComponent.cpp \
	src/2de_Prototype.cpp \
	src/2de_PlaceableComponent.cpp \
	src/2de_Physics.cpp \
	src/2de_Matrix2.cpp \
	src/2de_Math.cpp \
	src/2de_LuaUtils.cpp \
	src/2de_Log.cpp \
	src/2de_Input.cpp \
	src/2de_IncludeAllTU.cpp \
	src/2de_ImageUtils.cpp \
	src/2de_GraphicsLow.cpp \
	src/2de_GraphicsHigh.cpp \
	src/2de_Geometry.cpp \
	src/2de_GeometricComponent.cpp \
	src/2de_GameUtils.cpp \
	src/2de_GameObject.cpp \
	src/2de_Factory.cpp \
	src/2de_Event.cpp \
	src/2de_Engine.cpp \
	src/2de_DefaultFont.cpp \
	src/2de_DebugBox.cpp \
	src/2de_Core.cpp \
	src/2de_Config.cpp \
	src/2de_Box.cpp \
    src/2de_TypeInfo.cpp \
    src/2de_Storage.cpp \
    src/2de_Serialization.cpp \
    src/2de_Registration.cpp \
    src/2de_Property.cpp \
    src/2de_Object.cpp \
    src/2de_Memory.cpp
