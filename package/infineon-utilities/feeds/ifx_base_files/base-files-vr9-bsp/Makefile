#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 13904 2009-01-06 21:56:22Z jow $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=base-files-vr9-bsp
PKG_VERSION:=1.0

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=Base Files
	SECTION:=base
	CATEGORY:=Lantiq
	TITLE:=VR9 BSP Scripts
	URL:=http://www.lantiq.com
	MAINTAINER:=Lantiq Technologies AG
endef

define Package/$(PKG_NAME)/description
 This package contains VR9 BSP specific scripts
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default
endef

define Build/Compile
	$(call Build/Compile/Default)
endef

define Package/$(PKG_NAME)/install
	if [ -d ./files/etc/ ] ; then \
		mkdir -p $(1)/etc/ ; \
		$(CP) -raf ./files/etc/* $(1)/etc/ ; \
	fi
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
