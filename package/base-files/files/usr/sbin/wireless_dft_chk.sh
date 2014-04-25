# Script for check/get wireless related default value
# 		 		Now: WPAKEY and PIN
# 		


# Detect 2.4G band WPAKEY/PIN, if equal 0 then 
# update defalt value from uboot
# if PIN==0 WPAKEY equal 0 too, so we check PIN only

if [ "`ccfg_cli get pin_0_0@wps`" == "0" ] ; then

ccfgsal_cli gen_dft_pin

WPAKEY="`uboot_env --get --name wpa_key 2>&-`"
if [ -n "$WPAKEY" ]; then
ccfg_cli set ssnpskASCIIkey@8021x_0_0=$WPAKEY
ccfg_cli set ssnpskASCIIkey@8021x_0_1=$WPAKEY
ccfg_cli set ssnpskASCIIkey@8021x_1_0=$WPAKEY
ccfg_cli set ssnpskASCIIkey@8021x_1_1=$WPAKEY
else
ccfg_cli set ssnpskASCIIkey@8021x_0_0=1234567890
ccfg_cli set ssnpskASCIIkey@8021x_0_1=1234567890
ccfg_cli set ssnpskASCIIkey@8021x_1_0=1234567890
ccfg_cli set ssnpskASCIIkey@8021x_1_1=1234567890
fi

ccfg_cli commitcfg

fi