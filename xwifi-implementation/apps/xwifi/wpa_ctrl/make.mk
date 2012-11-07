include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR += 	\
	$(APP_DIR)/	\
	$(APP_DIR)/util	\
	$(OPENSOURCE_DIR)/wpa_supplicant/wpa_supplicant-1.0/src	\
	$(OPENSOURCE_DIR)/wpa_supplicant/wpa_supplicant-1.0/src/common	\
	$(OPENSOURCE_DIR)/wpa_supplicant/wpa_supplicant-1.0/src/utils		\

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))

CFLAGS += -DCONFIG_CTRL_IFACE
CFLAGS += -DCONFIG_CTRL_IFACE_UNIX
CFLAGS += -DCONFIG_P2P

OBJS += \
	wpa_ctrl.o	\
	os_unix.o	\
	eloop.o	\
	wpa_debug.o	

#	edit.o	\
	common.o	\
	wpa_cli.o	\
	
	
