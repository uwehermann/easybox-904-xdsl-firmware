# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libsndfile
PKG_VERSION:=1.0.25
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.mega-nerd.com/libsndfile/files/
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/libsndfile/Default
  SECTION:=libs
  CATEGORY:=Libraries
  URL:=http://www.mega-nerd.com
endef

define Package/libsndfile
  $(call Package/libsndfile/Default)
  TITLE:=Files containing sampled sound
endef

define Package/libsndfile/description
 Library for reading and writing files containing sampled sound
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

define Package/libsndfile/install
	$(INSTALL_DIR) $(1)/usr/lib
	mkdir -p $(STAGING_DIR)/usr/include/libsndfile
	$(CP) $(PKG_BUILD_DIR)/src/*.h  $(STAGING_DIR)/usr/include/libsndfile
	$(CP) $(PKG_BUILD_DIR)/src/.libs/libsndfile.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/.libs/libsndfile.so $(1)/usr/lib/libsndfile.so

endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/.libs/libsndfile.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/src/.libs/libsndfile.so $(1)/usr/lib/libsndfile.so
endef

$(eval $(call BuildPackage,libsndfile))
