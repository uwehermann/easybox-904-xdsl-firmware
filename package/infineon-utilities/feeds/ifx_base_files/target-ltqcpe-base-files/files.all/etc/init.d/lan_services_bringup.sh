#!/bin/sh /etc/rc.common
#START=48
START=21

	if [ ! "$ENVLOADED" ]; then
		if [ -r /etc/rc.conf ]; then
			. /etc/rc.conf 2> /dev/null
			ENVLOADED="1"
		fi
	fi

	if [ ! "$CONFIGLOADED" ]; then
	if [ -r /etc/rc.d/config.sh ]; then
		. /etc/rc.d/config.sh 2>/dev/null
#		. /etc/rc.d/model_config.sh 2>/dev/null
		CONFIGLOADED="1"
	fi
	fi

#        if [ -r /etc/rc.d/model_config.sh ]; then
#                . /etc/rc.d/model_config.sh 2>/dev/null
#        fi


start() {

## Handle IPv6
# Merge from S49ipv6.sh

	if [ "$CONFIG_PACKAGE_KMOD_IPV6" = "1"  -a  "$ipv6_status" = "1" ]; then
		echo 0 > /proc/sys/net/ipv6/conf/all/disable_ipv6
		echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
	elif [ "$CONFIG_PACKAGE_KMOD_IPV6" = "1"  -a  "$ipv6_status" = "0" ]; then
		 echo 1 > /proc/sys/net/ipv6/conf/all/disable_ipv6
		 echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
	elif [ "$CONFIG_FEATURE_IPv6" = "1" ]; then
		echo 1 > /proc/sys/net/ipv6/conf/all/forwarding
	fi


### Bringup SYSLOG Daemon
# Merge from S41syslogd.sh
	# Start System log
	if [ -f /sbin/syslogd ]; then
		killall syslogd 2>/dev/null
#		echo "Bringing up syslog"
		if [ "$system_log_mode" = "1" -o "$system_log_mode" = "2" ]; then
			if [ -n "$system_log_IP" -a "$system_log_IP" != "0.0.0.0" ]; then
				if [ -n "$system_log_port" -a "$system_log_port" != "0" ]; then
					if [ "$system_log_mode" = "2" ]; then
						/sbin/syslogd -L -s	$CONFIG_FEATURE_SYSTEM_LOG_BUFFER_SIZE -b $CONFIG_FEATURE_SYSTEM_LOG_BUFFER_COUNT -R $system_log_IP:$system_log_port -l $system_log_log_level
					else
						/sbin/syslogd -s $CONFIG_FEATURE_SYSTEM_LOG_BUFFER_SIZE -b $CONFIG_FEATURE_SYSTEM_LOG_BUFFER_COUNT -R $system_log_IP:$system_log_port -l $system_log_log_level
					fi
				else
					if [ "$system_log_mode" = "2" ]; then
						/sbin/syslogd -L -s $CONFIG_FEATURE_SYSTEM_LOG_BUFFER_SIZE -b $CONFIG_FEATURE_SYSTEM_LOG_BUFFER_COUNT 0 -R $system_log_IP -l $system_log_log_level
					else
						/sbin/syslogd -s $CONFIG_FEATURE_SYSTEM_LOG_BUFFER_SIZE -b $CONFIG_FEATURE_SYSTEM_LOG_BUFFER_COUNT 0 -R $system_log_IP -l $system_log_log_level
					fi
				fi
			fi
		else
			/sbin/syslogd -s $CONFIG_FEATURE_SYSTEM_LOG_BUFFER_SIZE -b $CONFIG_FEATURE_SYSTEM_LOG_BUFFER_COUNT -l $system_log_log_level
		fi
	fi

### Setup HostName for Loopback Interface ###
	# Setup lo Interface Addresses
	/sbin/ifconfig lo 127.0.0.1 netmask 255.0.0.0
	if [ -n "$CONFIG_TARGET_LTQCPE_PLATFORM_AR9_VB" ]; then
		echo 0 > /proc/sys/net/ipv4/ip_forward
	else
		echo 1 > /proc/sys/net/ipv4/ip_forward
	fi
	
	# Setup Hostname
	echo "127.0.0.1 localhost.localdomain localhost" > /etc/hosts
	if [ -f /bin/hostname ]; then
		/bin/hostname $hostname
	fi
	
	i=0
	while [ $i -lt $lan_main_Count ]
	do
		eval ip='$lan_main_'$i'_ipAddr'
		shorthost=${hostname%%.*}
		echo "$ip ${hostname} $shorthost" >> /etc/hosts
		i=$(( $i + 1 ))
	done
#Set Active Wan Pvc's parameter in /tmp/system/status for LCP Prioritization
	/usr/sbin/status_oper SET "LCP_Wan_Info" "active_wan_pvc" "0"
### Initialize the PPA hooks for use in LAN Start ###
### Merge from PPA.sh script in S17PPA.sh ###

 	if [ "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A5_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D5_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A4_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A4_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D4_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D4_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_E5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_E5_MOD" = "11" ]; then
		# initialize PPA hooks
		/sbin/ppacmd init 2> /dev/null

		# enable wan / lan ingress
		/sbin/ppacmd control --enable-lan --enable-wan 2> /dev/null

		# set WAN vlan range 3 to 4095		
		/sbin/ppacmd addvlanrange -s 3 -e 0xfff 2> /dev/null

		# In case of A4 or D4 and if ppe is loaded as module, in case of danube/tp-ve, 
		# then reinitialize TURBO MII mode since it is reset during module load.
		platform=${CONFIG_IFX_MODEL_NAME%%_*}
		if [ "$platform" = "DANUBE" -o "$platform" = "TP-VE" ]; then
                	if [ "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A4_MOD" = "11" ]; then
	                        echo w 0xbe191808 0000096E > /proc/eth/mem
        	        fi

                	if [ "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D4_MOD" = "11" ]; then
	                        echo w 0xbe191808 000003F6 > /proc/eth/mem
	                fi
		fi

		#For PPA, the ack/sync/fin packet go through the MIPS and normal data packet go through PP32 firmware.
		#The order of packets could be broken due to different processing time.
		#The flag nf_conntrack_tcp_be_liberal gives less restriction on packets and 
		# if the packet is in window it's accepted, do not care about the order

		echo 1 > /proc/sys/net/netfilter/nf_conntrack_tcp_be_liberal
		#if PPA is enabled, enable hardware based QoS to be used later
		/usr/sbin/status_oper SET "IPQoS_Config" "ppe_ipqos" "1"
		if [ "$CONFIG_IFX_CONFIG_CPU" = "XRX288" ]; then
				if [ "$qm_enable" = "0" ]; then
					if [ "$wanphy_phymode" = "2" ]; then
					echo eth1 prio 0 queue 1 prio 1 queue 1 prio 2 queue 1 prio 3 queue 1 prio 4 queue 1 prio 5 queue 1 prio 6 queue 1 prio 7 queue 0 > /proc/eth/prio
					elif [ "$wanphy_tc" = "1" -a "$wanphy_phymode" = "3" ]; then
					echo ptm0 prio 0 queue 1 prio 1 queue 1 prio 2 queue 1 prio 3 queue 1 prio 4 queue 1 prio 5 queue 1 prio 6 queue 1 prio 7 queue 0 > /proc/eth/prio
					fi
				
				fi
		elif [ "$CONFIG_IFX_CONFIG_CPU" = "AMAZON_S" ]; then
			if [ "$qm_enable" = "0" ]; then
                                if [ "$wanphy_phymode" = "2" ]; then
                                        echo eth1 prio 0 queue 1 prio 1 queue 1 prio 2 queue 1 prio 3 queue 1 prio 4 queue 1 prio 5 queue 1 prio 6 queue 1 prio 7 queue 0 > /proc/eth/prio
                                fi
				if [ "$wanphy_tc" = "1" -a "$wanphy_phymode" = "0" ]; then
					echo ptm0 prio 0 queue 1 prio 1 queue 1 prio 2 queue 1 prio 3 queue 1 prio 4 queue 1 prio 5 queue 1 prio 6 queue 1 prio 7 queue 0 > /proc/eth/prio
				fi
                        fi
		fi
fi

	if [ "$CONFIG_FEATURE_IFX_TR69_DEVICE" = "1" ]; then
		i=0
		while [ $i -lt $lan_main_Count ]
		do
			eval iface='$lan_main_'$i'_interface'
			eval ip='$'lan_main_${i}_ipAddr
			eval netmask='$'lan_main_${i}_netmask

			/usr/sbin/brctl addbr $iface
			/usr/sbin/brctl setfd $iface 1
			[ `mount|grep -q nfs;echo $?` -eq  0 ] && /sbin/ifconfig $iface $ip netmask $netmask up
			/usr/sbin/brctl addif $iface eth0
			if [ -n "$CONFIG_TARGET_LTQCPE_PLATFORM_AR9_VB" ]; then
				/usr/sbin/brctl addif $iface eth1
			fi

			/usr/sbin/brctl stp $iface off
			i=$(( $i + 1 ))
		done
		/sbin/ifconfig eth0 0.0.0.0 up
#		if [ -n "$CONFIG_TARGET_LTQCPE_PLATFORM_AR9_VB" ]; then
#			/sbin/ifconfig eth1 0.0.0.0 up
#			#enable flow control
#			ethtool --pause eth1 rx on tx on 
#		fi
		
		/etc/rc.d/rc.bringup_lan start
		
		if [ -f /etc/rc.d/udhcpc.script ]; then
			/etc/rc.d/init.d/udhcpc start 0
		fi
		
		/sbin/route add default dev $iface
		
		# Run some deamons likes http, telnetd
		/etc/rc.d/rc.bringup_services start
	else
		if [ "$CONFIG_FEATURE_NAPT" = "1" ]; then
			if [ -f /usr/sbin/naptcfg ]; then
				/usr/sbin/naptcfg --FWinit > /dev/null
				/usr/sbin/naptcfg --NAPTinit > /dev/null
				/usr/sbin/naptcfg --MACFilterinit > /dev/null
				/usr/sbin/naptcfg --PacketFilterinit > /dev/null
				/usr/sbin/naptcfg --ServicesACLinit > /dev/null
			fi
		fi
		
		i=0
		while [ $i -lt $lan_main_Count ]
		do
			eval iface='$lan_main_'$i'_interface'
			eval ip='$'lan_main_${i}_ipAddr
			eval netmask='$'lan_main_${i}_netmask
			/usr/sbin/brctl addbr $iface
			/usr/sbin/brctl setfd $iface 1
			[ `mount|grep -q nfs;echo $?` -eq  0 ] && /sbin/ifconfig $iface $ip netmask $netmask up
			/usr/sbin/brctl addif $iface eth0
			if [ -n "$CONFIG_TARGET_LTQCPE_PLATFORM_AR9_VB" ]; then
                                  /usr/sbin/brctl addif $iface eth1
                        fi
			/usr/sbin/brctl stp $iface off
			i=$(( $i + 1 ))
		done
		#806131:<IFTW-leon> Let board accessing by eth0 before ppa ready
		/sbin/ifconfig eth0 0.0.0.0 up
#		if [ -n "$CONFIG_TARGET_LTQCPE_PLATFORM_AR9_VB" ]; then
#			/sbin/ifconfig eth1 0.0.0.0 up
#			#enable flow control
#			ethtool --pause eth1 rx on tx on
#		fi		

		# Start DevM
		/etc/rc.d/rc.bringup_services start_devm
		
		/etc/rc.d/rc.bringup_lan start

		if [ "$CONFIG_FEATURE_IFX_A4" = "1" ]; then
			#806131:<IFTW-leon> Let board accessing by eth0 before ppa ready start
			/usr/sbin/brctl delif br0 eth0
			/sbin/ifconfig br0 0.0.0.0 down
			/usr/sbin/status_oper SET Lan1_IF_Info STATUS DOWN
			
			/sbin/ifconfig eth0 $lan_main_0_ipAddr netmask $lan_main_0_netmask up
			/usr/sbin/status_oper SET Lan1_IF_Info STATUS "UP" IP "$lan_main_0_ipAddr" MASK "$lan_main_0_netmask"
			/usr/sbin/naptcfg --ADDLAN eth0
			/usr/sbin/naptcfg --Servicesinit
			#806131:<IFTW-leon> Let board accessing by eth0 before  ppa ready end
		fi
		
		# Move this to DSL Link Up handling
		# For autosearch PVC
		#if  [ "$CONFIG_FEATURE_IFX_AUTOSEARCH" = "1" ]; then
			#if [ -e /usr/sbin/autosearch_main ]; then
				#echo "Autosearch PVc is running"
				#/usr/sbin/autosearch_main &
			#fi
		#Incomeplete commented IF loop ????!!!!
			
		
		#if [ "$CONFIG_FEATURE_CLIP" = "1" ]; then
			#/usr/sbin/atmarpd -b -l /dev/null
		#fi

		if [ "$CONFIG_FEATURE_NAPT" = "1" ]; then
			/etc/rc.d/rc.firewall start
			/usr/sbin/naptcfg --VSinit > /dev/null
			/usr/sbin/naptcfg --PMinit > /dev/null
			/usr/sbin/naptcfg --DMZinit > /dev/null
			/usr/sbin/naptcfg --VS 1 > /dev/null
			/usr/sbin/naptcfg --PM 1 > /dev/null
			
			if [ "$firewall_dmz_enable" = "1" ]; then
				/usr/sbin/naptcfg --DMZ 1 > /dev/null
			fi
			if [ "$ipnat_enable" = "1" ]; then
				/usr/sbin/naptcfg --NAPT 1 > /dev/null
				 [ `mount|grep -q nfs;echo $?` -eq  0 ] \
				&& iptables -t nat -A IFX_NAPT_PREROUTING_LAN --in-interface eth0 --jump ACCEPT
			else
				/usr/sbin/naptcfg --NAPT 0 > /dev/null
			fi
		fi

		# Run some deamons likes http, telnetd
		/etc/rc.d/rc.bringup_services except_devm

		#
		# Setup QOS
		#
		if  [ "$CONFIG_FEATURE_QOS" = "1" ]; then
			/etc/rc.d/init.d/qos init
	fi
	fi

	if [ -z "$CONFIG_BRIDGE_MODEM" ]; then
	# accept everything on loopback interface
	/usr/sbin/iptables -I INPUT -i lo -j ACCEPT
	/usr/sbin/iptables -I OUTPUT -o lo -j ACCEPT
	fi

}
