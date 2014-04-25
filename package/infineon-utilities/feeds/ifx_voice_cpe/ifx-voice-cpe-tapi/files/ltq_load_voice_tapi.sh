#!/bin/sh
#
# Install Voice CPE TAPI subsystem

START=30

# check for linux 2.6.x
uname -r | grep -q 2.6.
if [ $? -eq 0 ]; then
    MODEXT=.ko
fi

bin_dir=/opt/lantiq/bin
drv_dev_base_name=tapi
drv_obj_file_name=drv_tapi$MODEXT

start() {
	[ -z `cat /proc/modules | grep ifxos` ] && {
		echo "Ooops - IFXOS isn't loaded, TAPI will do it. Check your basefiles..."
		insmod ${bin_dir}/drv_ifxos$MODEXT
	}
	# temporary check for loading the eventlogger driver
	[ -e ${bin_dir}/drv_event_logger$MODEXT ] &&
	[ -z `cat /proc/modules | grep event_logger` ] && {
		insmod ${bin_dir}/drv_event_logger$MODEXT
	}
	[ -e ${bin_dir}/${drv_obj_file_name} ] && {
		insmod ${bin_dir}/${drv_obj_file_name};
		#create device nodes for...
		#major_no=`grep $drv_dev_base_name /proc/devices |cut -d' ' -f1`
	}
}


case "$1" in
	"boot")		start &	;;
	"start")	start &	;;
#	"stop")		stop	;;
	*)
			echo $0 'boot  - init  Voice CPE TAPI subsystem'
			echo $0 'start - start Voice CPE TAPI subsystem'
#			echo $0 'stop  - stop  Voice CPE TAPI subsystem'
esac
