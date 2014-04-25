#!/bin/sh
#START=59

start() {
	/usr/sbin/version.sh -r&
}


case "$1" in
	"boot")		start	;;
	"start")	start	;;
	*)
				echo $0 'boot  - show version'
				echo $0 'start - show version'
				;;
esac
