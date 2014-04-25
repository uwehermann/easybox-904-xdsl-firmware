#
# Copyright (C) 2010 Arcadyan Corporation
# All Rights Reserved.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arc-middle-framework
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/arc-middle/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Middle Layer
  SECTION:=utils
  CATEGORY:=Arcadyan
  TITLE:=middle layer
  DEPENDS:=+libpthread +libumng +lib-arc-com
endef

define Package/$(PKG_NAME)/description
 This package contains a middle layer framework.
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

endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_fmwk.h      $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_hdl.h       $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_sock.h      $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_glbcfg.h    $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_mapi_trnx.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_types.h     $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_file.h      $(1)/usr/include
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
