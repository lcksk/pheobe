
CXXFLAGS +=	\
	-I$(APP_DIR)/upnp	\
	
CFLAGS += `pkg-config --cflags gupnp-1.0`
#CFLAGS += `pkg-config --cflags gupnp-dlna-1.0`
CFLAGS += `pkg-config --cflags gupnp-av-1.0`



LDFLAGS += `pkg-config --libs gupnp-1.0`
#LDFLAGS += `pkg-config --libs gupnp-dlna-1.0`
LDFLAGS += `pkg-config --libs gupnp-av-1.0`


OBJS += \
	UpnpControlPoint.o
