include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR = \
	include 	\
	$(APPS_DIR)/framework	\
	$(APPS_DIR)/framework/service	\
	$(APP_DIR)/service/playback	\
	$(APP_DIR)/service/upnp	\
	$(APPS_DIR)/platform	\

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))

CFLAGS += `pkg-config clutter-1.0 --cflags`

#CFLAGS += `pkg-config --cflags gstreamer-0.10`
#CFLAGS += `pkg-config --cflags gio-unix-2.0`
CFLAGS += `pkg-config --cflags clutter-gst-1.0`

LDFLAGS += `pkg-config clutter-1.0 --libs`
 
#LDFLAGS += `pkg-config --libs gstreamer-0.10`
#LDFLAGS += `pkg-config --libs gio-unix-2.0`
LDFLAGS += `pkg-config --libs clutter-gst-1.0`

#LDFLAGS += -L$(APPS_DIR)/$(ARCH).$(DEBUG_SUFFIX)/lib -lframework -lplatform


vpath %.cpp %.h  $(APP_DIR)/platform
vpath %.cpp %.h  $(APP_DIR)/view
vpath %.cpp %.h  $(APP_DIR)/upnp

include $(APP_DIR)/platform/make.mk
include $(APP_DIR)/view/make.mk
include $(APP_DIR)/upnp/make.mk

OBJS += \
	main.o	\

