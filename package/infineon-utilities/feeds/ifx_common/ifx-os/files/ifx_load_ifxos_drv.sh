#!/bin/sh
# Copyright (C) 2010 OpenWrt.org

START=15
ENABLE_DEBUG_OUTPUT=0

bindir=/opt/lantiq/bin

start() {
	[ -e ${bindir}/debug_level.cfg ] && . ${bindir}/debug_level.cfg

	cd ${bindir}
	[ -e ${bindir}/inst_driver.sh ] && ${bindir}/inst_driver.sh $ENABLE_DEBUG_OUTPUT ifxos drv_ifxos
}


case "$1" in
	"boot")		start	;;
	"start")	start	;;
#	"stop")		stop	;;
	*)
				echo $0 'boot  - setup and start IFX OS driver'
				echo $0 'start - start IFX OS driver'
#				echo $0 'stop  - stop IFX OS driver'
				;;
esac
