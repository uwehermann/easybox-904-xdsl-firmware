#
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=captcha
PKG_VERSION:=bbbaaa33

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/captcha
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=
  TITLE:=CAPTCHA images generator
  URL:=http://brokestream.com/captcha.html
  SUBMENU:=
endef

define Package/captcha/description
	Small portable CAPTCHA images generator library without dependencies.
	It generates 200x70 images in style of reCAPTCHA ones with five random lower case letters. 
	Instead of 'g' and 'e' letters 'a' substituted, because they are too similar to 'q' and 'c'.
endef

define Build/Prepare
	$(CP) ./src/. $(PKG_BUILD_DIR)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/ $(TARGET_CONFIGURE_OPTS) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)"
endef

define Package/captcha/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/captcha $(1)/usr/bin/
endef

$(eval $(call BuildPackage,captcha))
