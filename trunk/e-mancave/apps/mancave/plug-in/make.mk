include $(BUILD_ROOT)/tools/tools.mk
include $(BUILD_ROOT)/tools/print.mk
include $(BUILD_ROOT)/tools/library.mk

CXXFLAGS +=	\
	-I./ 	\
	-I./plug-in	\

OBJS += \

vpath %.cpp %.hpp  $(APP_DIR)/plug-in/dlna

include $(APP_DIR)/plug-in/dlna/make.mk




