#!/bin/sh

Zone1="br-redo br-redx"
Zone2="br-lan"
#ZoneRelated=test.txt
#rm -f $ZoneRelated
. /usr/sbin/iptables_utility.sh

ipt_lock_res
for Zone1E in $Zone1
do
	for Zone2E in $Zone2
	do
#		echo "iptables -t filter -I FORWARD -i $Zone1E -o $Zone2E -j DROP" >> $ZoneRelated
#		echo "iptables -t filter -I FORWARD -o $Zone1E -i $Zone2E -j DROP" >> $ZoneRelated
		iptables -t filter -I FORWARD -i $Zone1E -o $Zone2E -j DROP
		iptables -t filter -I FORWARD -o $Zone1E -i $Zone2E -j DROP
	done
done
#echo "iptables -t filter -I INPUT -s 10.0.0.0/8 -d 192.168.2.1/24 -j DROP" >> $ZoneRelated
#echo "iptables -t filter -I INPUT -d 10.0.0.0/8 -s 192.168.2.1/24 -j DROP" >> $ZoneRelated
iptables -t filter -I INPUT -s 10.0.0.0/8 -d 192.168.2.1/24 -j DROP
iptables -t filter -I INPUT -d 10.0.0.0/8 -s 192.168.2.1/24 -j DROP
ipt_unlock_res
#cat $ZoneRelated
