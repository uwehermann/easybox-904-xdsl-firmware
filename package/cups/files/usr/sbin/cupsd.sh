#!/bin/sh
#
# Copyright (C) 2009 Arcadyan
# All Rights Reserved.
#

SPOOLER_DIR="/tmp/lpd"
LPD_SECT=printer
DEF_LPQ1=lpt1
DEF_LPQ2=lpt2

ONE_PRINTER=0	# 0 - one printer support, 1 - two printer support
FIX_PRINTER=lp0	# if one printer support, which USB is the printer port, lp0 - USB0, lp1 - USB1

lpd_boot()
{
	mkdir $SPOOLER_DIR

	lpd_start
}

# $1 - lp device name, lp0 or lp1
lpd_add()
{
	local LPQ

	if [ "`umngcli get enable@${LPD_SECT}`" != "1" ] ; then
		return 0
	fi

	if [ "${ONE_PRINTER}" == "0" ] ; then
		if [ -e /sys/class/usb/${1} ] ; then
			if [ `ls -al /sys/class/usb/${1} | grep -c "2-1/2-1"` -gt 0 ] ; then
				LPQ=`umngcli get queuename2@${LPD_SECT}`
				if [ -z "$LPQ" ] ; then
					LPQ=$DEF_LPQ2
				fi
			else
				LPQ=`umngcli get queuename1@${LPD_SECT}`
				if [ -z "$LPQ" ] ; then
					LPQ=$DEF_LPQ1
				fi
			fi
			rm -f ${SPOOLER_DIR}/${LPQ}
			ln -s /dev/${1} ${SPOOLER_DIR}/${LPQ}
			rm -f /dev/usb${1}
			ln -s /dev/${1} /dev/usb${1}
			sync; sync;
			/usr/bin/lpautoadd &
		fi
	else
		if [ "${1}" == "${FIX_PRINTER}" ] && [ -e /sys/class/usb/${FIX_PRINTER} ] ; then
			LPQ=`umngcli get queuename1@${LPD_SECT}`
			if [ -z "$LPQ" ] ; then
				LPQ=$DEF_LPQ1
			fi
			rm -f ${SPOOLER_DIR}/$LPQ
			ln -s /dev/${FIX_PRINTER} ${SPOOLER_DIR}/${LPQ}
		fi
		rm -f /dev/usb${1}
		ln -s /dev/${1} /dev/usb${1}
		sync; sync;
		/usr/bin/lpautoadd &
	fi
}

# $1 - lp device name, lp0 or lp1
lpd_del()
{
	if [ "${ONE_PRINTER}" == "0" ] ; then
		if [ "${1}" == "lp0" ] ; then
			ANOTHER="lp1"
		else
			ANOTHER="lp0"
		fi
		if [ -e /sys/class/usb/${ANOTHER} ] ; then
			if [ `ls -al /sys/class/usb/${ANOTHER} | grep -c "2-1/2-1"` -gt 0 ] ; then
				LPQ=`umngcli get queuename1@${LPD_SECT}`
				if [ -z "$LPQ" ] ; then
					LPQ=$DEF_LPQ1
				fi
			else
				LPQ=`umngcli get queuename2@${LPD_SECT}`
				if [ -z "$LPQ" ] ; then
					LPQ=$DEF_LPQ2
				fi
			fi
			rm -f ${SPOOLER_DIR}/${LPQ}
		else
			rm -f ${SPOOLER_DIR}/*
		fi
		rm -f /dev/usb${1}
	else
		if [ "${1}" == "${FIX_PRINTER}" ] ; then
			rm -f ${SPOOLER_DIR}/*
		fi
		rm -f /dev/usb${1}
	fi
}

lpd_start()
{
	local LPQ

	lpd_stop

	if [ "`umngcli get enable@${LPD_SECT}`" != "1" ] ; then
		return 0
	fi

	if ! ps | grep cupsd | grep -v grep; then
		/etc/init.d/cupsd start
	fi

	lpd_add lp0
	lpd_add lp1

	tcpsvd -E 0 515  /usr/lib/cups/daemon/cups-lpd -o document-format=application/octet-stream &
}

lpd_stop()
{
	local PID

	PID=`ps | grep "lpd" | grep "tcpsvd" | awk '{ print $1 }'`
	if ! [ -z "${PID}" ] ; then
		kill -KILL ${PID}
	fi

	if [ "`umngcli get enable@${LPD_SECT}`" != "1" ] ; then
		/etc/init.d/cupsd stop
	fi

	rm -f $SPOOLER_DIR/*
}

lpd_restart()
{
	lpd_start
}


case $1 in
  "boot")
		lpd_boot
		;;
  "start")
		lpd_start
		;;
  "stop")
		lpd_stop
		;;
  "restart")
		lpd_restart
		;;
  "add")
		if [ -n "$2" ] ; then
			lpd_add $2
		fi
		;;
  "del")
		if [ -n "$2" ] ; then
			lpd_del $2
		fi
		;;
  *)
		echo $0 'boot    - setup and start LPD'
		echo $0 'start   - start LPD'
		echo $0 'stop    - stop LPD'
		echo $0 'restart - restart LPD'
		echo $0 'add     - add printer queue'
		echo $0 'del     - remove printer queue'
		;;
esac
