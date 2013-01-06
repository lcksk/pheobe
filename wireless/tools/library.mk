BUILD_ROOT ?= ..

export LD_LIBRARY_PATH += $(BUILD_ROOT)/bin
#CFLAGS += -D_GNU_SOURCE 

#CFLAGS += `pkg-config --cflags gstreamer-0.10`
#CFLAGS += `pkg-config --cflags libdvbpsi`

#CFLAGS += `pkg-config --cflags libupnp`
#CFLAGS += `pkg-config --cflags libxml-2.0`

#LDFLAGS += `pkg-config --libs gstreamer-0.10`
#LDFLAGS += `pkg-config --libs libdvbpsi`
#LDFLAGS += `pkg-config --libs libupnp`
#LDFLAGS += `pkg-config --libs libxml-2.0`

