
BUILD_ROOT ?= ../

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

HOSTAPD_TOP := $(BUILD_ROOT)/opensource/hostapd
HOSTAPD_VER := 1.0
HOSTAPD_DIR := $(BUILD_ROOT)/opensource/hostapd/hostapd-$(HOSTAPD_VER)
HOSTAPD_SOURCE_TARBALL := $(HOSTAPD_TOP)/hostapd-$(HOSTAPD_VER).tar.gz

all: hostapd-source hostapd-config hostapd

hostapd-source:
	@if ! [ -e "$(HOSTAPD_DIR)/.unpacked" ]; then \
	cd $(HOSTAPD_TOP); \
	tar -xzkf $(HOSTAPD_SOURCE_TARBALL); \
	touch $(HOSTAPD_DIR)/.unpacked; \
	fi

hostapd:
	$(MAKE) -C $(HOSTAPD_DIR)/hostapd
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

clean:
	$(MAKE) -C $(HOSTAPD_DIR)/hostapd clean
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

hostapd-config:
	@if ! [ -e "$(HOSTAP_DIR)/hostapd/.config" ]; then \
    	cp $(HOSTAPD_DIR)/hostapd/defconfig $(HOSTAPD_DIR)/hostapd/.config;	\
	fi

#jpeg:
#	@make -C $(HOSTAP_DIR)
#	@if ! [ -e "$(HOSTAP_DIR)/.libs/libjpeg.a" ]; then \
	    $(MKDIR) $(HOSTAP_DIR)/.libs; \
	    $(CP) $(HOSTAP_DIR)/libjpeg.a $(HOSTAP_DIR)/.libs/. ;\
	fi

#define fn-hostap-configure
#   echo "*******************************"; \
    echo "* Configuring JPEG library *"; \
    echo "*******************************"; \
    cd $(JPEG_DIR); \
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
	$(JPEG_DIR)/configure --host=$(B_REFSW_ARCH) --prefix=/usr --disable-shared; \
    if [ $$? -ne 0 ]; then \
        echo -e "\n***********************************************"; \
        echo "* ERROR: Failed to configure jpeg - Aborting! *"; \
        echo "***********************************************"; \
        /bin/false; \
    else \
        touch $(JPEG_DIR)/.configured; \
    fi
#endef


include $(BUILD_ROOT)/tools/print.mk




