include $(BUILD_ROOT)/tools/tools.mk
include $(BUILD_ROOT)/tools/print.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR =	\
		include	\
		$(APPS_DIR)/include	\
		$(APPS_DIR)/libosl/include

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))


LDFLAGS += -L$(BUILD_ROOT)/bin -lnet
LDFLAGS += -losl


vpath %.cpp %.h util
include util/make.mk


OBJS += \
	tcprst.o
