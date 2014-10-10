
all:

prepare:

include cc.mk
include $(appdir)/xs.mk

# all make dirs, extra_dir define in xs.mk
make_dirs := $(appdir)
make_dirs += $(extra_dir)
make_dirs += ./com

# make include dirs 
inc_dirs := $(foreach d,$(make_dirs),$(shell find $(d) -type d))
exc_dirs := $(foreach d,$(make_dirs),$(shell find $(d) | grep ".git"))
exc_dirs += $(foreach d,$(make_dirs),$(shell find $(d) | grep ".svn"))
inc_dirs := $(filter-out $(exc_dirs),$(inc_dirs))
# inc_dirs = src/com src/hello_server/hello_server
inc_dirs := $(addprefix -I,$(inc_dirs))
# inc_dirs = -Isrc/com -Isrc/hello_xs/hello_server

CPPFLAGS += $(inc_dirs)
# CPPFLAGS += -Isrc/com -Isrc/hello_xs/hello_server
CPPFLAGS += -DXS_AUTO_GEN_FILES

# make all objects
srcs := $(foreach d,$(make_dirs),$(shell find $(d) -name "*.c"))
srcs += $(appdir)__all_files__.c


# objs
objs := $(srcs:.c=.o)

# depends
deps += $(objs:.o=.d)

ifneq ($(MAKECMDGOALS),clean) 
-include $(deps)
endif
# deps = com/xs.d com/xs_ev.d ....

# flags and libs
LIBS += -lrt -lpthread #-lzlog
CPPFLAGS += -g -Wall -Wextra 

#app := $(shell basename $(appdir))
app := $(appdir)$(shell basename $(appdir))
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

$(app): $(objs)
ifeq ($(template),app)
#	$(CC) -o $@ $^  $(LIBS) $(CPPFLAGS) -L. -lxs
	$(CC) -o $@ $^  $(LIBS) $(CPPFLAGS)
else
ifeq ($(template),share)
#	$(CC) -o $@ $(app_objs)  $(LIBS) $(CPPFLAGS) -L. -lxs
	$(CC) -o $@ $^  $(LIBS) $(CPPFLAGS) 
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

# for 
allfiles := $(foreach d,$(make_dirs),$(shell find $(d) -name "*.c" -o -name "*.h" ))
allfile_deps := $(allfiles)
allfile_deps := $(filter-out $(allfile_deps),$(appdir)__all_files__.c)
# sort
allfiles := $(sort $(allfiles))
# remove ./
allfiles := $(subst ./,,$(allfiles))
# add "com/files.h" for c style string
allfiles := $(addprefix \",$(allfiles))
allfiles := $(allfiles:=\",)


$(appdir)__all_files__.c: $(allfile_deps)
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
	rm $(objs) $(deps) $(app) *.log *.bin *.so *.d *.o *.tmp *.a -rf 

.PHONY: install
install:

.PHONY: uninstall
uninstall:

