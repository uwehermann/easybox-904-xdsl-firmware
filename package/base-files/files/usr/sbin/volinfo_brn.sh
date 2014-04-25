#!/bin/sh

#################################
#
# PLEASE CALL "diskinfo_brn.sh" first
#
#################################

#on test
#tmpfile_each="/home/daniel/tmp/fs/tmp/each"
#disk_info="/home/daniel/tmp/fs/diskinfo"
#file="/home/daniel/tmp/fs/sharefolderinfo"
#mounton="/home/daniel/tmp/fs/tmp/mountname"
#tmpfile="/home/daniel/tmp/fs/tmp/tmpfile"

#on server
disk_info="/tmp/fs/diskinfo"
disk_label="/tmp/fs/disklabel"
file="/tmp/fs/volinfo"
#tmpfile="/tmp/fs/tmp/tmpfile"
NBIOS_MAX_DISK_VOLUME=99



gettype()
{
	# in general
	#volType=`cat /proc/mounts | grep $partition_dev | awk '{print toupper($3)}'`
	# on ar9, cause ntfs mount will turn to fuseblk

	local tmp
	local byte1
	local byte2
	local byte3
	local byte4

	PART_HEAD_FILE=/tmp/fs/${DEVICENAME}
	if [ `hexdump -C -s 82 -n 5 ${PART_HEAD_FILE} | grep -c "|FAT32|"` -ge 1 ] ; then
		volType="FAT32"
		echo volType=$volType >> $file
		echo volFsIdx=4 >> $file
		echo volTypeID=12 >> $file
	elif [ `hexdump -C -s 3 -n 4 ${PART_HEAD_FILE} | grep -c "|NTFS|"` -ge 1 ] ; then
		volType="NTFS"
		echo volType=$volType >> $file
		echo volFsIdx=0 >> $file
		echo volTypeID=7 >> $file
	elif [ `hexdump -C -s 54 -n 5 ${PART_HEAD_FILE} | grep -c "|FAT16|"` -ge 1 ] ; then
		volType="FAT16"
		echo volType=$volType >> $file
		echo volFsIdx=4 >> $file
		echo volTypeID=12 >> $file
	elif [ `hexdump -C -s 54 -n 5 ${PART_HEAD_FILE} | grep -c "|FAT12|"` -ge 1 ] ; then
		volType="FAT12"
		echo volType=$volType >> $file
		echo volFsIdx=4 >> $file
		echo volTypeID=12 >> $file
	elif [ `hexdump -Cv -s 1080 -n 2 ${PART_HEAD_FILE} | grep -ci "53 ef"` -eq 1 ] || [ `hexdump -Cv -s 1080 -n 2 ${PART_HEAD_FILE} | grep -ci "51 ef"` -eq 1 ] ; then
		tmp=`hexdump -Cv -s 1116 -n 1 ${PART_HEAD_FILE}`
		volType=""
		tmp=${tmp#* }
		tmp=${tmp%% |*}
		tmp=${tmp#?}
		tmp=${tmp#?}
		if [ `echo $tmp | grep -ci [012389ab]` -eq 1 ] ; then
			volType="EXT2"
			echo volType=$volType >> $file
			echo volFsIdx=8 >> $file
			echo volTypeID=15 >> $file
			echo $volType  > /dev/console
		fi
		tmp=`hexdump -Cv -s 1124 -n 4 ${PART_HEAD_FILE}`
		tmp=${tmp#* }
		tmp=${tmp%%|*}
		tmp=${tmp# }
		num=1
		for i in $tmp
		do
			if [ $num -eq 1 ] ; then
				byte1=$i
			elif [ $num -eq 2 ] ; then
				byte2=$i
			elif [ $num -eq 3 ] ; then
				byte3=$i
			elif [ $num -eq 4 ] ; then
				byte4=$i
			fi
			let num=$num+1
		done
		if [ "$volType" != "EXT2" ] ; then
			if [ "$byte4" = "00" ] && [ "$byte3" = "00" ] && [ "$byte2" = "00" ] && [ $byte1 -lt 8 ] ; then
				volType="EXT3"
				echo volType=$volType >> $file
				echo volFsIdx=9 >> $file
				echo volTypeID=16 >> $file
				echo $volType  > /dev/console
			elif [ `hexdump -Cv -s 1292 -n 2 /dev/${DEVICENAME} | grep -ci "0a f3"` -eq 1 ] ; then
				volType="EXT4"
				echo volType=$volType >> $file
				echo volFsIdx=10 >> $file
				echo volTypeID=17 >> $file
				echo $volType  > /dev/console
			fi
		fi
	else
		volType="Unknown"
		echo volType=$volType >> $file
		echo volFsIdx=-1 >> $file
		echo voltypeID=0 >> $file
	fi
}



rm -f $file
touch $file

if ! [ -f "$disk_label" ] ; then
	echo "!" > $disk_label
fi

# check NBIOS_RES_PROFILE in cgi-src/file_cgi.h
if [ -f "$disk_info" ] ; then
	connect_disk_num=`umngcli -f $disk_info get connect_disk_num@diskinfo`
fi
if [ -z "$connect_disk_num" ] ; then
	connect_disk_num=0
fi
mount_all_num=`wc -l $disk_label | cut -d ' ' -f 1`

echo "@volinfo"                     >> $file
echo "mount_all_num=$mount_all_num" >> $file


disk_num=0
while [ $disk_num -lt $connect_disk_num ] ; do
	devName=`umngcli -f $disk_info get disk${disk_num}@diskinfo`
	mount_vol_num=`grep -c "^/dev/${devName}" $disk_label`
	echo "${devName}_id=$disk_num" >> $file
	echo "${devName}_mount_num=$mount_vol_num" >> $file
	let disk_num=$disk_num+1
done


disk_num=0
while [ $disk_num -lt $connect_disk_num ] ; do

	devName=`umngcli -f $disk_info get disk${disk_num}@diskinfo`
	mount_vol_num=`grep -c "^/dev/${devName}" $disk_label`
	partition_list=`grep "^/dev/${devName}" $disk_label | cut -d ' ' -f 2 | cut -d '/' -f 4`
#	mounton=${devName/sd/}
	echo partition list $partition_list >> /dev/console
	echo "@disk${disk_num}_0"  >> $file
	echo "volFsIdx=-1"         >> $file
	echo "volLabelMix=No Disk" >> $file
	echo "volID=0"             >> $file
	echo "volValid=true"       >> $file

	partition_num=1
	while [ $partition_num -le $NBIOS_MAX_DISK_VOLUME ] && [ $partition_num -le $mount_vol_num ] ; do
		for partition in $partition_list ; do
			#partition_dev=`echo $partition | sed 's/^/sd/'`
			partition_dev=sd${partition}
			real_partition_dev=${partition_dev}
			echo real part ${real_partition_dev} >> /dev/console
			if [ "${partition_dev:3}" = "1" ] ; then
				part_cnt=`grep -c "/dev/$partition_dev" $disk_label`
				echo 2 real part $real_partition_dev [$part_cnt] >> /dev/console
				if [ "$part_cnt" = "0" ] ; then
					echo 3 copy ${partition_dev:0:3} to $partition_dev >> /dev/console
					cp /tmp/fs/${partition_dev:0:3} /tmp/fs/$partition_dev
					real_partition_dev=${partition_dev:0:3}
				fi
			fi
			#if [ "$partition_dev" = "sdb1" ] ; then
			#	part_cnt=`grep -c /dev/sdb1 $disk_label`
			#	echo 2 real part $real_partition_dev [$part_cnt] >> /dev/console
			#	if [ "$part_cnt" = "0" ] ; then
			#		echo copy sdb to sdb1 >> /dev/console
			#		cp /tmp/fs/sdb /tmp/fs/sdb1
			#		real_partition_dev="sdb"
			#	fi
			#fi
			#get_label_serial
			volLabel=`grep "^/dev/${real_partition_dev} " $disk_label | cut -d ' ' -f 3 | cut -d '"' -f 2`
			if [ -z "$volLabel" ] ; then
				volLabel="Partition${partition_num}"
			fi
			volSerial=`grep "^/dev/${real_partition_dev} " $disk_label | cut -d ' ' -f 4`
			volSize=`df -h /dev/$real_partition_dev | sed '1d' | awk '{print $2}'`
			volFree=`df -h /dev/$real_partition_dev | sed '1d' | awk '{print $4}'`
			#
			echo "@disk${disk_num}_${partition_num}" >> $file
			echo "devName=$partition_dev" >> $file
			echo "volLabel=$volLabel"     >> $file
			echo "volSerial=$volSerial"   >> $file
			echo "volValid=true"          >> $file
			DEVICENAME=$partition_dev
			gettype
			echo "volID=$partition_num"   >> $file
			echo "volMount=1"             >> $file
			echo "volSize=$volSize"       >> $file
			echo "volFree=$volFree"       >> $file
			echo "volLabelMix=$volLabel ($volType)" >> $file
			echo "partInfo=$volLabel ($volSize, $volFree free, $volType)" >> $file

			let partition_num=$partition_num+1
			if [ $partition_num -gt $NBIOS_MAX_DISK_VOLUME ] ; then
				break 2
			fi
		done

#		while [ $partition_num -le $NBIOS_MAX_DISK_VOLUME ] ; do
#			echo "@disk${disk_num}_${partition_num}" >> $file
#			echo "volFsIdx=-1"     >> $file
#			echo "volValid=false"  >> $file
#			echo "volType=Unknown" >> $file
#			echo "volLabelMix="    >> $file
#			echo "volLabel="       >> $file
#			echo "volSerial="      >> $file
#			echo "volID=1"         >> $file
#			echo "volTypeID=0"     >> $file
#			echo "volSize=0"       >> $file
#			echo "volFree=0"       >> $file
#			echo "volMount=-1"     >>  $file
#			echo "partInfo=Partition${partition_num} (Unknown)" >> $file
#			let partition_num=$partition_num+1
#		done



	done

	let disk_num=$disk_num+1

done


echo "@DFT_SECTION" >> $file
echo "!"            >> $file

