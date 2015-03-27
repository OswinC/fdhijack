#---------------------------------------------------------------------
# Configurations
#---------------------------------------------------------------------

# general configuration
#---------------------------------------------------------------------
INCDIR	 	= 
TOOL_CONFIG = 
LIBS		= 

# specific configuration
#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "linux_release"
PLATFORM = _LINUX
SYSDEFS  = 
CFLAGS	 = -O3 -D_NDEBUG
USRDEFS  = 
endif

#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "vcwin32_release"
PLATFORM	= _WIN32_
TOOL_CONFIG = VCWin32
CFLAGS	 	= -D_NDEBUG -D_WIN32_WINNT=0x0400
endif

#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "armlinux_release"
PLATFORM	= _LINUX
TOOL_CONFIG	= ARMLinux
CFLAGS	 	= -O3 -D_NDEBUG -Wall
endif

#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "armlinux_debug"
PLATFORM	= _LINUX
TOOL_CONFIG	= ARMLinux
CFLAGS	 	= -O0 -D_DEBUG -Wall -g
endif

#---------------------------------------------------------------------
# tool settings
#---------------------------------------------------------------------

include $(MAKEINC)/tools.mk

ifeq "$(TOOL_CONFIG)" "CONFIG_NAME"
# you can add your additional tools configuration here
endif
