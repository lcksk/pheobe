
CXXFLAGS +=	\
	-I$(APP_DIR)/web	\
	-I$(APP_DIR)/json	\
	-I$(APPS_DIR)/libwebserver	\
	-I$(APPS_DIR)/libosl/include
	
	
LDFLAGS += -lwebserver -losl -ldl

OBJS += \
	webservice.o	\
	websocket.o	\

