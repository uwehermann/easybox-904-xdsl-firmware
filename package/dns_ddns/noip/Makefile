#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=noip2
PKG_VERSION:=2.1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=DDNS
  SECTION:=net
  CATEGORY:=Network
  TITLE:=no-ip.com dynamic DNS service
endef

define Package/$(PKG_NAME)/description
a second-generation Linux client for the no-ip.com dynamic DNS service.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/noip2 $(1)/usr/bin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
