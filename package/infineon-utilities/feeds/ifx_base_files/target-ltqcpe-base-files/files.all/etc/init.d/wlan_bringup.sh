#!/bin/sh /etc/rc.common
START=45
start() {
	if [ "$CONFIG_FEATURE_ATHEROS_WLAN_TYPE_USB" != "1" ]; then
		if [ "$CONFIG_FEATURE_IFX_WIRELESS" = "1" ]; then
			/etc/rc.d/rc.bringup_wlan load
			/etc/rc.d/rc.bringup_wlan start
		fi
	fi
}
