include $(BUILD_ROOT)/tools/library.mk


INCLUDE_DIR = \
			include	\
			$(APPS_DIR)/libosl/include	\
			$(FALCON_DIR)/src/include	\
			$(FALCON_DIR)/src/apps/bwl/linux	\
			$(FALCON_DIR)/src/wl/exe
			

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -DREENTRANT $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -pthread

CFLAGS += -DBCMWPA2
CFLAGS += -DBCMSUP_PSK
CFLAGS += -DWLCNT
CFLAGS += -DWLPFN
CFLAGS += -DWLC_HIGH


LDFLAGS += -L$(FALCON_DIR)/$(shell echo `uname -r`) -lbwl
LDFLAGS += -L$(BUILD_ROOT)/bin -lpthread -losl

OBJS += \
	xwifi.o	\
	xwifi_event.o	\
	xwifi_sys.o	\
	xwifi_dev.o	\
	

