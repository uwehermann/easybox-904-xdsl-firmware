################################################################################
# Makefile for the Realtek network device drivers.
################################################################################
#===============================================================================
# Include files
#===============================================================================
EXTRA_CFLAGS := -I$(srctree)/include/linux
EXTRA_CFLAGS += -I$(srctree)/include/linux/byteorder

EXTRA_CFLAGS += -I$(srctree)/include/asm
EXTRA_CFLAGS += -I$(srctree)/include/asm/ifx/vr9
EXTRA_CFLAGS += -I$(srctree)/include/asm-mips

EXTRA_CFLAGS += -I$(srctree)/include/switch_api

EXTRA_CFLAGS += -I$(srctree)/drivers/net/rtl8367_switch_api

ifeq ($(CONFIG_NET_VENDOR_REALTEK),y)
EXTRA_CFLAGS += -I$(srctree)/drivers/net/rtl8367_switch_api/realtek

ifeq ($(CONFIG_RTL8367RB),y)
EXTRA_CFLAGS += -I$(srctree)/drivers/net/rtl8367_switch_api/realtek/rtl8367rb
EXTRA_CFLAGS += -I$(srctree)/include/rtl8367_switch_api/rtl8367rb
endif # CONFIG_RTL8367RB

ifeq ($(CONFIG_RTL8367RB_APIs_v1_2_10),y)
#EXTRA_CFLAGS +=	-I$(srctree)/drivers/net/rtl8367_switch_api/realtek/rtl8367rb/switch_api_v1_2_10
EXTRA_CFLAGS +=	-I$(srctree)/drivers/net/rtl8367_switch_api/realtek/rtl8367rb/switch_api_v1_2_10/include/
endif # CONFIG_RTL8367RB_APIs_v1_2_10

endif # CONFIG_NET_VENDOR_REALTEK

#===============================================================================
# Definition
#===============================================================================
ifeq ($(CONFIG_LANTIQ_MDIO), y)
EXTRA_CFLAGS += -DMDIO_USED
endif

ifeq ($(CONFIG_NET_VENDOR_REALTEK),y)
#EXTRA_CFLAGS += -DSWITCH_REALTEK_RTL83XX_FAMILY

EXTRA_CFLAGS += -DMDC_MDIO_OPERATION
EXTRA_CFLAGS += -DBIG_ENDIAN

EXTRA_CFLAGS += -D__KERNEL__

# If vendor is RealTek
ifeq ($(CONFIG_RTL8367RB),y)
EXTRA_CFLAGS += -DCHIP_RTL8367RB
#EXTRA_CFLAGS += -DDISABLE_VLAN_SHADOW
endif

# If APIs version is V1.2.10
ifeq ($(CONFIG_RTL8367RB_APIs_v1_2_10),y)
EXTRA_CFLAGS += -D_RTK_RTL83XX_SWITCH_API=2
endif # CONFIG_RTL8367RB_APIs_v1_2_10

endif # CONFIG_NET_VENDOR_REALTEK

#===============================================================================
# Compile files
#===============================================================================
obj-y	+= rtl8367rb.o

rtl8367rb-objs	:= rtl83XX_hal_mdio.o
#rtl8367rb-objs	+= rtl83XX_hal.o
#rtl8367rb-objs	+= rtl83XX_hal_utility.o


ifeq ($(CONFIG_RTL8367RB),y)
rtl8367rb-objs	+= ./rtl8367rb/rtl8367_hal_linux.o
rtl8367rb-objs	+= ./rtl8367rb/rtl8367_hal_linux_ioctl.o
endif  # CONFIG_RTL8367RB

ifeq ($(CONFIG_RTL8367RB_APIs_v1_2_10),y)
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtk_api.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_acl.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_cputag.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_dot1x.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_eav.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_eee.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_fc.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_green.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_hsb.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_igmp.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_inbwctrl.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_interrupt.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_led.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_lut.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_meter.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_mib.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_mirror.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_misc.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_phy.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_port.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_portIsolation.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_qos.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_rma.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_scheduling.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_storm.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_svlan.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_trunking.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_unknownMulticast.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/rtl8367b_asicdrv_vlan.o
rtl8367rb-objs	+= ./rtl8367rb/switch_api_v1_2_10/smi.o
endif # CONFIG_RTL8367RB_APIs_v1_2_10


