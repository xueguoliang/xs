
all:
	cd src2 && make || exit "$$?"

.PHONY: clean install
clean:
	cd src2 && make clean

