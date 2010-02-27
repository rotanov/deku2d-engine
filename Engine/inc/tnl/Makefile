OBJECTS		=	obj/assert.o \
			obj/asymmetricKey.o \
			obj/bitStream.o \
			obj/byteBuffer.o \
			obj/certificate.o \
			obj/clientPuzzle.o \
			obj/connectionStringTable.o \
			obj/dataChunker.o \
			obj/eventConnection.o \
			obj/ghostConnection.o \
			obj/huffmanStringProcessor.o \
			obj/log.o \
			obj/netBase.o \
			obj/netConnection.o \
			obj/netInterface.o \
			obj/netObject.o \
			obj/netStringTable.o \
			obj/platform.o \
			obj/random.o \
			obj/rpc.o \
			obj/symmetricCipher.o \
			obj/tnlMethodDispatch.o \
			obj/journal.o \
			obj/udp.o \
			obj/vector.o

all: obj $(OBJECTS)

obj:
	mkdir -p obj/

obj/%.o: %.cpp
	g++ -fpermissive -w -c -o $@ $< -I../libtomcrypt/

clean:
	rm -f $(OBJECTS)
