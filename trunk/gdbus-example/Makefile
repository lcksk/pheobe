export BUILD_ROOT=$(shell echo `pwd`)

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

TARGETS = apps 

.PHONY : all $(TARGETS) veryclean prebuild install uninstall gdbus client

all :  $(TARGETS)

$(TARGETS) :
	$(call DISPLAY_RED , $(subst ,,* $@ ) )
	@ $(MAKE) -C $@
	
clean : $(patsubst %, %_clean, $(TARGETS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_clean,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _clean,,$@) clean
	
install : $(OBJ_DEST) $(patsubst %, %_install, $(TARGETS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_install,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _install,,$@) install

$(OBJ_DEST) :
	$(Q_)$(MKDIR) $@/{usr/local/bin,usr/local/lib}

uninstall :
	$(Q_)$(RM) -rf $(OBJ_DEST)
	
veryclean : uninstall clean
	@ $(RM) -rf $(BUILD_ROOT)/output

gdbus : $(patsubst %, %_gdbus, $(TARGETS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_gdbus,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _gdbus,,$@) gdbus 
	
client : $(patsubst %, %_client, $(TARGETS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_client,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _client,,$@) client


include $(BUILD_ROOT)/tools/print.mk
