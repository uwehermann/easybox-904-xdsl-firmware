#!/bin/sh

### Following Assumptions are made in this script


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#	MODIY THE BELOW VARIABLES TO MATCH YOUR ISP SETTINGS 		##
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#
# ATM PVC
PVC="0.0.32"


# Encapsulation for PPPoATM (vc-encaps - Default)
ENCAP="vc-encaps"
#ENCAP="llc-encaps"


# PPP Releated information
PPPIDX="1"
USERNAME="ifx-user"
PASSWD="ifx-user123"


# Only supported ATM Class UBR with the below values 
ATM_QOS="UBR,aal5:max_pcr=0,min_pcr=0"
#

# DNS Information
PRIM_DNS="125.22.47.125"
SEC_DNS="202.56.250.5"

# NAT Status 0 - Disable 1 -Enable (Default)
NAT_Status="1"
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##

case "$1" in
	start)
		/sbin/ifconfig ppp${PPPIDX} > /dev/null 2> /dev/null
		status=$? 
		if [ "$status" -ne 0 ]; then
			echo "Bringingup PPPoA Connection on (ppp${PPPIDX})"
			echo "  PVC ($PVC) with Encap - $ENCAP and QoS - $ATM_QOS"
		else
			echo "Interface - (ppp$PPPIDX) already configured!"
			echo "  Please modify the WAN Setting and try again"
			exit
		fi


		cd /etc/ppp/peers
		
		#Create the PPP Configuration file
		echo "linkname pppoatm-$PPPIDX" > pppoatm$PPPIDX
		echo "lcp-echo-interval 10" >> pppoatm$PPPIDX
		echo "lcp-echo-failure 3" >> pppoatm$PPPIDX
		echo "unit $PPPIDX" >> pppoatm$PPPIDX
		echo "maxfail 0" >> pppoatm$PPPIDX
		echo "usepeerdns" >> pppoatm$PPPIDX
		echo "noipdefault" >> pppoatm$PPPIDX
		echo "defaultroute" >> pppoatm$PPPIDX
		echo "user $USERNAME" >> pppoatm$PPPIDX
		echo "password $PASSWD" >> pppoatm$PPPIDX
		echo "mtu 1492" >> pppoatm$PPPIDX
		echo "mru 1492" >> pppoatm$PPPIDX
		echo "holdoff 4" >> pppoatm$PPPIDX
		echo "persist" >> pppoatm$PPPIDX
		echo "plugin /usr/lib/pppd/2.4.2/pppoatm.so" >> pppoatm$PPPIDX
		echo "$ENCAP" >> pppoatm$PPPIDX
		echo "$PVC" >> pppoatm$PPPIDX
		echo "qos $ATM_QOS" >> pppoatm$PPPIDX

		# Invoke the PPPD process
		/usr/sbin/pppd call pppoatm$PPPIDX & > /dev/null 2> /dev/null
		sleep 5

		# Check for the ppp interface creation
		/sbin/ifconfig ppp${PPPIDX} > /dev/null 2> /dev/null
		status=$? 
		if [ "$status" -ne 0 ]; then
			echo "ppp$PPPIDX is not up.  Killing pppd"
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
		/sbin/ifconfig ppp${PPPIDX} > /dev/null 2> /dev/null
		status=$? 
		if [ "$status" -ne 0 ]; then
			echo "Interface - (ppp$PPPIDX) not configured!"
			echo "  Please check the values and try again"
			exit
		else
			echo "Stopping PPPoA Connection on (ppp${PPPIDX})"
			echo "  PVC ($PVC) with Encap - $ENCAP and QoS - $ATM_QOS"
		fi

		# Stop the PPP client on the WAN interface
		/etc/rc.d/init.d/kill_ppp_proc $PPPIDX "OA"  > /dev/null 2> /dev/null
		sleep 2

		# Remove the PPP Configuration file
		/bin/rm -f /etc/ppp/peers/pppoatm$PPPIDX

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

