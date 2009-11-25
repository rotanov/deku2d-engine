all:
	cd src/Deku2d/ && make && cd ../../
	cd src/Sandbox/ && make && cd ../../
	cd src/Tanks/ && make && cd ../../
	cd "src/Font editor/" && make && cd ../../
	cd "src/Level Editor/" && make && cd ../../
	cd src/Pong/ && make && cd ../../

debug:
	cd src/Deku2d/ && make debug && cd ../../
	cd src/Sandbox/ && make debug && cd ../../
	cd src/Tanks/ && make debug && cd ../../
	cd "src/Font editor/" && make debug && cd ../../
	cd "src/Level Editor/" && make debug && cd ../../
	cd src/Pong/ && make debug && cd ../../

strict:
	cd src/Deku2d/ && make strict && cd ../../
	cd src/Sandbox/ && make strict && cd ../../
	cd src/Tanks/ && make strict && cd ../../
	cd "src/Font editor/" && make strict && cd ../../
	cd "src/Level Editor/" && make strict && cd ../../
	cd src/Pong/ && make strict && cd ../../

clean:
	rm obj/*.o
