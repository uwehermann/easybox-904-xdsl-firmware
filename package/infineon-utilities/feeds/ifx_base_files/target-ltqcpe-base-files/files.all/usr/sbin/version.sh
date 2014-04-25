#!/bin/sh
if [ -r /etc/rc.d/model_config.sh ]; then
	. /etc/rc.d/model_config.sh
	CPU_EXTRA_INFO=$CONFIG_IFX_CONFIG_CPU"-v"
	BOARD_EXTRA_INFO="`echo $CONFIG_IFX_CONFIG_CPU|cut -b8`"
	MODEL_EXTRA_INFO="-"${BOARD_EXTRA_INFO}${CONFIG_IFX_CONFIG_FLASH_SIZE}${CONFIG_IFX_CONFIG_MEMORY_SIZE}
fi

if [ -r /etc/rc.d/config.sh ]; then
  . /etc/rc.d/config.sh 2> /dev/null
  platform=${CONFIG_IFX_MODEL_NAME%%_*}
fi

if [ ! "$ENVLOADED" ]; then
        if [ -r /etc/rc.conf ]; then
                . /etc/rc.conf 2> /dev/null
                ENVLOADED="1"
        fi
fi

#if [ ! -s /tmp/version ] ; then
	echo BOOTLoader:`cat /dev/mtd/0|grep "U-Boot "|cut -f2 -d ' '`>/tmp/version
  if [ "$platform" = "AMAZON" ]; then
	echo CPU:$platform"-SE">>/tmp/version
  else
  echo CPU:$platform>>/tmp/version  
  fi
	#echo BSP:`cat /proc/lspinfo/build_id`>>/tmp/version
	#echo Wireless `iwpriv ra0 show driverinfo`>>/tmp/version
	echo Kernel:`uname -r` >>/tmp/version
	echo Software:`cat /etc/version`$MODEL_EXTRA_INFO"-"`cat /etc/timestamp`>>/tmp/version
	echo Tool Chain:`/usr/sbin/upgrade|grep ToolChain|cut -d: -f2`>>/tmp/version
#fi

if [ "A$wanphy_phymode" = "A0" -o "A$wanphy_phymode" = "A3" ]; then
	if [ -s /tmp/fw_version ];then
	FIRMWARE_STR=`grep "DSL: FW " /tmp/fw_version`
	fi
	if [ "$FIRMWARE_STR" = "" ] ; then
		DSL_MODULE_CHECK_STR=`grep drv_dsl_cpe_api /proc/modules`
		if [ "A$DSL_MODULE_CHECK_STR" != "A" ]; then
			j=0
			for i in `/opt/ifx/bin/dsl_cpe_pipe.sh vig`
			do
				param=${i%%=*}
				val=${i#*=}
				case $param in
					"DSL_ChipSetFWVersion")
						A_VER=$val
				;;	
					"DSL_DriverVersionMeiBsp")
						mei=$val
				;;
				esac
				j=$(($j + 1))
			done
			driver=`/opt/lantiq/bin/what.sh /opt/lantiq/bin/drv_dsl_cpe_api.ko | cut -c54-`
			api=`/opt/lantiq/bin/what.sh /opt/lantiq/bin/dsl_cpe_control | cut -d@ -f2 | sed 's/(#)//'`
				if [ "$platform" = "VRX288" ]; then
					V_VER=`/opt/lantiq/bin/what.sh /firmware/xcpe_hw.bin | cut -f2 -d@ | sed 's/(#)//'`
					A_VER=`/opt/lantiq/bin/what.sh /firmware/xcpe_hw.bin | cut -f3 -d@ | sed 's/(#)//'`
					FIRMWARE="DSL: FW $V_VER, $A_VER , DRIVER $driver , API $api, MEI $mei"
				else
					FIRMWARE="DSL: FW $A_VER , DRIVER $driver , API $api, MEI $mei "
				fi

				if [ "$FIRMWARE" != "" ] ; then
					echo $FIRMWARE>>/tmp/version
					echo $FIRMWARE>/tmp/fw_version
				else
					FIRMWARE="N/A"
				fi
				FIRMWARE_STR=$FIRMWARE
		fi
	else
		echo $FIRMWARE_STR>>/tmp/version
	fi
fi

# Finding PPE/PPA version info

PPA_SS_VER=`ppacmd getversion | grep "PPA SubSystem version info"`
>/tmp/ppe_version
if [ "$PPA_SS_VER" != "" ]; then
	echo $PPA_SS_VER>>/tmp/ppe_version
fi
PPA_UTIL_VER=`ppacmd getversion | grep "PPA ppacmd utility tool info"`
if [ "$PPA_UTIL_VER" != "" ]; then
	p=${PPA_UTIL_VER%% *}
	echo $PPA_UTIL_VER>>/tmp/ppe_version
fi
PPA_HAL_VER=`ppacmd getversion | grep "PPE HAL driver info"`
if [ "$PPA_HAL_VER" != "" ]; then
	q=${PPA_HAL_VER%% *}
	echo $PPA_HAL_VER>>/tmp/ppe_version
fi
PPE_FW_VER=`ppacmd getversion | grep "PPE.* FW info"`
if [ "$PPE_FW_VER" != "" ]; then
	r=${PPE_FW_VER%% *}
	echo $PPE_FW_VER>>/tmp/ppe_version
fi
cat /tmp/ppe_version >> /tmp/version

# only ARX168 and GRX168 don't support voice functionaility
	VOICE_STR=`grep Voice /tmp/version`
	if [ "$VOICE_STR" = "" ] && [ -r /proc/driver/tapi/version ] && [ -r /proc/driver/vmmc/version ] && [ -r /proc/driver/ifx_mps/version ]; then
		tapi_str="TAPI `grep TAPI /proc/driver/tapi/version | cut -d' ' -f3`, VMMC `grep VMMC /proc/driver/vmmc/version | cut -d ' ' -f6`, MPS `grep MPS /proc/driver/ifx_mps/version | cut -d' ' -f6`"
		echo Voice:$tapi_str>>/tmp/version
	fi

if [ "$CONFIG_FEATURE_IFX_WIRELESS_WAVE300" = "1" ]
then
	if [ -e /etc/wave_components.ver ]
	then
		. /etc/wave_components.ver
		echo "Wave wlan version: $wave_release_minor" >>/tmp/version
		echo "Wave wlan driver version: $wave_driver_ver" >>/tmp/version
		echo "Wave wlan MAC FW version: $wave_mac_ver" >>/tmp/version
		echo "Wave wlan PHY version: $wave_progmodel_ver" >>/tmp/version
		echo "Wave wlan scripts version: $wave_scripts_ver" >>/tmp/version
	fi
fi

if [ $# -ge 1 ]; then
	while [ $# -ge 1 ]; do
		case $1 in
		-b) VER=BOOTLoader;;
		-c) VER=CPU;;
		-f) echo `echo $FIRMWARE|cut -d: -f2-`
			exit;;
		-k) VER=Kernel;;
		-l) VER=BSP;;
		-r) VER=Software;;
		-v) echo `echo $tapi_str|cut -d: -f1-`
			exit;;
		*) echo "'$1' Error!    Usage : version [-b|-c|-f|-k|-l|-r|-v]"
			exit
		esac
		shift
		echo `grep $VER /tmp/version|cut -d: -f2|cut -d' ' -f1`
  	done
else
	cat /tmp/version
fi

if [ -f /proc/driver/dect/version ]; then
	cat /usr/sbin/VoIP_VERSION
	cat /usr/sbin/DECT_VERSION
	cat /proc/driver/dect/version | grep "COSIC s/w"
	cat /proc/driver/dect/version | grep "BMC"
fi
