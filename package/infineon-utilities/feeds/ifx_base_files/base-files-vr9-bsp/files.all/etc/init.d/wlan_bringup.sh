#!/bin/sh /etc/rc.common
#START=45
start() {
	if [ "$CONFIG_FEATURE_IFX_WIRELESS" = "1" ]; then
		/etc/rc.d/rc.bringup_wlan load
	fi
}
