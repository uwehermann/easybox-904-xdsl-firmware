#!/bin/sh

#################################
#
# PLEASE CALL "volinfo_brn.sh" first
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
vol_info="/tmp/fs/volinfo"
file="/tmp/fs/sharefolderinfo"
disk_label="/tmp/fs/disklabel"


# check NBIOS_RES_PROFILE in cgi-src/file_cgi.h

NBIOS_RES_PROFILE=42
each_num=0
disk_num=0

if [ -f "$disk_info" ] ; then
	connect_disk_num=`umngcli -f $disk_info get connect_disk_num@diskinfo`
fi
if [ -z "$connect_disk_num" ] ; then
	connect_disk_num=0
fi
partition_nolabel_num=0

if ! [ -f "$disk_label" ] ; then
	echo "!" > $disk_label
fi 


now=0

#echo $now
echo "@share$now"      > $file
echo "shEnable=0"     >> $file
echo "shDisk=0"       >> $file
echo "shVol=0"        >> $file
echo "shName=Printer" >> $file
echo "shPathStr="     >> $file
let now=$now+1

#echo $now
echo "@share$now"        >> $file
echo "shEnable=0"        >> $file
echo "shDisk=0"          >> $file
echo "shVol=0"           >> $file
echo "shName=Disk_Array" >> $file
echo "shPathStr=\\\\"    >> $file
let now=$now+1


while [ $disk_num -lt $connect_disk_num ] ; do
	devName=`umngcli -f $disk_info get disk${disk_num}@diskinfo`
	#echo $devName > $mounton
	#mountname=`cat $mounton | sed 's/sd//'`
	mountname=${devName/sd/}
	validPartition=`umngcli -f $disk_info get validPartition@${devName}`
	if [ -z "$validPartition" ] ; then
		validPartition=0
	fi
	eachValidPartition=0
	while [ $eachValidPartition -lt $validPartition ] ; do
		partition_list=`grep "^/dev/${devName}" $disk_label | cut -d ' ' -f 2 | cut -d '/' -f 4`
		if [ -z "$partition_list" ] ; then
			break
		fi
		for each in $partition_list ; do
			let eachValidPartition=$eachValidPartition+1
			echo "@share${now}"       >> $file
			echo "shEnable=1"         >> $file
			echo "shDisk=${disk_num}" >> $file
			echo "shVol=${eachValidPartition}" >> $file
			echo "shDir=${each}"      >> $file
			partitionname=sd${each}
			real_partitionname=$partitionname
			echo partitionname $partitionname > /dev/console
			if [ "${partitionname:3}" = "1" ] ; then
				part_cnt=`grep -c "/dev/$partitionname" $disk_label`
				if [ "$part_cnt" = "0" ] ; then
					real_partitionname=$partitionname
					echo real partitionname [$real_partitionname] > /dev/console	
				fi	
			fi
			#if [ "$partitionname" = "sdb1" ] ; then
			#	part_cnt=`grep -c /dev/sdb1 $disk_label`
			#	if [ "$part_cnt" = "0" ] ; then
			#		real_partitionname="sdb"
			#		echo real partitionname [$real_partitionname] >> /dev/console	
			#	fi	
			#fi
			shName=`grep "^/dev/${real_partitionname}" $disk_label | cut -d ' ' -f 3 | cut -d '"' -f 2`
			if [ -n "$shName" ] ; then
				echo "shName=$shName" >> $file
			else
				let partition_nolabel_num=$partition_nolabel_num+1
				echo "shName=partition${partition_nolabel_num}" >> $file
			fi
			echo "shPathStr=\\\\"     >> $file

			let now=$now+1
#			if [ $now -eq $NBIOS_RES_PROFILE ] ; then
#				break 3
#			fi
		done
		
	done

	let disk_num=$disk_num+1
done


#while [ $now -lt $NBIOS_RES_PROFILE ] ; do
#	echo "@share$now"     >> $file
#	echo "shEnable=0"     >> $file
#	echo "shDisk=0"       >> $file
#	echo "shVol=0 "       >> $file
#	echo "shName="        >> $file
#	echo "shPathStr=\\\\" >> $file
#	let now=$now+1
#done


echo "@DFT_SECTION" >> $file
echo "!"            >> $file

