#select config files
BUILD_ROOT ?= ..

export APPS_DIR = $(BUILD_ROOT)/apps
export NEXUS_TOP = $(BUILD_ROOT)/refsw/$(PLATFORM)/nexus
export APPLIBS = $(NEXUS_TOP)/../AppLibs
export BSEAV_TOP = $(NEXUS_TOP)/../BSEAV
export ROCKFORD_TOP = ${NEXUS_TOP}/../rockford
export PSIP_TOP := ${NEXUS_TOP}/../rockford/lib/psip
export MAGNUM_TOP = ${NEXUS_TOP}/../magnum
#OPENSOURCE_TOP ?= $(APPLIBS)/opensource
export BCHP_VER_LOWER ?= $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')
export ARCH ?= mipsel-linux
export CODESIGNO_DIR=$(BUILD_ROOT)/bin/codesigno

export EXT_DIR = $(BUILD_ROOT)/refsw/ext

export DEBUG=$(B_REFSW_DEBUG)

ifeq ($(B_REFSW_DEBUG),y)
export BUILD_TYPE ?= debug
else
export BUILD_TYPE ?= release
endif


export BIN_DIR = $(APPS_DIR)/$(ARCH).$(BUILD_TYPE)/bin
export LIB_DIR = $(BIN_DIR)

#TARGET_DIR=$(APPLIBS)/target/$(PLATFORM)$(BCHP_VER_LOWER).$(ARCH).$(BUILD_TYPE)
TARGET_DIR=$(APPLIBS)/target/

#export TARGET_DIR=$(APPLIBS)/target
export APPLIBS_TARGET_LIB_DIR = $(TARGET_DIR)/usr/local/lib
export APPLIBS_TARGET_INC_DIR = $(TARGET_DIR)/usr/local/include

OBJ_DEST:=/opt/nfs/$(USER)/$(PLATFORM)

PROJECT_DOMAIN = com.halkamalka
PROJECT_CODE = foo

