#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(TOPDIR)/.config
include $(INCLUDE_DIR)/kernel.mk
# re-define PATCH_DIR because kernel.mk will set it to kernel patches
PATCH_DIR:=./patches

PKG_NAME:=ifx-dsl-vr9-firmware-xdsl
PKG_VERSION_BETA_A:=05.03.00.08.01.06_05.03.00.0F.01.01
PKG_VERSION_BETA_A_1:=05.03.00.0E.01.06_05.03.01.0C.01.01
PKG_VERSION_BETA_A_2:=05.03.00.0F.01.06_05.03.02.01.01.01
PKG_VERSION_BETA_A_3:=05.03.01.02.01.06_05.03.02.06.01.01
PKG_VERSION_BETA_A_4:=05.03.01.05.01.06_05.03.02.0B.01.01
PKG_VERSION_BETA_A_5:=05.03.01.0A.01.06_05.03.03.00.01.01
PKG_VERSION_BETA_A_6:=05.03.02.02.01.06_05.03.03.0B.01.01
PKG_VERSION_BETA_A_7:=05.03.02.08.01.06_05.03.03.0B.01.01
PKG_VERSION_BETA_B:=05.03.01.05.01.06_05.03.02.08.01.02
PKG_VERSION_BETA_B_2:=05.03.01.0A.01.06_05.03.02.08.01.02
PKG_VERSION_BETA_B_3:=VR9_531C16_533312
PKG_VERSION_BETA_B_4:=05.03.02.01.00.06_05.03.03.0D.00.02
PKG_VERSION_BETA_B_5:=05.03.02.01.00.06_05.03.05.01.01.02
PKG_VERSION_BETA_B_6:=05.03.03.08.01.06_05.03.05.04.01.02
PKG_VERSION_B_1:=05.03.04.0A.01.06_05.03.05.0B.01.02
PKG_VERSION_B_2:=05.04.08.00.00.06_05.04.04.05.01.02
PKG_VERSION_B_3:=05.03.04.0A.01.06_05.03.06.04.01.02
PKG_VERSION_B_4:=05.05.00.0A.01.06_05.05.01.00.01.02
PKG_VERSION_B_5:=05.05.00.0E.00.06_05.05.01.00.01.02
PKG_VERSION_B_6:=05.05.01.06.01.06_05.05.01.07.01.02
PKG_VERSION_B_7:=05.05.00.0E.00.06_05.05.01.08.01.02
PKG_VERSION_B_8:=05.05.01.06.01.06_05.05.01.0A.01.02
#PKG_VERSION_B_9:=05.05.01.06.01.06_05.05.03.03.01.02
PKG_VERSION_B_9:=05.05.01.07.01.06_05.05.03.03.01.02

#Although we didn't use PKG_VERSION during compile.
#We setup this variable to force top-level Make can process compile.
#so...don't remove these.
ifdef CONFIG_FW_ARC_CO_COMPATIBLE_SUPPORT
PKG_VERSION:=1
else
PKG_VERSION:=2
endif

PKG_BASE_NAME:=dsl_vr9_firmware_xdsl
#PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
ifdef CONFIG_FW_XDSL_ANNEX_B_1
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION_B_1).tar.gz
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_2
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION_B_2).tar.gz
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_3
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION_B_3).tar.gz
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_4
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION_B_4).tar.gz
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_5
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION_B_5).tar.gz
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_6
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION_B_6).tar.gz
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_7
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION_B_7).tar.gz
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_8
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION_B_8).tar.gz
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_9
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION_B_9).tar.gz
endif
ifdef CONFIG_FW_ARC_CO_COMPATIBLE_SUPPORT
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION_B_3).tar.gz
endif
#PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
#PKG_UNPACK:=gzip -dc $(DL_DIR)/$(PKG_SOURCE) | $(TAR) -C $(PKG_BUILD_DIR) $(TAR_OPTIONS)
PKG_SOURCE_URL:=http://localhost
PKG_TARGET_BASE_NAME:=xcpe_hw.bin
OPKG_EXTRA_FLAG=--force-downgrade

ifdef CONFIG_FW_ARC_CO_COMPATIBLE_SUPPORT
PKG_VERSION:=1
else
PKG_VERSION:=2
endif

# include $(INCLUDE_DIR)/package-version-override.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

#FIRMWARE_DIR:=/lib/firmware/$(LINUX_VERSION)
FIRMWARE_DIR:=/lib/firmware/

ifeq ($(DUMP),)
  $(info "FIRMWARE_DIR = $(FIRMWARE_DIR)")
   STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell ( \
      grep '^CONFIG_UBOOT_CONFIG_FIRMWARE_IN_ROOTFS' $(TOPDIR)/.config; \
      grep '^CONFIG_$(PKG_NAME)' $(TOPDIR)/.config) | md5s)
endif

define Package/$(PKG_NAME)
	SUBMENU:=Firmware
	SECTION:=firmware
	CATEGORY:=Lantiq
	TITLE:=VR9 CPE xDSL firmware
	URL:=http://www.lantiq.com/
	MAINTAINER:=Lantiq Deutschland GmbH
	DEPENDS:=+ifx-dsl-cpe-mei-vrx
	MENU:=1
endef

define Package/$(PKG_NAME)/description
	VRX200 CPE xDSL firmware
endef

define Package/$(PKG_NAME)/config
	choice
		depends on PACKAGE_$(PKG_NAME)
		prompt "xDSL firmware type"
		default FW_XDSL_BETA_ANNEX_B_2
	config FW_XDSL_BETA_ANNEX_A
		bool "Beta VDSL + ADSL Annex A"
	config FW_XDSL_BETA_ANNEX_A_1
		bool "Beta VDSL + ADSL Annex A, 05.03.00.0E.01.06_05.03.01.0C.01.01"
	config FW_XDSL_BETA_ANNEX_A_2
		bool "Beta VDSL + ADSL Annex A, 05.03.00.0F.01.06_05.03.02.01.01.01"
	config FW_XDSL_BETA_ANNEX_A_3
		bool "Beta VDSL + ADSL Annex A, 05.03.01.02.01.06_05.03.02.06.01.01"
	config FW_XDSL_BETA_ANNEX_A_4
		bool "Beta VDSL + ADSL Annex A, 05.03.01.05.01.06_05.03.02.0B.01.01"
	config FW_XDSL_BETA_ANNEX_A_5
		bool "Beta VDSL + ADSL Annex A, 05.03.01.0A.01.06_05.03.03.00.01.01"
	config FW_XDSL_BETA_ANNEX_A_6
		bool "Beta VDSL + ADSL Annex A, 05.03.02.02.01.06_05.03.03.0B.01.01"
	config FW_XDSL_BETA_ANNEX_A_7
		bool "Beta VDSL + ADSL Annex A, 05.03.02.08.01.06_05.03.03.0B.01.01"
	config FW_XDSL_BETA_ANNEX_B
		bool "Beta VDSL + ADSL Annex B, 05.03.01.05.01.06_05.03.02.08.01.02"
	config FW_XDSL_BETA_ANNEX_B_2
		bool "Beta VDSL + ADSL Annex B, 05.03.01.0A.01.06_05.03.02.08.01.02"
	config FW_XDSL_BETA_ANNEX_B_3
		bool "Beta VDSL + ADSL Annex B, 05.03.01.0C.01.06_05.03.03.03.01.02"
	config FW_XDSL_BETA_ANNEX_B_4
		bool "Beta VDSL + ADSL Annex B, 05.03.02.01.00.06_05.03.03.0D.00.02"
	config FW_XDSL_BETA_ANNEX_B_5
		bool "Beta VDSL + ADSL Annex B, 05.03.02.01.00.06_05.03.05.01.01.02"
	config FW_XDSL_BETA_ANNEX_B_6
		bool "Beta VDSL + ADSL Annex B, 05.03.03.08.01.06_05.03.05.04.01.02"
	config FW_XDSL_ANNEX_B_1
		bool "VDSL + ADSL Annex B, 05.03.04.0A.01.06_05.03.05.0B.01.02"
	config FW_XDSL_ANNEX_J
		depends FW_XDSL_ANNEX_B_1
		bool "VDSL + ADSL Annex B+J, 05.03.04.0A.01.06_05.03.05.0B.01.02"
	config FW_XDSL_ANNEX_B_2
		bool "VDSL + ADSL Annex B, 05.04.08.00.00.06_05.04.04.05.01.02"		
	config FW_XDSL_ANNEX_J
		depends FW_XDSL_ANNEX_B_2
		bool "VDSL + ADSL Annex B+J, 05.04.08.00.00.06_05.04.04.05.01.02"
	config FW_XDSL_ANNEX_B_3
		bool "VDSL + ADSL Annex B, 05.03.04.0A.01.06_05.03.06.04.01.02"		
	config FW_XDSL_ANNEX_J
		depends FW_XDSL_ANNEX_B_3
		bool "VDSL + ADSL Annex B+J, 05.05.00.0C.00.06_05.05.01.0.00.01"
	config FW_XDSL_ANNEX_B_4
		bool "VDSL + ADSL Annex B, 05.05.00.0A.01.06_05.05.01.00.01.02"		
	config FW_XDSL_ANNEX_J
		depends FW_XDSL_ANNEX_B_4
		bool "VDSL + ADSL Annex B+J, 05.05.00.0A.01.06_05.05.01.00.01.02"
	config FW_XDSL_ANNEX_B_5
		bool "VDSL + ADSL Annex B, 05.05.00.0E.00.06_05.05.01.00.01.02"		
	config FW_XDSL_ANNEX_J
		depends FW_XDSL_ANNEX_B_5
		bool "VDSL + ADSL Annex B+J, 05.05.00.0E.00.06_05.05.01.00.01.02"
	config FW_XDSL_ANNEX_B_6
		bool "VDSL + ADSL Annex B, 05.05.01.06.01.06_05.05.01.07.01.02"		
	config FW_XDSL_ANNEX_J
		depends FW_XDSL_ANNEX_B_6
		bool "VDSL + ADSL Annex B+J, 05.05.01.06.01.06_05.05.01.07.01.02"									
	config FW_XDSL_ANNEX_B_7
		bool "VDSL + ADSL Annex B, 05.05.00.0E.00.06_05.05.01.08.01.02"		
	config FW_XDSL_ANNEX_J
		depends FW_XDSL_ANNEX_B_7
		bool "VDSL + ADSL Annex B+J, 05.05.00.0E.00.06_05.05.01.08.01.02"									
	config FW_XDSL_ANNEX_B_8
		bool "VDSL + ADSL Annex B, 05.05.01.06.01.06_05.05.01.0A.01.02"		
	config FW_XDSL_ANNEX_J
		depends FW_XDSL_ANNEX_B_8
		bool "VDSL + ADSL Annex B+J, 05.05.01.06.01.06_05.05.01.0A.01.02"									
	config FW_XDSL_ANNEX_B_9
		bool "VDSL + ADSL Annex B, 05.05.01.06.01.06_05.05.01.0B.01.02"		
	config FW_XDSL_ANNEX_J
		depends FW_XDSL_ANNEX_B_9
		bool "VDSL + ADSL Annex B+J, 05.05.01.06.01.06_05.05.01.0B.01.02"									
	endchoice
	config FW_XDSL_VDSL_SUPPORT
		depends on PACKAGE_$(PKG_NAME)
		bool "VDSL support"
		default y
	config FW_XDSL_VDSL_BOND_SUPPORT
		depends on PACKAGE_$(PKG_NAME)
		bool "VDSL port bonding support"
		default n
	config FW_ARC_CO_COMPATIBLE_SUPPORT
		depends on PACKAGE_$(PKG_NAME)
		bool "arcadyan in-house CO compatile support. This results in datapump rollback."
		default n
endef

ifdef CONFIG_FW_XDSL_BETA_ANNEX_A
	FW_VER = $(PKG_VERSION_BETA_A)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_A_1
	FW_VER = $(PKG_VERSION_BETA_A_1)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_A_2
	FW_VER = $(PKG_VERSION_BETA_A_2)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_A_3
	FW_VER = $(PKG_VERSION_BETA_A_3)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_A_4
	FW_VER = $(PKG_VERSION_BETA_A_4)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_A_5
	FW_VER = $(PKG_VERSION_BETA_A_5)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_A_6
	FW_VER = $(PKG_VERSION_BETA_A_6)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_A_7
	FW_VER = $(PKG_VERSION_BETA_A_7)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_B
	FW_VER = $(PKG_VERSION_BETA_B)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_B_2
	FW_VER = $(PKG_VERSION_BETA_B_2)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_B_3
	FW_VER = $(PKG_VERSION_BETA_B_3)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_B_4
	FW_VER = $(PKG_VERSION_BETA_B_4)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_B_5
	FW_VER = $(PKG_VERSION_BETA_B_5)
endif
ifdef CONFIG_FW_XDSL_BETA_ANNEX_B_6
	FW_VER = $(PKG_VERSION_BETA_B_6)
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_1
	FW_VER = $(PKG_VERSION_B_1)
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_2
	FW_VER = $(PKG_VERSION_B_2)
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_3
	FW_VER = $(PKG_VERSION_B_3)
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_4
	FW_VER = $(PKG_VERSION_B_4)
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_5
	FW_VER = $(PKG_VERSION_B_5)
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_6
	FW_VER = $(PKG_VERSION_B_6)
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_7
	FW_VER = $(PKG_VERSION_B_7)
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_8
	FW_VER = $(PKG_VERSION_B_8)
endif
ifdef CONFIG_FW_XDSL_ANNEX_B_9
	FW_VER = $(PKG_VERSION_B_9)
endif
ifdef CONFIG_FW_ARC_CO_COMPATIBLE_SUPPORT
	FW_VER = $(PKG_VERSION_B_3)
endif
define Build/Configure
endef

define Build/Prepare
	gzip -dc $(DL_DIR)/$(PKG_BASE_NAME)-$(FW_VER).tar.gz | $(TAR) -C $(PKG_BUILD_DIR) $(TAR_OPTIONS)
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/$(FIRMWARE_DIR)
	### ctc ###
	$(if $(CONFIG_UBOOT_CONFIG_FIRMWARE_IN_ROOTFS), \
		$(CP) $(PKG_BUILD_DIR)/*.bin $(1)/$(FIRMWARE_DIR)/$(PKG_BASE_NAME)-$(FW_VER).bin; \
		$(CP) $(PKG_BUILD_DIR)/LICENSE $(1)/$(FIRMWARE_DIR)/; \
		ln -sf $(PKG_BASE_NAME)-$(FW_VER).bin $(1)/$(FIRMWARE_DIR)/$(PKG_TARGET_BASE_NAME); \
	,\
		rm -rf $(STAGING_DIR)/firmware/; mkdir -p $(STAGING_DIR)/firmware/; \
		$(CP) $(PKG_BUILD_DIR)/*.bin $(STAGING_DIR)/firmware/$(PKG_BASE_NAME)-$(FW_VER).bin; \
		$(CP) $(PKG_BUILD_DIR)/LICENSE $(STAGING_DIR)/firmware/; \
		ln -sf $(PKG_BASE_NAME)-$(FW_VER).bin $(STAGING_DIR)/firmware/$(PKG_TARGET_BASE_NAME); \
	)
	###########
	### ctc ###
	if [ -f ./files/xcpe_hw.bin ] ; then \
		$(CP) ./files/xcpe_hw.bin $(1)/$(FIRMWARE_DIR)/xcpe_hw.bin ; \
	fi
	if [ "$(FW_XDSL_VDSL_BOND_SUPPORT)" == "y" ] && [ -f ./files/xcpe_hw_port2.bin ] ; then \
		$(CP) ./files/xcpe_hw_port2.bin $(1)/$(FIRMWARE_DIR)/xcpe_hw_port2.bin ; \
	fi
	###########
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
