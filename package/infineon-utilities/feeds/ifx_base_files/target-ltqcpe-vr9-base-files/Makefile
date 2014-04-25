#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 13904 2009-01-06 21:56:22Z jow $

include $(TOPDIR)/rules.mk

PKG_NAME:=target-ltqcpe-vr9-base-files
PKG_VERSION:=1.0

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=Base Files
	SECTION:=base
	CATEGORY:=Lantiq
	TITLE:=base files under target_linux_ltqcpe_vr9
endef

define Package/$(PKG_NAME)/description
 This package contains VR9 base files under target/linux/ifxcpe/platform_vr9/base-files/ from Lantiq.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/
	$(CP) ./files/* $(1)/.

#	$(INSTALL_DIR) $(1)/tmp
#	ln -sf /tmp/ramdisk   $(1)/ramdisk
	##### ctc added to support different projects ######
#	ln -sf /ramdisk/flash $(1)/flash
	ln -sf /ramdisk_copy/flash $(1)/flash
    ###################################################
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

