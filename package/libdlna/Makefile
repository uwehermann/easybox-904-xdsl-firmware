
include $(TOPDIR)/rules.mk

PKG_NAME:=libdlna
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/libdlna
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=DLNA server support library
  DEPENDS:=+ffmpeg
endef

define Package/libdlna/description
 This package contains a library for DLNA server support
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
			PKG_BLD_DIR=$(PKG_BUILD_DIR) \
			STG_DIR=$(STAGING_DIR) \
			STG_LIB_DIR=$(STAGING_DIR)/usr/lib 
endef

define Package/libdlna/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/libdlna.so* $(1)/usr/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/libdlna.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libdlna))
