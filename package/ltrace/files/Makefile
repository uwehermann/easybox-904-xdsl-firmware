#
# ltrace's Makefile.in
#

#OS		:= $(shell uname -s)
OS		:= linux-gnu

TOPDIR		= $(shell pwd)

prefix		= /usr
sysconfdir	= /etc
bindir		= $(prefix)/bin
mandir		= /usr/man
docdir		= $(prefix)/share/doc/ltrace

CC		=	mips-linux-uclibc-gcc
CFLAGS		=	-Wall -Os -pipe -mips32 -mtune=mips32 -funit-at-a-time -fhonour-copts  -Wp,-dD -save-temps  -v  -g
#CPPFLAGS	=	-iquote $(TOPDIR) -iquote $(TOPDIR)/sysdeps/$(OS) -DSYSCONFDIR=\"$(sysconfdir)\" -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/usr/include -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/include -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/usr/include/libelf
CPPFLAGS	=	-I $(TOPDIR) -I $(TOPDIR)/sysdeps/$(OS) -I- -DSYSCONFDIR=\"$(sysconfdir)\"  -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/usr/include -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/include -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/usr/include/libelf
#CPPFLAGS	=	-I $(TOPDIR) -I $(TOPDIR)/sysdeps/$(OS) -I- -I$(TOOLCHAIN_DIR)/include -I $(TOPDIR)/usr/include -I $(TOPDIR)/usr/include/libelf -DSYSCONFDIR=\"$(sysconfdir)\" -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/usr/include -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/include -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/usr/include/libelf
#CPPFLAGS	=	-I $(TOPDIR) -I $(TOPDIR)/sysdeps/$(OS) -I- -I $(TOPDIR)/usr/include -I $(TOPDIR)/usr/include/libelf -DSYSCONFDIR=\"$(sysconfdir)\" -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/usr/include -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/include -I/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/usr/include/libelf
LDFLAGS		=	-L/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/toolchain-mips_gcc3.4.6/lib -L/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/usr/lib -L/home/hwangtf/wrtprj/ammon_ar9/BT_ipsec/staging_dir/mips/lib   -Wl,--verbose
LIBS		=	-lelf -lsupc++ -liberty

INSTALL		=	install -c
INSTALL_FILE	=	$(INSTALL) -p    -m  644
INSTALL_PROGRAM	=	$(INSTALL) -p    -m  755
INSTALL_SCRIPT	=	$(INSTALL) -p    -m  755
INSTALL_DIR	=	$(INSTALL) -p -d -m  755

OBJ	=	libltrace.o options.o elf.o output.o read_config_file.o  \
		execute_program.o handle_event.o display_args.o          \
		breakpoints.o proc.o demangle.o dict.o debug.o summary.o

VERSION	=	0.5.3

all:		ltrace htf

ltrace:		main.o libltrace.a
		$(CC) $(LDFLAGS) $^ $(LIBS) -o $@   -Wl,-Map,ltrace.map -Wl,--cref

libltrace.a: 	sysdeps/sysdep.o $(OBJ)
		$(AR) rcv $@ $^

$(OBJ):		sysdeps/sysdep.o

sysdeps/sysdep.o: dummy
		$(MAKE) -C sysdeps/$(OS)

htf:htf.c
		$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LIBS) -o $@   -Wl,-Map,htf.map -Wl,--cref


clean-deja:
		$(RM) testrun.log testrun.sum
		cd testsuite; make clean

clean:		clean-deja
		$(MAKE) -C sysdeps/$(OS) clean
		rm -f ltrace main.o libltrace.a $(OBJ)
		rm -f *~ *.bak a.out core

distclean:	clean
		rm -f config.h Makefile

realclean:	distclean

install:	ltrace
		$(INSTALL_DIR) $(DESTDIR)$(bindir) $(DESTDIR)$(docdir) $(DESTDIR)$(mandir)/man1
		$(INSTALL_DIR) $(DESTDIR)$(sysconfdir)
		$(INSTALL_PROGRAM) ltrace $(DESTDIR)$(bindir)
		$(INSTALL_FILE) etc/ltrace.conf $(DESTDIR)$(sysconfdir)
		$(INSTALL_FILE) COPYING README TODO BUGS ChangeLog $(DESTDIR)$(docdir)
		$(INSTALL_FILE) ltrace.1 $(DESTDIR)$(mandir)/man1

check:
		cd testsuite;cat /proc/version;uptime;free -m;$(MAKE) check

dummy:

.PHONY:		all clean distclean dist install dummy

.EXPORT_ALL_VARIABLES:
