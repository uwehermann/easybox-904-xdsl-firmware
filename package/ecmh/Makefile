#
# Copyright (C) 2011 Arcadyan Inc.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ecmh
PKG_VERSION:=2005.02.09
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ecmh
#PKG_MD5SUM:=3b27365772d918050b2251d98a9c7c82

#PKG_BUILD_DEPENDS:=libncurses libpcre libreadline
#PKG_BUILD_DIR:=src

include $(INCLUDE_DIR)/package.mk

define Package/ecmh/Default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Easy Cast du Multi Hub
endef

define Package/ecmh
  $(call Package/ecmh/Default)
endef

define Package/ecmh/Compile
	$(call Package/ecmh/Compile/Default, all)
endef

define Package/ecmh/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ecmh $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ecmh))
