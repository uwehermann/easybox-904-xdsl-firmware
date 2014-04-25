scan_pppoe() {
	scan_ppp "$@"
}

setup_interface_pppoe() {
	local iface="$1"
	local config="$2"
	
	for module in slhc ppp_generic pppox pppoe; do
		/sbin/insmod $module 2>&- >&-
	done

	# make sure the network state references the correct ifname
	scan_ppp "$config"
	config_get ifname "$config" ifname
	set_interface_ifname "$config" "$ifname"

	config_get mtu "$cfg" mtu
	mtu=${mtu:-1492}
	
	echo "check to bring up pppoe" >>/dev/console 
	local wanif=$2
	local ivr_mode=`umngcli get ivr_mode@bootstrap`
	local proto=`umngcli get proto@$wanif`
	echo "$ivr_mode" >>/dev/console
	if [ "$ivr_mode" == "0" ] ; then
	   if [ "$wanif" == "wan050" ] || [ "$wanif" == "wan100" ] || [ "$wanif" == "wan000" ] ; then
	      if [ "$proto" == "pppoe" ]; then 
			     #echo "[pppoe.sh] don't invoke pppoe when not activate" >> /dev/console
			     return 1
			  fi   
		 fi	
	fi	
	
	start_pppd "$config" \
		plugin rp-pppoe.so \
		mtu $mtu mru $mtu \
		"nic-$iface"
}
