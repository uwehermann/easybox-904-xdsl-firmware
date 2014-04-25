#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libgsasl
PKG_VERSION:=1.8.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.gnu.org/gnu/gsasl/
PKG_MD5SUM:=5dbdf859f6e60e05813370e2b193b92b

include $(INCLUDE_DIR)/package.mk

define Package/libgsasl
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Lib of Simple Authentication and Security Layer framework
  URL:=http://www.gnu.org/software/gsasl/
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-threads=posix \
	--disable-nls \
	--disable-anonymous \
	--disable-external \
	--disable-securid \
	--disable-ntlm \
	--disable-saml20 \
	--disable-openid20 \
	--disable-gs2 \
	--disable-gssapi \
	--disable-scram-sha1 

define Host/Configure
endef

define Build/libgsasl/compile
	$(MAKE) -C $(PKG_BUILD_DIR)/ CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)"
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/.libs/libgsasl.so* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/src/gsasl*.h $(1)/usr/include/
endef

define Package/libgsasl/install
	$(INSTALL_DIR)	$(1)/usr/lib
	$(CP)	$(PKG_BUILD_DIR)/src/.libs/libgsasl.so*	$(1)/usr/lib/
endef

$(eval $(call BuildPackage,libgsasl))
