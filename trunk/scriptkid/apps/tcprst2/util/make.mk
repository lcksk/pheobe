#########################################################
# Include directories

INCLUDE_DIR +=	\
		$(APP_DIR)/util

CFLAGS += `pkg-config --cflags glib-2.0`
LDFLAGS += `pkg-config --libs glib-2.0`

OBJS += \
		CGKeyFile.o	\
		CFile.o
