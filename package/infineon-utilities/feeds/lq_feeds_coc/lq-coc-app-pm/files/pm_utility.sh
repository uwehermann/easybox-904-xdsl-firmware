#!/bin/sh
START=78

start() {
	if [ A"$CONFIG_PACKAGE_LQ_COC_APP_PM" = "A1" ] && 
       [ A"$CONFIG_IFX_ETHSW_API_COC_PMCU" = "A1" ]; then
           echo "set polling mode for Ethernet Switch"
           /opt/lantiq/bin/pmcu_utility -s1 -mswitch -n0&
	fi
}


case "$1" in
	"boot")		start &	;;
	"start")	start &	;;
#	"stop")		stop	;;
	*)
			echo $0 'boot  - init  PMCU utility'
			echo $0 'start - start PMCU utility'
#			echo $0 'stop  - stop  PMCU utility'
esac
