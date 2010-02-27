OBJECTS		=	obj/keyring.o obj/gf.o obj/mem.o obj/sprng.o obj/ecc.o \
			obj/base64.o obj/dh.o obj/rsa.o obj/bits.o obj/yarrow.o \
			obj/cfb.o obj/ofb.o obj/ecb.o obj/ctr.o obj/cbc.o obj/hash.o \
			obj/tiger.o obj/sha1.o obj/md5.o obj/md4.o obj/md2.o \
			obj/sha256.o obj/sha512.o obj/xtea.o obj/aes.o obj/des.o \
			obj/safer_tab.o obj/safer.o obj/safer+.o obj/rc4.o \
			obj/rc2.o obj/rc6.o obj/rc5.o obj/cast5.o obj/noekeon.o \
			obj/blowfish.o obj/crypt.o obj/prime.o obj/twofish.o \
			obj/packet.o obj/hmac.o obj/strings.o obj/rmd128.o \
			obj/rmd160.o obj/skipjack.o obj/omac.o obj/dsa.o obj/mpi.o

all: obj $(OBJECTS)

obj:
	mkdir -p obj/

obj/%.o: %.c
	gcc -w -c -o $@ $< -I.

obj/rsa.o: rsa.c rsa_sys.c
obj/ecc.o: ecc.c ecc_sys.c
obj/dh.o: dh.c dh_sys.c
obj/aes.o: aes.c aes_tab.c
obj/twofish.o: twofish.c twofish_tab.c
obj/sha512.o: sha512.c sha384.c
obj/sha256.o: sha256.c sha224.c

clean:
	rm -f $(OBJECTS)
