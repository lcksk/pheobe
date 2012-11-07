include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR += 	\

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))

OBJS += \
	linkedlist.o	\
	stringtokenizer.o	\
	str.o	\
	

