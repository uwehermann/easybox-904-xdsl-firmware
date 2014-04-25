#!/bin/sh


##################################################################
# Basic Configuration
# - VLAN aware = 1
# 
##################################################################



#Enable the switch
switch_utility Enable
 

#Set global switch parameter
# Parameter:
# - 2: 10 second MAC table aging timer
# - 1: VLAN Aware
# - 0: Packet Length 1522 bytes
# - 0: Alternative Pause Mac Mode disabled
# - 00:00:00:00:00:00: Alternative Pause Mac Address
switch_utility CfgSet 5 1 2 0 00:00:00:00:00:00

#MAC 0 as Ethernet WAN
switch_utility RegisterSet 0xCCD 0x01

#Create switch internal LAN VLAN ID
# - 40: VLAN ID
# - 1: FID
switch_utility VLAN_IdCreate 40 1
 
#Create switch internal CPU VLAN ID
# - 50: VLAN ID
# - 1: FID
switch_utility VLAN_IdCreate 50 1
 
#Create switch internal WAN VLAN IDs
# - 10: VLAN ID
# - 1: FID
switch_utility VLAN_IdCreate 10 1
 

# - argument: Port ID
# - 40: LAN Port VID
# - 0: VLAN Unknow Drop disabled
# - 0: VLAN ReAssign disabled
# - 3: Violation Mode Both
# - 0: Admit All
# - 1: TVM enabled
for argument in 1 2 3 4 5;do
   switch_utility VLAN_PortCfgSet $argument 40 0 0 3 0 1
done
 
#CPU Port VLAN configuration
# - 6: Port ID
# - 50: CPU Port VID
# - 0: VLAN Unknow Drop disabled
# - 0: VLAN ReAssign disabled
# - 3: Violation Mode Both
# - 0: Admit All
# - 1: TVM enabled
switch_utility VLAN_PortCfgSet 6 50 0 0 3 0 1
 

#DSL WAN Port VLAN configuration
# - 11: Port ID
# - 10: DSL WAN Port VID
# - 0: VLAN Unknow Drop disabled
# - 0: VLAN ReAssign disabled
# - 3: Violation Mode Both
# - 0: Admit All
# - 1: TVM enabled
switch_utility VLAN_PortCfgSet 11 10 0 0 3 0 1

#Ethernet WAN Port VLAN configuration
# - 0: Port ID
# - 10: Ethernet WAN Port VID
# - 0: VLAN Unknow Drop disabled
# - 0: VLAN ReAssign disabled
# - 3: Violation Mode Both
# - 0: Admit UnTagged
# - 1: TVM enabled
switch_utility VLAN_PortCfgSet 0 10 0 0 3 0 1


#LAN VLAN membership
# - 40: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 6;do
   switch_utility VLAN_PortMemberAdd 40 $argument 0
done
 
# WAN VLAN membership
# - 10: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 0 6 11;do
   switch_utility VLAN_PortMemberAdd 10 $argument 0
done
 
# CPU VLAN membership
# - 50: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 0 1 2 3 4 5 6 11;do
   switch_utility VLAN_PortMemberAdd 50 $argument 0
done

# Mirror Portmap
switch_utility RegisterSet 0x453 0x0
# Unknown Unicast/multicast Packet
switch_utility RegisterSet 0x454 0x07f
switch_utility RegisterSet 0x455 0x07f


# - 6: CPU Port is default router port map
switch_utility MulticastRouterPortAdd 6
# - 0: Ethernet WAN Port is default router port map
switch_utility MulticastRouterPortAdd 0

# Enable Multicast snooping function
# More comment should be added on

# - 1: Enable Auto Learning
# - 0: IGMPv3 Disable
# - 0: Cross VLAN Disable
# - 0: Default Port Forward 
# - 0: Port ID not used
# - 0: Class of Services 
# - 3: Robust
# - 0x64: Query interval
# - 0: Both Join & Report
# - 1: Fast Leave
# - 1: Learning Router
switch_utility MulticastSnoopCfgSet 1 0 0 0 0 0 3 0x64 0 1 1
