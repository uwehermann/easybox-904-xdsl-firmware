#!/bin/sh

### Following Assumptions are made in this script

KERNEL_VER=`uname -r`

grep "xDSL_MODE_VRX" /flash/rc.conf
if [ $? != "0" ] ; then 
        echo "#<< xDSL_MODE_VRX" >> /flash/rc.conf
        echo "xDSL_MODE_VRX=vdsl" >> /flash/rc.conf
        echo "#>> xDSL_MODE_VRX" >> /flash/rc.conf
        echo "#<< ACCELERATION" >> /flash/rc.conf
        echo "ACCELERATION=acc_no" >> /flash/rc.conf
        echo "#>> ACCELERATION" >> /flash/rc.conf
        echo " "
        echo "No mode was set , set VDSL mode as default"
        /usr/sbin/savecfg.sh
fi


. /flash/rc.conf

echo 1 > /proc/sys/net/netfilter/nf_conntrack_tcp_be_liberal
echo 3000 >/proc/sys/vm/min_free_kbytes  # -- OOM issue
echo 10 >/proc/sys/net/core/netdev_budget # -- bad voice quality

case "$xDSL_MODE_VRX" in
vdsl)
        echo "Setting in flash is VDSL mode"
        case "$ACCELERATION" in
                acc_no)
                        insmod /lib/modules/${KERNEL_VER}/ifxmips_ptm.ko
                        sleep 1
                        brctl setfd br0 1
                        brctl addif br0 eth0 2>/dev/null
                        ifconfig br0 192.168.1.1
                        ifconfig eth0 0.0.0.0 up
                        ifconfig eth1 0.0.0.0
                        ifconfig ptm0 up
                ;;
                acc_yes)
                        echo "Enter PPA mode"
		insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_e5.ko
		insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_e5.ko
		insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api.ko
		insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api_proc.ko


                        brctl setfd br0 1
                        brctl addif br0 eth0
                        /sbin/ifconfig eth1 0.0.0.0 down
                        /sbin/ifconfig eth0 0.0.0.0 up
                        ifconfig br0 192.168.1.1
                        sleep 3

                        echo enable > /proc/ppa/api/hook         
                        sleep 1

                        ppacmd control --enable-lan --enable-wan
                        ppacmd addlan -i eth0
                        ppacmd addlan -i br0
                        ifconfig ptm0 2> /dev/null
                        if [ $? = "0" ] ; then
                                ppacmd addwan -i ptm0
                                ifconfig ptm0 up
                        fi
                  ;;
                *)
                        echo "Please specific acc_no/acc_yes"
                        exit 1
                esac

                if [ -s /opt/lantiq/bin/dsl_cpe_pipe.sh ] ; then
                /opt/lantiq/bin/dsl_cpe_pipe.sh g997xtusecs 0 0 0 0 0 0 0 7
                /opt/lantiq/bin/dsl_cpe_pipe.sh acs 2
                fi
    ;;
adsl_a)
        echo "Setting in flash is ADSL Annex A mode"
        case "$ACCELERATION" in
        acc_no) 
                cat /proc/modules|grep ifxmips_atm
                insmod /lib/modules/${KERNEL_VER}/ifxmips_atm.ko
                /opt/lantiq/bin/dsl_cpe_pipe.sh g997xtusecs 5 0 4 0 C 1 0 0
                /opt/lantiq/bin/dsl_cpe_pipe.sh acs 2
                brctl setfd br0 1
                brctl addif br0 eth0 2>/dev/null
                ifconfig br0 192.168.1.1
                ifconfig eth0 0.0.0.0 up
                ifconfig eth1 0.0.0.0 down
		/flash/BSP-Test-VR9/bringup_wan_pvc.sh 0 0 33 01:20:23:43:53:33
		sleep 1
		ifconfig nas0 up
                ;;
        acc_yes)
		insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_a5.ko
		insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_a5.ko
		insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api.ko
		insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api_proc.ko

                /opt/lantiq/bin/dsl_cpe_pipe.sh g997xtusecs 5 0 4 0 C 1 0 0
                /opt/lantiq/bin/dsl_cpe_pipe.sh acs 2
                brctl setfd br0 1
                brctl addif br0 eth0 2>/dev/null
                ifconfig br0 192.168.1.1
                ifconfig eth0 0.0.0.0 up
                ifconfig eth1 0.0.0.0 down
		/flash/BSP-Test-VR9/bringup_wan_pvc.sh 0 0 33 01:20:23:43:53:33
                sleep 1
                echo enable > /proc/ppa/api/hook
                sleep 1
                ppacmd control --enable-lan --enable-wan
                ppacmd addlan -i eth0
                ppacmd addlan -i br0
                ifconfig nas0 2> /dev/null
                if [ $? = "0" ] ; then
                	ppacmd addwan -i nas0
                        ifconfig nas0 up
                fi
                ;;
        *)
                echo "Please specific acc_no/acc_yes"
                exit 1
        esac
    ;;
adsl_b)
        echo "Setting in flash is ADSL Annex B mode"
        case "$ACCELERATION" in
        acc_no)
                cat /proc/modules|grep ifxmips_atm
                insmod /lib/modules/${KERNEL_VER}/ifxmips_atm.ko 
                /opt/lantiq/bin/dsl_cpe_pipe.sh g997xtusecs 10 0 10 0 0 4 0 0
                /opt/lantiq/bin/dsl_cpe_pipe.sh acs 2
                brctl setfd br0 1
                brctl addif br0 eth0 2>/dev/null
                ifconfig br0 192.168.1.1
                ifconfig eth0 0.0.0.0 up
                ifconfig eth1 0.0.0.0 down
                /flash/BSP-Test-VR9/bringup_wan_pvc.sh 0 0 33 01:20:23:43:53:33
                sleep 1
                ifconfig nas0 up
        ;;
	acc_yes)
		insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_a5.ko
		insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_a5.ko
		insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api.ko
		insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api_proc.ko

                /opt/lantiq/bin/dsl_cpe_pipe.sh g997xtusecs 10 0 10 0 0 4 0 0
                /opt/lantiq/bin/dsl_cpe_pipe.sh acs 2
                brctl setfd br0 1
                brctl addif br0 eth0 2>/dev/null
                ifconfig br0 192.168.1.1
                ifconfig eth0 0.0.0.0 up
                ifconfig eth1 0.0.0.0 down
                /flash/BSP-Test-VR9/bringup_wan_pvc.sh 0 0 33 01:20:23:43:53:33
                sleep 1
                echo enable > /proc/ppa/api/hook
                sleep 1
                ppacmd control --enable-lan --enable-wan
                ppacmd addlan -i eth0
                ppacmd addlan -i br0
                ifconfig nas0 2> /dev/null
                if [ $? = "0" ] ; then
                        ppacmd addwan -i nas0
                        ifconfig nas0 up
                fi
        ;;
        *)
                echo "Please specific acc_no/acc_yes"
                exit 1
        esac
    ;;
eth_wan)
        echo "Setting in flash is Ethernet WAN mode"
	insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_datapath_vr9_d5.ko
	insmod /lib/modules/${KERNEL_VER}/ifxmips_ppa_hal_vr9_d5.ko
	insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api.ko
	insmod /lib/modules/${KERNEL_VER}/ifx_ppa_api_proc.ko

        brctl setfd br0 1
        brctl addif br0 eth0
        /sbin/ifconfig eth0 0.0.0.0 up
        /sbin/ifconfig eth1 0.0.0.0 up
        ifconfig br0 192.168.1.1
        sleep 3

        echo enable > /proc/ppa/api/hook
        sleep 1
        ppacmd control --enable-lan --enable-wan
        ppacmd addlan -i eth0
        ppacmd addlan -i br0
        ppacmd addwan -i eth1
    ;;
*)
    echo "No mode was found"
    exit 1
esac

exit 0
