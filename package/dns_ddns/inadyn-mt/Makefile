#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=inadyn-mt
PKG_VERSION:=v.02.24.36
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME).$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
#PKG_MD5SUM:=3b27365772d918050b2251d98a9c7c82

PKG_BUILD_DIR = $(BUILD_DIR)/$(PKG_NAME).$(PKG_VERSION)


include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=DDNS
	SECTION:=net
	CATEGORY:=Network
	DEPENDS:=+libumng +openssl
	TITLE:=inadyn-mt, dynamic DNS client
endef

define Package/$(PKG_NAME)/description
  inadyn-mt - inadyn, inadyn-advanced fork.
  A multi-OS (**NIX, NT, 32 bit Win) console/service/daemon dynamic DNS client.
  Supports many update services. Adds Win RAS trapping; service install, manage;
  unicode & language strings; audible alerts; IPv6.
endef



define Build/Prepare
	$(call Build/Prepare/Default)
	cp -Rf ./src/* $(PKG_BUILD_DIR)
	rm $(PKG_BUILD_DIR)/src/Makefile
#	echo [htf]
#	-ls -l $(PKG_BUILD_DIR)/src/
endef

CONFIGURE_ARGS += \
	--disable-sound




define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/inadyn-mt $(1)/usr/sbin/
	cp $(PKG_BUILD_DIR)/src/inadyn-mt $(PKG_BUILD_DIR)/src/inadynACS
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/inadynACS $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) files/etc/init.d/ddns     $(1)/etc/init.d/ddns
	$(INSTALL_BIN) files/etc/init.d/ddnsACS     $(1)/etc/init.d/ddnsACS
	$(INSTALL_BIN) files/usr/sbin/ddns_start $(1)/usr/sbin/ddns_start
	$(INSTALL_BIN) files/usr/sbin/ddns_startACS $(1)/usr/sbin/ddns_startACS
	$(INSTALL_BIN) files/usr/sbin/ddns_stop  $(1)/usr/sbin/ddns_stop
	$(INSTALL_BIN) files/usr/sbin/ddns_stopACS  $(1)/usr/sbin/ddns_stopACS
#	$(INSTALL_DATA)  files/xxx $(1)/xxx
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
