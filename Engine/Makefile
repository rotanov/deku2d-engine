SOURCES		=	src/2de_Core.cpp \
			src/2de_Engine.cpp \
			src/2de_GameUtils.cpp \
			src/2de_GraphicsHigh.cpp \
			src/2de_GraphicsLow.cpp \
			src/2de_Gui.cpp \
			src/2de_ImageUtils.cpp \
			src/2de_LuaUtils.cpp \
			src/2de_MathUtils.cpp \
			src/2de_Physics.cpp \
			src/2de_Resource.cpp \
			src/2de_Sound.cpp \
			src/2de_Xml.cpp

OBJECTS		=	obj/2de_Core.o \
			obj/2de_Engine.o \
			obj/2de_GameUtils.o \
			obj/2de_GraphicsHigh.o \
			obj/2de_GraphicsLow.o \
			obj/2de_Gui.o \
			obj/2de_ImageUtils.o \
			obj/2de_LuaUtils.o \
			obj/2de_MathUtils.o \
			obj/2de_Physics.o \
			obj/2de_Resource.o \
			obj/2de_Sound.o \
			obj/2de_Xml.o

SDL_CFLAGS	:=	$(shell sdl-config --cflags)
LUA_CFLAGS	:=	$(shell pkg-config --cflags lua5.1)

CFLAGS		:= 	${CFLAGS} ${SDL_CFLAGS} ${LUA_CFLAGS} -iquoteinc

SDL_LDFLAGS	:=	$(shell sdl-config --libs)
LUA_LDFLAGS	:=	$(shell pkg-config --libs lua5.1)

LDFLAGS		:=	$(LDFLAGS) $(SDL_LDFLAGS) $(LUA_LDFLAGS) -lGLU -lSDL_mixer

TARGET_STATIC	=	lib/libDeku2d.a
TARGET_SHARED	=	lib/libDeku2d.so

STRICTFLAG	=	-w

all: static

obj/%.o: src/%.cpp
	g++ $(DEBUGFLAG) $(STRICTFLAG) -c -o $@ $(CFLAGS) $<

# maybe replace this implicit rule by explicit one, that uses ar
$(TARGET_STATIC): $(TARGET_STATIC)($(OBJECTS))

$(TARGET_SHARED): $(OBJECTS)
	g++ $(DEBUGFLAG) $(STRICTFLAG) -shared -fPIC $(LDFLAGS) -o $(TARGET_SHARED) $(OBJECTS)

obj:
	mkdir -p obj/


static: obj $(TARGET_STATIC)

shared: obj $(TARGET_SHARED)

debug: DEBUGFLAG = -g3 -D_DEBUG
debug: static

strict: STRICTFLAG = -Wall
strict: static

debug-shared: DEBUGFLAG = -g3 -D_DEBUG
debug-shared: shared

strict-shared: STRICTFLAG = -Wall
strict-shared: shared

PREFIXDIR = $(DESTDIR)/usr/
INCLUDEDIR = $(PREFIXDIR)include/deku2d/

install: $(TARGET_STATIC) $(TARGET_SHARED)
	install -d $(PREFIXDIR)lib/
	install -d $(INCLUDEDIR)
	install -m644 $(TARGET) $(PREFIXDIR)$(TARGET_STATIC)
	install -m644 $(TARGET_SHARED) $(PREFIXDIR)$(TARGET_SHARED)
	install -m644 inc/2de_*.h $(INCLUDEDIR)
	

docs: doc/Doxyfile
	if [ -e /usr/bin/doxygen ]; then \
		doxygen doc/Doxyfile; \
	fi

clean:
	rm -f $(OBJECTS)

distclean: clean
	rm -f $(TARGET_STATIC)
	rm -f $(TARGET_SHARED)
	rm -rf doc/html/

.PHONY: clean distclean docs
