# Generated automatically from Makefile.in by configure.
#
include ../Rules.make

CFLAGS+=-I.. -I../libipulog/include -I../include
CFLAGS+=-I./pcap
SH_CFLAGS:=$(CFLAGS) -fPIC

#  Normally You should not need to change anything below
#

SHARED_LIBS=ulogd_PCAP.so ulogd_PCAP_ebt.so ulogd_PCAP_sock.so

all: $(SHARED_LIBS)

distrib:

$(SHARED_LIBS): %.so: %_sh.o
	$(LD) -shared -o $@ $<
#	$(LD) -shared -o $@ $< -lc -lpcap

%_sh.o: %.c
	$(CC) $(SH_CFLAGS) -o $@ -c $<
	
clean:
	$(RM) $(SHARED_LIBS) *.o

distclean:
	$(RM) Makefile

install: all
	$(INSTALL) -m 755 -d $(DESTDIR)$(ULOGD_LIB_PATH)
	$(INSTALL) -m 755 *.so $(DESTDIR)$(ULOGD_LIB_PATH)
