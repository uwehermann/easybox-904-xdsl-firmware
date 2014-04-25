
echo "[arc-ipv4-linkdown.sh] interface_section=${1}"

# link down interface section name, ex: wan000
IPV4_DOWN_INTERFACE_SECTION=$1
PROTO=`ccfg_cli get proto@${IPV4_DOWN_INTERFACE_SECTION}`
DEFAULTROUTE_ENABLE=`ccfg_cli get defaultroute@${IPV4_DOWN_INTERFACE_SECTION}`
IPV4_DOWN_INTERFACE_NAME=`ccfg_cli get ifname@${IPV4_DOWN_INTERFACE_SECTION}`
tmp=${PROTO:0:3}
if [ "${tmp}" == "ppp" ] ; then
	unit=${IPV4_DOWN_INTERFACE_SECTION:3:3}
	if [ ${#unit} -eq 1 ] ; then
		echo -n
	elif [ ${#unit} -eq 2 ] ; then
		unit=$(( 1${unit} - 100 ))
	elif [ ${#unit} -eq 3 ] ; then
		unit=$(( 1${unit} - 1000 ))
	else
		echo"[arc-ipv4-linkdown.sh] WRONG interface name $IPV4_DOWN_INTERFACE_SECTION"
		return
	fi
	IPV4_DOWN_INTERFACE_NAME="ppp$unit"
fi

echo "[arc-ipv4-linkdown.sh] IPV4_DOWN_INTERFACE_SECTION=${IPV4_DOWN_INTERFACE_SECTION}, IPV4_DOWN_INTERFACE_NAME=${IPV4_DOWN_INTERFACE_NAME}, PROTO=${PROTO}, DEFAULTROUTE_ENABLE=${DEFAULTROUTE_ENABLE}"

CNT=0
while [ $CNT -lt 3 ] ; do
	if [ "`ifconfig $IPV4_DOWN_INTERFACE_NAME | grep -c 'inet addr'`" -le 0 ] ; then
		break
	fi
	sleep 1
	let CNT=$CNT+1
done

# IGMP Proxy start
IGMPPROXY_UPSTREAM_SECTION=""
IGMPPROXY_UPSTREAM=""

IGMPPROXY_UPSTREAM_SECTION=`ccfg_cli get upstream@igmpproxy`
if [ `echo "$IGMPPROXY_UPSTREAM_SECTION" | grep -cw "$IPV4_DOWN_INTERFACE_SECTION"` -ge 1 ] ; then
	IGMPPROXY_UPSTREAM=$IPV4_DOWN_INTERFACE_NAME
fi
echo "[arc-ipv4-linkdown.sh] IGMPPROXY_UPSTREAM_SECTION=${IGMPPROXY_UPSTREAM_SECTION}, IGMPPROXY_UPSTREAM=$IGMPPROXY_UPSTREAM"

if [ "$IGMPPROXY_UPSTREAM" != "" ] ; then
#	killall igmpproxy
#
#	sleep 2

	/etc/init.d/arc_igproxy.sh stop
fi
# IGMP Proxy end

# OpenVPN daemon stop
/usr/sbin/arc-openvpn-ctrl stop
# OpenVPN daemon end

#RIP daemon start
RIPD_NET_SECTION=`ccfg_cli get network@ripd`
if [ `echo "$RIPD_NET_SECTION" | grep -cw "$IPV4_DOWN_INTERFACE_SECTION"` -ge 1 ] ; then
	/etc/init.d/ripd stop
fi
#RIP daemon end

#SIP config update start
NET_SECT=`ccfg_cli get wan_if@voip`
if [ `echo "$NET_SECT" | grep -cw "$IPV4_DOWN_INTERFACE_SECTION"` -ge 1 ] ; then
	/etc/init.d/brn_sip down_update $IPV4_DOWN_INTERFACE_SECTION
fi
#SIP config update end

if [ ${IPV4_DOWN_INTERFACE_NAME:0:3} == "ppp" ] ; then
	restart_wansvc.sh ${IPV4_DOWN_INTERFACE_NAME} 0
else
	restart_wansvc.sh ${IPV4_DOWN_INTERFACE_SECTION} 0
fi
#restart_lansvc.sh ${IPV4_DOWN_INTERFACE_SECTION} 0

#if [ -f /usr/sbin/arc-ip6o4-tunneling.sh ]; then
##arc-ip6o4-tunneling.sh paras:
##$1: up/down
##$2: wan section name
##$3: ipv4 address
#    /usr/sbin/arc-ip6o4-tunneling.sh 0 $IPV4_DOWN_INTERFACE_SECTION
#fi

