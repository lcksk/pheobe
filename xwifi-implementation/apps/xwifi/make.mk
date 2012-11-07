include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/library.mk

WPA_SUPPLICANT_SRC=$(OPENSOURCE_DIR)/wpa_supplicant/wpa_supplicant-1.0/src


INCLUDE_DIR = 	\
	$(APP_DIR)/util	\
	$(WPA_SUPPLICANT_SRC)/common	\
	$(WPA_SUPPLICANT_SRC)/utils		\

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))

vpath %.c %.h wpa_ctrl
include wpa_ctrl/make.mk

vpath %.c %.h util
include util/make.mk


OBJS += \
	xwifi.o	\
	xwifi_event.o	\
	xwifi_connect.o	\
	xwifi_power.o	\
	xwifi_wps.o	\
	xwifi_ap.o	\
	xwifi_wpa.o	\
	xwifi_wpa_parser.o	\
	xwifi_helper.o
	
	
