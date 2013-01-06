#select config files
BUILD_ROOT ?= ..

export BIN_DIR = $(BUILD_ROOT)/bin
APPS_DIR = $(BUILD_ROOT)/apps

ifeq ($(DEBUG),n)
OBJ_DIR:= $(BUILD_ROOT)/release
else
OBJ_DIR:= $(BUILD_ROOT)/debug
endif

APPLIBS_DIR = $(BUILD_ROOT)/applibs

FALCON_DIR = $(APPLIBS_DIR)/broadcom/falcon/falcon_rel_5_90_188_59



