export EMPTY =
export SPACE = $(EMPTY) $(EMPTY)
export UNDERBAR = _

DISPLAY_ = @echo  '$(1)  $(2)  $(3)'
ifeq "$(TERM)" "xterm"
DISPLAY_RED 		= @echo -e '\033[;31m$(1) \033[m$(2) \033[;31m$(3) \033[m\033[160P'
DISPLAY_GREEN 	= @echo -e '\033[;32m$(1) \033[m$(2) \033[;31m$(3) \033[m\033[160P'
DISPLAY_YELLOW 	= @echo -e '\033[;33m$(1) \033[m$(2) \033[;31m$(3) \033[m\033[160P'
DISPLAY_BLUE		= @echo -e '\033[;34m$(1) \033[m$(2) \033[;34m$(3)\033[m\033[160P' 
DISPLAY_MANGENTA= @echo -e '\033[;35m$(1) \033[m$(2) \033[;31m$(3) \033[m\033[160P'
DISPLAY_CYAN		= @echo -e '\033[;36m$(1) \033[m$(2) \033[;31m$(3) \033[m\033[160P'
else
DISPLAY_RED = @echo  '$(1)  $(2)  $(3)' 
DISPLAY_GREEN	= @echo  '$(1)  $(2)  $(3)' 
DISPLAY_YELLOW	= @echo  '$(1)  $(2)  $(3)' 
DISPLAY_BLUE= @echo  '$(1)  $(2)  $(3)' 
DISPLAY_MANGENTA	= @echo  '$(1)  $(2)  $(3)' 
DISPLAY_CYAN	= @echo  '$(1)  $(2)  $(3)' 
endif
