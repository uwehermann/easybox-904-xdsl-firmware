# This is the makefile for server of xylftp.
# Copyright (C) WANG Cong, May 17, 2007.
# GPLv2 applies.

.PHONY: all clean debug

#CC=gcc
#CFLAGS= -Wall -W -Wcast-align -Wstrict-prototypes -Wshadow
#LDFLAGS+= -L$(STAGING_DIR)/usr/lib -lssl
#CFLAGS+= -DDEBUG

all: xylftp

xylftp: xylftp.o read_configure.o write_log.o xylsocket.o telnet.o parse_cmd.o do_cmd.o
	$(CC) $(CFLAGS) $(LDFLAGS) -O2 -o $@ $^

xylftp.o: xylftp.c 
	$(CC) $(CFLAGS) -O2 -c -o $@ $^

read_configure.o: read_configure.c
	$(CC) $(CFLAGS) -O2 -c -o $@ $^

write_log.o: write_log.c
	$(CC) $(CFLAGS) -O2 -c -o $@ $^

xylsocket.o: xylsocket.c
	$(CC) $(CFLAGS) -O2 -c -o $@ $^

telnet.o: telnet.c
	$(CC) $(CFLAGS) -O2 -c -o $@ $^

parse_cmd.o: parse_cmd.c
	$(CC) $(CFLAGS) -O2 -c -o $@ $^

do_cmd.o: do_cmd.c
	$(CC) $(CFLAGS) -O2 -c -o $@ $^

clean:
	rm -f *.o xylftp xylftp.debug

debug: xylftp.debug
xylftp.debug: xylftp.c read_configure.c write_log.c xylsocket.c telnet.c parse_cmd.c do_cmd.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DDEBUG -ggdb -o $@ $^

#install: all
#	mkdir -p /etc/xylftp
#	touch /etc/xylftp/xylftp.pass
#	install -m 744 ../doc/xylftp.conf /etc/xylftp/
#	install -m 744 ./xylftp /usr/bin/

#uninstall: clean
#	rm -rf /etc/xylftp/
#	rm -rf /usr/bin/xylftp

