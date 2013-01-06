include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR = \
		include	\
		$(APPS_DIR)/libosl/include	\
		$(APPS_DIR)/libxwifi

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -DREENTRANT $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -pthread
LDFLAGS += -L$(FALCON_DIR)/$(shell echo `uname -r`) -lbwl
LDFLAGS += -L$(BUILD_ROOT)/bin -lpthread -lxwifi -losl

OBJS += \
	main.o
	

