#!/bin/sh /etc/rc.common

#START=51

start(){
	if [ -z "$CONFIG_FEATURE_IFX_TR69_DEVICE" -o "$CONFIG_FEATURE_IFX_TR69_DEVICE" = "0" ]; then
		if [  "$ipnat_enable" = "1" -a "$CONFIG_FEATURE_ALGS" = "1" ]; then
			/etc/rc.d/init.d/algs start
		fi
	fi
}
