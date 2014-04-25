
include $(TOPDIR)/rules.mk

PKG_NAME:=libcapi20
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=ISDN CAPI 2.0 support library
endef

define Package/$(PKG_NAME)/description
  This package contains a library for ISDN CAPI 2.0 support
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
		CFLAGS="$(TARGET_CFLAGS) -fPIC -Wall -I$(STAGING_DIR)/usr/include -I." \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		PKG_BLD_DIR=$(PKG_BUILD_DIR) \
		STG_DIR=$(STAGING_DIR) \
		STG_LIB_DIR=$(STAGING_DIR)/usr/lib
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/*.so* $(1)/usr/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/*.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/*.so* $(1)/usr/lib
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
