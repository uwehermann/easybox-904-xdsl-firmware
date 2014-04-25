#!/bin/sh
# Copyright (C) 2007 OpenWrt.org
# Copyright (C) 2007 infineon.com

START=99

bindir=/opt/lantiq/bin

start() {
   # start the dsl daemon
   [ -e ${bindir}/show_version.sh ] && ${bindir}/show_version.sh
	[ -e /opt/lantiq/image_version ] && cat /opt/lantiq/image_version
}


case "$1" in
	"boot")		start & ;;
	"start")	start & ;;
#	"stop")		stop	;;
	*)
				echo $0 'boot  - setup and start version display'
				echo $0 'start - start version display'
#				echo $0 'stop  - stop version display'
				;;
esac
