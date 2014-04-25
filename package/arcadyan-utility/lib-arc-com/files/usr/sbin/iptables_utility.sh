#!/bin/sh

IPT="iptables"

LOCK_FILE_NAME="/var/lock/iptables.lock"
ipt_lock_res(){
	#echo "---->LOCK file wait PPID:$PPID PID:$$<----"
	if [ -n "$1" ]; then
		LOCK_FILE_NAME=$1
	fi
	lock $LOCK_FILE_NAME
	#echo "---->LOCK file start PPID:$PPID PID:$$<----"
	#ps
}

ipt_unlock_res(){
	#echo "---->LOCK file end PPID:$PPID PID:$$<----"
	if [ -n "$1" ]; then
		LOCK_FILE_NAME=$1
	fi
	lock -u $LOCK_FILE_NAME
}

ipt_clear_redir_table(){
	#echo "run ipt_clear_redir_table"
	LAST_LAN_IP="$1"
	if [ -n "$LAST_LAN_IP" ]; then
		echo "ipt_clear_redir_table:Please send parameter LAST_LAN_IP"
		return
	fi
	ipt_lock_res
	($IPT -t nat -F HTTPREDIR_NAT_RULES >/dev/null 2>&1) || $IPT -t nat -N HTTPREDIR_NAT_RULES
#	$IPT -t nat -D PREROUTING -p udp --dport 53 -j REDIRECT --to-port 55330
#	$IPT -t nat -D PREROUTING -p tcp ! -d $LAST_LAN_IP --dport 80 -j REDIRECT --to-port 39990
#	$IPT -t nat -D PREROUTING -p tcp ! -d $LAST_LAN_IP --dport 443 -j REDIRECT --to-port 34567
	ipt_unlock_res
}

ipt_shift_port(){
	#port mapping to avoid those service from lan using 5060
	LAST_LAN_IP=`ccfg_cli get ip4addr@lan0`
	$IPT -t nat -I POSTROUTING -s ${LAST_LAN_IP}/24 -p udp --sport 5060 -j MASQUERADE --to-ports 15060-15100
}
