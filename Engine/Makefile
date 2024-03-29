SOURCES		=	src/2de_Core.cpp \
			src/2de_Box.cpp \
			src/2de_Config.cpp \
			src/2de_DefaultFont.cpp \
			src/2de_Engine.cpp \
			src/2de_Environment.cpp \
			src/2de_Event.cpp \
			src/2de_Factory.cpp \
			src/2de_FileSystem.cpp \
			src/2de_GameObject.cpp \
			src/2de_GameUtils.cpp \
			src/2de_GeometricComponent.cpp \
			src/2de_Geometry.cpp \
			src/2de_GraphicsHigh.cpp \
			src/2de_GraphicsLow.cpp \
			src/2de_ImageUtils.cpp \
			src/2de_Input.cpp \
			src/2de_Log.cpp \
			src/2de_LuaUtils.cpp \
			src/2de_Math.cpp \
			src/2de_Matrix2.cpp \
			src/2de_Memory.cpp \
			src/2de_Object.cpp \
			src/2de_PlaceableComponent.cpp \
			src/2de_Physics.cpp \
			src/2de_Property.cpp \
			src/2de_Prototype.cpp \
			src/2de_Registration.cpp \
			src/2de_RenderableComponent.cpp \
			src/2de_Resource.cpp \
			src/2de_RTTI.cpp \
			src/2de_Serialization.cpp \
			src/2de_ShroomTitle.cpp \
			src/2de_Singleton.cpp \
			src/2de_SpatialManager.cpp \
			src/2de_Sound.cpp \
			src/2de_Storage.cpp \
			src/2de_Text.cpp \
			src/2de_TimerComponent.cpp \
			src/2de_TypeInfo.cpp \
			src/2de_Vector3.cpp \
			src/2de_Xml.cpp

OBJECTS		=	obj/2de_Core.o \
			obj/2de_Box.o \
			obj/2de_Config.o \
			obj/2de_DefaultFont.o \
			obj/2de_Engine.o \
			obj/2de_Environment.o \
			obj/2de_Event.o \
			obj/2de_Factory.o \
			obj/2de_FileSystem.o \
			obj/2de_GameObject.o \
			obj/2de_GameUtils.o \
			obj/2de_GeometricComponent.o \
			obj/2de_Geometry.o \
			obj/2de_GraphicsHigh.o \
			obj/2de_GraphicsLow.o \
			obj/2de_ImageUtils.o \
			obj/2de_Input.o \
			obj/2de_Log.o \
			obj/2de_LuaUtils.o \
			obj/2de_Math.o \
			obj/2de_Matrix2.o \
			obj/2de_Object.o \
			obj/2de_PlaceableComponent.o \
			obj/2de_Physics.o \
			obj/2de_Property.o \
			obj/2de_Prototype.o \
			obj/2de_Registration.o \
			obj/2de_RenderableComponent.o \
			obj/2de_Resource.o \
			obj/2de_RTTI.o \
			obj/2de_Serialization.o \
			obj/2de_ShroomTitle.o \
			obj/2de_Singleton.o \
			obj/2de_SpatialManager.o \
			obj/2de_Sound.o \
			obj/2de_Storage.o \
			obj/2de_Text.o \
			obj/2de_TimerComponent.o \
			obj/2de_TypeInfo.o \
			obj/2de_Vector3.o \
			obj/2de_Xml.o

SINGLE_FILE_SOURCE	=	src/2de_IncludeAllTU.cpp
SINGLE_FILE_OBJECT	=	obj/2de_IncludeAllTU.o


SDL_CFLAGS	:=	$(shell sdl-config --cflags)
LUA_CFLAGS	:=	$(shell pkg-config --cflags lua5.1)

CFLAGS		:= 	${CFLAGS} ${SDL_CFLAGS} ${LUA_CFLAGS} -Iinc -std=c++0x

SDL_LDFLAGS	:=	$(shell sdl-config --libs)
LUA_LDFLAGS	:=	$(shell pkg-config --libs lua5.1)

LDFLAGS		:=	$(LDFLAGS) $(SDL_LDFLAGS) $(LUA_LDFLAGS) -lGLU -lGLEW -lSDL_mixer

TARGET_STATIC_FILE = libDeku2D.a
TARGET_SHARED_FILE = libDeku2D.so

TARGET_STATIC	=	lib/$(TARGET_STATIC_FILE)
TARGET_SHARED	=	lib/$(TARGET_SHARED_FILE)

STRICTFLAG	=	

all: static

obj/%.o: src/%.cpp
	g++ $(DEBUGFLAG) $(STRICTFLAG) -c -o $@ $(CFLAGS) $<

# maybe replace this implicit rule by explicit one, that uses ar
$(TARGET_STATIC): $(TARGET_STATIC)($(OBJECTS))

$(TARGET_SHARED): CFLAGS := $(CFLAGS) -fPIC
$(TARGET_SHARED): $(OBJECTS)
	g++ $(DEBUGFLAG) $(STRICTFLAG) -shared $(CFLAGS) $(LDFLAGS) -o $(TARGET_SHARED) $(OBJECTS)

obj:
	mkdir -p obj/


static: obj $(TARGET_STATIC)

shared: obj $(TARGET_SHARED)

singlefile: obj
	g++ -g3 -c $(CFLAGS) -o $(SINGLE_FILE_OBJECT) $(SINGLE_FILE_SOURCE)
	ar rv $(TARGET_STATIC) $(SINGLE_FILE_OBJECT)

debug: DEBUGFLAG = -g3 -D_DEBUG
debug: static

strict: STRICTFLAG = -Wall
strict: static

debug-shared: DEBUGFLAG = -g3 -D_DEBUG
debug-shared: shared

strict-shared: STRICTFLAG = -Wall
strict-shared: shared

PREFIXDIR = $(DESTDIR)/usr
LIBDIR = $(PREFIXDIR)/lib
INCLUDEDIR = $(PREFIXDIR)/include

install: $(TARGET_STATIC) $(TARGET_SHARED)
	install -d $(LIBDIR)
	install -d $(INCLUDEDIR)/deku2d/
	install -m644 $(TARGET_STATIC) $(LIBDIR)/$(TARGET_STATIC_FILE)
	install -m644 $(TARGET_SHARED) $(LIBDIR)/$(TARGET_SHARED_FILE)
	install -m644 inc/2de_*.h $(INCLUDEDIR)/deku2d/

uninstall:
	rm $(LIBDIR)/$(TARGET_STATIC_FILE)
	rm $(LIBDIR)/$(TARGET_SHARED_FILE)
	rm -r $(INCLUDEDIR)/deku2d/
	

docs: doc/Doxyfile
	@if [ -e /usr/bin/doxygen ]; \
	then \
		doxygen doc/Doxyfile; \
	else \
		echo "You must have Doxygen installed to make documentation."; \
	fi

clean:
	rm -f $(OBJECTS)

distclean: clean
	rm -f $(TARGET_STATIC)
	rm -f $(TARGET_SHARED)
	rm -rf doc/html/

.PHONY: clean distclean docs
