#!/bin/sh
# 512151:jelly lin:2005/12/15:add new feature "firmware upgrade"

get_connected_iface ()
{
	connected_iface="0";
	if [ -n "$1" ]; then
		get_mac=`arp -n | grep $1 | sed -n 's,[ ]*, ,gp' | cut -d' ' -f4`
		[ -n "$get_mac" ] && connected_iface=`brctl showmacs br0 | grep -i $get_mac | cut -f7`
		[ -z "$connected_iface" ] && connected_iface="0"
	fi
}
sync
killall oamd
killall dnrd
sleep 2
killall check_dsl
killall syslogd
killall klogd
#killall telnetd
killall snmpd
killall udhcpc
killall udhcpd
sleep 2
killall udhcpr
killall atmarpd
#killall inetd
#rm -rf /ramdisk/var;
killall ripd
killall cli_be
killall cli_fe
killall ifxsip
killall pppoe-relay
killall minidlna
killall smbd
killall mountd
killall mcastd

[ -f /etc/rc.conf ] && . /etc/rc.conf 2>/dev/null

if [ "$1" != "tr69" ] && [ "$1" != "tr69_postdl" ] && [ "$1" != "diag" ]; then
	killall devm
fi

if [ "$1" != "http" ]; then
	killall dwatch
	killall mini_httpd
fi
if [ "$1" != "tftp" ]; then
	killall tftpd
fi
if [ "$1" != "ftp" ]; then
	killall ftpd
fi

if [ "$1" != "diag" ]; then

  echo 256 > /proc/sys/vm/min_free_kbytes; sleep 1

  # Remove inactive ppp connections
  ([ -n "$wan_main_index" ] && [ -n "$wan_main_defConn" ]) && {
	split_out_index=`echo $wan_main_index | sed 's/,/ /g'`
	for idx in $split_out_index; do
		eval _connN='$'wan_$idx'_connName';
		[ "$wan_main_defConn" = "$_connN" ] || /etc/rc.d/rc.bringup_wan stop $idx
	done
  }

  if [ "$1" != "tr69" ]; then
	killall pppd
  fi

  if [ "$1" != "tr69" ] && [ "$1" != "tr69_postdl" ]; then
	rm -f /flash/rc.conf
  fi

  rm -rf /ramdisk/var /ramdisk/tftp_upload/* /flash/env.sh

  if [ "$1" != "http" ]; then
	grep mtlk /proc/modules
  	if [ $? -eq 0 ]; then
     		. /etc/rc.d/rc.bringup_wlan stop
     		sleep 2
  	fi
  	grep rt3062ap /proc/modules
	if [ $? -eq 0 ]; then
		ifconfig ra0 down
		rmmod rt3062ap
  	fi
  else
	if [ -n $CONFIG_UBOOT_CONFIG_BOOT_FROM_NAND -a $CONFIG_UBOOT_CONFIG_BOOT_FROM_NAND = "1" ]; then
		grep mtlk /proc/modules
	        if [ $? -eq 0 ]; then
	                . /etc/rc.d/rc.bringup_wlan stop
	                sleep 2
	        fi
	fi
  fi

  grep ifxusb_host /proc/modules
  if [ $? -eq 0 ]; then
	for drs in `ls /mnt/usb/`; do umount /mnt/usb/$drs; done
	grep fuse /proc/modules
	[ $? -eq 0 ] && rmmod fuse
	grep autofs4 /proc/modules
	[ $? -eq 0 ] && rmmod -f autofs4
	rmmod ifxusb_host;
  fi

  grep ifxusb_gadget /proc/modules 
  if [ $? -eq 0 ]; then
	brctl delif br0 usb0
	ifconfig usb0 down; sleep 1;
	rmmod g_ether.ko; sleep 1;
	rmmod ifxusb_gadget.ko; 
  fi

  if [ "$1" != "tr69" ]; then
  	grep ifx_nfext_ppp /proc/modules
	[ $? -eq 0 ] && rmmod ifx_nfext_ppp

	grep ifx_nfext_core /proc/modules
	[ $? -eq 0 ] && rmmod ifx_nfext_core
 
	grep iproxyd /proc/modules
	[ $? -eq 0 ] && rmmod iproxyd
  fi

  grep drv_ifxos /proc/modules
  if [ $? -eq 0 ]; then
	killall ifxsip
	rmmod danube_paging
	rmmod cosic
	rmmod drv_timer
	rmmod ifx_voip_timer_driver
	rmmod drv_kpi2udp
	rmmod drv_vmmc
	rmmod drv_tapi
	sleep 1
	rmmod drv_ifxos
	grep CONFIG_TARGET_LTQCPE_PLATFORM_DANUBE /etc/config.sh 2>/dev/null
	[ $? -ne 0 ] && {
		rmmod drv_ter1x66
		sleep 1
		grep drv_ter1x66 /proc/modules
		[ $? -eq 0 ] && rmmod -f drv_ter1x66
	}
  fi

  grep nf_conntrack_proto_esp /proc/modules
  [ $? -eq 0 ] && rmmod nf_conntrack_proto_esp

  grep pecostat_lkm /proc/modules
  [ $? -eq 0 ] && rmmod pecostat_lkm

  if [ "$1" != "tr69" ]; then
#	grep ifx_ppa_api_proc /proc/modules
#	if [ $? -eq 0 ]; then
#		rmmod ifx_ppa_api_proc
#		rmmod ifx_ppa_api
#	fi

	grep drv_dsl_cpe_api /proc/modules
	if [ $? -eq 0 ]; then
		ps | grep -v grep | grep dsl_cpe_control
		if [ $? -eq 0 ]; then
	
			## Set the AutoBoot Config Options to Stop the Link Activation
			/opt/ifx/bin/dsl_cpe_pipe.sh acos 1 1 1 1 0 1
			sleep 1

			###  restart the Autoboot dameon, which will reset the DSL-Line State
			/opt/ifx/bin/dsl_cpe_pipe.sh acs 2
			sleep 2

			###  Stop the Autoboot dameon
			/opt/ifx/bin/dsl_cpe_pipe.sh acs 0
			sleep 2

			### Exit the DSL Control Application
			/opt/ifx/bin/dsl_cpe_pipe.sh quit
			sleep 5

			### Stop the OAM Daemon that already has references to DSL-MEI Driver
			killall -9 oamd
			sleep 2

			### Remove the DSL Driver
			rmmod drv_dsl_cpe_api
			rm -rf /tmp/pipe/*

			grep drv_ifxos /proc/modules
			[ $? -eq 0 ] && rmmod drv_ifxos
	       fi
	fi
  fi
fi

echo 1 > /proc/sys/vm/drop_caches; sleep 1

if [ "$1" = "tr69" ]; then
  /etc/init.d/unload_ppa_modules.sh tr69
  killall -9 devm
  sync;echo 3 > /proc/sys/vm/drop_caches; sleep 1
fi

if [ "$1" = "tr69_postdl" ]; then
  /etc/init.d/unload_ppa_modules.sh
  killall -9 devm
  sync;echo 3 > /proc/sys/vm/drop_caches; sleep 1
fi

