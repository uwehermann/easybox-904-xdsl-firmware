#!/bin/sh

/usr/sbin/upgrade /etc/rc.conf.gz sysconfig 0 0
sync; sleep 4
reboot

