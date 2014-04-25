scan_ppp() {

	unit=${1:3:3}
	if [ ${#unit} -eq 1 ] ; then
		echo -n
	elif [ ${#unit} -eq 2 ] ; then
		unit=$(( 1${unit} - 100 ))
	elif [ ${#unit} -eq 3 ] ; then
		unit=$(( 1${unit} - 1000 ))
	else
		echo WRONG interface name $1
		return
	fi

	config_set "$1" ifname "ppp$unit"
	config_set "$1" unit   "$unit"
}

start_pppd() {
	local cfg="$1"; shift

	### ctc added to check if it is in scheduled time to start PPP ###
	if [ "`wan_ppp_sch_chk.sh $cfg`" == "0" ] ; then
		return 0
	fi
	#################

	# make sure only one pppd process is started
	lock "/var/lock/ppp-${cfg}"
	local pid="$(head -n1 /var/run/ppp-${cfg}.pid 2>/dev/null)"
	[ -d "/proc/$pid" ] && grep pppd "/proc/$pid/cmdline" 2>/dev/null >/dev/null && {
		lock -u "/var/lock/ppp-${cfg}"
		return 0
	}

	# Workaround: sometimes hotplug2 doesn't deliver the hotplug event for creating
	# /dev/ppp fast enough to be used here
	[ -e /dev/ppp ] || mknod /dev/ppp c 108 0

	config_get device    "$cfg" device
	config_get unit      "$cfg" unit
	config_get username  "$cfg" username
	config_get password  "$cfg" password
	config_get keepalive "$cfg" keepalive

	config_get connect           "$cfg" connect
	config_get disconnect        "$cfg" disconnect
	config_get pppd_options      "$cfg" pppd_options
	config_get_bool defaultroute "$cfg" defaultroute 1
	[ "$defaultroute" -eq 1 ] && defaultroute="defaultroute replacedefaultroute" || defaultroute=""

	interval="${keepalive##*[, ]}"
	[ "$interval" != "$keepalive" ] || interval=5

	config_get_bool peerdns "$cfg" peerdns 1
	[ "$peerdns" -eq 1 ] && peerdns="usepeerdns" || peerdns=""

	config_get demand "$cfg" demand
	### ctc remove it to prevent /tmp/resolv.conf.auto been over-written ###
#	[ -n "$demand" ] && {
#	    echo $pppd_options | grep [-]ip
#	    if [ "$?" -eq "1" ]; then
#	        echo "nameserver 1.1.1.1" > /tmp/resolv.conf.auto
#	    else
#	        echo $pppd_options | grep +ipv6
#	        [ "$?" -eq "0" ] && echo "nameserver 3fff::1" > /tmp/resolv.conf.auto
#	    fi
#    }

#	config_get_bool ipv6 "$cfg" ipv6 0
#	[ "$ipv6" -eq 1 ] && ipv6="+ipv6" || ipv6=""
	glb_ipv6=`umngcli get ip6_enable@system`
	ipv6=`umngcli get ip6_enable@$cfg`
	ip6_proto=`umngcli get ip6_proto@$cfg`

	if [ "$glb_ipv6" -eq 1 ]; then
		if [ "$ipv6" -eq 1 ] && [ "$ip6_proto" == "dhcp" ]; then
			echo 0 > /proc/sys/net/ipv6/conf/default/disable_ipv6
			ipv6="+ipv6"
		else
			ipv6=""
		fi
	else
		echo 1 > /proc/sys/net/ipv6/conf/default/disable_ipv6
		ipv6=""
	fi

	config_get_bool persist "$cfg" persist 1
    [ "$persist" -eq 0 ] && persist="nopersist" || persist="persist maxfail 0 holdoff 3"

	/usr/sbin/pppd "$@" \
		${keepalive:+lcp-echo-interval $interval lcp-echo-failure ${keepalive%%[, ]*}} \
		${demand:+precompiled-active-filter /etc/ppp/filter $persist demand idle }${demand:-persist maxfail 0} \
		$peerdns \
		$defaultroute \
		${username:+user "$username" password "$password"} \
		unit "$unit" \
		linkname "$cfg" \
		ipparam "$cfg" \
		${connect:+connect "$connect"} \
		${disconnect:+disconnect "$disconnect"} \
		${ipv6} \
		${pppd_options}

	lock -u "/var/lock/ppp-${cfg}"

	cfg=$(( 1${cfg:3} - 1000 ))

	key=`printf "wan%03d=wan" ${cfg}`
	if [ -f /tmp/ulogd.stat ] && [ `grep -c ${key} /tmp/ulogd.stat` -gt 0 ] ; then # for packet capturing
		echo -n
	else
		ppacmd addwan -i ppp${cfg}
	fi
}

setup_interface_ppp() {
	local iface="$1"
	local config="$2"

	config_get device "$config" device

	config_get mtu "$cfg" mtu
	mtu=${mtu:-1492}
	start_pppd "$config" \
		mtu $mtu mru $mtu \
		"$device"
}

