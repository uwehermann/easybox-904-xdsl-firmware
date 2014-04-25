#!/bin/sh /etc/rc.common

START=00

storage_dir=/mnt/overlay
o_dirs="/etc /lib /bin /opt /overlay /ramdisk_copy /rom /root /sbin /usr /www"

start()
{
        if [ -n "$CONFIG_UBOOT_CONFIG_OVERLAY" ]; then
                part_no=`cat /proc/mtd | grep overlay | cut -c4- | cut -d: -f1`
                if [ $? -eq 0 ]; then
                        mount -t jffs2 /dev/mtdblock$part_no $storage_dir
                        if [ $? -ne 0 ]; then
                                flash_eraseall -j /dev/mtd$part_no
                                mount -t jffs2 /dev/mtdblock$part_no $storage_dir
                        fi
                        for i in $o_dirs; do
			       mkdir -p /mnt/overlay/$i
                               mount -t mini_fo -o base=$i,sto=/mnt/overlay/$i $i $i
                        done
                else
                        echo "Partition 'overlay' not found..!!"
                fi
        fi
}

