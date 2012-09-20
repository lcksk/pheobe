
CXXFLAGS +=	\
	-I$(APP_DIR)/service/upnp	\
	
CFLAGS += `pkg-config --cflags gupnp-1.0`
CFLAGS += `pkg-config --cflags gupnp-dlna-1.0`


LDFLAGS += `pkg-config --libs gupnp-1.0`
LDFLAGS += `pkg-config --libs gupnp-dlna-1.0`


OBJS += \
	UpnpControlPointService.o
