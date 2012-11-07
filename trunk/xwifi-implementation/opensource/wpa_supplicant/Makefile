
BUILD_ROOT ?= ../

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

WPA_SUPPLICANT_TOP := $(OPENSOURCE_DIR)/wpa_supplicant
WPA_SUPPLICANT_VER := 1.0
WPA_SUPPLICANT_DIR := $(OPENSOURCE_DIR)/wpa_supplicant/wpa_supplicant-$(WPA_SUPPLICANT_VER)
WPA_SUPPLICANT_SOURCE_TARBALL := $(WPA_SUPPLICANT_TOP)/wpa_supplicant-$(WPA_SUPPLICANT_VER).tar.gz

all: wpa_supplicant-source wpa_supplicant-config wpa_supplicant

wpa_supplicant-source:
	@if ! [ -e "$(WPA_SUPPLICANT_DIR)/.unpacked" ]; then \
	cd $(WPA_SUPPLICANT_TOP); \
	tar -xzkf $(WPA_SUPPLICANT_SOURCE_TARBALL); \
	touch $(WPA_SUPPLICANT_DIR)/.unpacked; \
	fi

wpa_supplicant:
	$(MAKE) -C $(WPA_SUPPLICANT_DIR)/wpa_supplicant
	$(MAKE) -C $(WPA_SUPPLICANT_DIR)/src/utils
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

clean:
	$(MAKE) -C $(WPA_SUPPLICANT_DIR)/wpa_supplicant clean
	$(call DISPLAY_RED ,##  $@  COMPLETE !!!  )

wpa_supplicant-config:
	@if ! [ -e "$(WPA_SUPPLICANT_TOP)/wpa_supplicant/.config" ]; then \
		cp $(WPA_SUPPLICANT_TOP)/defconfig $(WPA_SUPPLICANT_DIR)/wpa_supplicant/.config;	\
		cp $(WPA_SUPPLICANT_TOP)/lib.rules $(WPA_SUPPLICANT_DIR)/src;	\
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




