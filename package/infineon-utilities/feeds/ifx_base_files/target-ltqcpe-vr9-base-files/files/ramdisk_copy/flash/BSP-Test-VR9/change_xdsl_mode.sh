#!/bin/sh

### Following Assumptions are made in this script

if [ "$1" != "adsl_a" -a "$1" != "adsl_b" -a "$1" != "vdsl" -a "$1" != "eth_wan"]; then 
        echo " "
                echo "Example: $0 <adsl_a/adsl_b/vdsl/eth_wan> <acc_no/acc_yes>"
        echo " "
        exit 1
fi

if [ "$2" != "acc_no" -a "$2" != "acc_yes" ]; then 
        echo " "
                echo "Example: $0 <adsl_a/adsl_b/vdsl/eth_wan> <acc_no/acc_yes>"
        echo " "
        exit 1
fi

CURRENT_TC_LAYER="none"
ptm_interted=""
atm_interted=""

cat /proc/modules|grep ifxmips_ptm
ptm_interted="$?"
cat /proc/modules|grep ifxmips_atm
atm_interted="$?"
if [ "$ptm_interted"="1" -a "$atm_interted"="1"]; then
	CURRENT_TC_LAYER="none"
else
	if [ "$atm_interted"  = "0" -a $1="adsl_a" ]; then
        	CURRENT_TC_LAYER="adsl_a"
	fi
        if [ "$atm_interted"  = "0" -a $1="adsl_b" ]; then
                CURRENT_TC_LAYER="adsl_b"
        fi
	if [ "$ptm_interted"  = "0" -a $1="vdsl"]; then
                CURRENT_TC_LAYER="vdsl"
        fi
fi
target=/flash/rc.conf
if [ -s $target ]; then
        /bin/sed -i -e "s,^xDSL_MODE_VRX=.*,xDSL_MODE_VRX=\"$1\",g" $target
        /bin/sed -i -e "s,^ACCELERATION=.*,ACCELERATION=\"$2\",g" $target
        /usr/sbin/savecfg.sh
	if [ $CURRENT_TC_LAYER = "none"]; then
		/flash/BSP-Test-VR9/bringup_xdsl_mode.sh
	else
		if [ $1 != "CURRENT_TC_LAYER" ]; then
                        #reboot -d 0;
                        mem -s 0x1f203010 -w 0x70000002 -u;
		fi
	fi
else
	echo "Default config is not exist!"
	exit 1
fi


exit 0
