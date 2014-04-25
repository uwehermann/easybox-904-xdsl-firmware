#!/bin/sh
#
# Install Voice CPE TAPI subsystem LL driver for VMMC

START=31

# check for linux 2.6.x
uname -r | grep -q 2.6.
if [ $? -eq 0 ]; then
    MODEXT=.ko
fi

bin_dir=/opt/lantiq/bin
drv_dev_base_name=vmmc
drv_obj_file_name=drv_vmmc$MODEXT

start() {
	[ -e ${bin_dir}/${drv_obj_file_name} ] && {
		CNT=0
		### ctc ###
		while [ `grep drv_tapi /proc/modules | grep -c Live` -le 0 ] && [ $CNT -le 10 ] ; do
			sleep 1
			let CNT=$CNT+1
		done
		###########
		insmod ${bin_dir}/${drv_obj_file_name};
		#create device nodes for...
		vmmc_major=`grep $drv_dev_base_name /proc/devices |cut -d' ' -f1`
		vmmc_minors="10 11 12 13 14 15 16 17 18"
		for vmmc_minor in $vmmc_minors ; do \
		   [ ! -e /dev/vmmc$vmmc_minor ] && mknod /dev/vmmc$vmmc_minor c $vmmc_major $vmmc_minor;
		done
	}
	[ `cat /proc/cpuinfo | grep system | cut -f 3 -d ' '` !=  "Danube" ] && {
		[ ! -e /dev/amazon_s-port ] && mknod /dev/amazon_s-port c 240 1
		echo "INFO configuring HW scheduling 33/66"
		echo "t0 0x0" > /proc/mips/mtsched
		echo "t1 0x1" > /proc/mips/mtsched
		echo "v0 0x0" > /proc/mips/mtsched
	}
	[ `cat /proc/cpuinfo | grep system | cut -f 3 -d ' '` =  "Danube" ] && {
		[ ! -e /dev/danube-port ] && mknod /dev/danube-port c 240 1
		# switch life-line relais
		echo 1 > /sys/class/leds/fxs_relay/brightness
	}
}


case "$1" in
	"boot")		start &	;;
	"start")	start &	;;
#	"stop")		stop	;;
	*)
			echo $0 'boot  - init  Voice CPE TAPI subsystem LL driver'
			echo $0 'start - start Voice CPE TAPI subsystem LL driver'
#			echo $0 'stop  - stop  Voice CPE TAPI subsystem LL driver'
esac
