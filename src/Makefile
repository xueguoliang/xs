
# com objects for all 
-include cc.mk
-include qt.mk
com_objs := $(shell find ./com -name "*.c")
com_objs := $(com_objs:.c=.o)
com_deps := $(com_objs:.o=.d)

xs_mks := $(shell find . -name "xs.mk")
xs_dir := $(dir $(xs_mks))

CPPFLAGS += -fPIC -shared
LIBS += -lpthread -lrt
CFLAGS += -DXS_AUTO_GEN_FILES

qt_mks := $(shell find . -name "*.pro")
qt_mks := $(dir $(qt_mks))

curdir := $(shell pwd)

#all:
#	@echo $(xs_dir)


#all: libxs.so
all: 
	@for target in $(xs_dir); \
		do \
		cd $(curdir); $(MAKE) -f temp.mk appdir=$$target || exit "$$?"; \
		done
ifeq ($(QT),yes)
	@for qt_mk in $(qt_mks); \
		do \
		cd $(curdir); cd $$qt_mk; qmake; $(MAKE); echo $$qt_mk  || exit "$$?"; \
		done
endif

#libxs.so: $(com_objs)
#	$(CC) -o $@ $^ -fPIC -shared $(LIBS) || exit "$$?"
#ifneq ($(MAKECMDGOALS),clean) 
#-include $(com_deps)
#endif

#%.d: %.c
#	@set -e; \
       $(CC) -MM $(CPPFLAGS) $< > $@; \
       sed -i 's#.*\.o#$@#' $@;  \
       sed -i 's#\.d#\.o#' $@;  \

.PHONY: clean
clean:
	@for target in $(xs_dir); \
		do \
		$(MAKE) -f temp.mk clean appdir=$$target; \
		done
ifeq ($(QT),yes)
	@for qt_mk in $(qt_mks); \
		do \
		cd $(curdir); cd $$qt_mk; qmake; make distclean; \
		done
endif
	@rm $(com_objs) -rf
	@rm $(com_deps) -rf
	@rm `find -name "__all_files__.c"` -rf

