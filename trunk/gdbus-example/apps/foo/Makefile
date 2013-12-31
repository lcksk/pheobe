BUILD_ROOT ?= ../..

include $(BUILD_ROOT)/tools/config.mk
include $(BUILD_ROOT)/tools/tools.mk

include $(BUILD_ROOT)/tools/library.mk

LIB_NAME=
LIB_VER=
SO_NAME = $(LIB_NAME).0

APP_DIR = $(shell echo `pwd`)
IS_LIB = n
APP = foo

OBJ_DIR = $(DEBUG_SUFFIX)
BIN = $(OBJ_DIR)/$(APP)

include $(APP_DIR)/make.mk

ifeq ($(DEBUG),y)
DEBUG_SUFFIX=debug
CFLAGS += -Wall -g
else
ifeq ($(DEBUG),)
DEBUG_SUFFIX=debug
CFLAGS += -Wall -g
else
DEBUG_SUFFIX=release
endif
endif

ifeq ($(IS_LIB),y)
CFLAGS += --shared -fPIC
endif

# Add C++ only options

CFLAGS += -pthread -DREENTRANT
CXXFLAGS += -Wall -MD
#CXXFLAGS +=	-fno-rtti -fno-exceptions
#CXXFLAGS +=	-fno-exceptions
CXXFLAGS += $(CFLAGS)
#CFLAGS += -static
LDFLAGS += -lstdc++

.PHONY : $(BIN) clean install gdbus $(OBJ_DIR)

$(BIN) : $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) $(addprefix $(OBJ_DIR)/,$(OBJS))
	@echo [Linking... $(notdir $@)]
ifeq ($(IS_LIB),y)
	$(Q_)$(CXX) $(CXXFLAGS) -o $(BIN) $(addprefix $(OBJ_DIR)/,$(OBJS)) $(EXT_OBJS) $(LDFLAGS)  
	@echo [Copy $(notdir $(APP)) to $(LIB_DIR)
	$(Q_)$(CP) $@ $(LIB_DIR)
#	$(Q_)$(CD) $(BIN_DIR) && $(LN) $(APP) $(LIB_NAME)
else
	$(Q_)$(CXX) $(CXXFLAGS) -o $(BIN) $(addprefix $(OBJ_DIR)/,$(OBJS)) $(EXT_OBJS) $(LDFLAGS) 
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
	$(Q_)$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJ_DIR)/%.o : %.cpp
	@echo [Compile... $(notdir $<) ]
	$(Q_)$(CXX) $(CFLAGS) $(CXXFLAGS) -c $< -o $@
	
clean :
	$(Q_)$(RM) -rf $(OBJ_DIR)
	
install :
ifeq ($(BUILD_APPLIBS), y)
	$(CP) $(BIN) $(OBJ_DEST)/usr/local/bin
else
	$(CP) $(BIN) $(OBJ_DEST)
endif

gdbus :
	@echo [gdbus... $(notdir $@)]
	@if [ -e $(APP_DIR)/client ]; then	\
	echo gdbus directory exist;	\
	$(CD) $(APP_DIR)/$@; gdbus-codegen --interface-prefix $(PROJECT_DOMAIN).GDBus.$(PROJECT_CODE).ObjectManager. \
              --generate-c-code gdbus-$(APP)-generated                      \
              --c-namespace $(PROJECT_CODE)                       \
              --c-generate-object-manager                 \
              --generate-docbook generated-docs                       \
              gdbus-$(APP)-objectmanager.xml;	\
     fi

include $(BUILD_ROOT)/tools/print.mk

	

