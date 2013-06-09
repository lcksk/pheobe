BUILD_ROOT ?= ..

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk


SUB_BUILD = jsoncpp-src jsonrpc-cpp

.PHONY : $(SUB_BUILD) $(OPENSOURCE_PREFIX)

all : 	$(OPENSOURCE_PREFIX) $(SUB_BUILD)

$(SUB_BUILD) : 
	$(call DISPLAY_RED , $(subst ,,* $@ ) )
	@ $(MAKE) -C $(OPENSOURCE_DIR)/$@ DEBUG=$(DEBUG)

clean : $(patsubst %, %_clean, $(SUB_BUILD))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )
	
$(patsubst %,%_clean,$(SUB_BUILD)) :
	$(Q_)$(MAKE) -C $(OPENSOURCE_DIR)/$(subst _clean,,$@) clean DEBUG=$(DEBUG)

install : $(patsubst %, %_install, $(SUB_BUILD))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_install,$(SUB_BUILD)) :
	$(Q_)$(MAKE) -C $(OPENSOURCE_DIR)/$(subst _install,,$@) install DEBUG=$(DEBUG)

$(BIN_DIR) :
	$(Q_)$(MKDIR) $(OPENSOURCE_PREFIX)


include $(BUILD_ROOT)/tools/print.mk

	
