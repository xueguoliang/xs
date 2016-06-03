
all:

all: $(target)

srcs := $(shell find . -name "*.c")
objs := $(srcs:.c=.o)
deps := $(srcs:.c=.d)

ifneq ($(MAKECMDGOALS),clean)
-include $(deps) 
endif

%.d: %.c
	$(CC) -MM $(CPPFLAGS) $(LIBS) $< > $@

.PHONY: clean
clean:
	rm -rf *.bin *.o *.d *.a

