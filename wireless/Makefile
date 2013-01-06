export BUILD_ROOT=$(shell echo `pwd`)

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

TARGETS = apps applibs

.PHONY : all $(TARGETS) veryclean install uninstall

all :  $(TARGETS)

$(TARGETS) :
	$(call DISPLAY_RED , $(subst ,,* $@ ) )
	@ $(MAKE) -C $@
	
clean : $(patsubst %, %_clean, $(TARGETS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )
	
	
$(patsubst %,%_clean,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _clean,,$@) clean
	
install : $(patsubst %, %_install, $(TARGETS))
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(patsubst %,%_install,$(TARGETS)) :
	$(Q_)$(MAKE) -C $(subst _install,,$@) install

uninstall :
	$(Q_)$(RM) -rf $(OBJ_DEST)
	
veryclean : uninstall clean
	@ $(RM) -rf $(BUILD_ROOT)/output

include $(BUILD_ROOT)/tools/print.mk