include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR = 	\
	$(APPS_DIR)/xwifi

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))

CFLAGS += -I$(APPS_DIR)/xwifi
LDFLAGS += -L$(APPS_DIR)/$(ARCH).$(DEBUG_SUFFIX)/lib -lxwifi


OBJS += \
	main.o

	
	
	

