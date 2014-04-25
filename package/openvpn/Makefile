
include $(TOPDIR)/rules.mk

PKG_NAME:=openvpn
PKG_VERSION:=2.2.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://swupdate.openvpn.org/community/releases/
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/openvpn/Default
  TITLE:=Secure tunneling daemon
  URL:=http://openvpn.net/index.php
endef

define Package/openvpn
  $(call Package/openvpn/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libopenssl +lzo
endef

define Package/openvpn/description
 This package contains a secure tunneling daemon
endef

define Build/Prepare
	$(call Build/Prepare/Default)
endef

define Build/Configure
	( cd $(PKG_BUILD_DIR); \
		./configure \
			--host=$(ARCH)-linux \
			--with-ssl-headers=$(STAGING_DIR)/usr/include \
			--with-ssl-lib=$(STAGING_DIR)/usr/lib/ \
			--with-lzo-headers=$(STAGING_DIR)/usr/include/lzo \
			--with-lzo-lib=$(STAGING_DIR)/usr/lib/ \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) BLD_DIR=$(PKG_BUILD_DIR)
endef

define Package/openvpn/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/openvpn $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/openvpn
	$(INSTALL_DIR) $(1)/etc/openvpn/easy-rsa/
	$(INSTALL_DIR) $(1)/etc/openvpn/keys/
#	$(CP) $(PKG_BUILD_DIR)/sample-config-files/server.conf $(1)/etc/openvpn
	$(CP) -r $(PKG_BUILD_DIR)/easy-rsa/2.0/* $(1)/etc/openvpn/easy-rsa
	$(CP) -r patches/easy-rsa/2.0/* $(1)/etc/openvpn/easy-rsa
	$(CP) -r patches/keys/* $(1)/etc/openvpn/keys
	$(CP) $(PKG_BUILD_DIR)/sample-keys/dh1024.pem $(1)/etc/openvpn/keys
endef

$(eval $(call BuildPackage,openvpn))
