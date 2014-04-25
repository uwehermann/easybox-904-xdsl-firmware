#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dhcp
PKG_VERSION:=4.1.1-P1
PKG_RELEASE:=1

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-P1-arcadyan.tar.gz
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://pkgs.fedoraproject.org/repo/pkgs/dhcp/dhcp-4.1.1-P1.tar.gz/ee390a35687dd75dbfc32c856c0938d1/
PKG_MD5SUM:=ee390a35687dd75dbfc32c856c0938d1

#PKG_BUILD_DIR:=$(PKG_NAME)-$(PKG_VERSION)-P1-arcadyan
#PKG_INSTALL_DIR:=$(PKG_NAME)-$(PKG_VERSION)-P1-arcadyan

include $(INCLUDE_DIR)/package.mk

define Package/dhcp/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  TITLE:=ISC's DHCP
  URL:=https://www.isc.org/software/dhcp
endef

define Package/dhcp-relay
  $(call Package/dhcp/Default)
  TITLE+= relay
  DEPENDS+= dhcp-libs
endef

define Package/dhcp-server
  $(call Package/dhcp/Default)
  TITLE+= server
  DEPENDS+= dhcp-libs
endef

define Package/dhcp-client
  $(call Package/dhcp/Default)
  TITLE+= client
  DEPENDS:=+dhcp-libs +libumng +libsyslog +arc-middle-framework +arc-middle-mod-ccfg +lib-arc-com
endef

define Package/dhcp-libs
  $(call Package/dhcp/Default)
  TITLE+= libs
endef

CONFIGURE_VARS += \
        ac_cv_prog_RANLIB=true

define Build/Prepare
	$(call Build/Prepare/Default)
	if [ -d ./src ] ; then \
		$(CP) ./src/* $(PKG_BUILD_DIR)/. ; \
	fi
endef

define Build/Configure
	$(call Build/Configure/Default, )
endef

TARGET_CFLAGS += -fPIC
EXTRA_LDFLAGS += -lumngsyslog -lmapitrnx -lmapi_ccfg -lArcCom
define Build/Compile
	$(call Build/Compile/Default, all)
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR=$(PKG_INSTALL_DIR) install
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/common/lib*.so $(PKG_INSTALL_DIR)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/omapip/lib*.so $(PKG_INSTALL_DIR)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dst/lib*.so $(PKG_INSTALL_DIR)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dhcpctl/lib*.so $(PKG_INSTALL_DIR)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/minires/lib*.so $(PKG_INSTALL_DIR)/usr/lib/
endef

define Package/dhcp-relay/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/dhcrelay $(1)/usr/sbin/
endef

define Package/dhcp-server/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/dhcpd $(1)/usr/sbin/
	cp $(1)/usr/sbin/dhcpd $(1)/usr/sbin/dhcp6d
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dhcpd.init $(1)/etc/init.d/dhcpd
endef

define Package/dhcp-client/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/dhclient $(1)/usr/sbin/
	cp $(1)/usr/sbin/dhclient $(1)/usr/sbin/dhclient6
endef

define Package/dhcp-libs/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/lib*.so $(1)/usr/lib
endef

$(eval $(call BuildPackage,dhcp-relay))
$(eval $(call BuildPackage,dhcp-server))
$(eval $(call BuildPackage,dhcp-client))
$(eval $(call BuildPackage,dhcp-libs))
