#!/bin/sh /etc/rc.common
START=06

# Include model information
if [ ! "$ENVLOADED" ]; then
	if [ -r /etc/rc.conf ]; then
	. /etc/rc.conf 2> /dev/null
		ENVLOADED="1"
	fi
fi

platform=${CONFIG_IFX_MODEL_NAME%%_*}
if [ "$platform" = "DANUBE" -o "$platform" = "AMAZON" -o "$platform" = "TP-VE" -o "$platform" = "GW188" ]; then
	target=$platform
else
	target=`echo $platform | cut -c -4`
fi
#echo "$target"

# changed min_free_kbytes to have more free memory a given point of execution

if [ "$platform" = "DANUBE" ]; then
# handle heavy traffic scenario with resulted in page allocation error of order 0
        echo 1024 > /proc/sys/vm/min_free_kbytes
	echo 2048 > /proc/sys/net/ipv4/route/max_size
elif [ "$platform" = "AMAZON" ]; then
        echo 1200 > /proc/sys/vm/min_free_kbytes
	echo 2048 > /proc/sys/net/ipv4/route/max_size
	echo 512 > /proc/sys/net/netfilter/nf_conntrack_max
else
        echo 1024 > /proc/sys/vm/min_free_kbytes
	echo 4096 > /proc/sys/net/ipv4/route/max_size
fi

# memory tunning for all platform lowmem_reserve_ratio is a MUST

echo 250 >  /proc/sys/vm/lowmem_reserve_ratio
echo 2 > /proc/sys/vm/dirty_background_ratio
echo 250 > /proc/sys/vm/dirty_writeback_centisecs
echo 10 > /proc/sys/vm/dirty_ratio
echo 16384 > /proc/sys/vm/max_map_count
echo 2 > /proc/sys/vm/page-cluster
echo 70 > /proc/sys/vm/swappiness


toUpper(){ echo "$1"|sed 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/';}
toLower(){ echo "$1"|sed 'y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/';}

set_platform_config() {
#	echo "set_platform_config $1 $2"
	module=""
	ppe_module=""
	ppe_platform=""
	builtin_package=""
	loadable_package=""
	phy_mode=$2

	case $target in
		AMAZON)
		platform="amazon_se"
		if [ "$1" = "ATM" ]; then
			module="a4"
			if [ "$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A2_MOD" = "1" -o "$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A2_BUILTIN" = "1" ]; then
				module="a2"
			fi
		elif [ "$1" = "PTM" ]; then
			module="e4"
		fi
	;;
		DANUBE)
		platform="danube"
		if [ "$1" = "ETHERNET" ]; then
			module="d4"
		elif [ "$1" != "PTM" ]; then
			module="a4"
		fi
	;;
		ARX1|GW188)
		platform="ar9"
		module="a5"
		if [ "$1" = "PTM" ]; then
			module="e5"
		elif [ "$1" = "ETHERNET" ]; then
			module="d5"
		fi
	;;
		VRX2)
		platform="vr9"
		module="a5"
		if [ "$1" = "ETHERNET" ]; then
			module="d5"
		elif [ "$1" = "PTM" ]; then
			phy_mode="3"
			module="e5"
		fi
	;;
		GRX1)
		platform="ar9"
		module="d5"
	;;
		GRX2)
		platform="vr9"
		module="d5"
	;;
		TP-VE)
		platform="danube"
		module="d4"
	;;
	esac

	ppe_module=`toUpper $module`
	ppe_platform=`toUpper $platform`

	if [ "$ppe_module" != "" ]; then
		builtin_package=CONFIG_PACKAGE_KMOD_LTQCPE_PPA_${ppe_module}_BUILTIN
		loadable_package=CONFIG_PACKAGE_KMOD_LTQCPE_PPA_${ppe_module}_MOD
		eval builtin_package_val='$'$builtin_package
		eval loadable_package_val='$'$loadable_package
	fi

#	echo "phy_mode = $phy_mode"
#	echo "module = $module"
#	echo "ppe_module = $ppe_module"
#	echo "platform = $platform"
#	echo "ppe_platform = $ppe_platform"
#	echo "builtin_package = $builtin_package"
#	echo "loadable_package = $loadable_package"
#	echo "builtin_package_val = $builtin_package_val"
#	echo "loadable_package_val = $loadable_package_val"
}

update_ppe_status() {
#	echo "/usr/sbin/status_oper SET "ppe_config_status" "ppe_platform" $ppe_platform "ppe_firmware" $ppe_module"
	/usr/sbin/status_oper SET "ppe_config_status" "ppe_platform" $ppe_platform "ppe_firmware" $ppe_module
}

load_modules() {
#	echo " load_modules $1 $2"
	set_platform_config $1 $2
	
	if [ "$builtin_package_val" = "1" ]; then
		echo "package builtin - $builtin_package"
		update_ppe_status
		echo -n
	elif [ "$loadable_package_val" = "1" ]; then
		datapath_driver=ifxmips_ppa_datapath_${platform}_${module}.ko
		hal_driver=ifxmips_ppa_hal_${platform}_${module}.ko
#		echo "datapath_driver - $datapath_driver"
#		echo "hal_driver - $hal_driver"

		if [ "$platform" = "amazon_se" -a "$module" = "a2" ]; then
			if [ -r /lib/modules/*/ifxmips_a2_se.ko ]; then
				insmod /lib/modules/*/ifxmips_a2_se.ko
				update_ppe_status
			else
				echo "ERROR: A2 Driver Not found"
			fi
		else
			if [ -r /lib/modules/*/$datapath_driver ]; then
				if [ "$target" = "TP-VE" -a "$1" = "ETHERNET" ]; then
					insmod /lib/modules/*/ifxmips_ppa_lite_datapath_${platform}_${module}.ko
				fi
				if [ "$ppe_module" = "A5" -o "$ppe_module" = "E5" ]; then
					insmod /lib/modules/*/$datapath_driver ethwan=$phy_mode wanqos_en=8
				elif [ "$ppe_module" = "D5" -a "$target" = "GRX1" ]; then
					insmod /lib/modules/*/$datapath_driver ethwan=$phy_mode wanqos_en=8
				elif [ "$ppe_module" = "D5" -a "$target" = "GRX2" ]; then
					#Disable fw qos by default on GRX288 platforms
					insmod /lib/modules/*/$datapath_driver ethwan=$phy_mode wanqos_en=0
				else
					insmod /lib/modules/*/$datapath_driver ethwan=$phy_mode
				fi
				insmod /lib/modules/*/$hal_driver
				insmod /lib/modules/*/ifx_ppa_api.ko
				insmod /lib/modules/*/ifx_ppa_api_proc.ko 
#				if [ "$target" = "VRX2" ]; then
#					ifconfig ptm0 up
#				fi
				update_ppe_status
			else
				"ERROR : datapath_driver - $datapath_driver not found"
			fi
		fi
	else
		if [ "$CONFIG_PACKAGE_KMOD_LTQCPE_ETH_DRV_BUILTIN" = "1" ]; then
			echo -n
		elif [ "$CONFIG_PACKAGE_KMOD_LTQCPE_ETH_DRV_MOD" = "1" ]; then
			grep ifxmips_eth_drv /proc/modules
			if [ "$?" != "0" ]; then
				insmod /lib/modules/*/ifxmips_eth_drv.ko
			fi
		else
			echo "Ethernet Driver not available. Exit Show"
		fi
		case $1 in 
			ETHERNET)
			echo -n
			;;
			ATM)
			if [ "$CONFIG_PACKAGE_KMOD_LTQCPE_ATM_MOD" = "1" ]; then
				# This is Non-Accelerated Mode, so insert stand-alone
				# Ethernet/Switch driver along with A1 ATM Driver
				insmod /lib/modules/*/ifxmips_atm.ko
				ppe_module="A1"
				update_ppe_status
			fi
			;;	
			PTM)
			if [ "$CONFIG_PACKAGE_KMOD_LTQCPE_PTM_MOD" = "1" ]; then	
				# This is Non-Accelerated Mode, so insert stand-alone
				# Ethernet/Switch driver along with E1 PTM Driver
				insmod /lib/modules/*/ifxmips_ptm.ko
#				ifconfig ptm0 up
				ppe_module="E1"
				update_ppe_status
			fi
			;;
		esac
	fi
}

disable_vrx_switch_ports() {
	SWITCH_PORT="0 1 2 3 4 5 7 8 9"
	for argument in $SWITCH_PORT ;do 
		switch_utility PortCfgSet $argument 0 0 0 0 0 0 1 255 0 0 
	done 

	SWITCH_PORT="6" 
	for argument in $SWITCH_PORT ;do 
		switch_utility PortCfgSet $argument 0 0 0 0 0 0 0 255 0 0 
	done 
}

enable_vrx_switch_ports() {
	SWITCH_PORT="6" 
	for argument in $SWITCH_PORT ;do 
		switch_utility PortCfgSet $argument 1 0 0 0 0 0 0 255 0 0 
	done 

	SWITCH_PORT="0 1 2 3 4 5 7 8 9"
	for argument in $SWITCH_PORT ;do 
		switch_utility PortCfgSet $argument 1 0 0 0 0 0 0 255 0 0 
	done 
}

start() {

	if [ "$platform" = "vr9" ]; then
		disable_vrx_switch_ports;
	fi

	if [ "$wanphy_phymode" = "1" -o "$wanphy_phymode" = "2" ]; then #Ethernet
		load_modules "ETHERNET" $wanphy_phymode
	else
		if [ "$Next_xTM_Mode" = "ATM" -o "$Next_xTM_Mode" = "PTM" ]; then
			load_modules $Next_xTM_Mode 0 
		else
			echo "UNSUPPORTED TC MODE : $Next_xTM_Mode"
			if [ ! -f /etc/rc.conf.gz ]; then
                                #Running in evaluation mode. Loading ethernet driver..
                                cat /proc/modules | grep ifxmips_eth_drv
                                [ $? -ne 0 ] && insmod /lib/modules/*/ifxmips_eth_drv.ko && [ `mount|grep -q nfs;echo $?` -eq  0 ] || ifconfig eth0 up
                        fi

		fi
	fi
	if [ "$platform" = "vr9" ]; then
		enable_vrx_switch_ports;
	fi
}

