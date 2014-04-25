#!/bin/sh

# Include model information
if [ -r /etc/rc.d/model_config.sh ]; then
	. /etc/rc.d/model_config.sh
fi

if [ -r /etc/rc.conf ]; then
	. /etc/rc.conf
fi

WANPHY_TC=${wanphy_tc}
XDSLRC_A5_MODE=0
if [ A`echo $MODEL_NAME | grep A5` != "A" ]; then
	XDSLRC_A5_MODE=1
fi

if [ "$XDSLRC_A5_MODE" = "0" ]; then
	CURRENT_TC_LAYER=0
	cat /proc/modules|grep ifx_ppe
	if [ "$?"  = "0" ]; then
		CURRENT_TC_LAYER=ATM
	fi
	cat /proc/modules|grep ifxmips_ptm
	if [ "$?"  = "0" ]; then
		CURRENT_TC_LAYER=EFM
	fi
fi

# DSL Event handling script - Triggered from DSL CPE control Application
case "$DSL_NOTIFICATION_TYPE" in
	DSL_STATUS)
	if [ "$XDSLRC_A5_MODE" = "0" ]; then
		if [ "$WANPHY_TC" = "0" ]; then
                    echo "TC = ATM"
                    if [ -f /lib/modules/2.6.20.19/ifxmips_atm.ko ]; then
                        echo "inserting atm module "
                        insmod /lib/modules/2.6.20.19/ifxmips_atm.ko
                    fi
		elif [ "$WANPHY_TC" = "1" ]; then
                    echo "TC = PTM"
                    if [ -f /lib/modules/2.6.20.19/ifxmips_ptm.ko ]; then
                        echo "inserting ptm module "
                        insmod /lib/modules/2.6.20.19/ifxmips_ptm.ko
                    fi
		fi
	else
		if [ "$DSL_TC_LAYER_STATUS" = "EFM" ]; then
			if [ A`echo $MODEL_NAME | grep VINAX` != "A" ]; then
				# for AR9+VINAX, EFM mode indicates running in Vinax/VDSL mode
				target=/etc/rc.conf
				if [ -s $target ]; then
					temp=$( ls -l "$target" )
					target=${temp#* -> }
				fi
				cd `dirname $target`
				target=`basename $target`
				/bin/sed -i -e "s,^ppaA5WanMode=.*,ppaA5WanMode=\'2\',g" $target
				/usr/sbin/savecfg.sh
				reboot
			else
				echo "$MODEL_NAME doesn't support EFM/E5 acceleration mode. Only ATM/A5 mode is supported."
			fi
		fi
	fi
	echo "DSL TC Mode = $DSL_TC_LAYER_STATUS"
	;;

	DSL_INTERFACE_STATUS)
	case "$DSL_INTERFACE_STATUS" in 
	"UP")
		# DSL link up trigger
		if [ "$CONFIG_FEATURE_LED" = "1" ]; then
			echo none > /sys/class/leds/broadband_led/trigger
			echo 1 > /sys/class/leds/broadband_led/brightness
		fi	
		echo "xDSL Enter SHOWTIME!!"
		. /etc/rc.d/adsl_up
		# Enable Far-End Parameter Request
		/usr/sbin/dsl_cpe_pipe.sh ifcs 0 0 0 0 0 0

		# Adjust the upstream/downstream rates of the queues created in the system
		UPSTREAM_RATE=$(( $DSL_DATARATE_US_BC0 / 1000 ))
		#echo "US Rate $UPSTREAM_RATE kbps"
		DOWNSTREAM_RATE=$(( $DSL_DATARATE_DS_BC0 / 1000 ))
		#echo "DS Rate $DOWNSTREAM_RATE kbps"
		/etc/rc.d/ipqos_rate_update $UPSTREAM_RATE $DOWNSTREAM_RATE
		;;

	"DOWN")
		# DSL link down trigger
		if [ "$CONFIG_FEATURE_LED" = "1" ]; then
			echo none > /sys/class/leds/broadband_led/trigger
			echo 0 > /sys/class/leds/broadband_led/brightness
		fi	
		echo "xDSL Leave SHOWTIME!!"
		. /etc/rc.d/adsl_down 
		;;

	"READY")
		# DSL Handshake 2 HZ
		if [ "$CONFIG_FEATURE_LED" = "1" ]; then
			echo timer > /sys/class/leds/broadband_led/trigger
			echo 1 > /sys/class/leds/broadband_led/brightness
			echo 250 > /sys/class/leds/broadband_led/delay_on
			echo 250 > /sys/class/leds/broadband_led/delay_off
		fi	
		#echo "xDSL Handshake!!"
		;;

	"TRAINING")
		# DSL Training 4 HZ
		if [ "$CONFIG_FEATURE_LED" = "1" ]; then
			echo timer > /sys/class/leds/broadband_led/trigger
			echo 1 > /sys/class/leds/broadband_led/brightness
			echo 125 > /sys/class/leds/broadband_led/delay_on
			echo 125 > /sys/class/leds/broadband_led/delay_off
		fi	
		#echo "xDSL Training !!"
		;;
	esac
	;;

	DSL_DATARATE_STATUS)
	echo "DSL US Data Rate = "`expr $DSL_DATARATE_US_BC0 / 1000`" kbps"
	echo $DSL_DATARATE_US_BC0 > /tmp/dsl_us_rate
	echo "DSL DS Data Rate = "`expr $DSL_DATARATE_DS_BC0 / 1000`" kbps"
	echo $DSL_DATARATE_DS_BC0 > /tmp/dsl_ds_rate
	# Adjust ATM and IP QoS Rate shaping parameters based on line rate
	#. /etc/rc.d/init.d/qos_rate_update 
	# This is handled in ADSL UP Event
	;;

	DSL_DATARATE_STATUS_US)
	echo "DSL US Data Rate = "$(( $DSL_DATARATE_US_BC0 / 1000 ))" kbps"
	# echo "DSL DS Data Rate = "$(( $DSL_DATARATE_DS_BC0 / 1000 ))" kbps"
	# convert the upstream data rate in kbps to cells/sec and store in running config file
	# this will be used for bandwidth allocation during wan connection creation
	# 8 * 53 = 424
	DSL_DATARATE_US_CPS=$(( ${DSL_DATARATE_US_BC0} / 424 ))
	/usr/sbin/status_oper SET BW_INFO max_us_bw "${DSL_DATARATE_US_CPS}"
	# Adjust ATM and IP QoS Rate shaping parameters based on line rate
	#. /etc/rc.d/init.d/qos_rate_update 
	# This is handled in ADSL UP Event
	;;
esac

