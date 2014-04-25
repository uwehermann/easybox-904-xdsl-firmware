#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ntpclient
PKG_VERSION:=2007_365
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://doolittle.icarus.com/ntpclient/
PKG_MD5SUM:=cb98711f11769cdd22fc592844cef414

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-2007

include $(INCLUDE_DIR)/package.mk

define Package/ntpclient
  SECTION:=net
  CATEGORY:=Network
  TITLE:=NTP (Network Time Protocol) client
  URL:=http://doolittle.icarus.com/ntpclient/
  DEPENDS:=+librt
endef

define Package/ntpclient/description
	NTP client for setting system time from NTP servers.
endef

define Build/Prepare
	$(call Build/Prepare/Default,)
	if [ -e ./src ] ; then \
		$(CP)  ./src/*  $(PKG_BUILD_DIR)/. ; \
	fi
endef

MAKE_FLAGS += \
	all adjtimex

define Package/ntpclient/install
#	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
#	$(INSTALL_DATA) ./files/ntpclient.hotplug $(1)/etc/hotplug.d/iface/20-ntpclient
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/ntpclient.config $(1)/etc/config/ntpclient
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ntpclient $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/adjtimex $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rate.awk $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/ntpclient2_setup.sh  $(1)/usr/sbin/ntpclient2_setup.sh
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ntpclient.init $(1)/etc/init.d/ntpclient
	$(INSTALL_BIN) ./files/ntpclient2     $(1)/etc/init.d/ntpclient2
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/www.pool.ntp.org_zone_de.html     $(1)/etc/config/www.pool.ntp.org_zone_de.html
endef

$(eval $(call BuildPackage,ntpclient))
