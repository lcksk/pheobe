include $(BUILD_ROOT)/tools/tools.mk
include $(BUILD_ROOT)/tools/print.mk
include $(BUILD_ROOT)/tools/library.mk

CXXFLAGS +=	\
	-I./ 	\
	-I./plug-in/echo

OBJS += \
	echo.o
	




