#!/bin/sh

gettype(){
	# in general
	#volType=`cat /proc/mounts | grep $partition_dev | awk '{print toupper($3)}'`
	# on ar9, cause ntfs mount will turn to fuseblk
	devtype=`hexdump -C -n 128 /dev/${partition_dev}`
	if [ `echo ${devtype} | grep -c "FAT12"` -eq 1 ]
	then
		volType="FAT12"
	elif [ `echo ${devtype} | grep -c "FAT16"` -eq 1 ]
	then
		volType="FAT16"
	elif [ `echo ${devtype} | grep -c "FAT32"` -eq 1 ]
	then
		volType="FAT32"
	elif [ `echo ${devtype} | grep -c "MSDOS"` -eq 1 ]
	then
		volType="MSDOS"
	elif [ `echo ${devtype} | grep -c "NTFS"` -eq 1 ]
	then
		volType="NTFS"
	else
		volType="Unknown"
	fi
}

set_drive_num()
{
	umngcli set drive_num@drive_now=${drive_num}
}

set_driveinfo()
{
	local drive=$1
	# get vendor, model, serial
	if [ -f /sys/block/${drive}/device ] ; then
		vendor=`cat /sys/block/${drive}/device/vendor`
		model=`cat /sys/block/${drive}/device/model`
		scsi=`ls -l /sys/block/${drive}/device | sed 's/^.*target//' | awk -F'/' '{print $2}' | awk -F':' '{print $1}'`
		serial=`cat /proc/scsi/usb-storage/${scsi} | grep "Serial Number" | sed 's/Serial Number: //'`
	else
		vendor=`grep storage.${drive}.vendor /proc/arcusb/status | cut -d ' ' -f 2`
		model=`grep storage.${drive}.model /proc/arcusb/status | cut -d ' ' -f 2`
		serial=
	fi
	# get partition_totalnum
	partition_num=`ls /tmp/usb| grep ${each##sd} | wc -l`
	driveinfo=\"${drive##sd},${vendor:= },${model:= },${serial},${partition_num}\"
	umngcli set driveinfo${drive##sd}@drive_now="${driveinfo}"
}

set_volume()
{
	local drive=$1
	volume=
	for partition in ${ValidPartition}
	do
		if [ -z `ls /dev/sd* | grep ${partition}` ] ; then
			ismount=0
		else
			ismount=1
		fi

		partition_dev=sd${partition}
		volLabel=`findfs -l | grep ${partition_dev} | awk -F';;;;;' '{print $2}'`
		volLabel=${volLabel#?}
		volLabel=${volLabel%?}
		if [ -z ${volLabel} ] || [ ${volLabel} = '' ] || [ ${volLabel} = \"\" ] ; then
			volLabel="Partition${partition#?}"
		fi
		gettype
		if [ -z ${volume}  ] || [ ${volume} = '' ] ; then
			volume="${partition},${volType},${volLabel},${ismount}"
		else
			volume=${volume}",${partition},${volType},${volLabel},${ismount}"
		fi


	done
	umngcli set volume${drive##sd}@drive_now="${volume}"

}

connect_drive=`ls /sys/block |grep sd.*`
drive_num=0
for each in ${connect_drive}
do
	drive_num=$((drive_num+1))
	ValidPartition=`ls /tmp/usb| grep ${each##sd}`
	set_driveinfo ${each}
	set_volume ${each}
done
for each in ${connect_drive}
do
	drive_num=${drive_num},${each}
done
set_drive_num








