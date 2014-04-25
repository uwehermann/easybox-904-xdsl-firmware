#!/bin/sh
# Note :
# 	this script decide the attached usb how to share 
# 	this script only run when detect usb attached
# 
# 

# Usage :
# 
# 
# 
# 


# check this drive if in samba history. if yes, load old setting, if no, use default setting

sleep 10
drive_brief.sh

drive_num=`umngcli get drive_num@drive_now`
if [ "${drive_num}" = "0" ] ; then
	umngcli set drive1@samba_sharelist=
	umngcli set drive2@samba_sharelist=
else
	num=${drive_num%%,*}
	drive_num=${drive_num#*,}
	alldrive=`echo ${drive_num} | sed 's/,/ /g'`
	i=1
	for each in ${alldrive}
	do
		drive=`umngcli get driveinfo${each##sd}@drive_now`
		vol=`umngcli get volume${each##sd}@drive_now`
	# now, check samba history to get old setting
	# skip it temporary. daniel  2010/11/23 02:32  
	
		# if not in history
		share_num=${drive##*,}
		share_num=`echo ${share_num} | sed 's/"//g'`
		info=`echo ${drive} | sed 's/"//g'`
		j=0
		while [ $j -lt $share_num ]
		do
			col1=$((4*j+1))
			col2=$((4*j+3))
			label=`echo ${vol} | cut -d , -f ${col2}`
			dir=`echo ${vol} | cut -d , -f ${col1}`
			info=${info},1,"${label}","${dir}",/,0,1,"",""

			j=$((j+1))
		done
		echo ${info}
		umngcli set drive${i}@samba_sharelist="${info}"
		i=$((i+1))
	done

fi

samba2uci.sh
rm /tmp/samba_timestamp

