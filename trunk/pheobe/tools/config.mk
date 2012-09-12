#select config files
BUILD_ROOT ?= ..

PROJECT_CODE = Pheobe
PROJECT_DOMAIN = com.halkamalka
APPS_DIR = $(BUILD_ROOT)/apps
ARCH ?= $(OS_TYPE)

#ifeq ($(DEBUG),y)
#BUILD_TYPE ?= debug
#else
#ifeq ($(DEBUG),)
#BUILD_TYPE ?= debug
#else
#BUILD_TYPE ?= release
#endif
#endif

#DEBUG=$(B_REFSW_DEBUG)

ifeq ($(DEBUG),y)
BUILD_TYPE ?= debug
else
BUILD_TYPE ?= release
endif


BIN_DIR = $(APPS_DIR)/$(ARCH).$(BUILD_TYPE)/bin
LIB_DIR = $(BIN_DIR)/../lib

TARGET_DIR=$(APPLIBS)/target/$(PLATFORM)$(BCHP_VER_LOWER).$(ARCH).$(BUILD_TYPE)

#ifeq ($(DEBUG),n)
#OBJ_DIR:= $(BUILD_ROOT)/release
#else
#OBJ_DIR:= $(BUILD_ROOT)/debug
#endif


OBJ_DEST:=/opt/nfs/$(LOGNAME)/$(PLATFORM)

