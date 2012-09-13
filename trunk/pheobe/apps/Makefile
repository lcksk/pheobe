BUILD_ROOT ?= ..

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

LIBS += 
#APPS = sampleplayer
APPS = Renderer smallpb

SUB_BUILD = $(LIBS) $(APPS)
.PHONY : $(SUB_BUILD) $(BIN_DIR)

all : 	$(BIN_DIR) $(SUB_BUILD)

$(SUB_BUILD) : 
	$(call DISPLAY_RED , $(subst ,,* $@ ) )
	@ $(MAKE) -C $(APPS_DIR)/$@ DEBUG=$(DEBUG)

gdbus : $(patsubst %, %_gdbus, $(SUB_BUILD))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_gdbus,$(SUB_BUILD)) :
	$(Q_)$(MAKE) -C $(APPS_DIR)/$(subst _gdbus,,$@) gdbus DEBUG=$(DEBUG)

client : $(patsubst %, %_client, $(SUB_BUILD))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_client,$(SUB_BUILD)) :
	@if [ -e $(APPS_DIR)/$(subst _client,,$@)/client ]; then	\
		$(Q_)$(MAKE) -C $(APPS_DIR)/$(subst _client,,$@)/client DEBUG=$(DEBUG);	\
	fi

clean : $(patsubst %, %_clean, $(SUB_BUILD))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )
	
$(patsubst %,%_clean,$(SUB_BUILD)) :
	$(Q_)$(MAKE) -C $(APPS_DIR)/$(subst _clean,,$@) clean DEBUG=$(DEBUG)
	
install : $(patsubst %, %_install, $(SUB_BUILD))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_install,$(SUB_BUILD)) :
	$(Q_)$(MAKE) -C $(APPS_DIR)/$(subst _install,,$@) install DEBUG=$(DEBUG)

$(BIN_DIR) :
	$(Q_)$(MKDIR) $(BIN_DIR)


include $(BUILD_ROOT)/tools/print.mk

	
