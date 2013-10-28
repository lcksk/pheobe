include $(BUILD_ROOT)/tools/tools.mk
include $(BUILD_ROOT)/tools/print.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR =	\
		include	\
		$(APPS_DIR)/include	\
		$(APPS_DIR)/libmulticastcapture/include	\
		$(APPS_DIR)/libosl/include
		
		
CFLAGS += $(addprefix -I, $(INCLUDE_DIR))

LDFLAGS += -L$(BUILD_ROOT)/bin -pthread -lmulticastcapture -losl

CFLAGS += `pkg-config --cflags liburiparser`
LDFLAGS += `pkg-config --libs liburiparser`

OBJS += \
	main.o
