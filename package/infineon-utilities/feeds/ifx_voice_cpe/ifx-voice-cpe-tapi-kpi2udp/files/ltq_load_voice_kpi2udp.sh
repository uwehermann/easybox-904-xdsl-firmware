#!/bin/sh
#
# Install Voice CPE TAPI subsystem LL driver for VMMC

START=35

# check for linux 2.6.x
uname -r | grep -q 2.6.
if [ $? -eq 0 ]; then
    MODEXT=.ko
fi

bin_dir=/opt/lantiq/bin
drv_obj_file_name=drv_kpi2udp$MODEXT

start() {
	[ -e ${bin_dir}/${drv_obj_file_name} ] && {
		insmod ${bin_dir}/${drv_obj_file_name};
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
