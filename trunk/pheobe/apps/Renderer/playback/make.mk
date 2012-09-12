include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR += \
	playback

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))

CFLAGS += `pkg-config --cflags gstreamer-0.10`
CFLAGS += `pkg-config --cflags gio-unix-2.0`

LDFLAGS += `pkg-config --libs gstreamer-0.10`
LDFLAGS += `pkg-config --libs gio-unix-2.0`

OBJS += \
	playback.o

