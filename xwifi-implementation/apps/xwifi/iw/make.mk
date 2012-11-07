include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR += 	\
	iw
	

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
LDFLAGS += -lnl

OBJS += \
	xwifi_nl80211.o	\
	sections.o	\
	xwifi_nl80211_event.o	\
	genl.o	\
	reg.o	\
	util.o	\
	status.o	\
	reason.o

