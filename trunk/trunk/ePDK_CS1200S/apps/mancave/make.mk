include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR = \
			include	\
			$(APPS_DIR)/libwebserver/include	\
			$(APPS_DIR)/libosl/include	\
			
			

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -DREENTRANT $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -pthread
LDFLAGS += -L$(BUILD_ROOT)/bin -lpthread -lwebserver -ldl -losl -ljsonrpc -ljson

#vpath %.cpp %.h webservice
#include webservice/make.mk

#vpath %.cpp %.h  json
#include json/make.mk

vpath %.cpp %.h  plug-in
include  plug-in/make.mk

#vpath %.cpp %.h  ui
#include ui/make.mk


OBJS += \
	main.o

