#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=inadyn
PKG_VERSION:=1.96.2
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=DDNS
	SECTION:=net
	CATEGORY:=Network
	DEPENDS:=+libumng
	TITLE:=INADYN, dynamic DNS client
endef

define Package/$(PKG_NAME)/description
INADYN is a dynamic DNS client
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		TARGET_ARCH=linux \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		all
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/linux/inadyn $(1)/usr/sbin/
	cp $(PKG_BUILD_DIR)/bin/linux/inadyn $(PKG_BUILD_DIR)/bin/linux/inadynACS
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/linux/inadynACS $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) files/etc/init.d/ddns     $(1)/etc/init.d/ddns
	$(INSTALL_BIN) files/etc/init.d/ddnsACS     $(1)/etc/init.d/ddnsACS
	$(INSTALL_BIN) files/usr/sbin/ddns_start $(1)/usr/sbin/ddns_start
	$(INSTALL_BIN) files/usr/sbin/ddns_startACS $(1)/usr/sbin/ddns_startACS
	$(INSTALL_BIN) files/usr/sbin/ddns_stop  $(1)/usr/sbin/ddns_stop
	$(INSTALL_BIN) files/usr/sbin/ddns_stopACS  $(1)/usr/sbin/ddns_stopACS
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
