include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR = \
			include	\
			$(APP_DIR)/json++/include	\
			$(APP_DIR)/jsonrpc++/include

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -DREENTRANT $(addprefix -I, $(INCLUDE_DIR))


OBJS += \
	system.o	\
	netstring.o	\
	jsonrpc_handler.o	\
	

