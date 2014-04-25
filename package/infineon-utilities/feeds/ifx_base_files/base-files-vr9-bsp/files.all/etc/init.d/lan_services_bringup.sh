#!/bin/sh /etc/rc.common
START=48

start() {
		i=0
		while [ $i -lt $lan_main_Count ]
		do
			eval iface='$lan_main_'$i'_interface'
			/usr/sbin/brctl addbr $iface
			/usr/sbin/brctl setfd $iface 1
			[ `mount|grep -q nfs;echo $?` -eq  0 ] || /usr/sbin/brctl addif $iface eth0
			/usr/sbin/brctl stp $iface off
			i=$(( $i + 1 ))
		done
		#806131:<IFTW-leon> Let board accessing by eth0 before ppa ready
		[ `mount|grep -q nfs;echo $?` -eq  0 ] || /sbin/ifconfig eth0 0.0.0.0 up
		
		#/etc/rc.d/rc.bringup_lan start

		#
		# Setup QOS
		#
		

		# Run some deamons likes http, telnetd
		/etc/rc.d/rc.bringup_services except_devm
}
