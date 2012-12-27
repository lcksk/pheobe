include $(BUILD_ROOT)/tools/tools.mk
include $(BUILD_ROOT)/tools/print.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR =	\
		include	\
		$(APPS_DIR)/include	\
		$(APPS_DIR)/bcmsw_igmp

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -pthread
LDFLAGS += -L$(BUILD_ROOT)/bin -lnet -lpcap


#vpath %.cpp %.h util
#include util/make.mk


OBJS += \
	main.o
