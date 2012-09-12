#select config files
BUILD_ROOT ?= ..

APPS_DIR = $(BUILD_ROOT)/apps
NEXUS_TOP = $(BUILD_ROOT)/refsw/$(PLATFORM)/nexus
APPLIBS = $(NEXUS_TOP)/../AppLibs
BSEAV_TOP = $(NEXUS_TOP)/../BSEAV
ROCKFORD_TOP = ${NEXUS_TOP}/../rockford
PSIP_TOP := ${NEXUS_TOP}/../rockford/lib/psip
BCHP_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
ARCH ?= mipsel-linux
SYSTEM_DIR = $(BUILD_ROOT)/system

EXT_DIR = $(BUILD_ROOT)/refsw/ext

#ifeq ($(DEBUG),y)
#BUILD_TYPE ?= debug
#else
#ifeq ($(DEBUG),)
#BUILD_TYPE ?= debug
#else
#BUILD_TYPE ?= release
#endif
#endif

DEBUG=$(B_REFSW_DEBUG)

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

