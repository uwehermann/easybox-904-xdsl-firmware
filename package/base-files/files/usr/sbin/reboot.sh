#!/bin/sh
#
# $1-delay seconds to reboot
#

remount_rootfs()
{
	/bin/mount -r -o sync,noatime,nodiratim,norelatime,nodev,remount `/usr/sbin/uboot_env --get --name rootfsmtd` /
}

#
# $1-delay seconds to reboot
#
sys_reboot()
{
	/bin/sync

	remount_rootfs
	/usr/sbin/libmapi_mon_cli stop_daemon
	
	if [ -n "$1" ] ; then
		/sbin/reboot -d $1 # -f
	else
		/sbin/reboot -d 2 # -f
	fi
}

sys_reboot $1
