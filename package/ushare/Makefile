#
# Copyright (C) 2009 Arcadyan Corporation
# All Rights Reserved.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ushare
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/ushare
  SECTION:=net
  CATEGORY:=Network
  TITLE:=DLNA Server
  DEPENDS:=+libdlna +libupnp
endef

define Package/ushare/description
 This package contains a DLNA/UPnP media server program.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CFLAGS="$(TARGET_CPPFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/ushare/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ushare $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/dlna.sh $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/scripts/ushare.conf $(1)/etc/ushare.conf
endef

$(eval $(call BuildPackage,ushare))
