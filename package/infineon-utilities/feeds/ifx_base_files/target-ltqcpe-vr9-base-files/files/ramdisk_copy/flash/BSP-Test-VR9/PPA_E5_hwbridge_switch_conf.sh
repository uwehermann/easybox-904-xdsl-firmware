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
# - argument: PortId 
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
for argument in 6;do
   switch_utility PortCfgSet 6 1 0 0 0 0 0 0 255 0 0
done


#MAC 0 as Ethernet WAN
switch_utility RegisterSet 0xCCD 0x01


# Mirror Portmap
switch_utility RegisterSet 0x453 0x0
# Unknown Unicast/multicast Packet
switch_utility RegisterSet 0x454 0x07f
switch_utility RegisterSet 0x455 0x07f


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

