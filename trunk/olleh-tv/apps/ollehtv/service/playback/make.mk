
CFLAGS +=	\
	-I$(APP_DIR)/service/playback	\
	-I$(APP_DIR)/platform	\
	-I$(APPS_DIR)/libosl/include	\
	
OBJS += \
	PlaybackService.o
