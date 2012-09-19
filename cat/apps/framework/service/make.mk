include $(BUILD_ROOT)/tools/tools.mk
include $(BUILD_ROOT)/tools/print.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR += 	\
	./ 	\

CXXFLAGS += $(addprefix -I, $(INCLUDE_DIR))

OBJS += \
	ServiceManager.o	\

