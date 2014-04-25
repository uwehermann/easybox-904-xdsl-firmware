#!/bin/sh

### Following Assumptions are made in this script


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#	MODIY THE BELOW VARIABLES TO MATCH YOUR ISP SETTINGS 		##
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#
# ATM PVC
PVC="0.0.36"
NASIDX="33"

# NAS MAC Address
MAC_ADDR="00:E0:92:32:32:3B"

# 0 - Routed, 1 - Bridged (Default)
BR2684_MODE="1" 

# 0 - LLC/SNAP (Default), 1 - VCMux
ENCAP="0"


# PPP Releated information
PPPIDX="33"
USERNAME="ifx-user"
PASSWD="ifx-user123"


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

# Acceleration Parameters
LAN_IF="br0"

# NAT Status 0 - Disable 1 -Enable (Default)
NAT_Status="1"
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##

case "$1" in
	start)
		/sbin/ifconfig nas${NASIDX} > /dev/null 2> /dev/null
		status=$? 
		if [ "$status" -ne 0 ]; then
			echo "Bringingup PPPoEoATM WAN Connection on (nas${NASIDX})"
			echo "  PVC ($PVC) with Encap - $ENCAP and QoS - $ATM_QOS"
		else
			echo "Interface - (nas$NASIDX) already configured!"
			echo "  Please modify the WAN Setting and try again"
			exit
		fi

		# Create the WAN (nas) interface
		/usr/sbin/br2684ctl -b -p $BR2684_MODE -c $NASIDX -e $ENCAP -q $ATM_QOS -a $PVC
		sleep 2
		/sbin/ifconfig nas${NASIDX} hw ether $MAC_ADDR up

		cd /etc/ppp/peers
		
		# Create the PPP Configuration file
		echo "linkname pppoe-$PPPIDX" > pppoe$PPPIDX
		echo "lcp-echo-interval 10" >> pppoe$PPPIDX
		echo "lcp-echo-failure 3" >> pppoe$PPPIDX
		echo "unit $PPPIDX" >> pppoe$PPPIDX
		echo "maxfail 0" >> pppoe$PPPIDX
		echo "usepeerdns" >> pppoe$PPPIDX
		echo "noipdefault" >> pppoe$PPPIDX
		echo "defaultroute" >> pppoe$PPPIDX
		echo "user $USERNAME" >> pppoe$PPPIDX
		echo "password $PASSWD" >> pppoe$PPPIDX
		echo "mtu 1492" >> pppoe$PPPIDX
		echo "mru 1492" >> pppoe$PPPIDX
		echo "holdoff 4" >> pppoe$PPPIDX
		echo "persist" >> pppoe$PPPIDX
		echo "plugin /usr/lib/pppd/2.4.2/rp-pppoe.so" >> pppoe$PPPIDX
		echo "nas${NASIDX}" >> pppoe$PPPIDX

		# Invoke the PPPD process
		/usr/sbin/pppd call pppoe$PPPIDX & > /dev/null 2> /dev/null
		sleep 5

		# Check for the ppp interface creation
		/sbin/ifconfig ppp${PPPIDX} > /dev/null 2> /dev/null
		status=$? 
		if [ "$status" -ne 0 ]; then
			echo "ppp$PPPIDX is not up. Trying indefinitely..."
		else
			echo "ppp$PPPIDX is up. "
		fi

		# Enable IP Forwarding
		ln -sf /ramdisk/etc/ppp/resolv.conf /ramdisk/etc/resolv.conf
		echo 1 >/proc/sys/net/ipv4/ip_forward

		# Clamp the MSS to path-mtu
		iptables -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS  --clamp-mss-to-pmtu

		# Enable Masquerading on the outgoing ppp interface
		if [ "$NAT_Status" = "1" ]; then
			/usr/sbin/iptables -t nat -A POSTROUTING -o ppp$PPPIDX -j MASQUERADE
		fi

		# Stop the existing dnrd service and start again 
		# with configured DNS Servers
		/etc/rc.d/killproc dnrd > /dev/null 2> /dev/null
		sleep 3
		/usr/sbin/dnrd -s $PRIM_DNS -s $SEC_DNS	

		# Enable Acceleration Mode now
		/sbin/ppacmd init
		/sbin/ppacmd control --enable-lan --enable-wan
		/sbin/ppacmd addlan -i $LAN_IF
		/sbin/ppacmd addwan -i ppp${PPPIDX}
		echo 1 > /proc/sys/net/netfilter/nf_conntrack_tcp_be_liberal
		;;

	stop)
		/sbin/ifconfig nas${NASIDX} > /dev/null 2> /dev/null
		status=$? 
		if [ "$status" -ne 0 ]; then
			echo "Interface - (nas$NASIDX::ppp$PPPIDX) not configured!"
			echo "  Please check the values and try again"
			exit
		else
			echo "Stopping PPPoEoATM Connection on (nas$NASIDX::ppp${PPPIDX})"
			echo "  PVC ($PVC) with Encap - $ENCAP and QoS - $ATM_QOS"
		fi

		# Disable Acceleration Mode now
		/sbin/ppacmd delwan -i ppp${PPPIDX}
		/sbin/ppacmd dellan -i $LAN_IF
		/sbin/ppacmd control --disable-lan --disable-wan
		/sbin/ppacmd exit
		echo 0 > /proc/sys/net/netfilter/nf_conntrack_tcp_be_liberal

		# Stop the PPP client on the WAN interface
		/etc/rc.d/init.d/kill_ppp_proc $PPPIDX "OE"  > /dev/null 2> /dev/null
		sleep 2

		# Remove the nas interface
		/sbin/ifconfig nas${NASIDX} down
		/usr/sbin/br2684ctl -k $NASIDX 
		sleep 1

		# Remove the PPP Configuration file
		/bin/rm -f /etc/ppp/peers/pppoe$PPPIDX

		# For NATed Scenerio remove Masquerading rule
		if [ "$NAT_Status" = "1" ]; then
			/usr/sbin/iptables -t nat -D POSTROUTING -o ppp${PPPIDX} -j MASQUERADE
		fi

		# Remove the MSS clamping 
		iptables -D FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS  --clamp-mss-to-pmtu

		# Stop the dnrd service
		/etc/rc.d/killproc dnrd > /dev/null 2> /dev/null
		;;

	*)
		echo "Usage $0 {start|stop}"
		exit 1
esac

