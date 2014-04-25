#!/bin/sh

#OUT_FILE=/dev/null
OUT_FILE=/dev/console

### bitonic
DFT_WAN_FID="2"
XDSL_RC_DO_ONLINE_CHANGEOVER_WAN_TYPE="1"

XDSL_RC_CONF=/etc/config/xdsl-rc.conf
SYSTEM_STATUS=/tmp/system_status
DEFAULT_SYSTEM_STATUS=/usr/bin-wan/system_status

# 0: leave showtime; 7: enter showtime
SHOW_TIME_STATUS="0"

# case: ADSL to Ether-Uplink, then Ether-Uplink to VDSL
#       => DSL_NOTIFICATION_TYPE should be DSL_MULTIMODE_FSM_STATUS.
#		   Sometimes this parameter is set to "DSL_STATUS" and causes xdslrc.sh does two times of wan type change.
#		   This is error condition. We must not do the second wan type change.
#		   Flow:
#		   DSL_XTU_STATUS=VDSL, Next_xTM_Mode=ATM.
#		   xdslrc.sh check these two value and change wan type to VDSL.
#		   Then get neg_xdsl_mode=ADSL from `ccfg_cli -f $SYSTEM_STATUS get status_Next_xDSL_Mode@xdsl_config_status`
#		   xdslrc.sh check neg_xdsl_mode and start to change wan type to ADSL again.
VDSL_WAN_TYPE_CHANGED="0"

ADSL_MODE=""
DSL_API_DEBUG=""
VDSL_MODE=""
xDSL_MODE=""
CNTL_MODE_ENA=""
CNTL_MODE=""
PWR_MODE_ENA=""
PWR_MODE=""
BS_ENA=""
SRA_ENA=""
RETX_ENA=""

wanphy_tc=""
wanphy_phymode=""
wanphy_settc=""
wanphy_setphymode=""
wanphy_mii1ethVlanMode=""
wanphy_ptmVlanMode=""
wanphy_tr69_encaprequested=""

xDSL_Mgmt_Mode=""
xTM_Mgmt_Mode=""
Next_xDSL_Mode=""
Next_xTM_Mode=""
xDSL_SW_Forced_Reboot=""
xDSL_ADSL_Sub_Prefix=""

get_data_from_xdsl_rc_conf()
{
	#echo "[xdslrc.sh get_data_from_xdsl_rc_conf]" > /dev/console

	ADSL_MODE=`ccfg_cli -f $XDSL_RC_CONF get ADSL_MODE@adsl_phy`
	DSL_API_DEBUG=`ccfg_cli -f $XDSL_RC_CONF get DSL_API_DEBUG@adsl_phy`
	VDSL_MODE=`ccfg_cli -f $XDSL_RC_CONF get VDSL_MODE@adsl_phy`
	xDSL_MODE=`ccfg_cli -f $XDSL_RC_CONF get xDSL_MODE@adsl_phy`
	CNTL_MODE_ENA=`ccfg_cli -f $XDSL_RC_CONF get CNTL_MODE_ENA@adsl_phy`
	CNTL_MODE=`ccfg_cli -f $XDSL_RC_CONF get CNTL_MODE@adsl_phy`
	PWR_MODE_ENA=`ccfg_cli -f $XDSL_RC_CONF get PWR_MODE_ENA@adsl_phy`
	PWR_MODE=`ccfg_cli -f $XDSL_RC_CONF get PWR_MODE@adsl_phy`
	BS_ENA=`ccfg_cli -f $XDSL_RC_CONF get BS_ENA@adsl_phy`
	SRA_ENA=`ccfg_cli -f $XDSL_RC_CONF get SRA_ENA@adsl_phy`
	RETX_ENA=`ccfg_cli -f $XDSL_RC_CONF get RETX_ENA@adsl_phy`

	wanphy_tc=`ccfg_cli -f $XDSL_RC_CONF get wanphy_tc@wan_phy_cfg`
	wanphy_phymode=`ccfg_cli -f $XDSL_RC_CONF get wanphy_phymode@wan_phy_cfg`
	wanphy_settc=`ccfg_cli -f $XDSL_RC_CONF get wanphy_settc@wan_phy_cfg`
	wanphy_setphymode=`ccfg_cli -f $XDSL_RC_CONF get wanphy_setphymode@wan_phy_cfg`
	wanphy_mii1ethVlanMode=`ccfg_cli -f $XDSL_RC_CONF get wanphy_mii1ethVlanMode@wan_phy_cfg`
	wanphy_ptmVlanMode=`ccfg_cli -f $XDSL_RC_CONF get wanphy_ptmVlanMode@wan_phy_cfg`
	wanphy_tr69_encaprequested=`ccfg_cli -f $XDSL_RC_CONF get wanphy_tr69_encaprequested@wan_phy_cfg`

	xDSL_Mgmt_Mode=`ccfg_cli -f $XDSL_RC_CONF get xDSL_Mgmt_Mode@xDSL_Config`
	xTM_Mgmt_Mode=`ccfg_cli -f $XDSL_RC_CONF get xTM_Mgmt_Mode@xDSL_Config`
	Next_xDSL_Mode=`ccfg_cli -f $XDSL_RC_CONF get Next_xDSL_Mode@xDSL_Config`
	Next_xTM_Mode=`ccfg_cli -f $XDSL_RC_CONF get Next_xTM_Mode@xDSL_Config`
	xDSL_SW_Forced_Reboot=`ccfg_cli -f $XDSL_RC_CONF get xDSL_SW_Forced_Reboot@xDSL_Config`
	xDSL_ADSL_Sub_Prefix=`ccfg_cli -f $XDSL_RC_CONF get xDSL_ADSL_Sub_Prefix@xDSL_Config`
}
### end

# This script has been enhanced to handle the XDSL Events for Multimode FSM
# and subsequent DSL Link bringup handling.

# Include model information
if [ -r /etc/rc.d/model_config.sh ]; then
	. /etc/rc.d/model_config.sh
fi

RC_CONF="/etc/config/rc.conf"

if [ -r $RC_CONF ]; then
	. $RC_CONF
fi

### bitonic
get_data_from_xdsl_rc_conf
#platform=`/usr/sbin/status_oper GET ppe_config_status ppe_platform`
platform=`ccfg_cli -f $SYSTEM_STATUS get ppe_platform@ppe_config_status`

# detect_wan@system => 0: orginal behavior(non auto-sensing); 1: auto-sensing
detect_wan=`ccfg_cli get detect_wan@system`
### end

# Define interface types
# NOTE: These values should match with the values in the enum
QOS_INTF_LAN=0
QOS_INTF_LOCAL=7
QOS_INTF_ALL=13
QOS_INTF_WAN_ATM=9
QOS_INTF_WAN_PTM=10
QOS_INTF_WAN_ETH_0=11
QOS_INTF_WAN_ETH_1=12
QOS_INTF_LAN_SPECIFIC=14

case "$wanphy_phymode" in
	0)
		case "$wanphy_tc" in
			0)
				#wan mode is ATM
				qIfTypeActive=$QOS_INTF_WAN_ATM;
				;;
			1)
				#wan mode is PTM
				qIfTypeActive=$QOS_INTF_WAN_PTM;
				;;
		esac
		;;
	1)
		#wan mode is MII0
		qIfTypeActive=$QOS_INTF_WAN_ETH_0;
		;;
	2)
		#wan mode is MII1
		qIfTypeActive=$QOS_INTF_WAN_ETH_1;
		;;
	3)
		#wan mode is PTM
		qIfTypeActive=$QOS_INTF_WAN_PTM;
		;;
esac


# get DSL actual line rate
get_dsl_rate()
{
	DSL_DATARATE_US_BC0=`/opt/lantiq/bin/dsl_cpe_pipe.sh g997csg 0 0 | awk '{ print $4 }'`
	${DSL_DATARATE_US_BC0:15}

	DSL_DATARATE_DS_BC0=`/opt/lantiq/bin/dsl_cpe_pipe.sh g997csg 0 1 | awk '{ print $4 }'`
	${DSL_DATARATE_DS_BC0:15}
}


# DSL Event handling script - Triggered from DSL CPE control Application

echo "[xdslrc.sh] platform=$platform,detect_wan=$detect_wan" >> $OUT_FILE
echo "[xdslrc.sh] DSL_NOTIFICATION_TYPE=$DSL_NOTIFICATION_TYPE" > /dev/console
echo "[xdslrc.sh] DSL_INTERFACE_STATUS=$DSL_INTERFACE_STATUS"  > /dev/console
echo "[xdslrc.sh] DSL_XTU_STATUS=$DSL_XTU_STATUS"        > /dev/console
echo "[xdslrc.sh] Next_xTM_Mode=$Next_xTM_Mode"        > /dev/console
echo "[xdslrc.sh] DSL_ADSL_SUB_PREF=$DSL_ADSL_SUB_PREF, DSL_TC_LAYER_STATUS=$DSL_TC_LAYER_STATUS"        >> $OUT_FILE
echo "[xdslrc.sh] DSL_EFM_TC_CONFIG_US=$DSL_EFM_TC_CONFIG_US, DSL_EFM_TC_CONFIG_DS=$DSL_EFM_TC_CONFIG_DS"        >> $OUT_FILE

VDSL_WAN_TYPE_CHANGED="0"

# enable/disable annex J relay ++
if [ "$DSL_NOTIFICATION_TYPE" == "DSL_STATUS" ]  && [ "$DSL_XTU_STATUS" == "ADSL" ]  &&  [ "$DSL_INTERFACE_STATUS" == "TRAINING" ] ; then
	xTSE4=`/opt/lantiq/bin/dsl_cpe_pipe.sh g997xtusesg | awk '{ print $5 }'`
	xTSE7=`/opt/lantiq/bin/dsl_cpe_pipe.sh g997xtusesg | awk '{ print $8 }'`
	current_sipo=`cat /proc/driver/sipo | grep relay: | awk '{ print $2 }'`
	echo "%% current_sipo=$current_sipo xTSE4=${xTSE4:8}, xTSE7=${xTSE7:8}" > /dev/console

	cat /proc/driver/sipo > /dev/console  #see annex J relay status before config

	if [ "${xTSE4:8}" == "40" ]  ||  [ "${xTSE7:8}" == "1" ] ; then
		if [ "$current_sipo" != "0x20" ] ; then
			echo "%% enable annex J: value should be 0x20" > /dev/console
			echo mask_set 0x20 0x20 > /proc/driver/sipo  #enable annex J relay
			dsl_cpe_pipe.sh acs 2
		fi
	else
		if [ "$current_sipo" != "0x0" ] ; then
			echo "%% disable annex J: value should be 0x0" > /dev/console
			echo mask_set 0x00 0x20 > /proc/driver/sipo #disable annex J relay
			dsl_cpe_pipe.sh acs 2
		fi
	fi

	cat /proc/driver/sipo > /dev/console  #see annex J relay status after config
fi
# enable/disable annex J relay --

##sowa add ++
if [ "$DSL_XTU_STATUS" == "VDSL" ] ; then
	if [ "$DSL_INTERFACE_STATUS" == "UP" ] ; then
		umng_syslog_cli add 21 Info Wan "VDSL Media Up !"
		echo "VDSL Media Up !" > /dev/console
		echo "UP" > /tmp/xdsl_wan_status.log
	elif [ "$DSL_INTERFACE_STATUS" == "DOWN" ] && [ `cat /tmp/xdsl_wan_status.log` == "UP" ]; then
		umng_syslog_cli add 21 Info Wan "VDSL Media DOWN !"
		echo "VDSL Media DOWN !" > /dev/console
		echo "DOWN" > /tmp/xdsl_wan_status.log
	fi

elif [ "$DSL_XTU_STATUS" == "ADSL" ] ; then
	if [ "$DSL_INTERFACE_STATUS" == "UP" ] ; then
		umng_syslog_cli add 21 Info Wan "ADSL Media Up !"
		echo "ADSL Media Up !"	> /dev/console
		echo "UP" > /tmp/xdsl_wan_status.log
	elif [ "$DSL_INTERFACE_STATUS" == "DOWN" ] && [ `cat /tmp/xdsl_wan_status.log` == "UP" ] ; then
		umng_syslog_cli add 21 Info Wan "ADSL Media DOWN !"
		echo "ADSL Media DOWN !" > /dev/console
		echo "DOWN" > /tmp/xdsl_wan_status.log
	fi
fi
#sowa add --

if [ "$DSL_NOTIFICATION_TYPE" == "DSL_STATUS" ] ; then

	# Handles the DSL Link Bringup sequence
	echo "[xdslrc.sh] DSL_STATUS Notification, DSL_XTU_STATUS=${DSL_XTU_STATUS}, Next_xTM_Mode=$Next_xTM_Mode" >> $OUT_FILE
	if [ "$DSL_XTU_STATUS" = "VDSL" ]; then
		echo "[xdslrc.sh] Negotiated DSL Status = $DSL_XTU_STATUS " >> $OUT_FILE
		# Check if the inserted TC Layer (with which the initial DSL FSM
		# negotiation has started) is comptabile with this current trained Mode
		if [ "$Next_xTM_Mode" = "PTM" ]; then
			# Here the xDSL Mode and xTM Mode matches, hence doesn't require any reboot of the CPE

			# Save the updated configuration to temp/system_status file
			# i.e, Current DSL Mode, xTM Status, SW Reboot
			# These values will be used to verify or cross-check against DSL_TC_LAYER_STATUS
			### bitonic
			#/usr/sbin/status_oper SET "xdsl_config_status" "status_Next_xDSL_Mode" $DSL_XTU_STATUS \
			#		"status_Next_xTM_Mode" $Next_xTM_Mode "status_xDSL_SW_Forced_Reboot" "0"
			ccfg_cli -f $SYSTEM_STATUS set status_Next_xDSL_Mode@xdsl_config_status="$DSL_XTU_STATUS"
			ccfg_cli -f $SYSTEM_STATUS set status_Next_xTM_Mode@xdsl_config_status="$Next_xTM_Mode"
			ccfg_cli -f $SYSTEM_STATUS set status_xDSL_SW_Forced_Reboot@xdsl_config_status="0"

#### wan up
			if [ "$XDSL_RC_DO_ONLINE_CHANGEOVER_WAN_TYPE" == 1 -a "$detect_wan" == "1" ] ; then
				# auto-sensing

				# xdslrc.sh only spports ADSL/VDSL mode. If the previous wan type is Ether-Uplink, we should change PPE module from Ether-Uplink to VDSL now.
				previous_wan_type=`ccfg_cli get active_wan_type@system`
				if [ "$previous_wan_type" == "2" ] ; then
					echo "[xdslrc.sh] DSL_NOTIFICATION_TYPE=$DSL_NOTIFICATION_TYPE, DSL_XTU_STATUS=$DSL_XTU_STATUS, Next_xTM_Mode=$Next_xTM_Mode, previous_wan_type=$previous_wan_type, we should change from Ether-Uplink to VDSL" > /dev/console
					/usr/bin-wan/online_changeover_wantype.sh $previous_wan_type 3

					echo "[xdslrc.sh] [VDSL] ==> change wan type" > /dev/console
					# sleep 3

					VDSL_WAN_TYPE_CHANGED="1"
				fi
			fi
####
			### end
		else
			echo "[xdslrc.sh] Next TC Mode = $Next_xTM_Mode. Should be PTM for VR9" >> /dev/console
			# The inserted TC mode doesn't match with the current trained Mode
			# Update the configuration in flash and reboot to restart the DSL
			# FSM over again. This is because we only support PTM-TC for VDSL

			# Update Next_xTM_Mode(PTM), xDSL_SW_Forced_Reboot(true=1)
			# and Next_xDSL_Mode(VDSL=2) in flash

####			if [ "$platform" = "VR9" -o "$platform" = "AR9" -o "$platform" = "AMAZON_SE" ]; then
####				. /etc/init.d/ltq_wan_changeover.sh stop
####			fi
			### bitonic
			#/usr/sbin/status_oper -u -f $RC_CONF SET "wan_phy_cfg" "wanphy_phymode" "3" \
			#		"wanphy_tc" "1"
			ccfg_cli -f $XDSL_RC_CONF set wanphy_phymode@wan_phy_cfg="3"
			ccfg_cli -f $XDSL_RC_CONF set wanphy_tc@wan_phy_cfg="1"
			### end
			# Providing a sleep to write cfg properly in NAND models before a reboot.
			# sleep 1
			### bitonic
			#/usr/sbin/status_oper -u -f $RC_CONF SET "xDSL_Config" "Next_xDSL_Mode" "2" \
			#		"Next_xTM_Mode" "PTM" "xDSL_SW_Forced_Reboot" "1"
			ccfg_cli -f $XDSL_RC_CONF set Next_xDSL_Mode@xDSL_Config="2"
			ccfg_cli -f $XDSL_RC_CONF set Next_xTM_Mode@xDSL_Config="PTM"
			ccfg_cli -f $XDSL_RC_CONF set xDSL_SW_Forced_Reboot@xDSL_Config="1"
			### end
			# sleep 1
			if [ "$platform" = "VR9" -o "$platform" = "AR9" -o "$platform" = "AMAZON_SE" ]; then
				#echo "Restarting apps at VDSL mode"
####				/etc/init.d/ltq_wan_changeover.sh start

#### wan up
				if [ "$XDSL_RC_DO_ONLINE_CHANGEOVER_WAN_TYPE" == 1 -a "$detect_wan" == "1" ] ; then
					# auto-sensing

					previous_wan_type=`ccfg_cli get active_wan_type@system`
					if [ "$previous_wan_type" != "3" ] ; then
						/usr/bin-wan/online_changeover_wantype.sh $previous_wan_type 3

						echo "[xdslrc.sh] [VDSL] ==> change wan type" > /dev/console
						# sleep 3

						VDSL_WAN_TYPE_CHANGED="1"
					fi
				fi
####
			else
			#Modify default WAN setting of upcoming mode
			/usr/sbin/ifx_event_util "DEFAULT_WAN" "MOD"
			# Reboot the CPE as the xDSL Mode and xTM Modes doesn't match
			# These new values will take effect on DSL bringup
			# sync; sleep 1
#			/sbin/reboot
			fi
		fi
	else
		echo "[xdslrc.sh] Negotiated DSL Status = $DSL_XTU_STATUS " >> $OUT_FILE
		# ATM Mode Configuration, so just save the xDSL_Mode to system status file
		### bitonic
		#/usr/sbin/status_oper SET "xdsl_config_status" "status_Next_xDSL_Mode" $DSL_XTU_STATUS \
		#			"status_Next_xTM_Mode" $Next_xTM_Mode "status_xDSL_SW_Forced_Reboot" $xDSL_SW_Forced_Reboot
		ccfg_cli -f $SYSTEM_STATUS set status_Next_xDSL_Mode@xdsl_config_status="$DSL_XTU_STATUS"
		ccfg_cli -f $SYSTEM_STATUS set status_Next_xTM_Mode@xdsl_config_status="$Next_xTM_Mode"
		ccfg_cli -f $SYSTEM_STATUS set status_xDSL_SW_Forced_Reboot@xdsl_config_status="$xDSL_SW_Forced_Reboot"

#### wan up
		if [ "$XDSL_RC_DO_ONLINE_CHANGEOVER_WAN_TYPE" == 1 -a "$detect_wan" == "1" ] ; then
			# auto-sensing
			# xdslrc.sh only spports ADSL/VDSL mode. If the previous wan type is Ether-Uplink, we should change PPE module from Ether-Uplink to ADSL now.
			previous_wan_type=`ccfg_cli get active_wan_type@system`
			if [ "$previous_wan_type" == "2" ] ; then
				echo "[xdslrc.sh] DSL_NOTIFICATION_TYPE=$DSL_NOTIFICATION_TYPE, DSL_XTU_STATUS=$DSL_XTU_STATUS, Next_xTM_Mode=$Next_xTM_Mode, previous_wan_type=$previous_wan_type, we should change from Ether-Uplink to ADSL" > /dev/console

				/usr/bin-wan/online_changeover_wantype.sh $previous_wan_type 1
				echo "[xdslrc.sh] [ADSL] ==> change wan type" > /dev/console
				# sleep 3
			fi
		fi
####
		### end
	fi
	# Handle the TC Layer Status Event - verify against the modes during DSL_XTU_STATUS events
	### bitonic
	#neg_xdsl_mode=`/usr/sbin/status_oper GET xdsl_config_status status_Next_xDSL_Mode`
	neg_xdsl_mode=`ccfg_cli -f $SYSTEM_STATUS get status_Next_xDSL_Mode@xdsl_config_status`
	### end
echo "[xdslrc.sh] neg_xdsl_mode=$neg_xdsl_mode" >> $OUT_FILE
	# For Negotiated xDSL Mode as "ADSL", check on the TC Layer for ATM (or) PTM
	# VDSL has already been handled as given above
	if [ "$neg_xdsl_mode" = "ADSL" ]; then
		if  [ "$DSL_TC_LAYER_STATUS" = "ATM" -a "$Next_xTM_Mode" = "ATM" ] ||
			[ "$DSL_TC_LAYER_STATUS" = "EFM" -a "$Next_xTM_Mode" = "PTM" ] ||
			[ "$VDSL_WAN_TYPE_CHANGED" = "1" ]; then
			# EVERYTHING MATCHES - Do nothing as we have already inserted & started the DSL SM with
			# the correct parameters
			echo -n
		else
			if [ "$xTM_Mgmt_Mode" = "Auto" ]; then
				echo "[xdslrc.sh] neg_xdsl_mode = $neg_xdsl_mode, Next_xTM_Mode = $Next_xTM_Mode, DSL_TC_LAYER_STATUS = $DSL_TC_LAYER_STATUS" >> $OUT_FILE
				# Save the Trained xTM Mode along with negotiated xDSL Mode and
				# set the SW Forced Reboot flag to true
				# Update the configuration in flash and reboot to restart the DSL
				# FSM over again.

				# Update Next_xTM_Mode(DSL_TC_LAYER_STATUS), xDSL_SW_Forced_Reboot(true=1)
				# and Next_xDSL_Mode(ADSL=1) in flash
				if [ "$DSL_TC_LAYER_STATUS" = "EFM" ]; then
					neg_xTM_Mode="PTM"
					tc=1
				elif [ "$DSL_TC_LAYER_STATUS" = "ATM" ]; then
					neg_xTM_Mode="ATM"
					tc=0
				else
					echo "[xdslrc.sh] neg_xTM_Mode = $DSL_TC_LAYER_STATUS " >> $OUT_FILE
				fi
####				if [ "$platform" = "VR9" -o "$platform" = "AR9" -o "$platform" = "AMAZON_SE" ]; then
####					. /etc/init.d/ltq_wan_changeover.sh stop
####				fi
				### bitonic
				#/usr/sbin/status_oper -u -f $RC_CONF SET "wan_phy_cfg" "wanphy_phymode" "0" \
				#		"wanphy_tc" $tc
				ccfg_cli -f $XDSL_RC_CONF set wanphy_phymode@wan_phy_cfg="0"
				ccfg_cli -f $XDSL_RC_CONF set wanphy_tc@wan_phy_cfg="$tc"
				### end
				# sleep 1
				### bitonic
				#/usr/sbin/status_oper -u -f $RC_CONF SET "xDSL_Config" "Next_xDSL_Mode" "1" \
				#		"Next_xTM_Mode" $neg_xTM_Mode "xDSL_SW_Forced_Reboot" "0"
				ccfg_cli -f $XDSL_RC_CONF set Next_xDSL_Mode@xDSL_Config="1"
				ccfg_cli -f $XDSL_RC_CONF set Next_xTM_Mode@xDSL_Config="$neg_xTM_Mode"
				ccfg_cli -f $XDSL_RC_CONF set xDSL_SW_Forced_Reboot@xDSL_Config="0"
				### end
				# sleep 1

				if [ "$platform" = "VR9" -o "$platform" = "AR9" -o "$platform" = "AMAZON_SE" ]; then
					#echo "Restarting apps at ADSL mode"
####					/etc/init.d/ltq_wan_changeover.sh start

#### wan up
					if [ "$XDSL_RC_DO_ONLINE_CHANGEOVER_WAN_TYPE" == 1 -a "$detect_wan" == "1" ] ; then
						# auto-sensing
						previous_wan_type=`ccfg_cli get active_wan_type@system`
						if [ "$previous_wan_type" != "1" ] ; then
							/usr/bin-wan/online_changeover_wantype.sh $previous_wan_type 1
							echo "[xdslrc.sh] [ADSL] ==> change wan type" > /dev/console
							# sleep 3
						fi
					fi
####

				else
					#Modify default WAN setting of upcoming mode
					/usr/sbin/ifx_event_util "DEFAULT_WAN" "MOD"
					# Reboot the CPE with the modified values
					# These new values will take effect on DSL bringup
					# sync; sleep 1
#					/sbin/reboot
				fi
			else
				echo "$DSL_TC_LAYER_STATUS not supported for $xTM_Mgmt_Mode Management Mode"
				### bitonic
				#/usr/sbin/status_oper -u -f $RC_CONF SET "wan_phy_cfg" "wanphy_phymode" "0"
				#/usr/sbin/status_oper -u -f $RC_CONF SET "xDSL_Config" "Next_xDSL_Mode" "1"
				ccfg_cli -f $XDSL_RC_CONF set wanphy_phymode@wan_phy_cfg="0"
				ccfg_cli -f $XDSL_RC_CONF set Next_xDSL_Mode@xDSL_Config="1"
				### end
			fi
		fi
	fi
	echo "[xdslrc.sh] xDSL Mode = $DSL_XTU_STATUS" >> $OUT_FILE
	echo "[xdslrc.sh] DSL TC Mode = $DSL_TC_LAYER_STATUS" >> $OUT_FILE

# This Notification type is valid only for VRX Platform
elif [ "$DSL_NOTIFICATION_TYPE" == "DSL_MULTIMODE_FSM_STATUS" ] ; then

	if [ "$DSL_NEXT_MODE" = "VDSL" ]; then
		echo "Event DSL NEXT MODE = $DSL_NEXT_MODE"
		# Save the updated configuration to temp/system_status file

		# Store the xDSL_ADSL_Sub_Prefix from the DSL SM Environment
		AdslSubPref=0
		if [ "$DSL_ADSL_SUB_PREF" = "ADSL" ]; then
			AdslSubPref=1
		fi
		# i.e, Current DSL Mode, xTM Status, ADSL Sub Prefix
		### bitonic
		#/usr/sbin/status_oper SET "xdsl_config_status" "status_Next_xDSL_Mode" "VDSL" \
		#		"status_Next_xTM_Mode" "PTM" "status_xDSL_ADSL_Sub_Prefix" $AdslSubPref
		ccfg_cli -f $SYSTEM_STATUS set status_Next_xDSL_Mode@xdsl_config_status="VDSL"
		ccfg_cli -f $SYSTEM_STATUS set status_Next_xTM_Mode@xdsl_config_status="PTM"
		ccfg_cli -f $SYSTEM_STATUS set status_xDSL_ADSL_Sub_Prefix@xdsl_config_status="$AdslSubPref"
		### end

		# If Next_xTM_Mode is PTM, then set the Multimode Configuration via
		# DSL CLI interface, else save the above parameters to rc.conf and Reboot
		if [ "$Next_xTM_Mode" = "PTM" ]; then
			# Send the Multimode FSM to DSL Application with VDSL (=2 as NextMode)
			/opt/ifx/bin/dsl_cpe_pipe.sh mfcs 2
		else
			echo " ChangeMode from $Next_xDSL_Mode/$Next_xTM_Mode"
####			if [ "$platform" = "VR9" -o "$platform" = "AR9" -o "$platform" = "AMAZON_SE" ]; then
####				. /etc/init.d/ltq_wan_changeover.sh stop
####			fi
			# Save the parameters to rc.conf and Reboot
			### bitonic
			#/usr/sbin/status_oper -u -f $RC_CONF SET "wan_phy_cfg" "wanphy_phymode" "3" \
			#		"wanphy_tc" "1"
			ccfg_cli -f $XDSL_RC_CONF set wanphy_phymode@wan_phy_cfg="3"
			ccfg_cli -f $XDSL_RC_CONF set wanphy_tc@wan_phy_cfg="1"
			### end
			# sleep 1
			### bitonic
			#/usr/sbin/status_oper -u -f $RC_CONF SET "xDSL_Config" "Next_xDSL_Mode" "2" \
			#		"Next_xTM_Mode" "PTM" "xDSL_SW_Forced_Reboot" "1" "xDSL_ADSL_Sub_Prefix" $AdslSubPref
			ccfg_cli -f $XDSL_RC_CONF set Next_xDSL_Mode@xDSL_Config="2"
			ccfg_cli -f $XDSL_RC_CONF set Next_xTM_Mode@xDSL_Config="PTM"
			ccfg_cli -f $XDSL_RC_CONF set xDSL_SW_Forced_Reboot@xDSL_Config="1"
			ccfg_cli -f $XDSL_RC_CONF set xDSL_ADSL_Sub_Prefix@xDSL_Config="$AdslSubPref"
			### end
			# sleep 1
			if [ "$platform" = "VR9" -o "$platform" = "AR9" -o "$platform" = "AMAZON_SE" ]; then
				#echo "Restarting apps at MMFSM mode"
####				/etc/init.d/ltq_wan_changeover.sh start

#### wan up
				if [ "$XDSL_RC_DO_ONLINE_CHANGEOVER_WAN_TYPE" == 1 -a "$detect_wan" == "1" ] ; then
					# auto-sensing
					previous_wan_type=`ccfg_cli get active_wan_type@system`
					if [ "$previous_wan_type" != "3" ] ; then
						/usr/bin-wan/online_changeover_wantype.sh $previous_wan_type 3
						echo "[xdslrc.sh] [MMFSM VDSL] ==> change wan type" > /dev/console
						# sleep 3
					fi
				fi
####

			else
				#Modify default WAN setting of upcoming mode
				/usr/sbin/ifx_event_util "DEFAULT_WAN" "MOD"

				# Reboot the CPE with the modified values
				# These new values will take effect on DSL bringup
				# sync; sleep 1
#			/sbin/reboot
			fi
		fi
	elif [ "$DSL_NEXT_MODE" = "ADSL" ]; then
		echo "Event DSL NEXT MODE = $DSL_NEXT_MODE"
		# Store the xDSL_ADSL_Sub_Prefix from the DSL SM Environment
		AdslSubPref=0
		if [ "$DSL_ADSL_SUB_PREF" = "ADSL" ]; then
			AdslSubPref=1
		fi

		# ADSL Mode Negotiated by Multimode FSM
		# Save the updated configuration to temp/system_status file
		# i.e, Current DSL Mode, xTM Status, SW Reboot
		### bitonic
		#/usr/sbin/status_oper SET "xdsl_config_status" "status_Next_xDSL_Mode" "ADSL" \
		#		"status_Next_xTM_Mode" $Next_xTM_Mode "status_xDSL_SW_Forced_Reboot" "0"
		ccfg_cli -f $SYSTEM_STATUS set status_Next_xDSL_Mode@xdsl_config_status="ADSL"
		ccfg_cli -f $SYSTEM_STATUS set status_Next_xTM_Mode@xdsl_config_status="$Next_xTM_Mode"
		ccfg_cli -f $SYSTEM_STATUS set status_xDSL_SW_Forced_Reboot@xdsl_config_status="$0"
		### end

		# Send the Multimode FSM to DSL Application with ADSL (=1 as NextMode)
		/opt/ifx/bin/dsl_cpe_pipe.sh mfcs 1
	fi
	echo "DSL NEXT MODE = $DSL_NEXT_MODE"
	echo "ADSL SUB PREF = $DSL_ADSL_SUB_PREF"

elif [ "$DSL_NOTIFICATION_TYPE" == "DSL_INTERFACE_STATUS" ] ; then

	case "$DSL_INTERFACE_STATUS" in
		"UP")
			# DSL link up trigger
			#if [ "$CONFIG_FEATURE_LED" = "1" ]; then
			#	echo none > /sys/class/leds/broadband_led/trigger
			#	echo 1    > /sys/class/leds/broadband_led/brightness
			#fi
			arc_led.sh adsl on
			echo "[xdslrc.sh] xDSL Enter SHOWTIME!!" >> /dev/console
			#. /etc/rc.d/adsl_up
			echo "7" > /tmp/adsl_status
			# Enable Far-End Parameter Request, Lantiq not supported yet
			#/opt/lantiq/bin/dsl_cpe_pipe.sh ifcs 0 0 0 0 0 0

			# Adjust the upstream/downstream rates of the queues created in the system

			get_dsl_rate
			let UPSTREAM_RATE=$DSL_DATARATE_US_BC0/1000
			echo "US Rate $UPSTREAM_RATE kbps" > /dev/console
			let DOWNSTREAM_RATE=$DSL_DATARATE_DS_BC0/1000
			echo "DS Rate $DOWNSTREAM_RATE kbps" > /dev/console
			if [ $qIfTypeActive -eq $QOS_INTF_WAN_ATM -o  $qIfTypeActive -eq $QOS_INTF_WAN_PTM ]; then
				. /etc/rc.d/ipqos_rate_update $UPSTREAM_RATE $DOWNSTREAM_RATE
			fi

			if [ -n "${UPSTREAM_RATE}" ] ; then
				/usr/sbin/sip_config SIP_CORE_Set_VcBandwidth $UPSTREAM_RATE
			fi

			[ "$Next_xTM_Mode" = "PTM" ] && [ "$CONFIG_IFX_CONFIG_CPU" = "AMAZON_SE" ] && ifconfig ptm0 txqueuelen 300

			#
			cat /proc/uptime > /tmp/dslup-time
			#
			if [ "$DSL_XTU_STATUS" == "ADSL" ] ; then
				if [ "$detect_wan" == "0" ] ; then
					# non auto-sensing

					previous_wan_type=`ccfg_cli get active_wan_type@system`
					ccfg_cli set wan_type@system=1
					ccfg_cli set active_wan_type@system=1
					ccfg_cli set pre_dsl_wan_type@system=1
					if [ "$previous_wan_type" != "1" ] ; then
						ccfg_cli commitcfg
					fi

					if [ `grep -c "ifxmips_ppa_datapath_vr9_a5" /proc/modules` -le 0 ] ; then
						echo "xdslrc.sh [ADSL] ==> reboot.sh" > /dev/console
						reboot.sh
					fi
				fi
				/etc/init.d/br2684ctl start
				#ifup wan
				CNT=0
				ethaddr=`getmacaddr.sh wan 0`
				while [ $CNT -lt 8 ] ; do
					wanIf=`printf "wan%03d" $CNT`
					proto=`ccfg_cli get proto@$wanIf`
					admin=`ccfg_cli get adminstate@$wanIf`
					ifname=`ccfg_cli get ifname@$wanIf`
					if [ "$admin" == "enable" ] ; then
						if [ "$proto" == "pppoa" ] ; then
							ifconfig ppp$CNT down
							ifconfig ppp$CNT hw ether $ethaddr
							switch_utility MAC_TableEntryRemove 0 $ethaddr
							switch_utility MAC_TableEntryAdd $DFT_WAN_FID 6 0 1 $ethaddr
							ifup $wanIf
						fi

						ifconfig $ifname down
						ifconfig $ifname hw ether $ethaddr
						switch_utility MAC_TableEntryRemove 0 $ethaddr
						switch_utility MAC_TableEntryAdd $DFT_WAN_FID 6 0 1 $ethaddr
						ifup $wanIf

					fi
					ethaddr=`echo $ethaddr | next_macaddr 0`
					let CNT=$CNT+1
				done
			elif [ "$DSL_XTU_STATUS" == "VDSL" ] ; then
				if [ "$detect_wan" == "0" ] ; then
					# non auto-sensing

					previous_wan_type=`ccfg_cli get active_wan_type@system`
					ccfg_cli set wan_type@system=3
					ccfg_cli set active_wan_type@system=3
					ccfg_cli set pre_dsl_wan_type@system=3
					if [ "$previous_wan_type" != "3" ] ; then
						ccfg_cli commitcfg
					fi

					if [ `grep -c "ifxmips_ppa_datapath_vr9_e5" /proc/modules` -le 0 ] ; then
						echo "xdslrc.sh [VDSL] ==> reboot.sh" > /dev/console
						reboot.sh
					fi
				fi

				ethaddr=`getmacaddr.sh wan 0`
				ifconfig ptm0 down
				ifconfig ptm0 hw ether $ethaddr
				ifconfig ptm0 up
				#ifup wan050
				ppacmd addwan -i ptm0
				CNT=0
				while [ $CNT -lt 8 ] ; do
					wanIf=`printf "wan%03d" $(($CNT+50))`
					proto=`ccfg_cli get proto@$wanIf`
					admin=`ccfg_cli get adminstate@$wanIf`
					ifname=`ccfg_cli get ifname@$wanIf`
					if [ "$admin" == "enable" ] ; then
						ifconfig $ifname down
						ifconfig $ifname hw ether $ethaddr
						switch_utility MAC_TableEntryRemove 0 $ethaddr
						switch_utility MAC_TableEntryAdd $DFT_WAN_FID 6 0 1 $ethaddr
						ifup $wanIf
					fi
					ethaddr=`echo $ethaddr | next_macaddr 0`
					let CNT=$CNT+1
				done
			fi
#### ygchen add, for arcadyan QoS implementation
			qos_enable=`ccfg_cli get enable@qos_glb`
			if [ "$qos_enable" = "1" ]; then
				. /usr/sbin/qos/qos_rate_change_dft.sh $DOWNSTREAM_RATE $UPSTREAM_RATE
			fi
####
			if [ -x /etc/init.d/brnsip ] ; then
				/etc/init.d/brnsip bandwidth $UPSTREAM_RATE
			fi
			;;

		"DOWN")
			SHOW_TIME_STATUS=`cat /tmp/adsl_status`
			if [ "$SHOW_TIME_STATUS" == "7" ] ; then
	#			ALLNETIF=`ppacmd getwan 2> /dev/null | awk '{ print $1 }'` # AR9
				ALLNETIF=`ppacmd getwan 2> /dev/null | awk '$3 !~ /---/ { print $3 }'` # VR9
				for NETIF in $ALLNETIF ; do
					ppacmd delwan -i $NETIF  >& $OUT_FILE
	#				echo del wan $NETIF > /proc/ppa/api/netif
				done
				if [ "$DSL_XTU_STATUS" == "ADSL" ] ; then
					#ifdown wan
					CNT=0
					while [ "$CNT" -lt 8 ] ; do
						wanIf=`printf "wan%03d" $CNT`
						admin=`ccfg_cli get adminstate@$wanIf`
						if [ "$admin" == "enable" ] ; then
							ifdown $wanIf
						fi
						let CNT=$CNT+1
					done
				elif [ "$DSL_XTU_STATUS" == "VDSL" ] ; then
					#ifdown wan050
					CNT=0
					while [ "$CNT" -lt 8 ] ; do
						wanIf=`printf "wan%03d" $(($CNT+50))`
						admin=`ccfg_cli get adminstate@$wanIf`
						if [ "$admin" == "enable" ] ; then
							ifdown $wanIf
						fi

						let CNT=$CNT+1
					done
				fi
				#
				/etc/init.d/br2684ctl stop
				# DSL link down trigger
	#			if [ "$CONFIG_FEATURE_LED" = "1" ]; then
	#				echo none > /sys/class/leds/broadband_led/trigger
	#				echo 0    > /sys/class/leds/broadband_led/brightness
	#			fi
				arc_led.sh adsl off
				echo "[xdslrc.sh] xDSL Leave SHOWTIME!!" > /dev/console
				#. /etc/rc.d/adsl_down
				echo "0" > /tmp/adsl_status
			fi
			;;

		"READY")
			# DSL Handshake 2 HZ
#			if [ "$CONFIG_FEATURE_LED" = "1" ]; then
#				echo timer > /sys/class/leds/broadband_led/trigger
#				echo 1     > /sys/class/leds/broadband_led/brightness
#				echo 250   > /sys/class/leds/broadband_led/delay_on
#				echo 250   > /sys/class/leds/broadband_led/delay_off
#			fi
			echo "[xdslrc.sh] xDSL Handshake!!"
			arc_led.sh adsl blink
			;;

		"TRAINING")
			# DSL Training 4 HZ
#			if [ "$CONFIG_FEATURE_LED" = "1" ]; then
#				echo timer > /sys/class/leds/broadband_led/trigger
#				echo 1     > /sys/class/leds/broadband_led/brightness
#				echo 125   > /sys/class/leds/broadband_led/delay_on
#				echo 125   > /sys/class/leds/broadband_led/delay_off
#			fi
			echo "[xdslrc.sh] xDSL Training !!"
			arc_led.sh adsl blink
			;;
	esac

elif [ "$DSL_NOTIFICATION_TYPE" == "DSL_DATARATE_STATUS" ] ||
	 [ "$DSL_NOTIFICATION_TYPE" == "DSL_DATARATE_STATUS_US" ] ||
	 [ "$DSL_NOTIFICATION_TYPE" == "DSL_DATARATE_STATUS_DS" ] ; then

	get_dsl_rate
	echo "DSL US Data Rate = "`expr $DSL_DATARATE_US_BC0 / 1000`" kbps"
	echo $DSL_DATARATE_US_BC0 > /tmp/dsl_us_rate
	echo "DSL DS Data Rate = "`expr $DSL_DATARATE_DS_BC0 / 1000`" kbps"
	echo $DSL_DATARATE_DS_BC0 > /tmp/dsl_ds_rate
	# convert the data rate in kbps to cells/sec and store in running config file
	# this will be used for bandwidth allocation during wan connection creation
	# 8 * 53 = 424
	DSL_DATARATE_US_CPS=$(( ${DSL_DATARATE_US_BC0} / 424 ))
	### bitonic
	#/usr/sbin/status_oper SET BW_INFO max_us_bw "${DSL_DATARATE_US_CPS}"
	ccfg_cli -f $SYSTEM_STATUS set max_us_bw@BW_INFO="${DSL_DATARATE_US_CPS}"
	### end
	DSL_DATARATE_DS_CPS=$(( ${DSL_DATARATE_DS_BC0} / 424 ))
	### bitonic
	#/usr/sbin/status_oper SET BW_INFO max_us_bw "${DSL_DATARATE_DS_CPS}"
	ccfg_cli -f $SYSTEM_STATUS set max_us_bw@BW_INFO="${DSL_DATARATE_DS_CPS}"
	### end
	# Adjust ATM and IP QoS Rate shaping parameters based on line rate
	UPSTREAM_RATE=$(( $DSL_DATARATE_US_BC0 / 1000 ))
	DOWNSTREAM_RATE=$(( $DSL_DATARATE_DS_BC0 / 1000 ))
	if [ $qIfTypeActive -eq $QOS_INTF_WAN_ATM -o  $qIfTypeActive -eq $QOS_INTF_WAN_PTM ]; then
		/etc/rc.d/ipqos_rate_update $UPSTREAM_RATE $DOWNSTREAM_RATE
	fi

fi

