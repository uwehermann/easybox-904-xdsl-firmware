#!/bin/sh

##########################################################################
#
# LAN Ports: Ports 0,1,2 are configured as Lan with VLAN ID 25
# WAN Port:  Port4 as Wan with VLAN ID 631
# CPU is attached to Port6 in ARx188
#
#########################################################################

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#	MODIY THE BELOW VARIABLES TO MATCH YOUR ISP SETTINGS 		##
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##
#

# PPA Enable Status (0 - Disable (default) 1 - Enable)
PPA_Enable="0"

# NAT Status 0 - Disable 1 -Enable (Default)
NAT_Status="1"

MII0_IF="eth0"

# LAN Configuration Information
LAN_VLAN_ID="25"
LAN_VLAN_ID_HexVal="0x0019"
LAN_IP="192.168.1.1"
LAN_MASK="255.255.255.0"

# WAN Configuration Information
WAN_VLAN_ID="631"
WAN_VLAN_ID_HexVal="0x0277"

# PPP Releated information
PPPIDX="44"
USERNAME="ifx-user"
PASSWD="ifx-user123"

# DNS Information
PRIM_DNS="125.22.47.125"
SEC_DNS="202.56.250.5"
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++##

# Remove eth0 from bridge
/sbin/ifconfig br0 > /dev/null 2> /dev/null
status=$?
if [ $status -ne 0 ]; then
	echo
else	
	brctl delif br0 eth0
	/sbin/ifconfig br0 down
	brctl delbr br0
fi

# Create VLAN interfaces on eth0
vconfig add $MII0_IF $LAN_VLAN_ID
vconfig add $MII0_IF $WAN_VLAN_ID
/sbin/ifconfig $MII0_IF down

#turn switch off
mem -s 0x1e1080cc -w 0x000004e1 -u
sleep 1

#set speed for port4,5,6 as 1000Mbit/s
mem -s 0x1e1080cc -w 0x0bbb04f5 -u
sleep 1

#enable port4,6
#mem -s 0x1e1080cc -w 0x00040481 -u
#sleep 1
mem -s 0x1e1080cc -w 0x000404c1 -u
sleep 1

#CPU is attached to port6
mem -s 0x1e1080cc -w 0x00c004e2 -u
sleep 1

#grouping port0,1,2,6 and 4,6 individually
mem -s 0x1e1080cc -w 0x04470403 -u
sleep 1
mem -s 0x1e1080cc -w 0x04470423 -u
sleep 1
mem -s 0x1e1080cc -w 0x04470443 -u
sleep 1
mem -s 0x1e1080cc -w 0x04500483 -u
sleep 1
mem -s 0x1e1080cc -w 0x043f04c3 -u
sleep 1

#set vid for port 0,1,2 with VLAN ID 25 (0x0019)
mem -s 0x1e1080cc -w ${LAN_VLAN_ID_HexVal}0404 -u
sleep 1
mem -s 0x1e1080cc -w ${LAN_VLAN_ID_HexVal}0424 -u
sleep 1
mem -s 0x1e1080cc -w ${LAN_VLAN_ID_HexVal}0444 -u
sleep 1
#set vid for port 4 with VLAN ID 631 (0x0277)
mem -s 0x1e1080cc -w ${WAN_VLAN_ID_HexVal}0484 -u
sleep 1
#Enable port 6 to forward
mem -s 0x1e1080cc -w 0x10cc04c4 -u
sleep 1

#turn switch on
mem -s 0x1e1080cc -w 0x800004e1 -u
sleep 1

# WAN PPPoE configuration
if [ -f /var/run/ppp-pppoe-$PPPIDX.pid ]; then
	line="`cat /var/run/ppp-pppoe-$PPPIDX.pid`"
	PPP_ID="`echo $line | cut -f1 -d ' ' `"
	echo "PPPD (Pid= $PPP_ID) is already configured for this interface - (ppp$PPPIDX)"
	echo "  Please modify the WAN Setting and try again"
	exit
else
	echo "Bringingup PPPoE WAN Connection on ($MII0_IF.$WAN_VLAN_ID)"
fi

# Configure the LAN IP Address
/sbin/ifconfig $MII0_IF 0.0.0.0 up
/sbin/ifconfig $MII0_IF.$LAN_VLAN_ID $LAN_IP netmask $LAN_MASK up

# Create WAN PPP Interface
/sbin/ifconfig $MII0_IF.$WAN_VLAN_ID up > /dev/null 2> /dev/null

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
echo "$MII0_IF.$WAN_VLAN_ID" >> pppoe$PPPIDX

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

echo wan lo 0 /proc/eth/genconf
echo wan hi 10 /proc/eth/genconf

# Configure PPA if enabled
if [ $PPA_Enable = "1" ]; then
	/sbin/ppacmd init
	/sbin/ppacmd addlan -i $MII0_IF.$LAN_VLAN_ID
	/sbin/ppacmd addwan -i ppp${PPPIDX}
	/sbin/ppacmd control --enable-lan --enable-wan
	echo 1 > /proc/sys/net/netfilter/nf_conntrack_tcp_be_liberal
fi


# Stop the existing dnrd service and start again 
# with configured DNS Servers
/etc/rc.d/killproc dnrd > /dev/null 2> /dev/null
sleep 3
/usr/sbin/dnrd -s $PRIM_DNS -s $SEC_DNS	

