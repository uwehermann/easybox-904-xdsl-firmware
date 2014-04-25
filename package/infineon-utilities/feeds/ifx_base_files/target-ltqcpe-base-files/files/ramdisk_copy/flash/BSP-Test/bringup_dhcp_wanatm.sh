#!/bin/sh

### Following Assumptions are made in this script


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#	MODIY THE BELOW VARIABLES TO MATCH YOUR ISP SETTINGS 		##
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#
# ATM PVC
PVC="0.8.81"
NASIDX="2"

# NAS MAC Address
MAC_ADDR="00:E0:92:21:21:2A"

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

# DNS Information
PRIM_DNS="125.22.47.125"
SEC_DNS="202.56.250.5"

# NAT Status 0 - Disable 1 -Enable (Default)
NAT_Status="1"
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##

case "$1" in
	start)
		/sbin/ifconfig nas${NASIDX} > /dev/null 2> /dev/null
		status=$? 
		if [ "$status" -ne 0 ]; then
			echo "Bringingup EoATM DHCP Connection on (nas${NASIDX})"
			echo "  PVC ($PVC) with Encap - $ENCAP and QoS - $ATM_QOS"
		else
			echo "Interface - (nas$NASIDX) already configured!"
			echo "  Please modify the WAN Setting and try again"
			exit
		fi

		#Create the WAN (nas) interface
		/usr/sbin/br2684ctl -b -p $BR2684_MODE -c $NASIDX -e $ENCAP -q $ATM_QOS -a $PVC
		sleep 2
		/sbin/ifconfig nas${NASIDX} hw ether $MAC_ADDR up


		# Start the dhcp client on the WAN interface
		/sbin/udhcpc -b -i nas${NASIDX} -p /var/run/udhcpc${NASIDX}.pid -s /etc/rc.d/udhcpc.script &

		# For NATed Scenerio enable Masquerading 
		if [ "$NAT_Status" = "1" ]; then
			/usr/sbin/iptables -t nat -A POSTROUTING -o nas${NASIDX} -j MASQUERADE
		fi

		# Enable this if nas is the preferred default gw
		# /sbin/route add default dev nas${NASIDX}

		# Stop the existing dnrd service and start again 
		# with configured DNS Servers
		/etc/rc.d/killproc dnrd > /dev/null 2> /dev/null
		sleep 3
		/usr/sbin/dnrd -s $PRIM_DNS -s $SEC_DNS	
		;;

	stop)
		/sbin/ifconfig nas${NASIDX} > /dev/null 2> /dev/null
		status=$? 
		if [ "$status" -ne 0 ]; then
			echo "Interface - (nas$NASIDX) not configured!"
			echo "  Please check the values and try again"
			exit
		else
			echo "Stopping EoATM DHCP Connection on (nas${NASIDX})"
			echo "  PVC ($PVC) with Encap - $ENCAP and QoS - $ATM_QOS"
		fi

		# Stop the dhcp client on the WAN interface
		/etc/rc.d/killproc udhcpc${NASIDX} > /dev/null 2> /dev/null
		sleep 2

		# Enable this if nas is the preferred default gw
		# /sbin/route del default dev nas${NASIDX}

		# Delete the nas interface
		/sbin/ifconfig nas${NASIDX} down
		/usr/sbin/br2684ctl -k $NASIDX 

		# For NATed Scenerio remove Masquerading rule
		if [ "$NAT_Status" = "1" ]; then
			/usr/sbin/iptables -t nat -D POSTROUTING -o nas${NASIDX} -j MASQUERADE
		fi

		# Stop the dnrd service
		/etc/rc.d/killproc dnrd > /dev/null 2> /dev/null
		;;

	*)
		echo "Usage $0 {start|stop}"
		exit 1
esac
