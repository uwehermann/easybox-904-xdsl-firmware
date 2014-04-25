#!/bin/sh

### Following Assumptions are made in this script

case "$1" in
0)
    switch_utility RegisterSet 0xCCD 0x01;
    LAN_PORT="1 2 3 4 5"
    WAN_PORT="0"
    ;;
1)
    switch_utility RegisterSet 0xCCD 0x02;
    LAN_PORT="0 2 3 4 5"
    WAN_PORT="1"
    ;;
2)
    switch_utility RegisterSet 0xCCD 0x04;
    LAN_PORT="0 1 3 4 5"
    WAN_PORT="2"
    ;;
3)
    switch_utility RegisterSet 0xCCD 0x08;
    LAN_PORT="0 1 2 4 5"
    WAN_PORT="3"
    ;;
4)
    switch_utility RegisterSet 0xCCD 0x10;
    LAN_PORT="0 1 2 3 5"
    WAN_PORT="4"
    ;;
5)
    switch_utility RegisterSet 0xCCD 0x20;
    LAN_PORT="0 1 2 3 4"
    WAN_PORT="5"
    ;;
*)
    echo "Usage:$0 <portId>"
    exit 1
esac

#Set global switch parameter
switch_utility CfgSet 5 1 2 0 00:00:00:00:00:00
#Create VLAN ID (LAN)
switch_utility VLAN_IdCreate 500 1
#Create VLAN ID (CPU)
switch_utility VLAN_IdCreate 501 1
#Create VLAN ID (WAN)
switch_utility VLAN_IdCreate 502 1
#Port VLAN configuration (LAN)

for lan in $LAN_PORT; do
    switch_utility VLAN_PortCfgSet $lan 500 0 0 3 1 1
done
for cpu in 6; do
    switch_utility VLAN_PortCfgSet $cpu 501 0 0 3 1 1
done
for wan in $WAN_PORT; do
    switch_utility VLAN_PortCfgSet $wan 502 0 0 3 1 1
done
#LAN VLAN membership
# - 40: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in $LAN_PORT 6;do
   switch_utility VLAN_PortMemberAdd 500 $argument 0
done
#CPU VLAN membership
# - 50: VLAN ID
# - 0,1,2,3,6: Port ID
# - 0: Tag base Number Egress disabled
for argument in $LAN_PORT $WAN_PORT 6 ;do
   switch_utility VLAN_PortMemberAdd 501 $argument 0
done
#WAN VLAN membership
# - 10: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in  $WAN_PORT 6;do
   switch_utility VLAN_PortMemberAdd 502 $argument 0
done

exit 0
