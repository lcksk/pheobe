
INCLUDE_DIR =	\
		$(APP_DIR)/platform

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += `pkg-config --cflags clutter-gst-1.0`

LDFLAGS += `pkg-config --libs clutter-gst-1.0`

OBJS += \
	GstPlaybin.o	\
	Platform.o

