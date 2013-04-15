include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR = \
			$(APP_DIR)/include	\
			$(APPS_DIR)/include	\
			$(APPS_DIR)/libosl/include
			

#CFLAGS :=      $(filter-out -std=c89, $(CFLAGS))
#CFLAGS :=      $(filter-out -Wstrict-prototypes, $(CFLAGS))

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -DREENTRANT $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -pthread
LDFLAGS += -L$(BUILD_ROOT)/bin -lpthread

OBJS += \
	multicastcapture.o
	
	
