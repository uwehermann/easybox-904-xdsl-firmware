# Script for check/get wireless default SSID
# Detect 2.4G band Main SSID, if equal NULL then 
# update defalt SSID according MAC

if [ "`ccfg_cli get AP_SSID@wireless`" == "" ] ; then


MAC="`getmacaddr.sh`"
if [ "$MAC" == "" ]; then
	MAC="00:01:02:03:04:05"
fi

SN="`uboot_env --get --name serial 2>&-`"
if [ "$SN" == "00000000" ]; then
	SN="R1530031409"
fi

SSID="EasyBox-${MAC:9:2}${MAC:12:2}${SN:5:1}${SN:9:1}"
#echo SSID=$SSID

ccfg_cli set AP_SSID@wireless=$SSID
ccfg_cli set AP_SSID_2@wireless=$SSID

ccfg_cli commitcfg

fi