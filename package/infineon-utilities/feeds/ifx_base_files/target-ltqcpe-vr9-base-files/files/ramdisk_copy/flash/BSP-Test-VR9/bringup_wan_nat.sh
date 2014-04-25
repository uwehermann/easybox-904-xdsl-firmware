#!/bin/sh

### Following Assumptions are made in this script

if [ "$1" = "" ]; then
        echo "Example: $0 <Ifname> <WANIP>"
	exit 1
fi

iptables -F
iptables -X 
iptables -Z
iptables -F -t nat
iptables -X -t nat
iptables -Z -t nat
iptables -P INPUT ACCEPT
iptables -P OUTPUT ACCEPT
iptables -P FORWARD ACCEPT
iptables -t nat -P PREROUTING ACCEPT
iptables -t nat -P POSTROUTING ACCEPT
iptables -t nat -P OUTPUT ACCEPT
sleep 1
iptables -t nat -A POSTROUTING -o $1 -p udp -j SNAT --to $2:60000-61000
iptables -t nat -A POSTROUTING -o $1 -p tcp -j SNAT --to $2:60000-61000

exit 0
