
all:
	make -C src
	make -C test

clean:
	make -C src clean
	make -C test clean

install:
	make -C src install

uninstall:
	make -C src uninstall

