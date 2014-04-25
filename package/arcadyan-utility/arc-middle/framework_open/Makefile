#
# Copyright (C) 2010 Arcadyan Corporation
# All Rights Reserved.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arc-middle-framework-open
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/arc-middle/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Middle Layer
  SECTION:=utils
  CATEGORY:=Arcadyan
  TITLE:=middle layer
  DEPENDS:=+libpthread +libumng +lib-arc-com +arc-middle-framework
endef

define Package/$(PKG_NAME)/description
 This package contains a open library of middle layer framework.
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
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS) -lArcCom"
endef

define Package/$(PKG_NAME)/install
 
	# libs
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmapitrnx.so* $(1)/usr/lib/
	
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_mapi_trnx.h $(1)/usr/include

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmapitrnx.so* $(1)/usr/lib
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
