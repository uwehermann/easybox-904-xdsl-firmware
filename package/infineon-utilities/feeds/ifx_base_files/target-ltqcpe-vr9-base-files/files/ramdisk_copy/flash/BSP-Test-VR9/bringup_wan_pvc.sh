#!/bin/sh

### Following Assumptions are made in this script


if [ "$1" = "" ]; then
	echo "Example: $0 <Ifcont> <VPI> <VCI> <macaddr>"
        exit 1
fi

/usr/sbin/br2684ctl -b -p 1 -c $1 -e 0 -q UBR -a $2.$3
sleep 3
ifconfig nas$1 hw ether $4
ifconfig nas$1 up
