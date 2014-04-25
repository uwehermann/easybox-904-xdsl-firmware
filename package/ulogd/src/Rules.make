#

prefix=/usr/local
exec_prefix=${prefix}
ETCDIR=${prefix}/etc
BINDIR=${exec_prefix}/sbin

#ULOGD_CONFIGFILE=${prefix}/etc/ulogd.conf
ULOGD_CONFIGFILE=/tmp/ulogd.conf

ULOGD_LIB_PATH=${exec_prefix}/lib/ulogd

# Path of libipulog (from iptables)
LIBIPULOG=./libipulog
INCIPULOG=-I./libipulog/include
INCCONFFILE=-I./conffile

#CC=gcc
#LD=ld
INSTALL=/usr/bin/install -c

#CFLAGS=-g -O2  -Wall
CFLAGS+=-g -O2  -Wall
c_CONFIGFILE=\"$(ULOGD_CONFIGFILE)\"
# doesn't work for subdirs
#CFLAGS+=$(INCIPULOG) $(INCCONFFILE)
#CFLAGS+=-I/lib/modules/`uname -r`/build/include
#CFLAGS+= -DHAVE_LIBDL=1 -DHAVE_DIRENT_H=1 -DSTDC_HEADERS=1 -DHAVE_FCNTL_H=1 -DHAVE_UNISTD_H=1 -DHAVE_VPRINTF=1 -DHAVE_SOCKET=1 -DHAVE_STRERROR=1 
#CFLAGS+=-g -DDEBUG -DDEBUG_MYSQL -DDEBUG_PGSQL

LIBS=-ldl 


# Names of the plugins to be compiled
ULOGD_SL:=BASE OPRINT PWSNIFF LOGEMU LOCAL SYSLOG

# mysql output support
#ULOGD_SL+=MYSQL
MYSQL_CFLAGS=-I 
MYSQL_LDFLAGS= 

# postgreSQL output support
#ULOGD_SL+=PGSQL
PGSQL_CFLAGS=-I 
PGSQL_LDFLAGS= 

# mysql output support
#ULOGD_SL+=SQLITE3
SQLITE3_CFLAGS=-I 
SQLITE3_LDFLAGS= 

