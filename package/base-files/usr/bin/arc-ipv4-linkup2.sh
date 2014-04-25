
echo "[arc-ipv4-linkup.sh] interface_section=${1}, ip=${2}, netmask=${3}, gateway=${4}"

# link up interface section name, ex: wan000
IPV4_UP_INTERFACE_SECTION=$1
IPV4_ADDR=$2
PROTO=`ccfg_cli get proto@${IPV4_UP_INTERFACE_SECTION}`
DEFAULTROUTE_ENABLE=`ccfg_cli get defaultroute@${IPV4_UP_INTERFACE_SECTION}`
IPV4_UP_INTERFACE_NAME=`ccfg_cli get ifname@${IPV4_UP_INTERFACE_SECTION}`
tmp=${PROTO:0:3}
if [ "${tmp}" == "ppp" ] ; then
	unit=${IPV4_UP_INTERFACE_SECTION:3:3}
	if [ ${#unit} -eq 1 ] ; then
		echo -n
	elif [ ${#unit} -eq 2 ] ; then
		unit=$(( 1${unit} - 100 ))
	elif [ ${#unit} -eq 3 ] ; then
		unit=$(( 1${unit} - 1000 ))
	else
		echo"[arc-ipv4-linkup.sh] WRONG interface name $IPV4_UP_INTERFACE_SECTION"
		return
	fi
	IPV4_UP_INTERFACE_NAME="ppp$unit"
fi

echo "[arc-ipv4-linkup.sh] IPV4_UP_INTERFACE_SECTION=${IPV4_UP_INTERFACE_SECTION}, IPV4_UP_INTERFACE_NAME=${IPV4_UP_INTERFACE_NAME}, PROTO=${PROTO}, DEFAULTROUTE_ENABLE=${DEFAULTROUTE_ENABLE}"

# IGMP Proxy start
IGMPPROXY_QUICKLEAVE=""
IGMPPROXY_UPSTREAM_SECTION=""
IGMPPROXY_UPSTREAM=""
IGMPPROXY_DOWNSTREAM=""

IGMPPROXY_UPSTREAM_SECTION=`ccfg_cli get upstream@igmpproxy`
if [ `echo "$IGMPPROXY_UPSTREAM_SECTION" | grep -cw "$IPV4_UP_INTERFACE_SECTION"` -ge 1 ] ; then
	IGMPPROXY_UPSTREAM=$IPV4_UP_INTERFACE_NAME
fi
echo "[arc-ipv4-linkup.sh] IGMPPROXY_UPSTREAM_SECTION=${IGMPPROXY_UPSTREAM_SECTION}, IGMPPROXY_UPSTREAM=$IGMPPROXY_UPSTREAM"

CNT=0
while [ $CNT -lt 3 ] ; do
	if [ "`ifconfig $IPV4_UP_INTERFACE_NAME | grep -c 'inet addr'`" -ge 1 ] ; then
		break
	fi
	sleep 1
	let CNT=$CNT+1
done

if [ "$IGMPPROXY_UPSTREAM" != "" ] ; then
        echo "[arc-ipv4-linkup.sh] run igmpproxy"
#        killall igmpproxy
#        IGMPPROXY_QUICKLEAVE=`ccfg_cli get quickleave@igmpproxy`
#        IGMPPROXY_DOWNSTREAM=`ccfg_cli get downstream@igmpproxy`
#
#        /usr/sbin/set_igmpproxy.sh $IGMPPROXY_QUICKLEAVE $IPV4_UP_INTERFACE_SECTION $IGMPPROXY_DOWNSTREAM
#
#		( sleep 2 ; exec >& /dev/console ; /usr/sbin/igmpproxy /etc/igmpproxy.conf ) &

		/etc/init.d/igmpproxy stop
		/etc/init.d/igmpproxy start $IPV4_UP_INTERFACE_SECTION
fi
# IGMP Proxy end

#RIP daemon start
NET_SECT=`ccfg_cli get network@ripd`
if [ `echo "$NET_SECT" | grep -cw "$IPV4_UP_INTERFACE_SECTION"` -ge 1 ] ; then
	/etc/init.d/ripd boot $IPV4_UP_INTERFACE_SECTION
fi
#RIP daemon end

#SIP config update start
NET_SECT=`ccfg_cli get wan_if@voip`
if [ `echo "$NET_SECT" | grep -cw "$IPV4_UP_INTERFACE_SECTION"` -ge 1 ] ; then
	/etc/init.d/brn_sip update $IPV4_UP_INTERFACE_SECTION
fi
#SIP config update end

#WebFTP config update start
if [ -f /usb/sbin/webftp.sh ] ; then
	/usb/sbin/webftp.sh $IPV4_UP_INTERFACE_SECTION $IPV4_UP_INTERFACE_SECTION
fi
#WebFTP config update end

if [ ${IPV4_UP_INTERFACE_NAME:0:3} == "ppp" ] ; then
	restart_wansvc.sh ${IPV4_UP_INTERFACE_NAME} 1
else
	restart_wansvc.sh ${IPV4_UP_INTERFACE_SECTION} 1
fi
restart_lansvc.sh ${IPV4_UP_INTERFACE_SECTION} 1

if [ -f /usr/sbin/arc-ip6o4-tunneling.sh ]; then
#arc-ip6o4-tunneling.sh paras:
#$1: up/down
#$2: wan section name
#$3: ipv4 address
    /usr/sbin/arc-ip6o4-tunneling.sh 1 $IPV4_UP_INTERFACE_SECTION $IPV4_ADDR
fi

if [ "${DEFAULTROUTE_ENABLE}" == 1 ] ; then
	create-application-cert.sh ${IPV4_ADDR}
fi
