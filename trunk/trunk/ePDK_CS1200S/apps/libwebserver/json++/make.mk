include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR = \
			include	\
			$(APP_DIR)/json++/include	\
			$(APP_DIR)/json++


CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -DREENTRANT $(addprefix -I, $(INCLUDE_DIR))


OBJS += \
	json_reader.o	\
	json_value.o	\
	json_writer.o

