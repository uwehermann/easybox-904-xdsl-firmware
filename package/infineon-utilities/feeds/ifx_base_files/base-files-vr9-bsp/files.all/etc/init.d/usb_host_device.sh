#!/bin/sh /etc/rc.common
#START=70
start() {
	kernel_version=`uname -r`
	if [ $usb0Mode -eq 1 -o $usb1Mode -eq 1 ]; then
	#       Kamal: new usb init method
		insmod /lib/modules/${kernel_version}/ifxusb_host.ko
	elif [ $usb0Mode -eq 2 ]; then
        	/sbin/insmod /lib/modules/${kernel_version}/ifxusb_gadget.ko
	        board_mac=`/usr/sbin/upgrade mac_get 0`
        	/sbin/insmod /lib/modules/${kernel_version}/g_ether.ko dev_addr="$board_mac"
	        /usr/sbin/brctl addif $lan_main_0_interface usb0
        	/sbin/ifconfig usb0 0.0.0.0 up
#	        echo "USB0 in Device Mode"
	else
        	echo "Load USB0/USB1 in Host/Device manually"
	fi
}
