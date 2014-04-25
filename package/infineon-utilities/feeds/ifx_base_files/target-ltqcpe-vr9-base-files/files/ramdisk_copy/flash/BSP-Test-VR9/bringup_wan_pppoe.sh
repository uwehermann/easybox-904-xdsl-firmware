#!/bin/sh

### Following Assumptions are made in this script


if [ "$1" = "" ]; then
        echo "Example: $0 <Ifname> <WANIP> <Username> <Password>"
        exit 1
fi

echo "linkname pppoe-1" > /etc/ppp/peers/pppoe1
echo "lcp-echo-interval 10" >> /etc/ppp/peers/pppoe1
echo "lcp-echo-failure 3" >> /etc/ppp/peers/pppoe1
echo "unit 1" >> /etc/ppp/peers/pppoe1
echo "maxfail 0" >> /etc/ppp/peers/pppoe1
echo "usepeerdns" >> /etc/ppp/peers/pppoe1
echo "defaultroute" >> /etc/ppp/peers/pppoe1
echo "user $3" >> /etc/ppp/peers/pppoe1
echo "password $4" >> /etc/ppp/peers/pppoe1
echo "mtu 1492" >> /etc/ppp/peers/pppoe1
echo "persist" >> /etc/ppp/peers/pppoe1
echo "plugin /usr/lib/pppd/2.4.2/rp-pppoe.so" >> /etc/ppp/peers/pppoe1
echo "$1" >> /etc/ppp/peers/pppoe1
echo "debug" >> /etc/ppp/peers/pppoe1
/usr/sbin/pppd call pppoe1
sleep 1
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
iptables -t nat -A POSTROUTING -o ppp1 -s $2/24 -j MASQUERADE

exit 0
