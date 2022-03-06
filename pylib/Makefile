#
#
#
#
all:
	make -C ./src
	mv ./src/libpylib.a  ./lib/
	cp ./src/*.h         ./include/
	cp ./src/code/*.h         ./include/
	make -C ./test

install:
	mkdir -p /usr/local/pylib
	cp -r include /usr/local/pylib
	cp -r lib/    /usr/local/pylib
	

clean:
	make clean -C ./src
	make clean -C ./test
