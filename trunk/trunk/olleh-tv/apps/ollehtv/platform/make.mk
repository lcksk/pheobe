#include $(BUILD_ROOT)/tools/tools.mk
#include $(BUILD_ROOT)/tools/print.mk
#include $(BUILD_ROOT)/tools/library.mk

#INCLUDE_DIR +=	\
		./	\
		include

#CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += `pkg-config --cflags clutter-gst-1.0`

LDFLAGS += `pkg-config --libs clutter-gst-1.0`

OBJS += \
	GstPlaybin.o	\
	Platform.o

