#!/bin/sh

case "$1" in
	start)	shift
		TAPI_WANIF=$1

		if [ A"$TAPI_WANIF" = "A" ]; then
			echo "Usage: bringup_tapidemo.sh start WAN-IF-NAME"
			exit 1
		fi

		if [ -r /etc/rc.conf ]; then
			. /etc/rc.conf
		fi

		TAPI_DEBUG_LEVEL=$tapiDebugLevel
		if [ A"$TAPI_DEBUG_LEVEL" = "A" ]; then
			TAPI_DEBUG_LEVEL=4
		fi

		TAPI_WANIF_IP=`ifconfig $TAPI_WANIF | grep 'inet addr:' | cut -f2 -d: | cut -f1 -d' '`

		TAPI_EXTRA_FLAGS_FXO=
		if [ A"$tapiDaaMode" = "A1" ]; then
			TAPI_EXTRA_FLAGS_FXO="-x"
		fi

		if [ A"$tapiKpi2Udp" = "A1" ]; then
			/opt/ifx/bin/tapidemo -d $TAPI_DEBUG_LEVEL $TAPI_EXTRA_FLAGS_FXO -q  -i $TAPI_WANIF_IP -l /opt/ifx/downloads/ &
		else
			/opt/ifx/bin/tapidemo -d $TAPI_DEBUG_LEVEL $TAPI_EXTRA_FLAGS_FXO -i $TAPI_WANIF_IP -l /opt/ifx/downloads/ &
		fi
		;;

	stop)
		killall tapidemo > /dev/null 2> /dev/null
		;;

	  *)	echo  "Usage: $0 {start WAN-IF-NAME | stop}"
		exit 1
esac
