BUILD_ROOT ?= ../..

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

ifeq ($(DEBUG),y)
DEBUG_SUFFIX=debug
CFLAGS = -Wall -g
LDFLAGS =
else
DEBUG_SUFFIX=release
CFLAGS = -Wall
endif

include $(BUILD_ROOT)/tools/library.mk

LIB_NAME=sampleplayer
LIB_VER=0.1.0
SO_NAME = $(LIB_NAME).0

APP_DIR = $(shell echo `pwd`)
IS_LIB = n
APP = $(LIB_NAME)
#APP = $(LIB_NAME).$(LIB_VER)

OBJ_DIR = $(DEBUG_SUFFIX)
BIN = $(OBJ_DIR)/$(APP)

include $(APP_DIR)/make.mk

ifeq ($(IS_LIB),y)
CFLAGS += --shared -fPIC
endif

# Add C++ only options

CFLAGS += -lrt -pthread -DREENTRANT 
CXXFLAGS += -Wall -MD
CXXFLAGS +=	-fno-rtti -fno-exceptions
CXXFLAGS += $(CFLAGS)

.PHONY : $(BIN) clean install $(OBJ_DIR)

$(BIN) : $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) $(addprefix $(OBJ_DIR)/,$(OBJS))
	@echo [Linking... $(notdir $@)]
ifeq ($(IS_LIB),y)
	@$(Q_)$(CC) $(CXXFLAGS) $(LDFLAGS) -o $(BIN) $(addprefix $(OBJ_DIR)/,$(OBJS))
	@echo [Copy $(notdir $(APP)) to $(LIB_DIR)
	$(Q_)$(CP) $@ $(LIB_DIR)
#	$(Q_)$(CD) $(BIN_DIR) && $(LN) $(APP) $(LIB_NAME)
else
	@$(Q_)$(CXX) $(CXXFLAGS) $(CFLAGS)  -o $(BIN) $(EXE_LIBS) $(addprefix $(OBJ_DIR)/,$(OBJS)) $(LDFLAGS)
	@echo [Copy $(notdir $@) to $(BIN_DIR)
	$(Q_)$(CP) $@ $(BIN_DIR)
endif

	
$(LIB_DIR) :
	@echo [mkdir : $(LIB_DIR) ]
	$(Q_)$(MKDIR) $(LIB_DIR)

$(BIN_DIR) :
	@echo [mkdir : $(BIN_DIR) ]
	$(Q_)$(MKDIR) $(BIN_DIR)
	
$(OBJ_DIR) :
	@echo [mkdir : $@ ]
	$(Q_)$(MKDIR) $@
	
$(OBJ_DIR)/%.o : %.c
	@echo [Compile... $(notdir $<) ]
	@$(Q_)$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJ_DIR)/%.o : %.cpp
	@echo [Compile... $(notdir $<) ]
	@$(Q_)$(CXX) $(CFLAGS) $(CXXFLAGS) -c $< -o $@
	
clean :
	$(Q_)$(RM) -rf $(OBJ_DIR)
	
install :
ifeq ($(IS_LIB),y)
	$(CP) $(BIN) $(OBJ_DEST)/usr/local/lib/
else
	$(CP) $(BIN) $(OBJ_DEST)/usr/local/bin/
endif


include $(BUILD_ROOT)/tools/print.mk

	

