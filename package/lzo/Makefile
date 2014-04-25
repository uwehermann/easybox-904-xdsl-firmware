
include $(TOPDIR)/rules.mk

PKG_NAME:=lzo
PKG_VERSION:=2.06
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.oberhumer.com/opensource/lzo/download/
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/lzo
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Real-time data compression library
endef

define Package/lzo/description
 This package contains a real-time data compression library
endef

define Build/Prepare
	$(call Build/Prepare/Default)
endef

define Build/Configure
	( cd $(PKG_BUILD_DIR); \
		./configure \
			--enable-shared \
			--disable-static \
			--host=mips-linux \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) BLD_DIR=$(PKG_BUILD_DIR)
endef

define Package/lzo/install
	$(INSTALL_DIR) $(1)/usr/lib
	mkdir -p $(STAGING_DIR)/usr/include/lzo
	$(CP) $(PKG_BUILD_DIR)/include/lzo/*.h  $(STAGING_DIR)/usr/include/lzo
	$(CP) $(PKG_BUILD_DIR)/src/.libs/liblzo2.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/.libs/liblzo2.so $(1)/usr/lib/liblzo.so
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/.libs/liblzo2.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/.libs/liblzo2.so $(1)/usr/lib/liblzo.so
endef

$(eval $(call BuildPackage,lzo))
