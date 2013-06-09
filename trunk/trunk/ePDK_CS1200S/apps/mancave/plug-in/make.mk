include $(BUILD_ROOT)/tools/tools.mk
include $(BUILD_ROOT)/tools/print.mk
include $(BUILD_ROOT)/tools/library.mk

CXXFLAGS +=	\
	-I./ 	\
	-I./plug-in	\

OBJS += \

vpath %.cpp %.h  $(APP_DIR)/plug-in/echo

include $(APP_DIR)/plug-in/echo/make.mk




