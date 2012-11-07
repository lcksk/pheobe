BUILD_ROOT ?= ..

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

.PHONY : hostapd wpa_supplicant

SUB_DIRS = hostapd wpa_supplicant

all: $(SUB_DIRS)


$(SUB_DIRS) : 
	$(call DISPLAY_RED , $(subst ,,* $@ ) )
	@ $(MAKE) -C $(OPENSOURCE_DIR)/$@ DEBUG=$(DEBUG)
	
#hostapd:
#	$(MAKE) -C $(OPENSOURCE_DIR)/$@
#	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

#wpa_supplicant:
#	$(MAKE) -C $(OPENSOURCE_DIR)/$@
#	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )
	
clean : $(patsubst %, %_clean, $(SUB_DIRS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_clean,$(SUB_DIRS)) :
	$(Q_)$(MAKE) -C $(OPENSOURCE_DIR)/$(subst _clean,,$@) clean DEBUG=$(DEBUG)

install:

	
include $(BUILD_ROOT)/tools/print.mk
