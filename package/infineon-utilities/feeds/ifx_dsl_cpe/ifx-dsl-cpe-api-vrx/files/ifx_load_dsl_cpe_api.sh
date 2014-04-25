#!/bin/sh
# Copyright (C) 2010 OpenWrt.org
START=18
ENABLE_DEBUG_OUTPUT=0

devices=@devices@
bindir=/opt/lantiq/bin

EXTRA_COMMANDS="dbg_on dbg_off"
EXTRA_HELP="	dbg_on	Enable debugging outputs \n
	dbg_off	Disable debugging outputs"
 
start() {

	if [ `/bin/grep -c -i voip /proc/cmdline` -ge 1 ] && ! [ -f /tmp/dect_demo_test ] ; then
		return
	fi

	[ -z "`cat /proc/modules | grep ifxos`" ] && {
		echo "Ooops - IFXOS isn't loaded, DSL CPE API will do it. Check your basefiles..."
		insmod /lib/modules/`uname -r`/drv_ifxos.ko
	}

	if [ -e ${bindir}/debug_level.cfg ]; then
	# read in the global debug mode
		. ${bindir}/debug_level.cfg
	fi
	# loading DSL CPE API driver -
	cd ${bindir}
	${bindir}/inst_drv_cpe_api.sh $ENABLE_DEBUG_OUTPUT $devices
}

dbg_on() {
	echo 8 > /proc/sys/kernel/printk
}

dbg_off() {
	echo 4 > /proc/sys/kernel/printk
}


case "$1" in
	"boot")		start	;;
	"start")	start	;;
#	"stop")		stop	;;
	*)
				echo $0 'boot  - setup and start DSL API'
				echo $0 'start - start DSL API'
#				echo $0 'stop  - stop DSL API'
				;;
esac
