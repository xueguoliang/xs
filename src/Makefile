
src := $(shell find -name "*.c")
dep := $(src:.c=.d)
obj := $(src:.c=.o)
header := $(shell find -name "*.h")
target := libxs.a

all:

-include $(dep)
-include ../config.mk

%.d: %.c
	$(CC) $(CPPFLAGS) -MM $< > $@

%.o: %.c
	$(CC) $(CPPFLAGS) -c -o $@ $<

all: $(target)

$(target): $(obj)
	$(AR) rcs -o $@ $^ $(LIBS) $(CPPFLAGS) $(CFLAGS)

.PHONY: clean install uninstall

clean:
	$(RM) $(obj) $(dep) $(target)

install:
	$(CP) $(target) /usr/local/lib
	$(RM) -r /usr/local/include/xs
	mkdir -p /usr/local/include/xs
	$(CP) $(header) /usr/local/include/xs

uninstall:
	$(RM) -r /usr/local/lib/libxs.a /usr/local/include/xs



