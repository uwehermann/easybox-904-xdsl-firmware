#!/bin/ash

get_vendor_model()
{
	if [ -f /sys/block/${now}/device ] ; then
		vendor=`cat /sys/block/${now}/device/vendor`
		model=`cat /sys/block/${now}/device/model`
	else
		vendor=`grep storage.${now}.vendor /proc/arcusb/status | cut -d ' ' -f 2`
		model=`grep storage.${now}.model /proc/arcusb/status | cut -d ' ' -f 2`
	fi
}

get_dev_name()
{
	local connect_disk_num usbinfo connect_disk_file i
	connect_disk_file="/tmp/fs/connect_disk"
	usbinfo="/tmp/fs/diskinfo_brief"
	mkdir -p /tmp/fs
	touch /tmp/fs/diskinfo_brief
	ls /sys/block | grep sd.* > ${connect_disk_file}
	connect_disk_num=`ls /sys/block | grep -c sd.* `
	connect_disk=`cat ${connect_disk_file}`
	#echo @usbinfo > ${usbinfo}
	#echo disknum=${connect_disk_num} >> ${usbinfo}
	i=0

	if [ ${connect_disk_num} -gt 0 ] ; then
		for now in ${connect_disk}
		do
			#echo dev${i}=$now >> ${usbinfo}
			get_vendor_model
			echo ${vendor} ${model}
			let "i+=1"
		done
	fi
}
#main program
get_dev_name
