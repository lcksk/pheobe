export BUILD_ROOT=$(shell echo `pwd`)

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

TARGETS = apps

.PHONY : all $(TARGETS) refsw veryclean install uninstall

all : refsw $(TARGETS)
	
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

refsw:
	$(call DISPLAY_RED , $(subst ,,* $@ ) )
	@ $(MAKE) -C $(THIRDPARTY_DIR)/common APPLIBS_TOP=$(APPLIBS_TOP)

refsw_clean:
	$(call DISPLAY_RED , $(subst ,,* $@ ) )
	@ $(MAKE) -C $(THIRDPARTY_DIR)/common clean APPLIBS_TOP=$(APPLIBS_TOP)

$(patsubst %,%_install,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _install,,$@) install


uninstall :
	$(Q_)$(RM) -rf $(OBJ_DEST)

	
include $(BUILD_ROOT)/tools/print.mk
