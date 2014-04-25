# Generated automatically from Makefile.in by configure.
#

#  Normally You should not need to change anything below
#
include ../Rules.make

CFLAGS+=-I.. -I../libipulog/include -I../include
SH_CFLAGS:=$(CFLAGS) -fPIC

SHARED_LIBS+=$(foreach T,$(ULOGD_SL),ulogd_$(T).so) ulogd_BASE_ebt.so


all: $(SHARED_LIBS)

distrib:

printpkt.o: printpkt.c
	$(CC) $(SH_CFLAGS) -o $@ -c $<

$(SHARED_LIBS): %.so: %_sh.o
	$(LD) -shared -o $@ $< -lc

ulogd_SYSLOG.so: printpkt.o ulogd_SYSLOG_sh.o 
	$(LD) -shared -o $@ $^ -lc

%_sh.o: %.c
	$(CC) $(SH_CFLAGS) -o $@ -c $<

clean:
	$(RM) $(SHARED_LIBS) *.o

distclean:
	$(RM) Makefile

install: all
	$(INSTALL) -m 755 -d $(DESTDIR)$(ULOGD_LIB_PATH)
	$(INSTALL) -m 755 *.so $(DESTDIR)$(ULOGD_LIB_PATH)
