#!/bin/sh
# Copyright (C) 2010 OpenWrt.org
START=20
xTSE=""

OUT_FILE=/dev/null
#OUT_FILE=/dev/console

### ctc ###
RC_CONF=/etc/config/rc.conf
ADSL_SCR=/tmp/adsl.scr
VDSL_SCR=/tmp/vdsl.scr
SYS_CONF=/etc/config.sh
##########
. $SYS_CONF

# Parameters that are externally defined and which are of relevance within
# this script handling

# xDSL_Mgmt_Mode - Stores the selected xDSL Mode configured from WEB interface.
#        Auto is used for VRX platform to negotiate ADSL/VDSL with CO
#        Valid values include ADSL, VDSL, Auto

# xTM_Mgmt_Mode  - Stores the selected xTM Mode configured from WEB interface.
#        Auto allows for negotiating either ATM or PTM with the CO
#        Valid values include ATM, PTM, Auto

# Next_xDSL_Mode - Stores the xDSL Mode to be used after the next CPE Reboot
#        It could also be referred to the last used xDSL mode before Reboot
#        Valid values include ADSL(1), VDSL(2), NA(0)

# xDSL_SW_Forced_Reboot - Specifies whether the DSL API Multimode state machine needs
#        to go into the SW Forced Reboot Path. When the CPE is in this
#        mode, Next_xDSL_Mode and xDSL_ADSL_Sub_Prefix parameters are
#        passed to the dsl_cpe_control program (dsl_cpe_control -M<NextMode>_<AdslSubPref> ..)
#        Valid values include True(1), False(0)

# xDSL_ADSL_Sub_Prefix  - Specifies the ADSL Sub Prefix value used by the DSL API Multimode FSM
#        Valid values include ADSL(1), NA(0)
#

### bitonic
DEFAULT_AUTO_ADSL_RC_CONF=/usr/bin-wan/auto-adsl-rc.conf
DEFAULT_AUTO_VDSL_RC_CONF=/usr/bin-wan/auto-vdsl-rc.conf
DEFAULT_ADSL_RC_CONF=/usr/bin-wan/adsl-rc.conf
DEFAULT_VDSL_RC_CONF=/usr/bin-wan/vdsl-rc.conf
XDSL_RC_CONF=/etc/config/xdsl-rc.conf
SYSTEM_STATUS=/tmp/system_status
DEFAULT_SYSTEM_STATUS=/usr/bin-wan/system_status

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
	echo "[ifx_cpe_control_init.sh get_data_from_xdsl_rc_conf]" >> $OUT_FILE

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

# Handle the xDSL Management Mode, for Auto Mode we need to pass
# additional parameters to the dsl_cpe_control program that specifies
# the Multimode configuration parameters (Next_Adsl_Mode, xDSL_ADSL_Sub_Prefix)

# Parse the ADSL_MODE string and set the Annex B related bits to 0
parse_annexA_bits() {
  G992_1_A=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_2=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_3_A=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_5_A=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  T1_413=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_3_L=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_3_I=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_5_I=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_3_M=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_5_M=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_1_B=0
  adsl_mode=${adsl_mode#*_}
  G992_3_B=0
  adsl_mode=${adsl_mode#*_}
  G992_5_B=0
  adsl_mode=${adsl_mode#*_}
  G992_3_J=0
  adsl_mode=${adsl_mode#*_}
  G992_5_J=0
}

# Parse the ADSL_MODE string and set the Annex A related bits to 0
parse_annexB_bits() {
  G992_1_A=0
  adsl_mode=${adsl_mode#*_}
  G992_2=0
  adsl_mode=${adsl_mode#*_}
  G992_3_A=0
  adsl_mode=${adsl_mode#*_}
  G992_5_A=0
  adsl_mode=${adsl_mode#*_}
  T1_413=0
  adsl_mode=${adsl_mode#*_}
  G992_3_L=0
  adsl_mode=${adsl_mode#*_}
  G992_3_I=0
  adsl_mode=${adsl_mode#*_}
  G992_5_I=0
  adsl_mode=${adsl_mode#*_}
  G992_3_M=0
  adsl_mode=${adsl_mode#*_}
  G992_5_M=0
  adsl_mode=${adsl_mode#*_}
  G992_1_B=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_3_B=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_5_B=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_3_J=${adsl_mode%%_*}
  adsl_mode=${adsl_mode#*_}
  G992_5_J=${adsl_mode%%_*}
}

get_fw_type() {
	### ctc ###
#   if [ -r /etc/rc.d/config.sh ]; then
#      . /etc/rc.d/config.sh 2> /dev/null
   if [ -r $SYS_CONF ]; then
      . $SYS_CONF 2> /dev/null
      platform=${CONFIG_IFX_MODEL_NAME%%_*}
   fi
	###########

   # The last digit of the xDSL firmware version specifies the Annex type
   # 1 - ADSL Annex A
   # 2 - ADSL Annex B
   # 6 - VDSL

   # Determine the type of the ADSL Firmware which is mounted
   if [ "$platform" = "VRX288" ]; then
      # For VRX platform there is always a combined VDSL/ADSL firmware binary used.
      # Therefore two what strings (version numbers) are included within binary.
      # The format of the extracted what strings has to have to following format
      # /firmware/xcpe_hw.bin : @(#)V_5.3.1.A.1.6 @(#)5.3.3.0.1.1
      #                               +---------+     +---------+
      #                              VDSL FW Vers.    ADSL FW Vers.
      FW_TYPE=`${BIN_DIR}/what.sh ${FW_DIR}/${FW_FILENAME} | cut -d'.' -f12`
   else
      # For ADSL only platforms (Danube, Amazon-SE, ARX100) there is only one
      # what string (version number) included.
      # The format of the extracted what strings has to have to following format
      # /firmware/dsl_firmware_a.bin : @(#)4.4.7.B.0.1
      #                                    +---------+
      #                                   ADSL FW Vers.
      FW_TYPE=`${BIN_DIR}/what.sh ${FW_DIR}/${FW_FILENAME} | cut -d'.' -f7`
   fi
echo "[ifx_cpe_control_init.sh get_fw_type] FW_TYPE=$FW_TYPE" >> $OUT_FILE
}

validate_fw_bits() {
   # Get the complete bit string (inclusive of Annex A and B)
   adsl_mode=${ADSL_MODE#*_}
   # Parse Annex A related bits if the mounted firmware type is Annex A.
   # If the Management mode is VDSL or if the parsed bits match Annex A, do nothing.
   # Else, reset the ADSL_MODE and xDSL_MODE parameter to default Annex A supported values
   # and store the values in rc.conf.
   if [ $FW_TYPE = "1" ]; then
echo "[ifx_cpe_control_init.sh validate_fw_bits] FW_TYPE=1" >> $OUT_FILE
	  ### bitonic
      #/usr/sbin/status_oper SET "dsl_fw_type" "type" "a"   # writing firmware type in /tmp/system_status file
      ccfg_cli -f $SYSTEM_STATUS set type@dsl_fw_type="a"
      ### end
      parse_annexA_bits
      if [ "$xDSL_Mgmt_Mode" = "VDSL" ]; then
         return
      fi
      # Restore Annex A bits in the config
      if [ $G992_1_A = "0" -a $G992_3_A = "0" -a $G992_5_A = "0" ]; then
         xDSL_MODE="Multimode-ADSL"
         if [ "$xDSL_Mgmt_Mode" = "Auto" ]; then
           xDSL_MODE="Multimode-xDSL"
         fi
         ADSL_MODE="Annex_1_0_1_1_0_1_0_0_0_0_0_0_0_0_0"
	  ### bitonic
      	#/usr/sbin/status_oper -u -f /etc/rc.conf SET "adsl_phy" "ADSL_MODE" $ADSL_MODE "xDSL_MODE" $xDSL_MODE
echo "[ifx_cpe_control_init.sh validate_fw_bits] FW_TYPE=1, set ADSL_MODE=$ADSL_MODE, xDSL_MODE=$xDSL_MODE" >> $OUT_FILE
      	ccfg_cli -f $XDSL_RC_CONF set ADSL_MODE@adsl_phy="$ADSL_MODE"
      	ccfg_cli -f $XDSL_RC_CONF set xDSL_MODE@adsl_phy="$xDSL_MODE"
      ### end
         adsl_mode=${ADSL_MODE#*_}
         parse_annexA_bits
      fi
   # If mounted firmware is Annex B, parse Annex B related bits.
   # If the Management mode is VDSL or if the parsed bits match Annex B, do nothing.
   # Else, reset the ADSL_MODE and xDSL_MODE parameter to default Annex B supported values
   # and store the values in rc.conf.
   elif [ $FW_TYPE = "2" ]; then
echo "[ifx_cpe_control_init.sh validate_fw_bits] FW_TYPE=2" >> $OUT_FILE
	  ### bitonic
      #/usr/sbin/status_oper SET "dsl_fw_type" "type" "b"  # writing firmware type in /tmp/system_status file
      ccfg_cli -f $SYSTEM_STATUS set type@dsl_fw_type="b"
      ### end
      parse_annexB_bits
      if [ "$xDSL_Mgmt_Mode" = "VDSL" ]; then
         return
      fi
      # Restore Annex B bits in the config
      if [ $G992_1_B = "0" -a $G992_3_B = "0" -a $G992_5_B = "0" ]; then
         xDSL_MODE="Multimode-ADSL"
         if [ "$xDSL_Mgmt_Mode" = "Auto" ]; then
           xDSL_MODE="Multimode-xDSL"
         fi
         ADSL_MODE="Annex_0_0_0_0_0_0_0_0_0_0_1_1_1_0_0"
	  ### bitonic
      	#/usr/sbin/status_oper -u -f /etc/rc.conf SET "adsl_phy" "ADSL_MODE" $ADSL_MODE "xDSL_MODE" $xDSL_MODE
echo "[ifx_cpe_control_init.sh validate_fw_bits] FW_TYPE=2, set ADSL_MODE=$ADSL_MODE, xDSL_MODE=$xDSL_MODE" >> $OUT_FILE
      	ccfg_cli -f $XDSL_RC_CONF set ADSL_MODE@adsl_phy="$ADSL_MODE"
      	ccfg_cli -f $XDSL_RC_CONF set xDSL_MODE@adsl_phy="$xDSL_MODE"
      ### end
         adsl_mode=${ADSL_MODE#*_}
         parse_annexB_bits
      fi
   fi
}

calc_xtse() {
   # This function will form the xTSE bits as per G.997.1 standard.
   # It will use the bitmap stored in rc.conf (in variable ADSL_MODE)
   # and convert to xTSE bits. This variable will be configured
   # from xDSL PHY configuration web page. Since rc.conf file is included
   # in this script (from /etc/rc.common), ADSL_MODE variable can be accessed.
   check=${#ADSL_MODE}
  select_adsl_mode=`cat $SYS_CONF | grep CONFIG_FW_XDSL_ANNEX_B | head -n 1 | cut -d'=' -f2`
  echo "[ifx_cpe_control_init.sh] select_adsl_mode=$select_adsl_mode" >> /dev/console
  if [ $select_adsl_mode -eq 1 ]; then
	#if [ $CONFIG_FW_XDSL_ANNEX_B_1 -eq 1 ]; then
		ADSL_MODE="Annex_1_0_1_1_0_1_0_0_0_0_1_1_1_0_0"
		echo "[ifx_cpe_control_init.sh calc_xtse] CONFIG_FW_XDSL_ANNEX_B_* is on" > /dev/console
		if [ $CONFIG_FW_XDSL_ANNEX_J -eq 1 ]; then
			ADSL_MODE="Annex_1_0_1_1_0_1_0_0_0_0_1_1_1_1_1"
			echo "[ifx_cpe_control_init.sh calc_xtse] CONFIG_FW_XDSL_ANNEX_J is on" > /dev/console
		fi
	else
		ADSL_MODE="Annex_1_0_1_1_0_1_0_0_0_0_0_0_0_0_0"
		echo "[ifx_cpe_control_init.sh calc_xtse] CONFIG_FW_XDSL_ANNEX_B_* is not DEFINED !!" > /dev/console
		echo "[ifx_cpe_control_init.sh calc_xtse] Set to Annex A by default !!" > /dev/console
	fi

   #determine if the ADSL Firmware is of type Annex A or Annex B
   get_fw_type
   #Validate if the ADSL_MODE bits in rc.conf are valid against the firmware type
   validate_fw_bits

   xTSE1=0
   if [ "$T1_413" = "1" ]; then
      xTSE1=$(( $xTSE1 + 1 ))
   fi
   if [ "$G992_1_A" = "1" ]; then
      xTSE1=$(( $xTSE1 + 4 ))
   fi
   if [ "$G992_1_B" = "1" ]; then
      xTSE1=$(( $xTSE1 + 16 ))
   fi
   x=$(( $xTSE1 / 16 ))
   y=$(( $xTSE1 % 16 ))
   xTSE1=$x$y

   xTSE2=0
   if [ "$G992_2" = "1" ]; then
      xTSE2=$(( $xTSE2 + 1 ))
   fi
   x=$(( $xTSE2 / 16 ))
   y=$(( $xTSE2 % 16 ))
   xTSE2=$x$y

   xTSE3=0
   if [ "$G992_3_A" = "1" ]; then
      xTSE3=$(( $xTSE3 + 4 ))
   fi
   if [ "$G992_3_B" = "1" ]; then
      xTSE3=$(( $xTSE3 + 16 ))
   fi
   x=$(( $xTSE3 / 16 ))
   y=$(( $xTSE3 % 16 ))
   xTSE3=$x$y

   xTSE4=0
   if [ "$G992_3_I" = "1" ]; then
      xTSE4=$(( $xTSE4 + 16 ))
   fi
   if [ "$G992_3_J" = "1" ]; then
      xTSE4=$(( $xTSE4 + 64 ))
   fi
   x=$(( $xTSE4 / 16 ))
   y=$(( $xTSE4 % 16 ))
   xTSE4=$x$y

   xTSE5=0
   if [ "$G992_3_L" = "1" ]; then
      xTSE5=$(( $xTSE5 + 4 ))
   fi
   if [ "$G992_3_M" = "1" ]; then
      xTSE5=$(( $xTSE5 + 64 ))
   fi
   x=$(( $xTSE5 / 16 ))
   y=$(( $xTSE5 % 16 ))
   xTSE5=$x$y

   #xTSE6
   xTSE6=0
   if [ "$G992_5_A" = "1" ]; then
      xTSE6=$(( $xTSE6 + 1 ))
   fi
   if [ "$G992_5_B" = "1" ]; then
      xTSE6=$(( $xTSE6 + 4 ))
   fi
   if [ "$G992_5_I" = "1" ]; then
      xTSE6=$(( $xTSE6 + 64 ))
   fi
   x=$(( $xTSE6 / 16 ))
   y=$(( $xTSE6 % 16 ))
   xTSE6=$x$y

   xTSE7=0
   if [ "$G992_5_J" = "1" ]; then
      xTSE7=$(( $xTSE7 + 1 ))
   fi
   if [ "$G992_5_M" = "1" ]; then
      xTSE7=$(( $xTSE7 + 4 ))
   fi
   x=$(( $xTSE7 / 16 ))
   y=$(( $xTSE7 % 16 ))
   xTSE7=$x$y

   xTSE8="07"
#   if [ "$xDSL_MODE" = "vdsl2" ]; then
#     # set the last octet to 07 for VDSL
#    xTSE8="07"
#   fi

   if [ "$xDSL_Mgmt_Mode" = "ADSL" ]; then
      xTSE=${xTSE1}_${xTSE2}_${xTSE3}_${xTSE4}_${xTSE5}_${xTSE6}_${xTSE7}_00
   elif [ "$xDSL_Mgmt_Mode" = "VDSL" ]; then
      xTSE=00_00_00_00_00_00_00_${xTSE8}
   else
      xTSE=${xTSE1}_${xTSE2}_${xTSE3}_${xTSE4}_${xTSE5}_${xTSE6}_${xTSE7}_${xTSE8}
   fi
}

### ctc ###
export_dsl_scr()
{
	echo "# auto-gen from $0"							 > ${ADSL_SCR}
	echo 												>> ${ADSL_SCR}
	echo '[WaitForConfiguration]={'						>> ${ADSL_SCR}
#	echo 'g997xtusecs ' `echo ${xTSE} | sed 's/_/ /g'`	>> ${ADSL_SCR}
	if [ "$xDSL_Mgmt_Mode" == "Auto" ] ; then
		echo 'mfcs 2 0'									>> ${ADSL_SCR}
	fi
		echo 'llcs 0 1 0x1 0x1 0x1 0x1e116000 5'									>> ${ADSL_SCR}
	echo '}'											>> ${ADSL_SCR}
	echo 												>> ${ADSL_SCR}
	echo '[WaitForLinkActivate]={'						>> ${ADSL_SCR}
	if [ "$RETX_ENA" == "1" ] ; then
		echo 'dms 5048 0 1 1'									>> ${ADSL_SCR}
	fi
	echo '}'											>> ${ADSL_SCR}
	echo 												>> ${ADSL_SCR}
	echo '[WaitForRestart]={'							>> ${ADSL_SCR}
	echo '}'											>> ${ADSL_SCR}
	echo 												>> ${ADSL_SCR}
	echo '[Common]={'									>> ${ADSL_SCR}
	echo '}'											>> ${ADSL_SCR}
	echo 												>> ${ADSL_SCR}

	echo "# auto-gen from $0"							 > ${VDSL_SCR}
	echo 												>> ${VDSL_SCR}
	echo '[WaitForConfiguration]={'						>> ${VDSL_SCR}
#	echo 'g997xtusecs ' `echo ${xTSE} | sed 's/_/ /g'`	>> ${VDSL_SCR}
	if [ "$xDSL_Mgmt_Mode" == "Auto" ] ; then
		echo 'mfcs 1 1'								>> ${VDSL_SCR}
	fi
	echo 'llcs 0 1 0x1 0x1 0x1 0x1e116000 5'											>> ${VDSL_SCR}
	echo 'dms 0x0045 0x0000 0x0002 0000 0004'											>> ${VDSL_SCR}
	echo '}'											>> ${VDSL_SCR}
	echo 												>> ${VDSL_SCR}
	echo '[WaitForLinkActivate]={'						>> ${VDSL_SCR}
		if [ "$RETX_ENA" == "1" ] ; then
		echo 'dms 5048 0 1 1'									>> ${VDSL_SCR}
	fi
	echo 'dms 0x0045 0x0000 0x0002 0000 0004'											>> ${VDSL_SCR}
	echo '}'											>> ${VDSL_SCR}
	echo 												>> ${VDSL_SCR}
	echo '[WaitForRestart]={'							>> ${VDSL_SCR}
	echo '}'											>> ${VDSL_SCR}
	echo 												>> ${VDSL_SCR}
	echo '[Common]={'									>> ${VDSL_SCR}
	echo '}'											>> ${VDSL_SCR}
	echo 												>> ${VDSL_SCR}
}
###########
set_inventory_information()
{
		ARC_PRODUCT_ADSL_VERSION=`ls /firmware/dsl_vr9_firmware_xdsl-*.bin`
		ARC_PRODUCT_ADSL_VERSION=${ARC_PRODUCT_ADSL_VERSION#*-}
		ARC_PRODUCT_ADSL_VERSION=${ARC_PRODUCT_ADSL_VERSION#*_}
		ARC_PRODUCT_ADSL_VERSION=${ARC_PRODUCT_ADSL_VERSION%%.bin*}
		ARC_PRODUCT_VENDOR_ID=${CONFIG_ARC_PRODUCT_VENDOR_ID#*x}
		arVenId1=${ARC_PRODUCT_VENDOR_ID%%" "*}
		ARC_PRODUCT_VENDOR_ID=${ARC_PRODUCT_VENDOR_ID#*x}
		arVenId2=${ARC_PRODUCT_VENDOR_ID%%" "*}
		ARC_PRODUCT_VENDOR_ID=${ARC_PRODUCT_VENDOR_ID#*-}
		ARC_PRODUCT_VENDOR_ID=${ARC_PRODUCT_VENDOR_ID#*" "}

		SPACE=" "
		for j in $(seq 1 6)
		do
		  i=`expr $j + 2`
 			if [ $j -le ${#ARC_PRODUCT_VENDOR_ID} ];
 			then
     		eval arVenId$i=`echo $ARC_PRODUCT_VENDOR_ID | cut -c $j-$j | od -A n -t x1 | cut -d" " -f2`
 			else
     		eval arVenId$i=0;
 			fi
		done

		for i in $(seq 1 16)
		do
 			if [ $i -le ${#ARC_PRODUCT_ADSL_VERSION} ];
 			then
     		eval arAdslVer$i="`echo $ARC_PRODUCT_ADSL_VERSION | cut -c $i-$i | od -A n -t x1 | cut -d" " -f2`"
 			else
     		eval arAdslVer$i=0;
 			fi
		done

		for i in $(seq 1 32)
		do
 			if [ $i -le ${#CONFIG_ARC_PRODUCT_SERIEAL_NUMBER} ];
 			then
     		eval arSerial$i=`echo $CONFIG_ARC_PRODUCT_SERIEAL_NUMBER | cut -c $i-$i | od -A n -t x1 | cut -d" " -f2`
 			else
     		eval arSerial$i=0;
 			fi
		done

		TOTAL_PARAM=""
		for i in $(seq 1 8)
		do
   		eval assign="\$arVenId$i"
   		TOTAL_PARAM=$TOTAL_PARAM$assign$SPACE
		done

		for i in $(seq 1 16)
		do
   		eval assign="\$arAdslVer$i"
   		TOTAL_PARAM=$TOTAL_PARAM$assign$SPACE
		done

		for i in $(seq 1 32)
		do
   		eval assign="\$arSerial$i"
   		TOTAL_PARAM=$TOTAL_PARAM$assign$SPACE
		done

		${BIN_DIR}/dsl_cpe_pipe.sh g997lis $TOTAL_PARAM
}

start()
{

	### ctc ###
	if [ `/bin/grep -c -i voip /proc/cmdline` -ge 1 ] && ! [ -f /tmp/dect_demo_test ] ; then
		return
	fi
	###########

   BIN_DIR=/opt/lantiq/bin
   #FW_DIR=/lib/firmware/`uname -r`
   FW_DIR=/firmware
   INIT_DIR=/etc/init.d

   AUTOBOOT_ADSL=""
   AUTOBOOT_VDSL=""
   NOTIFICATION_SCRIPT=""
   TCPM_IF=""
   DTI_IF=""
   FW_FILENAME=""
   FW_FOUND=0
   START_API=1

   # This default initializations will be overwritten with external values defined
   # within rc.conf. In case of inconsistencies within rc.conf it takes care that
   # this script can be executed without problems using default settings
   xTM_Mgmt_Mode=""
   wanphy_phymode=0
   BS_ENA=1
   SRA_ENA=1
   RETX_ENA=0
   CNTL_MODE_ENA=0
   CNTL_MODE=0

   # detect_wan@system => 0: orginal behavior(non auto-sensing); 1: auto-sensing
   detect_wan=`ccfg_cli get detect_wan@system`
   echo "[ifx_cpe_control_init.sh] detect_wan=$detect_wan" > /dev/console

   # This script handles the DSL FSM for Multimode configuration
   # Determine the mode in which the DSL Control Application should be started
   echo "0" > /tmp/adsl_status
	### ctc ###
#   if [ -r /etc/rc.conf ]; then
#      . /etc/rc.conf 2> /dev/null
   if [ -r $RC_CONF ]; then
      . $RC_CONF 2> /dev/null
   fi
	###########

	### bitonic
	####if [ -f $XDSL_RC_CONF ]; then
	####	echo "[ifx_cpe_control_init.sh] $XDSL_RC_CONF exists" > /dev/console
	####else
	if [ "$detect_wan" == "1" ] ; then # auto-mode
		if [ "`ccfg_cli get wan_type@system`" == "0" ] ; then # auto-mode
			echo "[ifx_cpe_control_init.sh] cp $DEFAULT_AUTO_VDSL_RC_CONF $XDSL_RC_CONF" > /dev/console
			cp $DEFAULT_AUTO_VDSL_RC_CONF $XDSL_RC_CONF
		elif [ "`ccfg_cli get wan_type@system`" == "1" ] ; then # ADSL-mode
			echo "[ifx_cpe_control_init.sh] cp $DEFAULT_AUTO_ADSL_RC_CONF $XDSL_RC_CONF" > /dev/console
			cp $DEFAULT_AUTO_ADSL_RC_CONF $XDSL_RC_CONF
		elif [ "`ccfg_cli get wan_type@system`" == "3" ] ; then # VDSL-mode
			echo "[ifx_cpe_control_init.sh] cp $DEFAULT_AUTO_VDSL_RC_CONF $XDSL_RC_CONF" > /dev/console
			cp $DEFAULT_AUTO_VDSL_RC_CONF $XDSL_RC_CONF
		else
			echo "[ifx_cpe_control_init.sh] cp $DEFAULT_AUTO_VDSL_RC_CONF $XDSL_RC_CONF" > /dev/console
			cp $DEFAULT_AUTO_VDSL_RC_CONF $XDSL_RC_CONF
		fi
	else
		if [ "`ccfg_cli get wan_type@system`" == "0" ] ; then # auto-mode
			echo "[ifx_cpe_control_init.sh] cp $DEFAULT_AUTO_VDSL_RC_CONF $XDSL_RC_CONF" > /dev/console
			cp $DEFAULT_AUTO_VDSL_RC_CONF $XDSL_RC_CONF
		elif [ "`ccfg_cli get wan_type@system`" == "1" ] ; then # ADSL-mode
			echo "[ifx_cpe_control_init.sh] cp $DEFAULT_ADSL_RC_CONF $XDSL_RC_CONF" > /dev/console
			cp $DEFAULT_ADSL_RC_CONF $XDSL_RC_CONF
		elif [ "`ccfg_cli get wan_type@system`" == "3" ] ; then # VDSL-mode
			echo "[ifx_cpe_control_init.sh] cp $DEFAULT_VDSL_RC_CONF $XDSL_RC_CONF" > /dev/console
			cp $DEFAULT_VDSL_RC_CONF $XDSL_RC_CONF
		else
			echo "[ifx_cpe_control_init.sh] cp $DEFAULT_AUTO_VDSL_RC_CONF $XDSL_RC_CONF" > /dev/console
			cp $DEFAULT_AUTO_VDSL_RC_CONF $XDSL_RC_CONF
		fi
	fi

	chmod +r+w $XDSL_RC_CONF
	####fi
	if [ -f $SYSTEM_STATUS ]; then
		echo "[ifx_cpe_control_init.sh] $SYSTEM_STATUS exists" > /dev/console
	else
		echo "[ifx_cpe_control_init.sh] cp $DEFAULT_SYSTEM_STATUS $SYSTEM_STATUS" > /dev/console
		cp $DEFAULT_SYSTEM_STATUS $SYSTEM_STATUS
		chmod +r+w $SYSTEM_STATUS
	fi

	get_data_from_xdsl_rc_conf

	echo "[ifx_cpe_control_init.sh] wanphy_tc=$wanphy_tc" > /dev/console
	echo "[ifx_cpe_control_init.sh] wanphy_phymode=$wanphy_phymode" > /dev/console
	echo "[ifx_cpe_control_init.sh] Next_xDSL_Mode=$Next_xDSL_Mode" > /dev/console
	echo "[ifx_cpe_control_init.sh] Next_xTM_Mode=$Next_xTM_Mode" > /dev/console
	echo "[ifx_cpe_control_init.sh] xDSL_SW_Forced_Reboot=$xDSL_SW_Forced_Reboot" > /dev/console
	echo "[ifx_cpe_control_init.sh] xDSL_ADSL_Sub_Prefix=$xDSL_ADSL_Sub_Prefix" > /dev/console

	if [ "$detect_wan" == "1" ] ; then
		# auto-sensing
   		echo "[ifx_cpe_control_init.sh] auto-sensing" > /dev/console
		xDSL_Mgmt_Mode="Auto"
	else
		# non auto-sensing
   		echo "[ifx_cpe_control_init.sh] non auto-sensing" > /dev/console

		### ctc ###
		if [ "`ccfg_cli get wan_type@system`" == "0" ] ; then # auto-mode
			xDSL_Mgmt_Mode="Auto"
		elif [ "`ccfg_cli get wan_type@system`" == "1" ] ; then # ADSL-mode
			xDSL_Mgmt_Mode="ADSL"
		elif [ "`ccfg_cli get wan_type@system`" == "3" ] ; then # VDSL-mode
			xDSL_Mgmt_Mode="VDSL"
		else
			return
		fi
		###########
	fi
	### end
	echo "[ifx_cpe_control_init.sh] xDSL_Mgmt_Mode=$xDSL_Mgmt_Mode" > /dev/console

	### ctc ###
	MODE=`grep CONFIG_FW_XDSL_ $SYS_CONF`
	if [ `echo $MODE | grep -c ANNEX_A` -ge 1 ] ; then
		xDSL_MODE_VRX="adsl_a"
	elif [ `echo $MODE | grep -c ANNEX_B` -ge 1 ] ; then
		xDSL_MODE_VRX="adsl_b"
	else
		xDSL_MODE_VRX="vdsl"
	fi
	echo "[ifx_cpe_control_init.sh] xDSL_MODE_VRX=$xDSL_MODE_VRX" > /dev/console
	###########

   # Only for VRX Platform, we can use either ADSL or VDSL, hence the
   # DSL Control should be started in both cases
   if [ "$wanphy_phymode" = "0" -o "$wanphy_phymode" = "3" ]; then
		### ctc ###
#      if [ -e ${BIN_DIR}/adsl.scr ]; then
#         AUTOBOOT_ADSL="-a ${BIN_DIR}/adsl.scr"
#      fi
      AUTOBOOT_ADSL="-a ${ADSL_SCR}"
	echo "[ifx_cpe_control_init.sh] AUTOBOOT_ADSL=$AUTOBOOT_ADSL" > /dev/console
		###########

		### ctc ###
#      if [ -e ${BIN_DIR}/vdsl.scr ]; then
#         AUTOBOOT_VDSL="-A ${BIN_DIR}/vdsl.scr"
#      fi
      AUTOBOOT_VDSL="-A ${VDSL_SCR}"
	echo "[ifx_cpe_control_init.sh] AUTOBOOT_VDSL=$AUTOBOOT_VDSL" > /dev/console
		###########

      if [ -e ${INIT_DIR}/xdslrc.sh ]; then
         NOTIFICATION_SCRIPT="-n ${INIT_DIR}/xdslrc.sh"
      fi

      if [ -e ${FW_DIR}/xcpe_hw.bin ]; then
         FW_FILENAME="xcpe_hw.bin"
         FW_FOUND=1
      fi

      echo `${BIN_DIR}/dsl_cpe_control -h` | grep -q "(-d)" && {
         DTI_IF="-d0.0.0.0"
      }

      echo `${BIN_DIR}/dsl_cpe_control -h` | grep -q "(-t)" && {
         TCPM_IF="-t0.0.0.0"
      }

      echo "[ifx_cpe_control_init.sh] xDSL_Mgmt_Mode=$xDSL_Mgmt_Mode, xDSL_SW_Forced_Reboot=$xDSL_SW_Forced_Reboot" > /dev/console
      if [ "$xDSL_Mgmt_Mode" = "Auto" ]; then
         # In Auto Mode check if xDSL_SW_Forced_Reboot is set. We need to pass
         # additional parameters to the dsl_cpe_control program that specifies
         # the Multimode configuration parameters (Next_Adsl_Mode, xDSL_ADSL_Sub_Prefix)
         # i.e, dsl_cpe_control -M<NextMode>_<AdslSubPref>
         if [ "$xDSL_SW_Forced_Reboot" = "1" ]; then
            # Reset the xDSL_SW_Forced_Reboot state to False(0) in rc.conf
            # Save the system configuration (rc.conf)
	  		### bitonic
      		#/usr/sbin/status_oper -u -f /etc/rc.conf SET "xDSL_Config" "xDSL_SW_Forced_Reboot" "0"
      		ccfg_cli -f $XDSL_RC_CONF set xDSL_SW_Forced_Reboot@xDSL_Config="0"
      		### end

			### ctc ###
           	# Save the system configuration (rc.conf)
           	ccfg_cli commitcfg
			###########

            # Populate the Multimode parameters and pass to dsl_cpe_control program
            XDSL_MULTIMODE="-M${Next_xDSL_Mode}_${xDSL_ADSL_Sub_Prefix}"
         else
            # Ignore the Multimode parameters (set to NULL)
            XDSL_MULTIMODE=""
         fi

         if [ "$xTM_Mgmt_Mode" = "Auto" ]; then
            XTM_MULTIMODE="-T4_0_1"
         elif [ "$xTM_Mgmt_Mode" = "PTM" ]; then
            XTM_MULTIMODE="-T2_0_1"
         elif [ "$xTM_Mgmt_Mode" = "ATM" ]; then
            XTM_MULTIMODE="-T1_0_1"
         else
            XTM_MULTIMODE=""
         fi
      fi

		if [ "$detect_wan" == "0" ] ; then
		### ctc ###
			if [ "`umngcli get active_wan_type@system`" == "3" ] ; then # VDSL-learned so preferred
#           	XDSL_MULTIMODE="-M1_1"
				XDSL_MULTIMODE="-M2_0"
			else # ADSL-learned so preferred
#           	XDSL_MULTIMODE="-M2_0"
				XDSL_MULTIMODE="-M1_1"
			fi
		###########
		fi

	echo "[ifx_cpe_control_init.sh] XDSL_MULTIMODE=$XDSL_MULTIMODE, XTM_MULTIMODE=$XTM_MULTIMODE" > /dev/console

      ## Rest of the DSL Bringup is handled below - from ifx_dsl_cpe_control_init.sh
      #  with the addition of XDSL_MODES and XDSL_MULTIMODE options while invoking the
      #  control daemon

      ##########################################################################
      # start dsl cpe control application with appropriate options

      if [ ${FW_FOUND} = 0 -o ${START_API} = 0 ]; then
         echo "[ifx_cpe_control_init] ERROR: No firmware binary available within '${FW_DIR}'" > /dev/console
         echo "or API start blocked manually." > /dev/console
         echo "NO API started!!!" > /dev/console
      else
         # call the function to calculate the xTSE bits
         calc_xtse

		### ctc ###
		export_dsl_scr
		###########

echo "[ifx_cpe_control_init.sh] ADSL_MODE=$ADSL_MODE" >> $OUT_FILE
echo "[ifx_cpe_control_init.sh] ${BIN_DIR}/dsl_cpe_control -i${xTSE} -f ${FW_DIR}/${FW_FILENAME} ${XDSL_MULTIMODE} ${XTM_MULTIMODE} ${AUTOBOOT_VDSL} ${AUTOBOOT_ADSL} ${NOTIFICATION_SCRIPT} ${TCPM_IF} ${DTI_IF}" >> $OUT_FILE

         # start DSL CPE Control Application in the background
         ${BIN_DIR}/dsl_cpe_control -i${xTSE} -f ${FW_DIR}/${FW_FILENAME} \
            ${XDSL_MULTIMODE} ${XTM_MULTIMODE} ${AUTOBOOT_VDSL} ${AUTOBOOT_ADSL} \
            ${NOTIFICATION_SCRIPT} ${TCPM_IF} ${DTI_IF} &

         PS=`ps`
         echo $PS | grep -q dsl_cpe_control && {
            # workaround for nfs: allow write to pipes for non-root
            while [ ! -e /tmp/pipe/dsl_cpe1_ack ] ; do sleep 1; done
            chmod a+w /tmp/pipe/dsl_*
         }
         echo $PS | grep -q dsl_cpe_control || {
            echo "Start of dsl_cpe_control failed!!!"
            false
         }

         if [ "$wanphy_phymode" = "0" ]; then
echo "[ifx_cpe_control_init.sh start] wanphy_phymode=0" >> $OUT_FILE
	  		### bitonic
      		#/usr/sbin/status_oper SET BW_INFO max_us_bw "512"
      		ccfg_cli -f $SYSTEM_STATUS set max_us_bw@BW_INFO="512"
      		### end
         fi

         if [ "$wanphy_phymode" = "0" -o "$wanphy_phymode" = "3" ]; then
            # If BitSwap is disabled, or Retransmission is enabled
            # then set the DSL Parameters accordingly
            #if [ "$BS_ENA" != "1" ]; then
            	 #RETX_ENA=1
               dir="0 1"
               for j in $dir ; do
                  LFCG_VALS=`${BIN_DIR}/dsl_cpe_pipe.sh lfcg $j`
                  if [ "$?" = "0" ]; then
                     for i in $LFCG_VALS; do eval $i 2>/dev/null; done
                     ${BIN_DIR}/dsl_cpe_pipe.sh lfcs $nDirection \
                        $bTrellisEnable $BS_ENA $RETX_ENA $bVirtualNoiseSupport \
                        $b20BitSupport >/dev/null
                  else
                     if [ "$j" = "0" ]; then
                        ${BIN_DIR}/dsl_cpe_pipe.sh lfcs $j 1 $BS_ENA $RETX_ENA \
                           0 -1 >/dev/null
                     else
                        ${BIN_DIR}/dsl_cpe_pipe.sh lfcs $j 1 $BS_ENA $RETX_ENA \
                           0 0 >/dev/null
                     fi
                  fi
               done

               sleep 1
            #fi
            # call to set up inventory information
						set_inventory_information

            if [ "$CNTL_MODE_ENA" = "1" ]; then
               if [ "$CNTL_MODE" = "0" ]; then
                  ${BIN_DIR}/dsl_cpe_pipe.sh tmcs 1 >/dev/null
               elif [ "$CNTL_MODE" = "1" ]; then
                  ${BIN_DIR}/dsl_cpe_pipe.sh tmcs 2 >/dev/null
               fi
            fi
         fi

		### ctc ###
         ${BIN_DIR}/dsl_cpe_pipe.sh llcs 0 1 107 107 107 -1 -1 #jack hsiao recommend
		###########
         ${BIN_DIR}/dsl_cpe_pipe.sh llcs 0 1 1 1 1 1e116000 55 0 # enable only B43 tone per latiq suggestion.
        ###########
        # Since DSL CPE Control daemon and PPE modules have some init timing issue,
        # we use "acs 0" and "acs 1" commands to stop and start DSL CPE Control daemon completely.
        # Otherwise, IAD will happen PADI send out but PADO can not be received from DSL to PPE.(bitonic)
         #${BIN_DIR}/dsl_cpe_pipe.sh acs 2
         ${BIN_DIR}/dsl_cpe_pipe.sh acs 0
         sleep 1
         ${BIN_DIR}/dsl_cpe_pipe.sh acs 1
      fi
   fi
}

stop()
{
	pkill -9 dsl_cpe_control
}


case "$1" in
#	"boot")		start &	;;
	"boot")		start 	;;
	"start")	start &	;;
	"stop")		stop	;;
	*)
				echo $0 'boot  - setup and start CPE control function'
				echo $0 'start - start CPE control function'
				echo $0 'stop  - stop CPE control function'
				;;
esac
