#!/usr/bin/env python
# Copyright 2013 Chris Johns (chrisj@rtems.org)
# Copyright 2013 Chirayu Desai (chirayudesai1@gmail.com)
#
# This file's license is 2-clause BSD as in this distribution's LICENSE.2 file.
#

# Waf build script for Rtems Kernel Image
#
# To configure, build and run do:
#
# $ waf configure --rtems=/Users/chris/Development/rtems/build/4.11 \
#                 --rtems-tools=/Users/chris/Development/rtems/4.11 \
#                 --rtems-bsps=sparc/sis
# $ waf
#
# Currently supported bsps are sparc/sis and arm/raspberrypi
#

import sys

try:
    import rtems_waf.rtems as rtems
except:
    print 'error: no rtems_waf git submodule; see README.waf'
    sys.exit(1)

def init(ctx):
    rtems.init(ctx)

def options(opt):
    rtems.options(opt)

def configure(conf):
    rtems.configure(conf)

def build(bld):
    rtems.build(bld)

    arch = bld.get_env()['RTEMS_ARCH']
    bsp = bld.get_env()['RTEMS_BSP']
    objcopy = bld.get_env()['OBJCOPY']

    bld.env.CFLAGS += ['-O0','-g']
    bld(rule='tar -C ' + bld.path.find_dir('rootfs').abspath() + \
              ' -cf tarfile . ;' + \
              bld.get_env()['LD'] + ' -r --noinhibit-exec -o tarfile.o -b binary tarfile ',
        target='tarfile.o',
        name='tarfile.o')
    bld(features = 'c cprogram',
        target = 'rki.elf',
        source = ['init.c', 'rtems_net.c', 'rtems_net_svc.c', \
                  'local_shell_cmds.c', 'filesys.c', 'ramdisk.c', \
                  'nvramdisk.c', 'task_cmd.c', 'hello_cmd.c', \
                  'dhrystone_cmd.c', 'whetstone_cmd.c', 'benchmarks.c', \
                  'tarfile.o'],
        includes = 'include',
        lib = 'm',
        name = 'rki.elf')
    bld(rule=objcopy + ' -O binary --strip-all rki.elf ${TGT}',
        source = 'rki.elf',
        target='rki.bin',
        name='rki.bin')
