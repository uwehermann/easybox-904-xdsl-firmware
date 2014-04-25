#!/bin/sh

### Following Assumptions are made in this script


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#	MODIY THE BELOW VARIABLES TO MATCH YOUR ISP SETTINGS 		##
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#
# ATM PVC
VPI=1
VCI=32

# NAS MAC Increment Value 
MAC_INCR=1


# 0 - Routed, 1 - Bridged (Default)
BR2684_MODE="1" 

# 0 - LLC/SNAP (Default), 1 - VCMux
ENCAP="0"

# Possible ATM Classes include UBR (Default), CBR, RT-VBR, NRT-VBR & UBR+
ATM_QOS="UBR,aal5:"
#ATM_QOS="UBR,aal5:max_pcr=1250,cdv=100"
#ATM_QOS="CBR,aal5:min_pcr=1250,cdv=100"
#ATM_QOS="NRT-VBR,aal5:max_pcr=1250,cdv=100,min_pcr=1000,scr=1100,mbs=1000"
#ATM_QOS="RT-VBR,aal5:max_pcr=1250,cdv=100,scr=1100,mbs=1000"
#ATM_QOS="UBR+,aal5:max_pcr=1250,cdv=100,min_pcr=1000"
#

# Bridge Interface
BRIDGE_IF="br0"
BRIDGE_IP="192.168.1.1"

# Number of Multi-PVC (Max. 8 supported)
PVC_CNT="4"

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##

case "$1" in
	start)
		if [ $PVC_CNT -gt 8 ]; then
			echo "Only Max: 8 PVCs supported!"
			exit 1
		fi

		# Create the bridge	
		/usr/sbin/brctl addbr $BRIDGE_IF > /dev/null 2> /dev/null
		/sbin/ifconfig $BRIDGE_IF $BRIDGE_IP down

		i=0
		while [ $i -lt $PVC_CNT ]
		  do
			NASIDX=$i
			VCI=`expr $VCI + 1`
			MAC_ADDR="00:E0:92:${MAC_INCR}1:11:1A"
			/usr/sbin/br2684ctl -b -p $BR2684_MODE -c $NASIDX -e $ENCAP -q $ATM_QOS -a 0.$VPI.$VCI
			sleep 3
			/sbin/ifconfig nas${NASIDX} hw ether $MAC_ADDR up
			/usr/sbin/brctl addif $BRIDGE_IF nas${NASIDX}
			sleep 1
			MAC_INCR=`expr $MAC_INCR + 1`
			i=`expr $i + 1`
		  done

		# Bringup the bridge now	
		sleep 5
		/sbin/ifconfig $BRIDGE_IF up
		;;

	stop)
		# Bringdown the bridge
		/sbin/ifconfig $BRIDGE_IF down

		# Delete the nas interface
		i=0
		while [ $i -lt $PVC_CNT ]
		  do
			NASIDX=$i
			/sbin/ifconfig nas${NASIDX} down
			sleep 1
			/usr/sbin/br2684ctl -k $NASIDX 
			i=`expr $i + 1`
		  done

		# Delete the bridge	
		/usr/sbin/brctl delbr $BRIDGE_IF > /dev/null 2> /dev/null
		;;

	*)
		echo "Usage $0 {start | stop}"
		exit 1
esac

