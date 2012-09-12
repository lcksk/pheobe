include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/library.mk

CFLAGS += `pkg-config --cflags gstreamer-0.10`
LDFLAGS += `pkg-config --libs gstreamer-0.10`

OBJS += \
	main.o	\

