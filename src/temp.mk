
all:

prepare:

include cc.mk
include $(appdir)/xs.mk

# extra_dir define in xs.mk
# extra_src define in xs.mk

app := $(shell basename $(appdir))
ifeq ($(template),app)
app := $(app).bin
else
ifeq ($(template),share)
app := lib$(app).so
CPPFLAGS += -fPIC -shared
else
ifeq ($(template),static)
app := lib$(app).a
else
app := $(app).bin
endif
endif
endif

all: prepare

all: $(app)
	@echo $(appdir)

# include dirs
includir := $(shell find ./com -type d)
includir += $(shell find $(appdir) -type d)
excludir := $(shell find  -type d | grep ".git")
excludir += $(shell find  -type d | grep ".svn")
includir := $(filter-out $(excludir),$(includir))
includir := $(addprefix -I,$(includir))

# make deps
sources := $(shell find $(appdir) -name "*.c")
sources += $(extra_src)
sources += __all_files__.c
app_objs := $(sources:.c=.o)

# com objects for all 
com_objs := $(shell find ./com -name "*.c")
com_objs := $(com_objs:.c=.o)

app_objs += $(com_objs)

#app_objs := $(shell find $(appdir) -name "*.c")
#app_objs := $(app_objs:.c=.o)

#depends := $(sources:.c=.d)
depends += $(app_objs:.o=.d)
ifneq ($(MAKECMDGOALS),clean) 
-include $(depends)
endif

# flags and libs
LIBS += -lrt -lpthread #-lzlog
CPPFLAGS += $(includir)
CPPFLAGS += -g -Wall -Wextra 

$(app): $(app_objs)
ifeq ($(template),app)
#	$(CC) -o $@ $^  $(LIBS) $(CPPFLAGS) -L. -lxs
	$(CC) -o $@ $^  $(LIBS) $(CPPFLAGS)
else
ifeq ($(template),share)
#	$(CC) -o $@ $(app_objs)  $(LIBS) $(CPPFLAGS) -L. -lxs
	$(CC) -o $@ $(app_objs)  $(LIBS) $(CPPFLAGS) 
else
ifeq ($(template),static)
#	$(AR) rcs -o $@ $(app_objs) $(LIBS) $(CPPFLAGS) -L. -lxs
	$(AR) rcs -o $@ $(app_objs) $(LIBS) $(CPPFLAGS) 
else
#	$(CC) -o $@ $^ $(LIBS) $(CPPFLAGS) -L. -lxs
	$(CC) -o $@ $^ $(LIBS) $(CPPFLAGS)
endif
endif
endif
	rm __all_files__.*

# for 
allfiles := $(shell find $(appdir) -name "*.c" -o -name "*.h")
allfiles += $(shell find ./com -name "*.c" -o -name "*.h")
ifdef extra_dir
allfiles += $(shell find $(extra_dir) -name "*.c" -o -name "*.h")
endif
allfile_deps := $(allfiles)
allfiles := $(sort $(allfiles))
allfiles := $(subst ./,,$(allfiles))
allfiles := $(addprefix \",$(allfiles))
allfiles := $(allfiles:=\",)

__all_files__.c: $(allfile_deps)
	@echo '#ifdef __cplusplus' > $@
	@echo 'extern "C"{' >> $@
	@echo '#endif' >> $@
	@echo 'const char* g_mem_files[] = {' >> $@
	@for filename in $(allfiles); \
		do \
		echo $$filename >> $@; \
		done
	@echo '};' >> $@
	@echo 'int g_mem_file_count=' >> $@
	@echo $(words $(allfiles)) >> $@
	@echo ';' >> $@
	@echo '#ifdef __cplusplus' >> $@
	@echo '}' >> $@
	@echo '#endif' >> $@

# all files end


%.d: %.c
	@set -e; \
       $(CC) -MM $(CPPFLAGS) $< > $@; \
       sed -i 's#.*\.o#$@#' $@;  \
       sed -i 's#\.d#\.o#' $@;  \

.PHONY: clean
clean:
	rm $(app_objs) $(depends) $(app) *.log *.bin *.so *.d *.o *.tmp *.a -rf 

.PHONY: install
install:

.PHONY: uninstall
uninstall:

