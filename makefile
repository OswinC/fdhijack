# environment settings
include $(MAKEINC)/default.mk

include $(CONFIGFILE)
include $(MAKEINC)/libs.mk

# Targets
all: staticlib
	
