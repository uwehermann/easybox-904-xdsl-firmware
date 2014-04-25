#!/bin/sh

### Following Assumptions are made in this script

rm -rf /flash/rc.conf
cp /etc/rc.conf.gz /flash/rc.conf.gz
gzip -d /flash/rc.conf.gz
/usr/sbin/savecfg.sh

exit 0
