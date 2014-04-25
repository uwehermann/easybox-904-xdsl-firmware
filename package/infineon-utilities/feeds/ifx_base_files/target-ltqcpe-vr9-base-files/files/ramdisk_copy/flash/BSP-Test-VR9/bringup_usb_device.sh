#!/bin/sh

### Following Assumptions are made in this script

insmod /lib/modules/2.6.20.19/ifxusb_gadget.ko;
sleep 1
insmod /lib/modules/2.6.20.19/g_ether.ko;
sleep 1
brctl addif br0 usb0
sleep 1
ifconfig usb0 up
