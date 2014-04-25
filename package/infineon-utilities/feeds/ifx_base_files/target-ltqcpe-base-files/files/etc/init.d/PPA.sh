#!/bin/sh

START=41

start() {
 	if [ "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A5_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D5_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A4_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A4_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D4_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D4_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_E5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_E5_MOD" = "11" ]; then
		# initialize PPA hooks
		/sbin/ppacmd init 2> /dev/null
		# enable wan / lan ingress
		/sbin/ppacmd control --enable-lan --enable-wan 2> /dev/null

		# set WAN vlan range 3 to 4095		
		/sbin/ppacmd addvlanrange -s 3 -e 0xfff 2> /dev/null

		# In PPE firmware is A4 or D4 and ppe is loaded as module then reinitialize TURBO MII mode since it is reset during module load.
                if [ "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A4_MOD" = "11" ]; then
                        echo w 0xbe191808 0000096E > /proc/eth/mem
                fi

                if [ "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D4_MOD" = "11" ]; then
                        echo w 0xbe191808 000003F6 > /proc/eth/mem
                fi

		#For PPA, the ack/sync/fin packet go through the MIPS and normal data packet go through PP32 firmware.
		#The order of packets could be broken due to different processing time.
		#The flag nf_conntrack_tcp_be_liberal gives less restriction on packets and 
		# if the packet is in window it's accepted, do not care about the order

		echo 1 > /proc/sys/net/netfilter/nf_conntrack_tcp_be_liberal
		#if PPA is enabled, enable hardware based QoS to be used later
		/usr/sbin/status_oper SET "IPQoS_Config" "ppe_ipqos" "1"
	fi
}

stop() {

	if [ "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A5_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D5_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A4_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A4_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D4_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D4_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_E5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_E5_MOD" = "11" ]; then
		/sbin/ppacmd exit 2> /dev/null

		/sbin/ppacmd control --disable-lan --disable-wan 2> /dev/null

		# reset to defaul WAN vlan range 0 to 16		
		/sbin/ppacmd addvlanrange -s 0 -e 0x10 2> /dev/null

		#if PPA is stopped, disable hardware based QoS to be used later
		/usr/sbin/status_oper SET "IPQoS_Config" "ppe_ipqos" "0"
	fi
}


case "$1" in
	"boot")		start	;;
	"start")	start	;;
	"stop")		stop	;;
	*)
				echo $0 'boot  - setup and start PPA function'
				echo $0 'start - start PPA function'
				echo $0 'stop  - stop PPA function'
				;;
esac
