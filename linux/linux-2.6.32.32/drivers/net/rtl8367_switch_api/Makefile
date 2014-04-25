################################################################################
# Makefile for the Realtek network device drivers.
################################################################################
#===============================================================================
# Include files
#===============================================================================
EXTRA_CFLAGS := -I$(srctree)/drivers/net/rtl8367_switch_api

ifeq ($(CONFIG_NET_VENDOR_REALTEK), y)
EXTRA_CFLAGS += -I$(srctree)/drivers/net/rtl8367_switch_api/realtek
EXTRA_CFLAGS += -I$(srctree)/drivers/net/rtl8367_switch_api/realtek/rtl8367rb/switch_api_v1_2_10/include
endif

#===============================================================================
# Definition
#===============================================================================
ifeq ($(CONFIG_NET_VENDOR_REALTEK), y)
#EXTRA_CFLAGS += -DSWITCH_REALTEK_RTL83XX_FAMILY
#EXTRA_CFLAGS += -D_RTK_RTL83XX_SWITCH_API=2
endif

#===============================================================================
# Compile files
#===============================================================================
#obj-y	+= switch_vendor_hal.o

ifeq ($(CONFIG_NET_VENDOR_REALTEK), y)

obj-y += realtek/
endif


