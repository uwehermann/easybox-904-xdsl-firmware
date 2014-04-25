#!/bin/sh

### Following Assumptions are made in this script


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#	MODIY THE BELOW VARIABLES TO MATCH YOUR ISP SETTINGS 		##
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#
# ETHERNET CONFIGURATIONS VALID FOR MII-1 ONLY!
MII_IF="eth1"
ETH_WANIDX="3"

# PPP Releated information
PPPIDX="2"
USERNAME="ifx-user"
PASSWD="ifx-user123"

# DNS Information
PRIM_DNS="125.22.47.125"
SEC_DNS="202.56.250.5"
#
# NAT Status 0 - Disable 1 -Enable (Default)
NAT_Status="1"
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##

case "$1" in
	start)
		if [ -f /var/run/ppp-pppoe-$PPPIDX.pid ]; then
			line="`cat /var/run/ppp-pppoe-$PPPIDX.pid`"
			PPP_ID="`echo $line | cut -f1 -d ' ' `"
			echo "PPPD (Pid= $PPP_ID) is already configured for this interface - (ppp$PPPIDX)"
			echo "  Please modify the WAN Setting and try again"
			exit
		else
			echo "Bringingup PPPoE WAN Connection on ($MII_IF:${ETH_WANIDX})"
		fi
		
		#Create the WAN (eth) interface
		/sbin/ifconfig $MII_IF up
		/sbin/ifconfig $MII_IF:$ETH_WANIDX > /dev/null 2> /dev/null

		# Create the PPP Configuration file
		cd /etc/ppp/peers
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
		echo "${MII_IF}" >> pppoe$PPPIDX

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
		;;

	stop)
		if [ -f /var/run/ppp-pppoe-$PPPIDX.pid ]; then
			line="`cat /var/run/ppp-pppoe-$PPPIDX.pid`"
			PPP_ID="`echo $line | cut -f1 -d ' ' `"
			echo "Stopping PPPoE Connection on (ppp${PPPIDX})"
		else
			echo "Interface - (ppp$PPPIDX) not configured!"
			echo "  Please check the values and try again"
			exit
		fi

		# Stop the PPP client on the WAN interface
		/bin/kill -9 $PPP_ID  > /dev/null 2> /dev/null
		sleep 2
		/bin/rm -f /var/run/ppp-pppoe-$PPPIDX.pid

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

