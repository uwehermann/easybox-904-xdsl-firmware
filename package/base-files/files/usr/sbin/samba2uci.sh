#!/bin/sh


config="/etc/config/samba"
drive1=`umngcli get drive1@samba_sharelist`
drive2=`umngcli get drive2@samba_sharelist`
num1=0
num2=0

sed -i 's/config /config interface /g' ${config}

if [ -n "${drive1}" ] ; then
	num1=`echo ${drive1} | cut -d , -f 5`
fi
if [ -n "${drive2}" ] ; then
	num1=`echo ${drive2} | cut -d , -f 5`
fi

num=0

while [ ${num} -lt ${num1} ] 
do
	idx=$((num*8+6))
	shareable=`echo ${drive1} | cut -d , -f $((idx))`
	name=`echo ${drive1} | cut -d , -f $((idx+1))`
	volume=`echo ${drive1} | cut -d , -f $((idx+2))`
	path=`echo ${drive1} | cut -d , -f $((idx+3))`
	security=`echo ${drive1} | cut -d , -f $((idx+4))`
	access_mode=`echo ${drive1} | cut -d , -f $((idx+5))`
	ropasswd=`echo ${drive1} | cut -d , -f $((idx+6))`
	rwpasswd=`echo ${drive1} | cut -d , -f $((idx+7))`
	
	i=$((num+1))
	if [ ${shareable} -eq 1 ] ; then
		uci set ${config}.sambashare${i}.name=${name}
		uci set ${config}.sambashare${i}.path=/tmp/usb/${volume}
	fi
	num=$((num+1))
done


sed -i 's/config interface /config /g' ${config}
/etc/init.d/samba boot


