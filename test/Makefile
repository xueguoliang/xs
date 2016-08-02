

src := $(shell find -name "*.c")
dep := $(src:.c=.d)
obj := $(src:.c=.o)
targets := $(src:.c=.bin)

all: $(targets)

-include $(dep)
-include ../config.mk

%.d: %.c
	$(CC) $(CPPFLAGS) -MM $< > $@

%.o: %.c
	$(CC) $(CPPFLAGS) -c -o $@ $<

%.bin: %.o
	$(CC) $(CPPFLAGS) -o $@ $^ -L../src -lxs

.PHONY: clean install uninstall

clean:
	$(RM) $(obj) $(dep) $(targets)

install:

uninstall:


