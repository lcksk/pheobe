BUILD_ROOT ?= ..
#include $(BUILD_ROOT)/tools/config.mk

CFLAGS += -D_GNU_SOURCE -std=c99
#CFLAGS += -I$(LINUX)/include
#CFLAGS += -I$(TARGET_DIR)/usr/local/include
#LDFLAGS += -L$(TARGET_DIR)/usr/local/lib

