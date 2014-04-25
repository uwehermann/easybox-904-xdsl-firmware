SYSLOGD_FILENAME = umng_syslogd
SYSLOG_CLI_FILENAME = umng_syslog_cli
SYSLOGLIB_FILENAME = libumngsyslog.so

LIB_VERMAJOR = 0
LIB_VERMINOR = 1

LIB_CFLAGS  = $(CFLAGS) -shared -fPIC

CLI_CFLAGS  = $(CFLAGS)
CLI_LDFLAGS = $(LDFLAGS)

SYSLOGD_OBJ = umng_syslogd.o
SYSLOG_CLI_OBJ = umng_syslog_cli.o
LIB_OBJ = umng_syslog.o

all: umng_syslogd umng_syslog_cli libumngsyslog

# syslog tools
umng_syslogd:
	$(CC) $(CLI_CFLAGS) -c -o umng_syslogd.o syslog/umng_syslogd.c
	$(CC) -o $(SYSLOGD_FILENAME) $(CLI_LDFLAGS) $(SYSLOGD_OBJ)

umng_syslogd.o: syslog/umng_syslogd.c
	$(CC) $(CLI_CFLAGS) -c -o $@ $<

umng_syslog_cli: libumngsyslog
	$(CC) $(CLI_CFLAGS) -c -o umng_syslog_cli.o syslog/umng_syslog_cli.c
	$(CC) -o umng_syslog_cli $(CLI_LDFLAGS) $(SYSLOG_CLI_OBJ) \
		$(SYSLOGLIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR)


umng_syslog_cli.o: syslog/umng_syslog_cli.c
	$(CC) $(CLI_CFLAGS) -c -o $@ $<

#end of syslog tools

libumngsyslog:
	$(CC) $(LIB_CFLAGS) -c -o umng_syslog.o syslog/umng_syslog.c
	$(CC) $(LIB_CFLAGS) $(LDFLAGS) -Wl,-soname,$(SYSLOGLIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) \
		-o $(SYSLOGLIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) umng_syslog.o
	ln -sf $(SYSLOGLIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) $(SYSLOGLIB_FILENAME)
	ln -sf $(SYSLOGLIB_FILENAME).$(LIB_VERMAJOR).$(LIB_VERMINOR) $(SYSLOGLIB_FILENAME).$(LIB_VERMAJOR)

clean:
	rm -f $(SYSLOGD_FILENAME) SYSLOG_CLI_FILENAME *.o
