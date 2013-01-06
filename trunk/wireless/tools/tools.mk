#select tool setting 
include $(BUILD_ROOT)/tools/linux_x86_tools.mk
#include $(BUILD_ROOT)/tools/brcm_tools.mk

# $(call make-depend, src, obj, dep)
define make-depend
	$(CC) -MM -MF $3 -MP -MT $2 $(CFLAGS) $1
endef
