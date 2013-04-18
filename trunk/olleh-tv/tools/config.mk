#select config files
BUILD_ROOT ?= ..

export BIN_DIR = $(BUILD_ROOT)/bin
APPS_DIR = $(BUILD_ROOT)/apps

OPENSOURCE_DIR = $(BUILD_ROOT)/opensource
OPENSOURCE_PREFIX = $(OPENSOURCE_DIR)/$(ARCH)

export PKG_CONFIG_PATH = $(OPENSOURCE_PREFIX)/usr/local/lib/pkgconfig

ifeq ($(DEBUG),n)
OBJ_DIR:= $(BUILD_ROOT)/release
else
OBJ_DIR:= $(BUILD_ROOT)/debug
endif
