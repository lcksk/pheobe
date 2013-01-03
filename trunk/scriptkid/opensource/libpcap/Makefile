
BUILD_ROOT ?= ../

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

VENDOR_NAME := libnl
VENDOR_TOP := $(BUILD_ROOT)/opensource/$(VENDOR_NAME)
VENDOR_VER := 3.2.14
VENDOR_DIR := $(VENDOR_TOP)/$(VENDOR_NAME)-$(VENDOR_VER)
VENDOR_SOURCE_TARBALL := $(VENDOR_TOP)/$(VENDOR_NAME)-$(VENDOR_VER).tar.gz

all: $(VENDOR_NAME)-source $(VENDOR_NAME)-config $(VENDOR_NAME)

$(VENDOR_NAME)-source:
	@if ! [ -e "$(VENDOR_DIR)/.unpacked" ]; then \
	cd $(VENDOR_TOP); \
	tar -xvzf $(VENDOR_SOURCE_TARBALL); \
	touch $(VENDOR_DIR)/.unpacked; \
	fi

$(VENDOR_NAME):
	$(MAKE) -C $(VENDOR_DIR)
	$(MAKE) -C $(VENDOR_DIR) install
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

clean:
	$(MAKE) -C $(VENDOR_DIR) clean
	$(RM) -f $(VENDOR_TOP)/.configured
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

$(VENDOR_NAME)-config:
	@if ! [ -e "$(VENDOR_TOP)/.configured" ]; then \
        $(call fn-vendor-configure); \
	fi

install:
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

define fn-vendor-configure
    echo "================ Configure "; \
    cd $(VENDOR_DIR); \
    export AS=$(AS); \
    export LD=$(LD); \
    export CC=$(CC); \
    export CXX=$(CXX); \
    export AR=$(AR); \
    export NM=$(NM); \
    export STRIP=$(STRIP); \
    export OBJCOPY=$(OBJCOPY); \
    export OBJDUMP=$(OBJDUMP); \
    export RANLIB=$(RANLIB); \
    export CFLAGS="-fPIC -DPIC -O2"; \
    export CXXFLAGS="-fPIC -DPIC -O2"; \
    export CPPFLAGS="-fPIC -DPIC -O2"; \
    $(VENDOR_DIR)/configure --host=$(ARCH) --prefix=$(TARGET_DIR)/usr/local --disable-shared --enable-silent-rules; \
    if [ $$? -ne 0 ]; then \
        echo -e "\n**************************************************"; \
        echo "* ERROR: Failed to configure VENDOR_NAME-$(VENDOR_VER) - Aborting! *"; \
        echo "**************************************************"; \
        /bin/false; \
    else \
        touch $(VENDOR_TOP)/.configured; \
    fi
endef

include $(BUILD_ROOT)/tools/print.mk




