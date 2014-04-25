#!/bin/sh /etc/rc.common
# Copyright (C) 2010 OpenWrt.org
START=99

#############################################
# please change settings here...
#
#
# set UPDATE_xDSL_FW=1 if update required
#

UPDATE_ADSL_FW=0
UPDATE_VDSL_FW=0
START_DBG_IF=0

TFTP_SERVER="10.5.3.10"

# Selects the used ADSL firmware annex type 'A' or 'B'
ADSLFW_ANNEX=A

REMOTENAME_VDSLFW="vcpe_hw_v9.9.4.1.1.2.bin"
REMOTENAME_ADSLFW="acpe_hw_v9.3.0.11.1.5.bin"

#
#
#############################################

bindir=/opt/lantiq/bin
fwdir=/tmp/fw
cmddir=/usr/bin

start() {

	### ctc ###
	if [ `/bin/grep -c -i voip /proc/cmdline` -ge 1 ] && ! [ -f /tmp/dect_demo_test ] ; then
		return
	fi
	###########

	cd ${fwdir}
	if [ ${UPDATE_ADSL_FW} != 0 ]; then
		if [ -e ${fwdir}/acpe_hw.bin ]; then
			echo "removing existing ADSL FW ${fwdir}/acpe_hw.bin ..."
			rm ${fwdir}/acpe_hw.bin
		fi
		cd ${fwdir}
		echo "downloading new ADSL FW ${REMOTENAME_ADSLFW} ..."
		${cmddir}/tftp -gr ${REMOTENAME_ADSLFW} ${TFTP_SERVER}
		if [ ! -e ${fwdir}/acpe_hw.bin ]; then
			ln -s ${fwdir}/${REMOTENAME_ADSLFW} acpe_hw.bin
		fi
	fi

	if [ ${UPDATE_VDSL_FW} != 0 ]; then
		if [ -e ${fwdir}/vcpe_hw.bin ]; then
			echo "removing existing VDSL FW ${fwdir}/vcpe_hw.bin ..."
			rm ${fwdir}/vcpe_hw.bin
		fi
		cd ${fwdir}
		echo "downloading new VDSL FW ${REMOTENAME_VDSLFW} ..."
		${cmddir}/tftp -gr ${REMOTENAME_VDSLFW} ${TFTP_SERVER}
		if [ ! -e ${fwdir}/vcpe_hw.bin ]; then
			ln -s ${fwdir}/${REMOTENAME_VDSLFW} vcpe_hw.bin
		fi
	fi

	if [ ${UPDATE_ADSL_FW} != 0 ] || [ ${UPDATE_VDSL_FW} != 0 ]; then
		echo "do a restart of the autoboot daemon ..."
		${bindir}/dsl_cpe_pipe.sh "acs 2" > /dev/null
	fi

	if [ ${START_DBG_IF} != 0 ]; then

		# VDSL2 firmware exists?
		if [ -e ${fwdir}/vcpe_hw.bin ]; then
			FW_VARIANT=V
		fi
		# ADSL Annex B or Annex A firmware exists?
		if [ -e ${fwdir}/acpe_hw.bin ]; then
			if [ ${ADSLFW_ANNEX} == B]; then
				FW_VARIANT=${FW_VARIANT}B
			else
				FW_VARIANT=${FW_VARIANT}A
			fi
		fi
		
		echo "Detected firmware variant <${FW_VARIANT}>"

		# set startup options according to firmware avaiability

		case $FW_VARIANT in
		V|VA|VB)
			FW_VDSL="-f ${fwdir}/vcpe_hw.bin"
			;;
		esac

		case $FW_VARIANT in
		A|B|VA|VB)
			FW_ADSL="-F ${fwdir}/acpe_hw.bin"
			;;
		esac
		
		echo "stop the autoboot driver handling ..."
		${bindir}/dsl_cpe_pipe.sh "acs 0" > /dev/null
		echo "exit current control application instance ..."
		${bindir}/dsl_cpe_pipe.sh "quit" > /dev/null
		
		# wait some time to let control application finish
		sleep 5
      
		local myipaddr
		config_load network
		# get addr of interface "lan"
		config_get myipaddr lan ipaddr
		echo "start new control application instance including tcp_message (WinHost) debug interface ..."
		${bindir}/dsl_cpe_control ${FW_ADSL} ${FW_VDSL} -t${myipaddr} &
      
		# wait some time to let control application start
		sleep 5
      
		echo "start the autoboot driver handling ..."
		${bindir}/dsl_cpe_pipe.sh "acs 1" > /dev/null
      
	fi
	
}
