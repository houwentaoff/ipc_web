################################################################################
################################################################################
##
## Makefile -- makefile for ADI test application
##
## (C) Copyright 2012-2014 by Goke Microelectronics Co.,Ltd
##
## Version: $Id: Makefile 2359 2014-11-12 09:07:32Z huangmin $
##
## Mandatory settings:
##
## o TOPDIR      = the toplevel directory (using slashes as path separator)
## o TARGET      = the machine qualifier to generate code for
##
################################################################################
################################################################################

  TOPDIR = ../..
  SUBDIR = subsystem/web

include $(TOPDIR)/env/make/Makefile.config
ECHO := echo
################################################################################
# source files
################################################################################

#SOURCES += src/main.c
#SOURCES += src/shell.c
#SOURCES += src/parser.c
#SOURCES += src/video.c
#SOURCES += src/isp.c

################################################################################
# define define user cc flags
################################################################################

#CCFLAGS_USER += -O3 -DDEBUG
#CCFLAGS_USER += -I $(TOPDIR)/legacy/build/include
#CCFLAGS_USER += -I $(TOPDIR)/adi/include
#CCFLAGS_USER += -I $(TOPDIR)/env/temp/headers

################################################################################
# define define user ld flags
################################################################################

#LDFLAGS_USER += -lrt 
#LDFLAGS_USER += -L $(TOPDIR)/legacy/prebuild/imgproc
#LDFLAGS_USER += -limg_algo_a5s -limg_dsp_a5s

################################################################################
# define library used
################################################################################

#LIBRARIES = adi

################################################################################
# define target name
################################################################################

PROGRAM = boa-0.94.13/src/boa

################################################################################
# define local file to be installed
################################################################################
LOCAL_FILE += boa-0.94.13/boa.conf

BOA_LOG_FILES := $(ROOTFS_TOP)/var/log/boa/access_log $(ROOTFS_TOP)/var/log/boa/error_log

################################################################################
# define local file install location
# if this was commented out, the target will be installed into the default 
# location:
#   $(ROOTFS_TOP)/usr/local/bin/
################################################################################

INSTALL_LOCAL_FILE_DIR = $(ROOTFS_TOP)/etc/boa/boa.conf

################################################################################
# define target install location
# if this was commented out, the target will be installed into the default 
# location:
#   $(ROOTFS_TOP)/usr/local/bin/
################################################################################

# INSTALL_BIN_DIR = 

################################################################################
# define user targets
################################################################################

default: install

#include $(TOPDIR)/env/make/Makefile.rules

clean: 
	#@make -C www clean
	@make -C server clean

build: 
	@make -C server
	@make -C www

install-configname:
	@make -C server install

install-program:
	@make -C www install
	@make -C server install

install: install-configname install-program 
#install-debug-program install-file

headers:

all: clean build install



#-include $(TOPDIR)/env/make/Makfile.dep


