#!/bin/sh
#
# Install VoIP common drivers

START=37

# check for linux 2.6.x
uname -r | grep -q 2.6.
if [ $? -eq 0 ]; then
    MODEXT=.ko
fi

bin_dir=/lib/modules/
### ctc ###
#drv_obj_file_name1=voip_timer_driver.ko
drv_obj_file_name1=ifx_voip_timer_driver.ko
###########

start() {

	### ctc ###
	if [ `/bin/grep -c -i voip /proc/cmdline` -ge 1 ] && ! [ -f /tmp/dect_demo_test ] ; then
		return
	fi
	###########

	[ -e ${bin_dir}/${drv_obj_file_name1} ] && {
		insmod ${bin_dir}/${drv_obj_file_name1};
		#create device nodes for...
		#major_no=`grep $drv_dev_base_name /proc/devices |cut -d' ' -f1`
		/bin/mknod /dev/voip_timer_driver c 229 0
	}
}


case "$1" in
	"boot")		start	;;
	"start")	start	;;
#	"stop")		stop	;;
	*)
				echo $0 'boot  - setup and start VoIP common driver'
				echo $0 'start - start VoIP common driver'
#				echo $0 'stop  - stop VoIP common driver'
				;;
esac
