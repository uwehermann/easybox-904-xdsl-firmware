#!/bin/sh
# Copyright (C) 2010 OpenWrt.org
START=16

bindir=/opt/lantiq/bin

start() {

	### ctc ###
	if [ `/bin/grep -c -i voip /proc/cmdline` -ge 1 ] && ! [ -f /tmp/dect_demo_test ] ; then
		return
	fi
	###########

	# read in the global debug mode
	### ctc ###
#	. ${bindir}/debug_level.cfg
	if [ -e ${bindir}/debug_level.cfg ]; then
		. ${bindir}/debug_level.cfg
	fi
	###########

	# loading VINAX driver -
	cd ${bindir}
	${bindir}/inst_drv_mei_cpe.sh $ENABLE_DEBUG_OUTPUT @lines@ @devices@
}


case "$1" in
	"boot")		start	;;
	"start")	start	;;
#	"stop")		stop	;;
	*)
				echo $0 'boot  - setup and start MEI driver'
				echo $0 'start - start MEI driver'
#				echo $0 'stop  - stop MEI driver'
				;;
esac
