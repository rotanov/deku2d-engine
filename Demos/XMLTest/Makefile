SOURCES	=	src/main.cpp \
		../../Engine/src/2de_Xml.cpp \
		../../Engine/src/2de_Core.cpp

CFLAGS	:=	${CFLAGS} -iquote../../Engine/inc/ -DSIMPLIFIED_LOG

TARGET	=	bin/XMLTest

all:
	g++ -o ${TARGET} ${SOURCES} ${CFLAGS}

debug:
	g++ -g3 -o ${TARGET} ${SOURCES} ${CFLAGS}

strict:
	g++ -Wall -o ${TARGET} ${SOURCES} ${CFLAGS}

distclean:
	rm -f ${TARGET}
