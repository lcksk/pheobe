#select config files
BUILD_ROOT ?= ..

export BIN_DIR = $(BUILD_ROOT)/bin
APPS_DIR = $(BUILD_ROOT)/apps

ifeq ($(DEBUG),n)
OBJ_DIR:= $(BUILD_ROOT)/release
else
OBJ_DIR:= $(BUILD_ROOT)/debug
endif
