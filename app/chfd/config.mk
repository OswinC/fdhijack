# general configuration, for each configuration
#---------------------------------------------------------------------
PLATFORM 	= _LINUX
INCDIR	 	= 
SYSDEFS  	= 
CFLAGS	 	= 
USRDEFS  	= 
LINKFLAGS	= 
BUILD_CONF  = armlinux_release
LIBS 		= utility/fdhijack

# specific configuration
#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "linux_release"
CFLAGS	 	= -O3 -Wall
endif
#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "linux_debug"
CFLAGS		= -O0 -Wall
endif

#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "armlinux_release"
#CFLAGS	 	= -O3 -D_NDEBUG -Wall
CFLAGS		= -O3 -Wall
TOOL_CONFIG = ARMLinux
endif

#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "vcwin32_release"
PLATFORM 	= _WIN32_
TOOL_CONFIG = VCWin32
LIBS 		= common
CFLAGS	 	= /O2 -D_NDEBUG
endif

#---------------------------------------------------------------------
# tool settings
#---------------------------------------------------------------------

include $(MAKEINC)/tools.mk
ifeq "$(TOOL_CONFIG)" "CONFIG_NAME"
# you can add your additional tools configuration here
endif
