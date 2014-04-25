#!/bin/sh
#
# Install DECT drivers

START=38

# check for linux 2.6.x
uname -r | grep -q 2.6.
if [ $? -eq 0 ]; then
    MODEXT=.ko
fi

bin_dir=/usr/drivers/
drv_obj_file_name1=drv_timer.ko
drv_obj_file_name2=drv_dect.ko
drv_obj_file_name3=paging.ko

start() {

	### ctc ###
	if [ `/bin/grep -c -i voip /proc/cmdline` -ge 1 ] && ! [ -f /tmp/dect_demo_test ] ; then
		return
	fi
	###########

	[ -e ${bin_dir}/${drv_obj_file_name1} ] && {
		insmod ${bin_dir}/${drv_obj_file_name1};
		# for netdev budget 10
		echo 10 > /proc/sys/net/core/netdev_budget
		#create device nodes for...
		#major_no=`grep $drv_dev_base_name /proc/devices |cut -d' ' -f1`
		/bin/mknod /dev/timer_drv c 212 0
	}
	[ -e ${bin_dir}/${drv_obj_file_name2} ] && {
		insmod ${bin_dir}/${drv_obj_file_name2};
		#create device nodes for...
		#major_no=`grep $drv_dev_base_name /proc/devices |cut -d' ' -f1`
		/bin/mknod /dev/dect_drv c 213 0
	}
	[ -e ${bin_dir}/${drv_obj_file_name3} ] && {
		insmod ${bin_dir}/${drv_obj_file_name3};
		#create device nodes for...
		#major_no=`grep $drv_dev_base_name /proc/devices |cut -d' ' -f1`
		/bin/mknod /dev/pb c 150 0
	}
}


case "$1" in
	"boot")		start &	;;
	"start")	start &	;;
#	"stop")		stop	;;
	*)
			echo $0 'boot  - init  DECT drivers'
			echo $0 'start - start DECT drivers'
#			echo $0 'stop  - stop  DECT drivers'
esac
