#!/bin/sh /etc/rc.common

#START=25

start() {
	kernel_version=`uname -r`
	/sbin/insmod /lib/modules/${kernel_version}/ifx_nfext_core.ko
	/sbin/insmod /lib/modules/${kernel_version}/ifx_nfext_ppp.ko
}
