##
## RTEMS RKI makefile
##

##
## paths for the RTEMS tools and RTEMS BSP
##
RTEMS_TOOL_BASE=/home/alan/Projects/rtems/4.11
RTEMS_BSP_BASE=/home/alan/Projects/rtems/4.11

##
## Architecture Definitions
##
ARCH           = sparc-rtems4.11
BSP            = sis
PREFIX         = $(RTEMS_TOOL_BASE)
RTEMS_PREFIX   = $(RTEMS_BSP_BASE)
RTEMS_ARCH_LIB = $(RTEMS_PREFIX)/$(ARCH)/$(BSP)/lib

##
## Linker flags / options
##
LDFLAGS = --pipe -B$(RTEMS_ARCH_LIB) -specs bsp_specs -qrtems $(WARNINGS) -mcpu=cypress

##
## Compiler Architecture Switches 
##
ARCH_OPTS = --pipe -mcpu=cypress -B$(RTEMS_ARCH_LIB) -specs bsp_specs -qrtems -D__SPARC__


#################################################################################

##
## Let the makefile find the source
##
VPATH := ../

INCLUDE_PATH := -I../ -I.

WARNINGS	= -Wall
DEBUG_OPTS      = -g -O0

##
## define build products
##
EXE_TARGET       = rki.elf
BINARY_TARGET    = rki.bin
TAR_IMAGE        = tarfile.o
LINKSCRIPT       = linkcmds

##
## Objects to build
##
OBJS = init.o rtems_net.o rtems_net_svc.o local_shell_cmds.o filesys.o 

##
## Optional objects in src directory
##
OBJS += ramdisk.o
OBJS += nvramdisk.o

##
## RTEMS examples / Demos
## 
OBJS += task_cmd.o
OBJS += hello_cmd.o
OBJS += dhrystone_cmd.o
OBJS += whetstone_cmd.o
OBJS += benchmarks.o

##
## Libraries to link in
##
LIBS = -lm

##
## Optional libs depending on the features needed
##
# LIBS += -lftpd
# LIBS += -ltelnetd
# LIBS += -lnfs

##
## Extra Cflags for Assembly listings, etc.
##
LIST_OPTS    = -Wa,-a=$*.lis

##
## General gcc options that apply to compiling and dependency generation.
##
COPTS=$(LIST_OPTS) $(ARCH_OPTS) $(WARNINGS) $(INCLUDE_PATH)  -I.

##
## Extra defines and switches for assembly code
##
ASOPTS = -P -xassembler-with-cpp

####################################################
## Host Development System and Toolchain defintions
##
## Host OS utils
##
RM=rm -f
CP=cp
MV=mv
CD=cd
TAR=tar
CAT=cat
MKDIR=mkdir

##
## Compiler tools
##
COMPILER   = $(ARCH)-gcc
ASSEMBLER  = $(ARCH)-gcc
LINKER	   = $(ARCH)-ld
AR	   = $(ARCH)-ar
NM         = $(ARCH)-nm
OBJCOPY    = $(ARCH)-objcopy
SIZE       = $(ARCH)-size

###############################################################################################
##
## Build Targets
##

##
## The default "make" target is the subsystem object module.
##
default::$(EXE_TARGET)

#  Install rule is mission/target specific
#  install::$(EXE_TARGET)

##
## Compiler rule
##
.c.o:
	$(COMPILER)  $(COPTS) $(DEBUG_OPTS)  -c -o $@ $<

##
## Assembly Code Rule
##
.s.o:
	$(COMPILER) $(ASOPTS) $(COPTS) $(DEBUG_OPTS)  -c -o $@ $<

##
## Build Tar image
##
$(TAR_IMAGE)::
	$(CD) rootfs; $(TAR) cf ../tarfile shell-init etc 
	$(LINKER) -r --noinhibit-exec -o $(TAR_IMAGE) -b binary tarfile

##
## Link Rule to make the final executable image
## add symtab.o for symbol table
$(EXE_TARGET): $(OBJS)  $(TAR_IMAGE)
	$(COMPILER) $(DEBUG_FLAGS) $(LDFLAGS) -o $(EXE_TARGET) $(OBJS) $(TAR_IMAGE) $(LIBS)
	$(OBJCOPY) -O binary --strip-all $(EXE_TARGET) $(BINARY_TARGET)
	$(SIZE) $(EXE_TARGET)

##
## Make clean rule
##
clean::
	-$(RM) $(OBJS) $(EXE_TARGET) $(BINARY_TARGET)  *.slf
	-$(RM) $(TAR_IMAGE) tarfile
	-$(RM) *.lis
	-$(RM) *.img


