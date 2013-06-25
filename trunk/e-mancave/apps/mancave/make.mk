include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR = \
			include	\
			$(APPS_DIR)/libmulticastcapture/include	\
			$(APPS_DIR)/libframework/	\
			$(APPS_DIR)/libframework/service	\
						

#CFLAGS :=      $(filter-out -std=c89, $(CFLAGS))
#CFLAGS :=      $(filter-out -Wstrict-prototypes, $(CFLAGS))

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -DREENTRANT $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -pthread
LDFLAGS += -L$(BUILD_ROOT)/bin -lpthread

vpath %.cpp %.hpp web
include web/make.mk

vpath %.cpp %.h  json
include json/make.mk

#vpath %.cpp %.h  ui
#include ui/make.mk


OBJS += \
	main.o
