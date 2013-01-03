include $(BUILD_ROOT)/tools/library.mk

INCLUDE_DIR = \
			include	\
			$(APPS_DIR)/include
			

#CFLAGS :=      $(filter-out -std=c89, $(CFLAGS))
#CFLAGS :=      $(filter-out -Wstrict-prototypes, $(CFLAGS))

CFLAGS += $(addprefix -I, $(INCLUDE_DIR))
CFLAGS += -DREENTRANT $(addprefix -I, $(INCLUDE_DIR))

LDFLAGS += -L$(BUILD_ROOT)/bin -pthread 

OBJS += \
	hashmap.o	\
	kaonsys_mem.o	\
	kaonsys_mqueue.o	\
	kaonsys_mutex.o	\
	kaonsys_semaphore.o	\
	kaonsys_task.o	\
	linkedlist.o	\
	stringtokenizer.o	\
	workerthread.o	\
	lazyworker.o
