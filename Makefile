##
## RTEMS RKI makefile
##

##
## paths for the RTEMS tools and RTEMS BSP
##
RTEMS_TOOL_BASE ?= /home/alan/Projects/rtems/4.11
RTEMS_BSP_BASE ?= /home/alan/Projects/rtems/4.11

##
## Windows paths
## RTEMS_TOOL_BASE=c:\opt\rtems\4.11
## RTEMS_BSP_BASE=e:\Projects\rtems\bsps\rtems-4.11
##

##
## Architecture Definitions
##
ARCH           ?= arm-rtems4.11
BSP            ?= raspberrypi
PREFIX         = $(RTEMS_TOOL_BASE)
RTEMS_PREFIX   = $(RTEMS_BSP_BASE)
RTEMS_ARCH_LIB = $(RTEMS_PREFIX)/$(ARCH)/$(BSP)/lib
BUILD_DIR ?= legacy-build/$(ARCH)-$(BSP)

##
## Linker flags that are needed
##
#LDFLAGS ?= mcpu=arm1176jzf-s
LDFLAGS += --pipe -B$(RTEMS_ARCH_LIB) -specs bsp_specs -qrtems $(WARNINGS)

##
## Compiler Architecture Switches
##
#ARCH_OPTS ?= -mcpu=arm1176jzf-s -D__ARM__
ARCH_OPTS += --pipe -B$(RTEMS_ARCH_LIB) -specs bsp_specs -qrtems

INCLUDE_PATH := -I. -Iinclude/

WARNINGS	= -Wall
DEBUG_OPTS	 = -g -O2

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
## RTEMS examples / demos
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
LIST_OPTS    = -Wa,-a=$(BUILD_DIR)/$*.lis

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

# Create the output directory
$(BUILD_DIR):
	$(MKDIR) -p $(BUILD_DIR)

#  Install rule is mission/target specific
#  install::$(EXE_TARGET)

OBJS2 := $(addprefix $(BUILD_DIR)/, $(OBJS))

##
## Compiler rule
##
.c.o:
	$(COMPILER)  $(COPTS) $(DEBUG_OPTS)  -c -o $(BUILD_DIR)/$@ $<

##
## Assembly Code Rule
##
.s.o:
	$(COMPILER) $(ASOPTS) $(COPTS) $(DEBUG_OPTS)  -c -o $(BUILD_DIR)/$@ $<

##
## Build Tar image
##
## Apparently linker does not like getting paths as the last argument, so
## passing $(BUILD_DIR)/tarfile creates an unusable file
## So create it like it was used to, and then move it to the build dir
$(TAR_IMAGE)::
	$(CD) rootfs; $(TAR) cf ../tarfile shell-init etc
	$(LINKER) -r --noinhibit-exec -o $(BUILD_DIR)/$(TAR_IMAGE) -b binary tarfile
	$(MV) tarfile $(BUILD_DIR)/tarfile

##
## Link Rule to make the final executable image
## add symtab.o for symbol table
$(EXE_TARGET): $(BUILD_DIR) $(OBJS)  $(TAR_IMAGE)
	$(COMPILER) $(DEBUG_FLAGS) $(LDFLAGS) -o $(BUILD_DIR)/$(EXE_TARGET) $(OBJS2) $(BUILD_DIR)/$(TAR_IMAGE) $(LIBS)
	$(OBJCOPY) -O binary --strip-all $(BUILD_DIR)/$(EXE_TARGET) $(BUILD_DIR)/$(BINARY_TARGET)
	$(SIZE) $(BUILD_DIR)/$(EXE_TARGET)

##
## Make clean rule
##
clean::
	-$(RM) -r $(BUILD_DIR)

