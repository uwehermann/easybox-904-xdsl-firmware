#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 13904 2009-01-06 21:56:22Z jow $

include $(TOPDIR)/rules.mk

PKG_NAME:=target-ltqcpe-base-files
PKG_VERSION:=1.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/target.mk

define Package/$(PKG_NAME)
	SUBMENU:=Base Files
	SECTION:=base
	CATEGORY:=Lantiq
	TITLE:=base files under target_linux_ltqcpe
endef

define Package/$(PKG_NAME)/description
 This package contains common base files under target/linux/ifxcpe/base-files/ from Lantiq.
endef

define Package/$(PKG_NAME)/config
	##### 20100723 ctc added to support different projects ######
	choice
		depends on PACKAGE_$(PKG_NAME)
		prompt "Target board"
		default $(PKG_NAME)_DT_723
	config $(PKG_NAME)_XRX200
		bool "XRX-200 EVM"
	config $(PKG_NAME)_DT_723
		bool "DT-723"
	config $(PKG_NAME)_DT_921
		bool "DT-921"
	config $(PKG_NAME)_KPN
		bool "KPN"
	config $(PKG_NAME)_VOX2
		bool "VOX 2.0"
	endchoice
	###################################################
endef

	##### ctc added to support different projects ######
TLBF_DIR=XX
ifeq ($(CONFIG_$(PKG_NAME)_XRX200),y)
	TLBF_DIR=files.xrx200
endif
ifeq ($(CONFIG_$(PKG_NAME)_DT_723),y)
	TLBF_DIR=files.723
endif
ifeq ($(CONFIG_$(PKG_NAME)_DT_921),y)
	TLBF_DIR=files.921
endif
ifeq ($(CONFIG_$(PKG_NAME)_KPN),y)
	TLBF_DIR=files.kpn
endif
ifeq ($(CONFIG_$(PKG_NAME)_VOX2),y)
	TLBF_DIR=files.vox2
endif
    ###################################################

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/
	$(CP) ./files/* $(1)/.

	$(INSTALL_DIR) $(1)/etc/config
	gunzip -c ./files/etc/rc.conf.gz > $(1)/etc/config/rc.conf
	##### ctc added to support different projects ######
	$(INSTALL_DIR) $(1)/ramdisk_copy/flash
	ln -sf /etc/config/rc.conf $(1)/ramdisk_copy/flash/rc.conf
	if [ -d "./$(TLBF_DIR)" ] ; then \
		$(CP) ./$(TLBF_DIR)/* $(1)/. ; \
	fi
    ###################################################
endef

### ctc ###
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) ./files/etc/rc.conf.gz $(1)/usr/include/rc.conf.gz
endef
###########

$(eval $(call BuildPackage,$(PKG_NAME)))
