#!/bin/sh

### Following Assumptions are made in this script

#echo "$1"

if [ "$1" = "" ]; then
	echo " " 
	echo "Example: $0 </ramdisk/usb_test_sda>"
	echo " "
	exit 1
fi

grep "passFileLineCount1" /flash/rc.conf
if [ $? != "0" ] ; then 
   echo "passFileLineCount1=\'ftp:\$1\$\$CoERg7ynjYLsj2j4glJ34.0:0:ftp:$1:/dev/null\'" >> /flash/rc.conf
fi

target=/flash/rc.conf
if [ -s $target ]; then
    /bin/sed -i -e "s,^passFileLineCount1=.*,passFileLineCount1=\'ftp:\$1\$\$CoERg7ynjYLsj2j4glJ34.:0:0:ftp:$1:/dev/null\',g" $target
    /bin/sed -i -e "s,^passFileLineCount2=.*,passFileLineCount2=\'ftp:\$1\$\$CoERg7ynjYLsj2j4glJ34.:0:0:ftp:$1:/dev/null\',g" $target
    /usr/sbin/savecfg.sh
else
    echo "Default config of usb host ftp is not exist!"
    exit 1
fi

target=/ramdisk/flash/passwd
if [ -s $target ]; then
/bin/sed -i -e "s,^passFileLineCount1=.*,passFileLineCount1=\'ftp:\$1\$\$CoERg7ynjYLsj2j4glJ34.:0:0:ftp:$1:/dev/null\',g" $target
/bin/sed -i -e "s,^passFileLineCount2=.*,passFileLineCount2=\'ftp:\$1\$\$CoERg7ynjYLsj2j4glJ34.:0:0:ftp:$1:/dev/null\',g" $target
else
    echo "Default config of usb host ftp is not exist!"
    exit 1
fi

mem -s 0x1f10201c -w 0x02 -u

sleep 1

mkdir $1;
insmod /lib/modules/2.6.20.19/ifxusb_host.ko;
sleep 5
cat /proc/scsi/scsi > /ramdisk/usb_disk_info
grep "Host" /ramdisk/usb_disk_info
while [ $? != "0" ] ; do
      echo "Please plug usb flash disk!!!"
      sleep 1
      cat /proc/scsi/scsi > /ramdisk/usb_disk_info
      grep "Host" /ramdisk/usb_disk_info
done
mount /dev/sda1 $1
exit 0
