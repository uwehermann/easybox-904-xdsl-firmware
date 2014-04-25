#!/bin/sh

start() {
	if [ A"$CONFIG_PACKAGE_LQ_COC_APP_PMD" = "A1" ]; then
		echo "start Power Management Daemon"
        /opt/lantiq/bin/pmd 20 60 85 200 20 30 0&
	fi
}


case "$1" in
	"boot")		start &	;;
	"start")	start &	;;
#	"stop")		stop	;;
	*)
			echo $0 'boot  - init  Power Management Daemon'
			echo $0 'start - start Power Management Daemon'
#			echo $0 'stop  - stop  Power Management Daemon'
esac
