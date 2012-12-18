include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR = \
			include	\
			$(APPS_DIR)/include
			

#CFLAGS :=      $(filter-out -std=c89, $(CFLAGS))
#CFLAGS :=      $(filter-out -Wstrict-prototypes, $(CFLAGS))

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -DREENTRANT $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -pthread
LDFLAGS += -L$(BUILD_ROOT)/bin -lpthread

OBJS += \
	hashmap.o	\
	osl_mutex.o	\
	osl_semaphore.o	\
	osl_thread.o	\
	linkedlist.o	\
	stringtokenizer.o	\
	workerthread.o	\
	lazyworker.o	\
	osl_net.o	\
	osl_event.o
	
