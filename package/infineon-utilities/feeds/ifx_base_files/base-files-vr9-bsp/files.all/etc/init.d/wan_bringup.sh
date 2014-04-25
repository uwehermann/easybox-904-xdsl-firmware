#!/bin/sh /etc/rc.common

#START=54

start(){
	if [ -z "$CONFIG_FEATURE_IFX_TR69_DEVICE" -o "$CONFIG_FEATURE_IFX_TR69_DEVICE" = "0" ]; then
		/usr/sbin/status_oper SET "wan_con_index" "windex" ""
		
		#804281:<IFTW-fchang>.added
		if [ "$CONFIG_FEATURE_IFX_A4" = "1" ]; then
			insmod amazon_se_ppa_ppe_a4_hal.o
			insmod ifx_ppa_api.o
		fi
		
		/etc/rc.d/rc.bringup_wan start &
		
		/usr/sbin/status_oper SET "http_wan_vcc_select"	"WAN_VCC" "1"
		/usr/sbin/status_oper SET "wan_main" "ipoption_wan" "UNKNOWN" "WAN_VCC" "0/0"
		#echo "0" > /tmp/adsl_status
	fi
}
