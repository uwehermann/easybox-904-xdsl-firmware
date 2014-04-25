#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ndisc6
PKG_VERSION:=1.0.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.remlab.net/files/ndisc6/
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ndisc6/Default
  SECTION:=ipv6
  CATEGORY:=IPv6
  TITLE:=Ndisc6
  URL:=http://rdnssd.linkfanel.net/
  DEPENDS:=+kmod-ipv6
endef

define Package/ndisc6
  $(call Package/ndisc6/Default)
  TITLE+= Daemon
endef

define Package/ndisc6/description
NDisc6 is a small collection of useful tools for IPv6 networking
endef

define Package/ndisc6
  $(call Package/ndisc6/Default)
  TITLE+= Dumper
endef

define Build/Prepare
	$(call Build/Prepare/Default,)
	if [ -e ./src ] ; then \
		$(CP)  ./src/*  $(PKG_BUILD_DIR)/. ; \
	fi
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/compat CC="$(TARGET_CC) -std=c99" CFLAGS="-std=c99 -Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time -fhonour-copts -fhonour-copts -msoft-float"
	$(MAKE) -C $(PKG_BUILD_DIR)/src CC="$(TARGET_CC) -std=c99" CFLAGS="-std=c99 -Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time -fhonour-copts -fhonour-copts -msoft-float -D__FAVOR_BSD"
	$(MAKE) -C $(PKG_BUILD_DIR)/rdnssd CC="$(TARGET_CC) -std=c99" CFLAGS="-std=c99 -Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time -fhonour-copts -fhonour-copts -msoft-float"
endef


define Package/ndisc6/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rdnssd/rdnssd $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ndisc6))
