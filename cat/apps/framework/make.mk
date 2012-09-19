include $(BUILD_ROOT)/tools/tools.mk
include $(BUILD_ROOT)/tools/print.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR =	\
		./	\
		include

CXXFLAGS += $(addprefix -I, $(INCLUDE_DIR))


vpath %.cpp %.c %.h service
include service/make.mk

OBJS += \
	CWorkerThread.o 

