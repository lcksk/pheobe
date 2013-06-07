include $(BUILD_ROOT)/tools/tools.mk
include $(BUILD_ROOT)/tools/print.mk
include $(BUILD_ROOT)/tools/library.mk

CXXFLAGS +=	\
	-I./ 	\
	-I./json	\

OBJS += \
	json_writer.o	\
	json_value.o	\
	json_reader.o




