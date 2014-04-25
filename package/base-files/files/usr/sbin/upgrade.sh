#!/bin/sh

#
# return
#	0 - successfully
#	1 - file does not existed
#	2 - partition failed
#	3 - firmware format error
#	4 - firmware type error
#

REBOOT=0

BACKUP_FILES="/bin/busybox /usr/sbin/httpd-brn"	# special backup files
BACKUP_DIRS="/bin /usr/bin /usr/sbin"			# size-limited directories
BACKUP_DIRS2="/sbin /lib /usr/lib"				# size-non-limited directories

IMAGE_TYPE="NA"


if [ "$3" == "reboot" ] ; then
	REBOOT=1
fi

BOOT_FROM_NAND=0

if [ `grep -c "ifx_nand" /proc/cmdline` -ge 1 ] ; then
	BOOT_FROM_NAND=1
fi

DUAL_IMAGE=0

if [ `grep -c "CONFIG_UBOOT_CONFIG_DUAL_IMAGE" /etc/config.sh` -ge 1 ] ; then
	DUAL_IMAGE=1
fi

BACKUP_ROOTFS=0

remount_rootfs()
{
	mount -r -o sync,noatime,nodiratim,norelatime,nodev,remount `uboot_env --get --name rootfsmtd` /
}

make_backup_rootfs()
{
	local	DIR
	local	FILE

	echo 1 > /proc/sys/vm/drop_caches

	for DIR in $BACKUP_DIRS $BACKUP_DIRS2 ; do
		mkdir -p /tmp${DIR}
	done

	for DIR in $BACKUP_DIRS ; do
		FILES=`ls -lA $DIR | awk '{ if ( $5 > 0 && $5 < 40960 ) print $9 }'`
		for FILE in $FILES ; do
			cp -a "${DIR}/${FILE}" "/tmp${DIR}/${FILE}"
		done
	done

	for DIR in $BACKUP_DIRS2 ; do
		FILES=`ls -lA $DIR | awk '{ if ( $5 > 0 ) print $9 }'`
		for FILE in $FILES ; do
			cp -a "${DIR}/${FILE}" "/tmp${DIR}/${FILE}"
		done
	done

	for FILE in $BACKUP_FILES ; do
		cp -a "$FILE" "/tmp${FILE}"
	done
}

remove_backup_rootfs()
{
	for DIR in $BACKUP_DIRS $BACKUP_DIRS2 ; do
		rm -rf $DIR
	done
}

sys_reboot()
{
	reboot.sh
}

# $1: partition name
# $2: 1-jffs2
partition_erase()
{
	local DEVNAME

	if [ -z "$1" ] ; then
		return 0
	fi

	DEVNAME=`grep -w \"$1\" /proc/mtd | awk 'BEGIN { FS=":" } { print $1 }'`

	if [ -z "$DEVNAME" ] ; then
		return 1
	fi

	if [ "$2" == "1" ] ; then
		flash_eraseall -j /dev/$DEVNAME
	else
		flash_eraseall /dev/$DEVNAME
	fi

	if [ "$?" != 0 ] ; then
		echo ERROR! flash_eraseall failed
		return 1
	fi
	
	return 0
}

# $1: partition name
# $2: file name
partition_write()
{
	local DEVNAME

	if [ -z "$2" ] || ! [ -f "$2" ] ; then
		return 0
	fi

	DEVNAME=`grep -w \"$1\" /proc/mtd | awk 'BEGIN { FS=":" } { print $1 }'`

	if [ -z "$DEVNAME" ] ; then
		return 1
	fi

	nandwrite -m /dev/$DEVNAME $2
	
	if [ "$?" != 0 ] ; then
		echo ERROR! nandwrite failed
		return 1
	fi

	return 0
}

# $1 - file name
# $2 - optional, expected type
# $? - 0-allowed image, 1-unknown image, 2-no file
# output - $IMAGE_TYPE is set as the image type name, or "NA" if unknown
image_type()
{
	IMAGE_TYPE=NA

	if [ -z "$1" ] || ! [ -f "$1" ] ; then
		return 2
	fi

	if [ -z "$2" ] || [ "$2" == "full" ] ; then
		if [ `tail -c 16 $1 | grep -c "UEfullFW"` -eq 1 ] ; then
			IMAGE_TYPE=fullUEN
			return 0
		fi
		if [ `tail -c 32 $1 | hexdump -c | awk '{ if ( $2 != "" ) print $2$3$4$5$6$7$8$9$10$11$12$13$14$15$16$17 }' | grep -c -e arcadyan -e ARCADYAN` -eq 2 ] ; then
			IMAGE_TYPE=full
			return 0
		fi
		if [ `hexdump -C -s 32 -n 16 $1 | grep -c 'VR9 Fullimage'` -ge 1 ] ; then
			IMAGE_TYPE=full
			return 0
		fi
	fi

	if [ -z "$2" ] || [ "$2" == "rootfs" ] ; then
		if  [ `hexdump -C -s 32 -n 16 $1 | grep -c 'LTQCPE RootFS'` -ge 1 ] ||
			[ `hexdump -C -s 64 -n  2 $1 | grep -c '19 85'` -ge 1 ] ; then
			IMAGE_TYPE=rootfs
			return 0
		fi
	fi

	if [ -z "$2" ] || [ "$2" == "kernel" ] ; then
		if [ `hexdump -C -s 32 -n 16 $1 | grep -c 'MIPS LTQCPE Linu'` -ge 1 ] ; then
			IMAGE_TYPE=kernel
			return 0
		fi
	fi

	if [ -z "$2" ] || [ "$2" == "module" ] ; then
		if [ `hexdump -C -s 0 -n 4 $1 | grep -c '1f 8b 08 00'` -ge 1 ] ; then
			IMAGE_TYPE=module
			return 0
		fi
	fi

	if [ -z "$2" ] || [ "$2" == "uboot" ] ; then
		if [ $BOOT_FROM_NAND == 1 ] ; then
			if [ `hexdump -C -n 4 $1        | grep -c '22 0c dd f3'` -ge 1 ] &&
			   [ `hexdump -C -s 512 -n 4 $1 | grep -c '10 00 00 0b'` -ge 1 ] ; then
				IMAGE_TYPE=uboot
				return 0
			fi
		else
			if [ `hexdump -C -n 4 $1 | grep -c '10 00 00 0b'` -ge 1 ] ; then
				IMAGE_TYPE=uboot
				return 0
			fi
		fi
	fi

	return 1
}

# $1 - file name
upgrade_uboot()
{
	FILE=$1

	if [ -z "$FILE" ] || ! [ -f "$FILE" ] ; then
		return 1
	fi

	image_type "$FILE" uboot
	if [ $? -ne 0 ] ; then
			rm -f $FILE
			echo wrong u-boot image
			return 3
		fi
		arc_led.sh upgrade on

	if [ $BOOT_FROM_NAND == 1 ] ; then
		partition_erase "uboot"
		if [ "$?" != 0 ] ; then
			rm -f ${FILE}
			arc_led.sh upgrade off
			return 2
		fi
		partition_write "uboot" $FILE
		if [ "$?" != 0 ] ; then
			rm -f ${FILE}
			arc_led.sh upgrade off
			return 2
		fi
		rm -f $1
	else
		upgrade $FILE uboot 0 0
	fi

	arc_led.sh upgrade off

	if [ $REBOOT -eq 1 ] ; then
		sys_reboot
	fi

	return 0
}

# $1 - file name
# $2 - image ID, 1: primary, 2: secondary
upgrade_kernel()
{
	FILE=$1

	if [ -z "$FILE" ] || ! [ -f "$FILE" ] ; then
		return 1
	fi

	if [ $DUAL_IMAGE != 1 ] && [ "$2" == "2" ] ; then
		return 2
	fi

	if [ "$2" == "2" ] ; then
		PART=kernel2
	else
		PART=kernel
	fi

	image_type "$FILE" kernel
	if [ $? -ne 0 ] ; then
		rm -f $FILE
		echo wrong kernel image
		return 3
	fi

	arc_led.sh upgrade on

	if [ $BOOT_FROM_NAND == 1 ] ; then
		partition_erase $PART
		if [ "$?" != 0 ] ; then
			rm -f ${FILE}
			arc_led.sh upgrade off
			return 2
		fi
		partition_write $PART $FILE
		if [ "$?" != 0 ] ; then
			rm -f ${FILE}
			arc_led.sh upgrade off
			return 2
		fi
		rm -f $FILE
	else
		upgrade $FILE $PART 0 1
	fi

	arc_led.sh upgrade off

	if [ $REBOOT -eq 1 ] ; then
		sys_reboot
	fi

	return 0
}

# $1 - file name
# $2 - image ID, 1: primary, 2: secondary
upgrade_rootfs()
{
	FILE=$1

	if [ -z "$FILE" ] || ! [ -f "$FILE" ] ; then
		return 1
	fi

	if [ $DUAL_IMAGE != 1 ] && [ "$2" == "2" ] ; then
		return 2
	fi

	if [ "$2" == "2" ] ; then
		PART=rootfs2
	else
		PART=rootfs
	fi

	image_type "$FILE" rootfs
	if [ $? -ne 0 ] ; then
		rm -f $FILE
		echo wrong rootfs image
		return 3
	fi

	if [ "$BACKUP_ROOTFS" == "1" ]; then
		make_backup_rootfs
		remount_rootfs
	else
	    echo 1 > /proc/sys/vm/drop_caches
	fi

	arc_led.sh upgrade on

	echo 0 0 0 0 > /proc/sys/kernel/printk

	if [ `hexdump -C -n 2 -s 64 $FILE | grep -c "19 85"` == "1" ] ; then
		partition_erase $PART 1
	else
		partition_erase $PART 0
	fi
	if [ "$?" != 0 ] ; then
		if [ "$BACKUP_ROOTFS" == "1" ]; then
			remove_backup_rootfs
		fi
		arc_led.sh upgrade off
		return 2
	fi

	if [ $BOOT_FROM_NAND == 1 ] ; then
		FILESIZE=`ls -l $FILE | awk '{ print $5 }'`
		let FILESIZE=$FILESIZE-64
		tail -c $FILESIZE $FILE > ${FILE}.tmp
		rm -f $FILE
		partition_write $PART ${FILE}.tmp
		if [ "$?" != 0 ] ; then
			rm -f ${FILE}.tmp
			arc_led.sh upgrade off
			return 2
		fi
		rm -f ${FILE}.tmp
	else
		upgrade $FILE $PART 0 1
	fi

	arc_led.sh upgrade off

	if [ $REBOOT -eq 1 ] ; then
		sys_reboot
	fi

	return 0
}

# $1 - file name
upgrade_module()
{
	local	IS_HTTPD

	FILE=$1

	if [ -z "$FILE" ] || ! [ -f "$FILE" ] ; then
		return 1
	fi

	image_type "$FILE" module
	if [ $? -ne 0 ] ; then
		rm -f $FILE
		echo wrong module ipk image
		return 3
	fi

	mv -f $FILE ${FILE}.ipk

	arc_led.sh upgrade on

	IS_HTTPD=0
	NEED_COMMIT=0

	mod_names=`opkg -test install ${FILE}.ipk | grep Installing | awk '{ print $FILE }'`

	for mod_name in $mod_names ; do
		if [ "${mod_name:0:9}" == "httpd-brn" ] ; then
			IS_HTTPD=1
			/etc/init.d/httpd stop
			rm /tmp/*.img 2>&-
			break
		fi
	done

	for mod_name in $mod_names ; do
		if [ "$mod_name" != "busybox" ] ; then
			opkg -force-depends remove $mod_name
		fi
	done

	for mod_name in $mod_names ; do
		if [ `opkg files $mod_name | grep -c /etc/config` -ge 1 ] ; then
			NEED_COMMIT=1
			break
		fi
	done

	opkg install -force-reinstall -force-overwrite -force-downgrade -force-depends ${FILE}.ipk

	sync

	arc_led.sh upgrade off

	if [ $NEED_COMMIT == 1 ] ; then
		umngcli commitcfg
	fi

	if [ $REBOOT == 1 ] ; then
		sys_reboot
	fi

	rm -f ${FILE}.ipk
	rm -f /tmp/opkg-*

	if [ $IS_HTTPD == 1 ] ; then
		/etc/init.d/httpd start
	fi

	return 0
}

# $1 - file name
upgrade_fullimage_legacy() # LANTIQ's
{
	FILE=$1

	if [ "$2" == "2" ] ; then
		PART=full2
	else
		PART=full
	fi

	image_type "$FILE" full
	if [ $? -ne 0 ] ; then
		rm -f $FILE
		echo wrong full image
		return 3
	fi

	if [ "$BACKUP_ROOTFS" == "1" ]; then
		make_backup_rootfs
		remount_rootfs
	else
		echo 1 > /proc/sys/vm/drop_caches
	fi

	arc_led.sh upgrade on

	upgrade $FILE $PART 0 1

	arc_led.sh upgrade off

	sys_reboot

	return 0
}

# $1 - file name
# $2 - image ID, 1: primary, 2: secondary
upgrade_fullimage_nor_or_nand()
{
	FILE=$1

	image_type "$FILE" full
	if [ $? -ne 0 ] ; then
		rm -f $FILE
		echo wrong full image
		return 3
	fi

	HEAD_FILE=/tmp/fullimage.header.$$
	KERNEL_FILE=$FILE.kernel
	ROOTFS_FILE=$FILE.rootfs

	tail -c 32 $FILE | hexdump -c | awk '{ if ( $2 != "" ) print $2$3$4$5$6$7$8$9"\n"$10$11$12$13$14$15$16$17 }' > $HEAD_FILE

	split -l 1 -a 1 $HEAD_FILE $HEAD_FILE.
	rm -f $HEAD_FILE

	ROOTFS_SZ=0x`cat $HEAD_FILE.b`
	ROOTFS_SZ=`printf "%u" $ROOTFS_SZ`
	KERNEL_SZ=0x`cat $HEAD_FILE.c`
	KERNEL_SZ=`printf "%u" $KERNEL_SZ`

	rm -f $HEAD_FILE.?

# modified by htf
        tail -c $(( $KERNEL_SZ + 32 )) $FILE > $KERNEL_FILE
        truncate -s $KERNEL_SZ $KERNEL_FILE
        truncate -s $ROOTFS_SZ $FILE
        mv  $FILE $ROOTFS_FILE

# Code from ammon
#	if [ $ROOTFS_SZ -ge $KERNEL_SZ ] ; then
#		dd if="$FILE" of="$ROOTFS_FILE" bs=$ROOTFS_SZ count=1
#		dd if="$FILE" of="$KERNEL_FILE" bs=$ROOTFS_SZ count=1 skip=1
#	else
#		dd if="$FILE" of="$ROOTFS_FILE" bs=$ROOTFS_SZ count=1
#		let CNT=$KERNEL_SZ/$ROOTFS_SZ+1
#		dd if="$FILE" of="$KERNEL_FILE" bs=$ROOTFS_SZ count=$CNT skip=1
#	fi
#
#	dd if="$KERNEL_FILE" of="${KERNEL_FILE}2" bs=$KERNEL_SZ count=1
#	mv -f "${KERNEL_FILE}2" "$KERNEL_FILE"
#
#	rm -f $FILE

	if  ! [ -f "$ROOTFS_FILE" ] || ! [ -f "$KERNEL_FILE" ] ; then
		rm -f "$KERNEL_FILE"
		rm -f "$ROOTFS_FILE"
		echo wrong full image
		return 3
	fi

	NEW_ROOTFS_SZ=`wc -c "$ROOTFS_FILE" | cut -d ' ' -f 1`
	NEW_KERNEL_SZ=`wc -c "$KERNEL_FILE" | cut -d ' ' -f 1`

	if  [ "$NEW_ROOTFS_SZ" != $ROOTFS_SZ ] || [ "$NEW_KERNEL_SZ" != $KERNEL_SZ ] ; then
		rm -f "$KERNEL_FILE"
		rm -f "$ROOTFS_FILE"
		echo system memory error
		return 5
	fi

	OLD=$REBOOT
	REBOOT=0
	if [ "$2" != "2" ] ; then
		upgrade_kernel "$KERNEL_FILE" 1
		RET=$?
		if [ "$RET" != 0 ] ; then
			rm -f "$KERNEL_FILE"
			rm -f "$ROOTFS_FILE"
			return $RET
		fi
		upgrade_rootfs "$ROOTFS_FILE" 1
		RET=$?
	else
		upgrade_kernel "$KERNEL_FILE" 2
		RET=$?
		if [ "$RET" != 0 ] ; then
			rm -f "$KERNEL_FILE"
			rm -f "$ROOTFS_FILE"
			return $RET
		fi
		upgrade_rootfs "$ROOTFS_FILE" 2
		RET=$?
	fi

	rm -f "$KERNEL_FILE"
	rm -f "$ROOTFS_FILE"

	REBOOT=$OLD

	return $RET
}

# $1 - file name
# $2 - image ID, 1: primary, 2: secondary
upgrade_fullimage()
{
	if [ -z "$1" ] || ! [ -f "$1" ] ; then
		return 1
	fi

	if [ $DUAL_IMAGE != 1 ] && [ "$2" == "2" ] ; then
		return 2
	fi

	upgrade_fullimage_nor_or_nand "$1" $2
	# upgrade_fullimage_legacy "$1" $2

	return $?
}

EVENT_CLI=/usr/sbin/libmapi_event_cli
FW_UPGRADE_NOTE_FILE=/tmp/in_fw_upgrade

if [ -n "$2" ] ; then
	case $1 in
	  "uboot")
			touch $FW_UPGRADE_NOTE_FILE
			[ -x "$EVENT_CLI" ] && $EVENT_CLI add upgrade.sh FWUPGRD 1 uboot
			upgrade_uboot "$2"
			exit $?
			;;
	  "kernel")
			touch $FW_UPGRADE_NOTE_FILE
			[ -x "$EVENT_CLI" ] && $EVENT_CLI add upgrade.sh FWUPGRD 1 kernel
			upgrade_kernel "$2" 1
			exit $?
			;;
	  "kernel2")
			if [ $DUAL_IMAGE != 1 ] ; then
				break
			fi
			touch $FW_UPGRADE_NOTE_FILE
			[ -x "$EVENT_CLI" ] && $EVENT_CLI add upgrade.sh FWUPGRD 1 kernel
			upgrade_kernel "$2" 2
			exit $?
			;;
	  "rootfs")
			touch $FW_UPGRADE_NOTE_FILE
			[ -x "$EVENT_CLI" ] && $EVENT_CLI add upgrade.sh FWUPGRD 1 rootfs
			upgrade_rootfs "$2" 1
			exit $?
			;;
	  "rootfs2")
			if [ $DUAL_IMAGE != 1 ] ; then
				break
			fi
			touch $FW_UPGRADE_NOTE_FILE
			[ -x "$EVENT_CLI" ] && $EVENT_CLI add upgrade.sh FWUPGRD 1 rootfs
			upgrade_rootfs "$2" 2
			exit $?
			;;
	  "module")
			upgrade_module "$2"
			exit $?
			;;
	  "full")
			touch $FW_UPGRADE_NOTE_FILE
			[ -x "$EVENT_CLI" ] && $EVENT_CLI add upgrade.sh FWUPGRD 1 fullimage
			upgrade_fullimage "$2" 1
			RET=$?
			if [ $RET == 0 ] && [ $REBOOT -eq 1 ] ; then
				sys_reboot
			fi
			exit $RET
			;;
	  "full2")
			if [ $DUAL_IMAGE != 1 ] ; then
				break
			fi
			touch $FW_UPGRADE_NOTE_FILE
			[ -x "$EVENT_CLI" ] && $EVENT_CLI add upgrade.sh FWUPGRD 1 fullimage
			upgrade_fullimage "$2" 2
			RET=$?
			if [ $RET == 0 ] && [ $REBOOT -eq 1 ] ; then
				sys_reboot
			fi
			exit $RET
			;;
	  "fullN")
	        touch $FW_UPGRADE_NOTE_FILE
			[ -x "$EVENT_CLI" ] && $EVENT_CLI add upgrade.sh FWUPGRD 1 fullimage
			if [ $DUAL_IMAGE != 1 ] || [ "`uboot_env --get --name bootid`" == "2" ] ; then
				upgrade_fullimage "$2" 1
				RET=$?
				if [ $RET == 0 ] && [ $DUAL_IMAGE == 1 ] ; then
					uboot_env --set --name bootid --value ""
					if [ $REBOOT -eq 1 ] ; then
						sys_reboot
					fi
				fi
			else
				upgrade_fullimage "$2" 2
				RET=$?
				if [ $RET == 0 ] ; then
					uboot_env --set --name bootid --value "2"
					if [ $REBOOT -eq 1 ] ; then
						sys_reboot
					fi
				fi
			fi
			exit $RET
			;;
	  "type")
			image_type "$2"
			RET=$?
			echo $IMAGE_TYPE
			exit $RET
			;;
	esac
fi

echo $0 'uboot   <file> [ reboot ] - upgrade U-Boot'
echo $0 'kernel  <file> [ reboot ] - upgrade Linux kernel'
echo $0 'rootfs  <file> [ reboot ] - upgrade root file system'
echo $0 'module  <file> [ reboot ] - upgrade module'
echo $0 'full    <file>            - upgrade full image'
echo $0 'type    <file>            - detect file type'
if [ $DUAL_IMAGE == 1 ] ; then
	echo $0 'kernel2 <file> [ reboot ] - upgrade second Linux kernel'
	echo $0 'rootfs2 <file> [ reboot ] - upgrade second root file system'
	echo $0 'full2   <file>            - upgrade second full image'
	echo $0 'fullN   <file>            - upgrade full image, next partition'
fi
echo $0 'type    <file>            - check the image type of file'

exit 4
