#!/bin/sh /etc/rc.common
#
# Install Voice CPE TAPI subsystem LL driver for VMMC

START=35


bin_dir=/opt/lantiq/bin

start() {
	[ ! -e /dev/ifx_gpio ] && {
		mknod /dev/ifx_gpio c 240 0;
	}
}