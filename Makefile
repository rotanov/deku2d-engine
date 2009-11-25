all:
	cd Engine/ && make && cd ../
	cd Demos/Sandbox/ && make && cd ../../
	cd Demos/Tanks/ && make && cd ../../
	cd "Demos/Font editor/" && make && cd ../../
	cd "Demos/Level editor/" && make && cd ../../
	cd Demos/Pong/ && make && cd ../../

debug:
	cd Engine/ && make debug && cd ../
	cd Demos/Sandbox/ && make debug && cd ../../
	cd Demos/Tanks/ && make debug && cd ../../
	cd "Demos/Font editor/" && make debug && cd ../../
	cd "Demos/Level editor/" && make debug && cd ../../
	cd Demos/Pong/ && make debug && cd ../../

strict:
	cd Engine/ && make strict && cd ../
	cd Demos/Sandbox/ && make strict && cd ../../
	cd Demos/Tanks/ && make strict && cd ../../
	cd "Demos/Font editor/" && make strict && cd ../../
	cd "Demos/Level editor/" && make strict && cd ../../
	cd Demos/Pong/ && make strict && cd ../../

clean:
	cd Engine/ && make clean && cd ../

dist-clean: 
	cd Engine/ && make dist-clean && cd ../
	cd Demos/Sandbox/ && make dist-clean && cd ../../
	cd Demos/Tanks/ && make dist-clean && cd ../../
	cd "Demos/Font editor/" && make dist-clean && cd ../../
	cd "Demos/Level editor/" && make dist-clean && cd ../../
	cd Demos/Pong/ && make dist-clean && cd ../../
