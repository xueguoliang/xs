
pros := $(shell find -name "*.pro")
pros := $(dir $(pros))

all:
	@echo $(pros)
	@for pro in $(pros); \
		do \
		cd $$pro && qmake && make; \
		done

.PHONY: clean
clean:
	@for pro in $(pros); \
		do \
		cd $$pro && qmake && make distclean; \
		done



