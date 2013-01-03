BUILD_ROOT ?= ..

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

SUB_DIRS = libpcap

.PHONY : $(SUB_DIRS)

#SUB_DIRS = libnl

all: $(SUB_DIRS)

$(SUB_DIRS) :
	$(call DISPLAY_RED , $(subst ,,* $@ ) )
	@ $(MAKE) -C $(OPENSOURCE_DIR)/$@ DEBUG=$(DEBUG)

clean : $(patsubst %, %_clean, $(SUB_DIRS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_clean,$(SUB_DIRS)) :
	$(Q_)$(MAKE) -C $(OPENSOURCE_DIR)/$(subst _clean,,$@) clean DEBUG=$(DEBUG)

install : $(patsubst %, %_install, $(SUB_DIRS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_install,$(SUB_DIRS)) :
	$(Q_)$(MAKE) -C $(OPENSOURCE_DIR)/$(subst _install,,$@) install DEBUG=$(DEBUG)

include $(BUILD_ROOT)/tools/print.mk
