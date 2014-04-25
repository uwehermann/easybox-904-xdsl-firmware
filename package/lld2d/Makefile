#
# Copyright (C) 2012 Arcadyan Corporation
# All Rights Reserved.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lld2d
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/lld2d
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  TITLE:=Link Layer Topology Discovery daemon
endef

define Package/lld2d/description
  Microsoft Windows Link Layer Topology Discovery
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="-O3 -pipe -mips32 -mtune=mips32 -Wall -Wno-unused -s -g -fpack-struct -Wpadded" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/lld2d/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lld2d $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/lld2d.init $(1)/etc/init.d/lld2d
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
