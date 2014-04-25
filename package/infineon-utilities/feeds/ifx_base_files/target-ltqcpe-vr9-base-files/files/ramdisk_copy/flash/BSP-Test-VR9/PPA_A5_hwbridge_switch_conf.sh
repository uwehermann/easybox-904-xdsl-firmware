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

# Configure Ports.
# - argument: PortId 
# - 1: Enable
# - 0: Unicast unknown drop disabled
# - 0: Multicast unknown drop disabled
# - 0: Reserved packet drop disabled
# - 0: Broadcast packet drop disabled
# - 0: Aging Enabled?
# - 1: Learning Mac Port Lock enable
# - 255: Learning Limitation Disable
# - 0: Port Moniter Mormal Usage
# - 0: Flow Control as Auto
for argument in 0 1 2 3 4 5 7 8 9 11;do
  switch_utility PortCfgSet $argument 1 0 0 0 0 0 1 255 0 0
done


# Configure Ports.
# - 6: PortId 
# - 1: Enable
# - 0: Unicast unknown drop disabled
# - 0: Multicast unknown drop disabled
# - 0: Reserved packet drop disabled
# - 0: Broadcast packet drop disabled
# - 0: Aging Enabled?
# - 0: Learning Mac Port Lock Disable
# - 255: Learning Limitation Disable
# - 0: Port Moniter Mormal Usage
# - 0: Flow Control as Auto
switch_utility PortCfgSet 6 1 0 0 0 0 0 0 255 0 0


#MAC 0 as Ethernet WAN
switch_utility RegisterSet 0xCCD 0x01


# Mirror Portmap
switch_utility RegisterSet 0x453 0x0
# Unknown Unicast/multicast Packet
switch_utility RegisterSet 0x454 0x07f
switch_utility RegisterSet 0x455 0x07f


#Create DSL WAN Per PVC VLAN IDs
# - argument: VLAN ID
# - 1: FID
for argument in 4080 4081 4082 4083 4084 4085 4086 4087 4088 4089 4090 4091 4092 4093 4094;do
   switch_utility VLAN_IdCreate $argument 1
done

#Create Common VLAN ID
# - 50: VLAN ID
# - 1: FID
switch_utility VLAN_IdCreate 50 1 

# LAN/CPU/ETHWAN Port VLAN Configuration 
# - argument: Port ID
# - 50: port VID
# - 0: VLAN Unknow Drop disabled
# - 0: VLAN ReAssign disabled
# - 3: Violation Mode Both
# - 0: Admit All
# - 1: TVM enabled
for argument in 1 2 3 4 5 0 6;do
   switch_utility VLAN_PortCfgSet $argument 50 0 0 3 0 1
done


#DSL WAN Port VLAN configuration
# - 11: Port ID
# - 50: Port VID
# - 0: VLAN Unknow Drop disabled
# - 0: VLAN ReAssign disabled
# - 3: Violation Mode Both
# - 0: Admit All
# - 0: TVM disabled
switch_utility VLAN_PortCfgSet 11 50 0 0 3 0 0


# Common VLAN membership
# - 50: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 6 0 11;do
   switch_utility VLAN_PortMemberAdd 50 $argument 0
done

# DSL PCV0 VLAN membership
# - 4080: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4080 $argument 0
done


# DSL PCV1 VLAN membership
# - 4081: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4081 $argument 0
done

# DSL PCV2 VLAN membership
# - 4082: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4082 $argument 0
done

# DSL PCV3 VLAN membership
# - 4083: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4083 $argument 0
done

# DSL PCV4 VLAN membership
# - 4084 VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4084 $argument 0
done

# DSL PCV5 VLAN membership
# - 4085: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4085 $argument 0
done

# DSL PCV6 VLAN membership
# - 4086: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4086 $argument 0
done

# DSL PCV7 VLAN membership
# - 4087: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4087 $argument 0
done

# DSL PCV8 VLAN membership
# - 4088: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4088 $argument 0
done

# DSL PCV9 VLAN membership
# - 4089: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4089 $argument 0
done

# DSL PCV10 VLAN membership
# - 4090: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4090 $argument 0
done

# DSL PCV11 VLAN membership
# - 4091: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4091 $argument 0
done

# DSL PCV12 VLAN membership
# - 4092: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4092 $argument 0
done

# DSL PCV13 VLAN membership
# - 4093: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4093 $argument 0
done

# DSL PCV14 VLAN membership
# - 4094: VLAN ID
# - argument: Port ID
# - 0: Tag base Number Egress disabled
for argument in 1 2 3 4 5 11;do
   switch_utility VLAN_PortMemberAdd 4094 $argument 0
done

# DSL VLAN membership
# - argument: VLAN ID
# - 6: Port ID
# - 0: Tag base Number Egress disabled
for argument in 4080 4081 4082 4083 4084 4085 4086 4087 4088 4089 4090 4091 4092 4093 4094;do
   switch_utility VLAN_PortMemberAdd $argument 6 1
done


#DSL WAN Port VLAN configuration
# - 11: Port ID
# - 50: Port VID
# - 0: VLAN Unknow Drop disabled
# - 0: VLAN ReAssign disabled
# - 3: Violation Mode Both
# - 0: Admit All
# - 0: TVM disabled
switch_utility VLAN_PortCfgSet 11 50 0 0 3 0 0


# Traffic flow port 10 is forwarded to CPU directly
switch_utility PCE_RuleWrite 0 1 1 10 0 0 0 0 0 0 0 0 00:00:00:00:00:00 0 0 00:00:00:00:00:00 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0x0 0x0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 0 0 0 0 0 0 0 0 0 0 2

# - 11: DSL WAN Port is default router port map
switch_utility MulticastRouterPortAdd 11
# - 0: Ethernet WAN Port is default router port map
switch_utility MulticastRouterPortAdd 0

# Enable Port 11 L3 Multicast Mode
switch_utility RegisterSet 0x4ee 0x2037

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



