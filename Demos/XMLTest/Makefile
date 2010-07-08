SOURCES	=	src/main.cpp \
		../../Engine/src/2de_Xml.cpp \
		../../Engine/src/2de_Core.cpp

TARGET	=	bin/XMLTest

CFLAGS	=	-iquote../../Engine/inc/

all:
	g++ -o $(TARGET) $(SOURCES) $(CFLAGS)

debug:
	g++ -g3 -o $(TARGET) $(SOURCES) $(CFLAGS)
