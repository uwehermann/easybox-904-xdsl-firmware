#!/bin/sh
# Copyright (C) 2006 OpenWrt.org

# DEBUG="echo"

## bitonic update, ctc added
DEFAULT_ROUTER="0.0.0.0"
## end of bitonic update

do_sysctl() {
	[ -n "$2" ] && \
		sysctl -n -e -w "$1=$2" >/dev/null || \
		sysctl -n -e "$1"
}

find_config() {
	local iftype device iface ifaces ifn
	for ifn in $interfaces; do
		config_get iftype "$ifn" type
		config_get iface "$ifn" ifname
		case "$iftype" in
			bridge) config_get ifaces "$ifn" ifnames;;
		esac
		config_get device "$ifn" device
		for ifc in $device $iface $ifaces; do
			[ ."$ifc" = ."$1" ] && {
				echo "$ifn"
				return 0
			}
		done
	done

	return 1;
}

scan_interfaces() {
	local cfgfile="${1:-network}"
	interfaces=
	config_cb() {
		case "$1" in
			interface)
				config_set "$2" auto 1
			;;
		esac
		local iftype ifname device proto
		config_get iftype "$CONFIG_SECTION" TYPE
		case "$iftype" in
			interface)
				# daniel  2010/05/27 15:53, ctc added
				# if the interface asminstate is disable, then do not ifup
				if [ "${CONFIG_SECTION}" != "loopback" ]; then	
					local state
					if [ "${CONFIG_SECTION}" == "lan" ]; then
						state=`umngcli get adminstate@${CONFIG_SECTION}0`
					else
						state=`umngcli get adminstate@${CONFIG_SECTION}`
					fi
					#echo "config" ${CONFIG_SECTION} ${state} > /dev/console
					if [ "${state}" == "disable" ] ; then
						return ;
					fi
				fi
				#####################
				append interfaces "$CONFIG_SECTION"
				config_get proto "$CONFIG_SECTION" proto
				config_get iftype "$CONFIG_SECTION" type
				config_get ifname "$CONFIG_SECTION" ifname
				config_get device "$CONFIG_SECTION" device "$ifname"
				config_set "$CONFIG_SECTION" device "$device"
				case "$iftype" in
					bridge)
						config_set "$CONFIG_SECTION" ifnames "$device"
						config_set "$CONFIG_SECTION" ifname br-"$CONFIG_SECTION"
					;;
				esac
				( type "scan_$proto" ) >/dev/null 2>/dev/null && eval "scan_$proto '$CONFIG_SECTION'"
			;;
		esac
	}
	config_load "${cfgfile}"
}

add_vlan() {
	local vif="${1%\.*}"

	[ "$1" = "$vif" ] || ifconfig "$1" >/dev/null 2>/dev/null || {
		ifconfig "$vif" up 2>/dev/null >/dev/null || add_vlan "$vif"
		$DEBUG vconfig add "$vif" "${1##*\.}"
		return 0
	}
	return 1
}

# add dns entries if they are not in resolv.conf yet
add_dns() {
	local cfg="$1"; shift

	remove_dns "$cfg"

	# We may be called by pppd's ip-up which has a nonstandard umask set.
	# Create an empty file here and force its permission to 0644, otherwise
	# dnsmasq will not be able to re-read the resolv.conf.auto .
	[ ! -f /tmp/resolv.conf.auto ] && {
		touch /tmp/resolv.conf.auto
		chmod 0644 /tmp/resolv.conf.auto
	}

	local dns
	local add
	for dns in "$@"; do
		grep -qsF "nameserver $dns" /tmp/resolv.conf.auto || {
			add="${add:+$add }$dns"
			echo "nameserver $dns" >> /tmp/resolv.conf.auto
		}
	done

	[ -n "$cfg" ] && {
		uci_set_state network "$cfg" dns "$add"
		uci_set_state network "$cfg" resolv_dns "$add"
	}
}

# remove dns entries of the given iface
remove_dns() {
	local cfg="$1"

	[ -n "$cfg" ] && {
		[ -f /tmp/resolv.conf.auto ] && {
			local dns=$(uci_get_state network "$cfg" resolv_dns)
			for dns in $dns; do
				sed -i -e "/^nameserver $dns$/d" /tmp/resolv.conf.auto
			done
		}

		uci_revert_state network "$cfg" dns
		uci_revert_state network "$cfg" resolv_dns
	}
}

# sort the device list, drop duplicates
sort_list() {
	local arg="$*"
	(
		for item in $arg; do
			echo "$item"
		done
	) | sort -u
}

# Create the interface, if necessary.
# Return status 0 indicates that the setup_interface() call should continue
# Return status 1 means that everything is set up already.

prepare_interface() {
	local iface="$1"
	local config="$2"
	local vifmac="$3"

	# if we're called for the bridge interface itself, don't bother trying
	# to create any interfaces here. The scripts have already done that, otherwise
	# the bridge interface wouldn't exist.
	[ "br-$config" = "$iface" -o -e "$iface" ] && return 0;

	ifconfig "$iface" 2>/dev/null >/dev/null && {
		local proto
		config_get proto "$config" proto

		# make sure the interface is removed from any existing bridge and deconfigured,
		# (deconfigured only if the interface is not set to proto=none)
		if [ "$proto" != none ]; then
			unbridge "$iface"
		fi
		[ "$proto" = none ] || ifconfig "$iface" 0.0.0.0

		# Change interface MAC address if requested
		[ -n "$vifmac" ] && {
			ifconfig "$iface" down
			ifconfig "$iface" hw ether "$vifmac" up
		}
	}

	# Setup VLAN interfaces
	add_vlan "$iface" && return 1
	ifconfig "$iface" 2>/dev/null >/dev/null || return 0

	# Setup bridging
	local iftype
	config_get iftype "$config" type
	case "$iftype" in
		bridge)
			local macaddr
			config_get macaddr "$config" macaddr
			[ -x /usr/sbin/brctl ] && {
				ifconfig "br-$config" 2>/dev/null >/dev/null && {
					local newdevs devices
					config_get devices "$config" device
					for dev in $(sort_list "$devices" "$iface"); do
						append newdevs "$dev"
					done
					uci_set_state network "$config" device "$newdevs"
					$DEBUG ifconfig "$iface" 0.0.0.0
					$DEBUG do_sysctl "net.ipv6.conf.$iface.disable_ipv6" 1
					$DEBUG brctl addif "br-$config" "$iface"
					# Bridge existed already. No further processing necesary
				} || {
					local stp
					config_get_bool stp "$config" stp 0
					$DEBUG brctl addbr "br-$config"
					$DEBUG brctl setfd "br-$config" 0
					$DEBUG ifconfig "$iface" 0.0.0.0
					$DEBUG do_sysctl "net.ipv6.conf.$iface.disable_ipv6" 1
					$DEBUG brctl addif "br-$config" "$iface"
					$DEBUG brctl stp "br-$config" $stp
					$DEBUG ifconfig "br-$config" up
					# Creating the bridge here will have triggered a hotplug event, which will
					# result in another setup_interface() call, so we simply stop processing
					# the current event at this point.
				}
				ifconfig "$iface" ${macaddr:+hw ether "${macaddr}"} up 2>/dev/null >/dev/null
				return 1
			}
		;;
	esac
	return 0
}

set_interface_ifname() {
	local config="$1"
	local ifname="$2"

	local device
	config_get device "$1" device
	uci_set_state network "$config" ifname "$ifname"
	uci_set_state network "$config" device "$device"
}

setup_interface_none() {
	env -i ACTION="ifup" INTERFACE="$2" DEVICE="$1" PROTO=none /sbin/hotplug-call "iface" &
}

setup_interface_static() {
	local iface="$1"
	local config="$2"

	local ipaddr netmask ip6addr
	config_get ipaddr "$config" ipaddr
	config_get netmask "$config" netmask
#	config_get ip6addr "$config" ip6addr
	[ -z "$ipaddr" -o -z "$netmask" ] && [ -z "$ip6addr" ] && return 1

	local gateway ip6gw dns bcast metric
	config_get gateway "$config" gateway
#	config_get ip6gw "$config" ip6gw
	config_get dns "$config" dns
	config_get bcast "$config" broadcast
	config_get metric "$config" metric

	dft_gw=`umngcli get defaultroute@$config`
	## bitonic update, ctc added
	if [ "$dft_gw" = "1" ]; then
		if [ "$DEFAULT_ROUTER" = "0.0.0.0" ]; then
			DEFAULT_ROUTER=$gateway
		fi
	fi
	## end of bitonic update

	[ -z "$ipaddr" ] || $DEBUG ifconfig "$iface" "$ipaddr" netmask "$netmask" broadcast "${bcast:-+}"

#	# ygchen added, to allow ip6addr specified in prefix format, ctc added
#	if [ -n "$ip6addr" ]; then
#		pfx=`echo $ip6addr | cut -d'/' -f1`
#		echo $pfx | grep -q '::$'
#		if [ "$?" == 0 ]; then
#			pfx_len=`echo $ip6addr | cut -d'/' -f2`
#			set -- $(sed 's!:! !g' /sys/devices/virtual/net/br-"$config"/address)
#	        mac0=$1
#	        mac1=$2
#	        mac2=$3
#	        mac3=$4
#	        mac4=$5
#	        mac5=$6
#	        ip6addr=`echo $pfx$(($mac0|2))$mac1:${mac2}ff:fe$mac3:$mac4$mac5/$pfx_len`
#		fi
#	fi
#	###########################

#	[ -z "$ip6addr" ] || $DEBUG ifconfig "$iface" add "$ip6addr"
	[ "$dft_gw" = "0" ] || [ -z "$gateway" ] || $DEBUG route add default gw "$gateway" ${metric:+metric $metric} dev "$iface"
#	[ -z "$ip6gw" ] || $DEBUG route -A inet6 add default gw "$ip6gw" ${metric:+metric $metric} dev "$iface"
	[ -z "$dns" ] || add_dns "$config" $dns

	config_get type "$config" TYPE
	[ "$type" = "alias" ] && return 0

	#env -i ACTION="ifup" INTERFACE="$config" DEVICE="$iface" PROTO=static /sbin/hotplug-call "iface" &
	env -i ACTION="ifup" INTERFACE="$config" DEVICE="$iface" PROTO=static IP4ADDR="$ipaddr" IP4MASK="$netmask" IP4GW="$gateway" /sbin/hotplug-call "iface" &

	## bitonic update, ctc added
	/usr/sbin/resolv_update.sh SYSTEM
	. /usr/sbin/arc-ipv4-linkup.sh $config $ipaddr $netmask $DEFAULT_ROUTER
	## end of bitonic update
}

setup_interface_alias() {
	local config="$1"
	local parent="$2"
	local iface="$3"

	local cfg
	config_get cfg "$config" interface
	[ "$parent" == "$cfg" ] || return 0

	# parent device and ifname
	local p_device p_type
	config_get p_device "$cfg" device
	config_get p_type   "$cfg" type

	# select alias ifname
	local layer use_iface
	config_get layer "$config" layer 2
	case "$layer:$p_type" in
		# layer 3: e.g. pppoe-wan or pptp-vpn
		3:*)      use_iface="$iface" ;;

		# layer 2 and parent is bridge: e.g. br-wan
		2:bridge) use_iface="br-$cfg" ;;

		# layer 1: e.g. eth0 or ath0
		*)        use_iface="$p_device" ;;
	esac

	# alias counter
	local ctr
	config_get ctr "$parent" alias_count 0
	ctr="$(($ctr + 1))"
	config_set "$parent" alias_count "$ctr"

	# alias list
	local list
	config_get list "$parent" aliases
	append list "$config"
	config_set "$parent" aliases "$list"

	use_iface="$use_iface:$ctr"
	set_interface_ifname "$config" "$use_iface"

	local proto
	config_get proto "$config" proto "static"
	case "${proto}" in
		static)
			setup_interface_static "$use_iface" "$config"
		;;
		*)
			echo "Unsupported type '$proto' for alias config '$config'"
			return 1
		;;
	esac
}

setup_interface() {
	local iface="$1"
	local config="$2"
	local proto="$3"
	local vifmac="$4"
	## bitonic test, ctc added
	local upper_iface
	#config_get upper_iface "$config" ifname
	## end of bitonic test
	## ygchen
	glb_ip6_enable=`ccfg_cli get ip6_enable@system`

	[ -n "$config" ] || {
		config=$(find_config "$iface")
		[ "$?" = 0 ] || return 1
	}

	prepare_interface "$iface" "$config" "$vifmac" || return 0
	config_get upper_iface "$config" ifname

	[ "$iface" = "br-$config" ] && {
		# need to bring up the bridge and wait a second for
		# it to switch to the 'forwarding' state, otherwise
		# it will lose its routes...
		ifconfig "$iface" up
		sleep 1
	}

	# Interface settings
	grep "$iface:" /proc/net/dev > /dev/null && {
		local mtu macaddr
		config_get mtu "$config" mtu
		config_get macaddr "$config" macaddr
		## bitonic test, ctc added
		if [ "$upper_iface" == "$iface" ]; then
		{
			[ -n "$macaddr" ] && $DEBUG ifconfig "$iface" down
			$DEBUG ifconfig "$iface" ${macaddr:+hw ether "$macaddr"} ${mtu:+mtu $mtu} up
		}
		fi
		##end of bitonic test
	}
	set_interface_ifname "$config" "$iface"

	ip6_enable=`umngcli get ip6_enable@${config}`
	ip6_proto=`umngcli get ip6_proto@${config}`
	ip6_tunnel_type=`umngcli get ip6_tunnel_type@${config}`
	[ -n "$proto" ] || config_get proto "$config" proto
	case "$proto" in
		static)
#		    ### ygchen add ###
#	        ip6_enable=`umngcli get ip6_enable@${config}`
#	        if [ "$ip6_enable" == "1" ]; then
#	            . /usr/sbin/arc-ipv6-linkup.sh $config $iface
#	        fi
#		    ##################

			### yuelin add ###
			### This block setting will stop global IPv6 fucntion. So, comment now.
			### end
			### yuzhang remove the comment, modify arc-ipv6.c to fix the bug
			if [ "$glb_ip6_enable" == "1" ] && [ "$iface" == "br-lan" ]; then
				echo "[config.sh] /usr/sbin/arc_ipv6_lan iface=$iface" >> /tmp/ipv6.log
				ccfg_cli set ip6_wan_ready@system=0
				/usr/sbin/arc_ipv6_lan 0 &
			fi
			if [ "$glb_ip6_enable" == "1" ] && [ "$iface" == "br-lan1" ]; then
				echo "[config.sh] /usr/sbin/arc_ipv6_lan iface=$iface" >> /tmp/ipv6.log
				/usr/sbin/arc_ipv6_lan 1 &
			fi

			setup_interface_static "$iface" "$config"

			#if [ "$glb_ip6_enable" == "1" ]; then
			#	if [ "$ip6_enable" == "1" ]; then
			#		echo "[config.sh] wan6c_restart $iface wan4c_proto=$proto" >> /tmp/ipv6.log
			#		/usr/sbin/wan6c_restart &
			#	else
			#		do_sysctl "net.ipv6.conf.$iface.disable_ipv6" 1
			#	fi
			#fi
		;;
		dhcp)
#		    ### ygchen add ###
#	        ip6_enable=`umngcli get ip6_enable@${config}`
#	        if [ "$ip6_enable" == "1" ]; then
#	            . /usr/sbin/arc-ipv6-linkup.sh $config $iface
#	        fi
#		    ##################

			# kill running udhcpc instance
			local pidfile="/var/run/dhcp-${iface}.pid"
			service_kill udhcpc "$pidfile"

			local ipaddr netmask hostname proto1 clientid vendorid broadcast
			config_get ipaddr "$config" ipaddr
			config_get netmask "$config" netmask
			config_get hostname "$config" hostname
			config_get proto1 "$config" proto
			config_get clientid "$config" clientid
			config_get vendorid "$config" vendorid
			config_get_bool broadcast "$config" broadcast 0

			[ -z "$ipaddr" ] || \
				$DEBUG ifconfig "$iface" "$ipaddr" ${netmask:+netmask "$netmask"}

			# don't stay running in background if dhcp is not the main proto on the interface (e.g. when using pptp)

			if [ "$ip6_enable" == "1" ] && [ "$ip6_proto" == "tunnel" ] && [ "$ip6_tunnel_type" == "1" ]; then
				ip6rd="-O ip6rd"
			else
				ip6rd=
			fi
			echo "[config.sh] ip6rd=$ip6rd" >> /tmp/ipv6.log
			local dhcpopts
			[ ."$proto1" != ."$proto" ] && dhcpopts="-n -q"
			[ "$broadcast" = 1 ] && broadcast="-O broadcast" || broadcast=

			# 20120731 Titan Liu -- If there is no specified client ID, we assign a one based on DUT WAN MAC
			[ -z "$clientid" ] && clientid=`/usr/sbin/getmacaddr.sh wan | sed "s/://g"`

			# 20130128 Ron Huang -- Hostname
			hostname=`ccfg_cli get host_name@system` 

			$DEBUG eval udhcpc -t 0 -i "$iface" \
				${ipaddr:+-r $ipaddr} \
				${hostname:+-H $hostname} \
				${clientid:+-c $clientid} \
				${vendorid:+-V $vendorid} \
				-b -p "$pidfile" $broadcast $ip6rd \
				-O staticroutes -R &
			### ctc ###
			restart_wansvc.sh "$config"
			restart_lansvc.sh "$config"
			###########

			if [ "$glb_ip6_enable" == "1" ]; then
				if [ "$ip6_enable" == "1" ] && [ "$ip6_proto" == "dhcp" ] ; then
					echo "[config.sh] dhcp6c_restart $iface" >> /tmp/ipv6.log
					/usr/sbin/dhcp6c_restart &
				else
					do_sysctl "net.ipv6.conf.$iface.disable_ipv6" 1
				fi
			fi
		;;
		none)
			setup_interface_none "$iface" "$config"
			### ygchen ###
			# to workaround the issue of "online-active of bridging WAN does not work"
			ifconfig "$iface" up 2>/dev/null >/dev/null
		    ##############

		    if [ "$glb_ip6_enable" == "1" ]; then
		    	do_sysctl "net.ipv6.conf.$iface.disable_ipv6" 1
		    fi
		;;
		*)
			if ( eval "type setup_interface_$proto" ) >/dev/null 2>/dev/null; then
				eval "setup_interface_$proto '$iface' '$config' '$proto'"
			### ctc ###
			elif [ $proto = "3g" ]; then
				eval "setup_interface_$proto '$iface' '$config' '$proto'"
			###########
			else
				echo "Interface type $proto not supported."
				return 1
			fi
			### ctc ###
			restart_wansvc.sh "$config"
			restart_lansvc.sh "$config"
			###########

		    if [ "$glb_ip6_enable" == "1" ]; then
		    	do_sysctl "net.ipv6.conf.$iface.disable_ipv6" 1
		    fi
		;;
	esac
}

stop_interface_dhcp() {
	local config="$1"

	local ifname
	config_get ifname "$config" ifname

	local lock="/var/lock/dhcp-${ifname}"
	[ -f "$lock" ] && lock -u "$lock"

	remove_dns "$config"

	local pidfile="/var/run/dhcp-${ifname}.pid"
	service_kill udhcpc "$pidfile"

	uci -P /var/state revert "network.$config"
}

unbridge() {
	local dev="$1"
	local brdev

	[ -x /usr/sbin/brctl ] || return 0
	brctl show 2>/dev/null | grep "$dev" >/dev/null && {
		# interface is still part of a bridge, correct that

		for brdev in $(brctl show | awk '$2 ~ /^[0-9].*\./ { print $1 }'); do
			brctl delif "$brdev" "$dev" 2>/dev/null >/dev/null
			do_sysctl "net.ipv6.conf.$dev.disable_ipv6" 0
		done
	}
}
