
all:
	cd src && make || exit "$$?"

.PHONY: clean install
clean:
	cd src && make clean

