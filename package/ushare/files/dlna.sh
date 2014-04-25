#!/bin/sh
#
# Copyright (C) 2009 Arcadyan
# All Rights Reserved.
#

START=99

CONF_FILE=/tmp/ushare.conf

# uShare UPnP Friendly Name (default is 'uShare').
USHARE_NAME=uShare

# Interface to listen to (default is eth0).
# Ex : USHARE_IFACE=eth1
USHARE_IFACE=br-lan

# Port to listen to (default is random from IANA Dynamic Ports range)
# Ex : USHARE_PORT=49200
USHARE_PORT=49200

# Port to listen for Telnet connections
# Ex : USHARE_TELNET_PORT=1337
USHARE_TELNET_PORT=1337

# Directories to be shared (space or CSV list).
# Ex: USHARE_DIR=/dir1,/dir2
#USHARE_DIR=/tmp/usb/a1, /tmp/usb/b1
USHARE_DIR=`umngcli get DLNA_DIR@UPNPD`
[ -z "$USHARE_DIR" ] && USHARE_DIR=/tmp/usb

# Use to override what happens when iconv fails to parse a file name.
# The default uShare behaviour is to not add the entry in the media list
# This option overrides that behaviour and adds the non-iconv'ed string into
# the media list, with the assumption that the renderer will be able to
# handle it. Devices like Noxon 2 have no problem with strings being passed
# as is. (Umlauts for all!)
#
# Options are TRUE/YES/1 for override and anything else for default behaviour
USHARE_OVERRIDE_ICONV_ERR=

# Enable Web interface (yes/no)
ENABLE_WEB=no

# Enable Telnet control interface (yes/no)
ENABLE_TELNET=no

# Use XboX 360 compatibility mode (yes/no)
ENABLE_XBOX=yes
USHARE_OPTIONS=-x

# Use DLNA profile (yes/no)
# This is needed for PlayStation3 to work (among other devices)
ENABLE_DLNA=yes

update_conf() {
		rm -f $CONF_FILE
		touch $CONF_FILE
		echo "USHARE_NAME=$USHARE_NAME" >> $CONF_FILE
		echo "USHARE_IFACE=$USHARE_IFACE" >> $CONF_FILE
		echo "USHARE_PORT=$USHARE_PORT" >> $CONF_FILE
		echo "USHARE_TELNET_PORT=$USHARE_TELNET_PORT" >>  $CONF_FILE
		echo "USHARE_DIR=$USHARE_DIR" >>  $CONF_FILE		
		echo "USHARE_OVERRIDE_ICONV_ERR=" >> $CONF_FILE
		echo "ENABLE_WEB=$ENABLE_WEB" >> $CONF_FILE
		echo "ENABLE_TELNET=$ENABLE_TELNET" >> $CONF_FILE
		echo "ENABLE_XBOX=$ENABLE_XBOX" >> $CONF_FILE
		echo "USHARE_OPTIONS=$USHARE_OPTIONS" >> $CONF_FILE
		echo "ENABLE_DLNA=$ENABLE_DLNA" >> $CONF_FILE
}


boot() {
	sleep 60
	start
}

start() {
	stop
	sleep 3
	update_conf
	/usr/sbin/ushare &
}

stop() {
	killall -q -KILL ushare
}


case $1 in
  "boot")
		while [ "$2" ] ; do
			case $2 in
				"-p")
				  shift
				  USHARE_DIR=$2
				  shift
				  ;;
				"-f")
				  shift
				  USHARE_IFACE=$2
				  shift
				  ;;
			esac
		done
		boot &
		;;
  "start")
		while [ "$2" ] ; do
			case $2 in
				"-p")
				  shift
				  USHARE_DIR=$2
				  shift
				  ;;
				"-f")
				  shift
				  USHARE_IFACE=$2
				  shift
				  ;;
			esac
		done
		start &
		;;
  "stop")
		stop
		;;
  *)
		echo $0 'boot	- Setup and start DLNA server'
		echo $0 'start	- Start DLNA server'
		echo $0 'stop	- Stop DLNA server'
		echo $0 '[-p PATH]	- Directories to be shared. ex: dlna.sh start -p "/tmp/usb"'
		echo $0 '[-f IFACE]	- Interface to listen to. Default is br-lan'
		;;
esac
