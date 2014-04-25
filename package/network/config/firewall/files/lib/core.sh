# Copyright (C) 2009-2010 OpenWrt.org

FW_LIBDIR=${FW_LIBDIR:-/lib/firewall}

. $FW_LIBDIR/fw.sh
include /lib/network

fw_start() {
	local fwfastCurVer fwfastVer fwfastinit
	pre_rebuildfile="/etc/config/pre_iptables_rebuild.save"
	pre_rebuildfilev6="/etc/config/pre_ip6tables_rebuild.save"
	rebuildfile="/etc/config/iptables_rebuild.save"
	rebuildfilev6="/etc/config/ip6tables_rebuild.save"
	rebuildcfg="/etc/config/firewall"
	rebuildsub="rebuild"
	fwfastParamsPath="/etc/config"
	fwfastParamsfile="fwfast"
	fwfastCurVer="2.55.1"
	firewallStatefile="var/state/firewall"
	isFastInit=0
	fwfastinit=0
	if [ ! -f "$fwfastParamsPath/$fwfastParamsfile" ]; then
		echo "" > "$fwfastParamsPath/$fwfastParamsfile"
		uci add $fwfastParamsfile Params
		fwfastVer=""
	else
		fwfastVer=`uci get $fwfastParamsfile.@Params[-1].Version`
	fi


	idx=0
	for cfg in $rebuildcfg
	do
	    if [ -f "$cfg.$rebuildsub" ]; then
	        rst=`cmp "$cfg" "$cfg.$rebuildsub"`
		    if [ -z "$rst" ]; then
		    	isFastInit=$(($isFastInit+1))
		    fi
	    fi
	    idx=$(($idx+1))
	done

    if [ $isFastInit -eq $idx ] && [ "$fwfastVer" = "$fwfastCurVer" ] ; then
    	echo "[Firewall] fast init"
    	fwfastinit=1
	fi

	fw_init

	FW_DEFAULTS_APPLIED=

	fw_is_loaded && {
		echo "firewall already loaded" >&2
		exit 1
	}

	uci_set_state firewall core "" firewall_state

	if [ $fwfastinit -eq 0 ]; then
		fw_clear DROP
#		(iptables -t mangle -D FORWARD -j zone_lan3_MSSFIX >/dev/null 2>&1)
#		(iptables -t mangle -D FORWARD -j zone_lan2_MSSFIX >/dev/null 2>&1)
#		(iptables -t mangle -D FORWARD -j zone_lan1_MSSFIX >/dev/null 2>&1)
#		(iptables -t mangle -D FORWARD -j zone_lan_MSSFIX >/dev/null 2>&1)
		iptables-save -t mangle > $pre_rebuildfile
#		ip6tables-save -t mangle > $pre_rebuildfilev6
		#create clean iptable-save for later save firewall&nat rules
		iptables -t mangle -Z
		iptables -t mangle -F
		iptables -t mangle -X
#		ip6tables -t mangle -Z
#		ip6tables -t mangle -F
#		ip6tables -t mangle -X


		fw_callback pre core

		echo "Loading defaults"
		fw_config_once fw_load_defaults defaults

		echo "Loading zones"
		config_foreach fw_load_zone zone

		echo "Loading forwardings"
		config_foreach fw_load_forwarding forwarding


		echo "Loading interfaces"
		config_foreach fw_configure_interface interface add

		echo "Loading rules"
		config_foreach fw_load_rule rule

		echo "Loading redirects"
		config_foreach fw_load_redirect redirect

		[ -z "$FW_NOTRACK_DISABLED" ] && {
			echo "Optimizing conntrack"
			config_foreach fw_load_notrack_zone zone
		}

		fw_callback post core

		iptables-save > $rebuildfile
#		ip6tables-save > $rebuildfilev6
		for cfg in $rebuildcfg
		do
			cp "$cfg" "$cfg.$rebuildsub"
		done
		cp -f $firewallStatefile $fwfastParamsPath/firewall.state
		#restore previous iptables-save other thean firewall&nat
		iptables-restore --noflush "$pre_rebuildfile"
#		ip6tables-restore --noflush "$pre_rebuildfilev6"
		rm -f $pre_rebuildfile $pre_rebuildfilev6
		uci set $fwfastParamsfile.@Params[-1].FW_ZONES="$FW_ZONES"
		uci set $fwfastParamsfile.@Params[-1].Version="$fwfastCurVer"
		uci commit $fwfastParamsfile
		ccfg_cli commitcfg
	else
		echo "found $rebuildfile quick rebuild start"
		FW_ZONES=`uci get $fwfastParamsfile.@Params[-1].FW_ZONES`
		cp -f $fwfastParamsPath/firewall.state $firewallStatefile
		(iptables -t mangle -D FORWARD -j zone_lan3_MSSFIX >/dev/null 2>&1)
		(iptables -t mangle -D FORWARD -j zone_lan2_MSSFIX >/dev/null 2>&1)
		(iptables -t mangle -D FORWARD -j zone_lan1_MSSFIX >/dev/null 2>&1)
		(iptables -t mangle -D FORWARD -j zone_lan_MSSFIX >/dev/null 2>&1)
		iptables-restore --noflush "$rebuildfile"
#		ip6tables-restore --noflush "$rebuildfilev6"

		logFile="/tmp/ifhotplug.log"
		tmpFile="/tmp/ifhotplug.sh"
		if [ -f $logFile ]; then
			echo "add hot plug interface"
			echo "#!/bin/sh" > $tmpFile
			echo "" >> $tmpFile
			echo "" >> $tmpFile
			echo ". /etc/functions.sh" >> $tmpFile
			echo ". /lib/firewall/core.sh" >> $tmpFile
			echo "fw_init" >> $tmpFile
			cat "$logFile" >> $tmpFile
			. $tmpFile
		fi
		echo "found $rebuildfile quick rebuild end"

	fi


	echo "Loading includes"
	config_foreach fw_load_include include


	#set default policy for ipv6 before openwrt is intergated with arcadyan firewall
	ip6tables -P INPUT ACCEPT
	ip6tables -P FORWARD ACCEPT
	ip6tables -P OUTPUT ACCEPT
	uci_set_state firewall core zones "$FW_ZONES"
	uci_set_state firewall core loaded 1

}

fw_stop() {
	fw_init

	fw_callback pre stop

	local z n i
	config_get z core zones
	for z in $z; do
		config_get n core "${z}_networks"
		for n in $n; do
			config_get i core "${n}_ifname"
			[ -n "$i" ] && env -i ACTION=remove ZONE="$z" \
				INTERFACE="$n" DEVICE="$i" /sbin/hotplug-call firewall
		done

		config_get i core "${z}_tcpmss"
		[ "$i" == 1 ] && {
			fw del i m FORWARD zone_${z}_MSSFIX
			fw del i m zone_${z}_MSSFIX
		}
	done

	fw_clear ACCEPT

	fw_callback post stop

	uci_revert_state firewall
	config_clear

	local h
	for h in $FW_HOOKS; do unset $h; done

	unset FW_HOOKS
	unset FW_INITIALIZED
}

fw_restart() {
	fw_stop
	fw_start
}

fw_reload() {
	fw_restart
}

fw_is_loaded() {
	local bool=$(uci_get_state firewall.core.loaded)
	return $((! ${bool:-0}))
}


fw_die() {
	echo "Error:" "$@" >&2
	fw_log error "$@"
	fw_stop
	exit 1
}

fw_log() {
	local level="$1"
	[ -n "$2" ] && shift || level=notice
	[ "$level" != error ] || echo "Error: $@" >&2
	logger -t firewall -p user.$level "$@"
}


fw_init() {
	[ -z "$FW_INITIALIZED" ] || return 0

	. $FW_LIBDIR/config.sh

	scan_interfaces
	fw_config_append firewall

	local hooks="core stop defaults zone notrack synflood"
	local file lib hk pp
	for file in $FW_LIBDIR/core_*.sh; do
		. $file
		hk=$(basename $file .sh)
		hk=${hk#core_}
		append hooks $hk
	done
	for file in $FW_LIBDIR/*.sh; do
		lib=$(basename $file .sh)
		lib=${lib##[0-9][0-9]_}
		case $lib in
			core*|fw|config|uci_firewall) continue ;;
		esac
		. $file
		for hk in $hooks; do
			for pp in pre post; do
				type ${lib}_${pp}_${hk}_cb >/dev/null && {
					append FW_CB_${pp}_${hk} ${lib}
					append FW_HOOKS FW_CB_${pp}_${hk}
				}
			done
		done
	done

	fw_callback post init

	FW_INITIALIZED=1
	return 0
}
