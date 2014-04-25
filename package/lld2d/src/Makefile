VPATH = ./

#CC = /home/ch_liu/904DSL/staging_dir/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1/usr/bin/mips-linux-uclibc-gcc-4.3.3
LD = $(CC)
#CFLAGS = -O3 -pipe -mips32 -mtune=mips32 -Wall -Wno-unused -s -g -fpack-struct -Wpadded
#LDFLAGS = -L/home/ch_liu/904DSL/staging_dir/target-mips_r2_uClibc-0.9.30.1/usr/lib -L/home/ch_liu/904DSL/staging_dir/target-mips_r2_uClibc-0.9.30.1/lib -L/home/ch_liu/904DSL/staging_dir/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1/usr/lib -L/home/ch_liu/904DSL/staging_dir/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1/lib -Wl,-z,now 

OS_LAYER = osl-linux.c

all: lld2d lld2test

include ./common.mk

depend:
	$(CC) $(CFLAGS) -M $(DCFILES) >.depend
	$(CC) $(CFLAGS) -M $(TCFILES) >>.depend

-include .depend
