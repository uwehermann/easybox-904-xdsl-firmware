#!/bin/sh

### Following Assumptions are made in this script

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#	MODIY THE BELOW VARIABLES TO MATCH YOUR ISP SETTINGS 		##
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#
# ETHERNET CONFIGURATIONS VALID FOR MII-1 ONLY!
MII_IF="eth1"
ETH_WANIDX="2"

# DNS Information
PRIM_DNS="125.22.47.125"
SEC_DNS="202.56.250.5"

# NAT Status 0 - Disable 1 -Enable (Default)
NAT_Status="1"
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##

case "$1" in
	start)
		PID="`cat /var/run/udhcpc${ETH_WANIDX}.pid 2> /dev/null`"
		/etc/rc.d/checkpid $PID
		status=$?
		if [ "$status" -ne 0 ]; then
			echo "Interface - ($MII_IF:$ETH_WANIDX) already configured!"
			echo "  Please modify the WAN Setting and try again"
			exit
		else
			echo "Bringingup WAN Ethernet DHCP Connection on ($MII_IF:$ETH_WANIDX)"
		fi

		#Create the WAN (eth) interface
		/sbin/ifconfig $MII_IF up
		/sbin/ifconfig $MII_IF:$ETH_WANIDX > /dev/null 2> /dev/null

		# Start the dhcp client on the WAN interface - if not already started
		/sbin/udhcpc -b -i $MII_IF:$ETH_WANIDX -p /var/run/udhcpc${ETH_WANIDX}.pid -s /etc/rc.d/udhcpc.script &

		# For NATed SScenerio enable Masquerading 
		if [ "$NAT_Status" = "1" ]; then
			/usr/sbin/iptables -t nat -A POSTROUTING -o $MII_IF -j MASQUERADE
		fi

		## Enable this if eth1 is the preferred default gw
		# /sbin/route add default dev $MII_IF

		# Stop the existing dnrd service and start again 
		# with configured DNS Servers
		/etc/rc.d/killproc dnrd > /dev/null 2> /dev/null
		sleep 3
		/usr/sbin/dnrd -s $PRIM_DNS -s $SEC_DNS	
		;;

	stop)
		PID="`cat /var/run/udhcpc${ETH_WANIDX}.pid 2> /dev/null`"
		/etc/rc.d/checkpid $PID
		status=$?
		if [ "$status" -ne 0 ]; then
			echo "Stopping WAN Ethernet DHCP Connection on ($MII_IF:$ETH_WANIDX)"
		else
			echo "Interface - ($MII_IF:$ETH_WANIDX) not configured!"
			echo "  Please check the values and try again"
			exit
		fi

		## Enable this if eth1 is the preferred default gw
		# /sbin/route del default dev $MII_IF

		# Stop the dhcp client on the WAN interface
		/etc/rc.d/killproc udhcpc${ETH_WANIDX} > /dev/null 2> /dev/null
		sleep 2
		/sbin/ifconfig $MII_IF 0.0.0.0 down > /dev/null 2> /dev/null

		# For NATed Scenerio remove Masquerading rule
		if [ "$NAT_Status" = "1" ]; then
			/usr/sbin/iptables -t nat -D POSTROUTING -o $MII_IF -j MASQUERADE
		fi

		# Stop the dnrd service
		/etc/rc.d/killproc dnrd > /dev/null 2> /dev/null
		;;

	*)
		echo "Usage $0 {start|stop}"
		exit 1
esac

