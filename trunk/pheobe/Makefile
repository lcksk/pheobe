export BUILD_ROOT=$(shell echo `pwd`)

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

TARGETS = apps

.PHONY : all $(TARGETS) veryclean install uninstall

all : $(TARGETS)
	
$(TARGETS) :
	$(call DISPLAY_RED , $(subst ,,* $@ ) )
	@ $(MAKE) -C $@

gdbus : $(patsubst %, %_gdbus, $(TARGETS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )
	
	
$(patsubst %,%_gdbus,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _gdbus,,$@) gdbus

client : $(patsubst %, %_client, $(TARGETS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )
	
	
$(patsubst %,%_client,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _client,,$@) client
	

clean : $(patsubst %, %_clean, $(TARGETS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )
	
	
$(patsubst %,%_clean,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _clean,,$@) clean
	

install : $(patsubst %, %_install, $(TARGETS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )
	@ $(CP) $(SYSTEM_DIR)/* $(OBJ_DEST)/usr/local/bin

	
$(patsubst %,%_install,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _install,,$@) install


uninstall :
	$(Q_)$(RM) -rf $(OBJ_DEST)

	
include $(BUILD_ROOT)/tools/print.mk
