#!/bin/sh
#on test
#file="/home/daniel/tmp/fs/diskinfo"
#mkdir -p /home/daniel/tmp/fs/proc/scsi
#touch /home/daniel/tmp/fs/diskinfo
#cp -f /proc/scsi/scsi /home/daniel/tmp/fs/proc/scsi/scsi
#mkdir -p /home/daniel/tmp/fs/tmp
#myproc="/home/daniel/tmp/fs/proc/scsi/scsi"
#connect_disk_file="/home/daniel/tmp/fs/connect_disk"


#on server
file="/tmp/fs/diskinfo"
disk_label="/tmp/fs/disklabel"
#mkdir -p /tmp/fs/proc/scsi
mkdir -p /tmp/fs
touch /tmp/fs/diskinfo
#cp -f /proc/scsi/scsi /tmp/fs/proc/scsi/scsi
#myproc="/tmp/fs/proc/scsi/scsi"
#file_getcd="/tmp/fs/tmp/file_getcd"
connect_disk_file="/tmp/fs/connect_disk"

get_validPartition()
{
	# in general
	#validPartition=`ls /sys/block/${now}/ | grep ${now} | wc -l`
	# for ar9, we have to check if it already mount
	#echo $now > /tmp/fs/tmp/valid
	#partition_name_in_tmp=`cat /tmp/fs/tmp/valid | sed 's/sd//'`
#	partition_name_in_tmp=${now//sd/}
#	validPartition=`ls /tmp/usb | grep -c ${partition_name_in_tmp}.*`
	validPartition=`grep -c "^/dev/$now" $disk_label`
	
	if [ "$validPartition" = "0" ] ; then
		validPartition=`ls /sys/block/${now}/ | grep ${now} | wc -l`
		if [ "$validPartition" = "0" ] ; then
			validPartition="1"
		fi			
	fi
	echo validPartition=$validPartition >> $file	
}

get_scsinum()
{
	# in general
	#scsinum=`ls /sys/block/${now}/device/ | grep scsi_device | sed 's/scsi_device://'`
	# on ar9
	if [ -f /sys/block/${now}/device ] ; then
		scsinum=`ls -l /sys/block/${now}/device | sed 's/^.*target//' | awk -F'/' '{print $2}'`
	else
		scsinum=0
	fi

	echo scsinum=$scsinum >> $file
}

get_vendor_model_type_removable()
{
	if [ -f /sys/block/${now}/device ] ; then
		vendor=`cat /sys/block/${now}/device/vendor`
		model=`cat /sys/block/${now}/device/model`
		type=`cat /sys/block/${now}/device/type`
	else
		str=`grep storage.${now}.vendor /proc/arcusb/status | cut -d ':' -f 1`
		let len=${#str}+2
		vendor=`grep storage.${now}.vendor /proc/arcusb/status`
		vendor=${vendor:${len}}
		str=`grep storage.${now}.model /proc/arcusb/status | cut -d ':' -f 1`
		let len=${#str}+2
		model=`grep storage.${now}.model /proc/arcusb/status`
		model=${model:${len}}
		str=`grep storage.${now}.type /proc/arcusb/status | cut -d ':' -f 1`
		let len=${#str}+2
		type=`grep storage.${now}.type /proc/arcusb/status`
		type=${type:${len}}
	fi
	if [ -z "$type" ] ; then
		type=0
	fi
	echo "vendor=$vendor" >> $file
	echo "model=$model" >> $file
	echo "type=$type" >> $file
	get_serial
	removable=`cat /sys/block/${now}/removable`
	if [ "$removable" -eq 1 ] ; then
		echo removable=true >> $file
	else
		echo removable=false >> $file
	fi
	echo reload=false >> $file
}

get_serial()
{
	# in general
	#scsi=`ls /sys/block/${now}/device | grep scsi_device | awk -F':' '{print $2}'`
	if [ -f /sys/block/${now}/device ] ; then
		scsi=`ls -l /sys/block/${now}/device | sed 's/^.*target//' | cut -d '/' -f 2 | cut -d ':' -f 1`
		serial=`grep "Serial Number" /proc/scsi/usb-storage/${scsi} | sed 's/Serial Number: //'`
	else
		serial=
	fi
	echo "serial=$serial" >> $file
}



#begin

rm $connect_disk_file
touch $connect_disk_file

if ! [ -f "$disk_label" ] ; then
	echo "!" > $disk_label
fi 

connect_disk=`ls /sys/block | grep sd.*`
for each_disk in $connect_disk ; do
	if [ -e "/dev/${each_disk}" ] ; then
		echo $each_disk >> $connect_disk_file
	fi
done
connect_disk=`cat $connect_disk_file`

connect_disk_num=`wc -l $connect_disk_file | cut -d ' ' -f 1`
connect_disk_alive_num=0

#disk which has partition mount
for i in $connect_disk ; do
	alive_num=`wc -l $disk_label | cut -d ' ' -f 1`
	if [ $alive_num -gt 0 ] ; then
		let connect_disk_alive_num=$connect_disk_alive_num+1
	fi
done

echo "@diskinfo" > $file
echo "connect_disk_num=$connect_disk_num" >> $file
echo "connect_disk_alive_num=$connect_disk_alive_num" >> $file

if [ $connect_disk_num -gt 0 ] ; then
	j=0
	for i in $connect_disk ; do
		#echo disk$j=$i 
		echo disk$j=$i >> $file 
		let j=$j+1
	done
fi



#printf "connetc_disk_num=$connect_disk_num \n" >> $file

if [ $connect_disk_num -gt 0 ] ; then
	for now in $connect_disk ; do
		echo @$now >> $file
		echo devName=$now >> $file
		get_validPartition
		get_scsinum
		get_vendor_model_type_removable
	done
fi


echo "@DFT_SECTION"    >> $file
echo "devName="        >> $file
echo "validPartition=" >> $file
echo "vendor="         >> $file
echo "model="          >> $file
echo "type="           >> $file
echo "serail="         >> $file
echo "removable="      >> $file
echo "reload="         >> $file
echo "!"               >> $file






