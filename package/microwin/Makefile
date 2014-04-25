#
# Copyright (C) 2010 Zeta Labs
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=microwin
#PKG_VERSION:=20100612
PKG_VERSION:=full-0.92
PKG_REV:=639bcc4f0e5cda23e8c35f170606998441191b0f
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://microwindows.org/microwin
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_BUILD_DIR=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_INSTALL:=1
#PKG_FIXUP:=libtool

include $(INCLUDE_DIR)/package.mk

define Package/microwin
  SECTION:=xorg-server
  CATEGORY:=Xorg
  SUBMENU:=server
  TITLE:=Nano-X Window System
  DEPENDS:=
  URL:=http://www.microwindows.org/
endef

define Package/microwin/description
  Nano-X is an Open Source project aimed at bringing
  the features of modern graphical windowing environments
  to smaller devices.
endef

define Build/Configure
	$(CP) ./src/src/config $(PKG_BUILD_DIR)/src/config
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/src/* $(PKG_BUILD_DIR)/src/.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
	    CC=$(TARGET_CC) \
		$(TARGET_CONFIGURE_OPTS) \
		INCFT2LIB=$(STAGING_DIR)/usr/include \
	    LDFLAGS="-L$(PKG_BUILD_DIR)/src/lib -L$(STAGING_DIR)/usr/lib -lz -lfreetype -lumng -lmapitrnx -lmapi_ccfg -lmapi_ccfgsal -lArcCom -lmapi_wlan -lmapi_lanhs -lmapi_voip -lmapi_lte -lmapi_mon -lmapi_wan -lmapi_usb"
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
		INSTALL_PREFIX=$(PKG_BUILD_DIR)/src/bin \
		INSTALL_OWNER1= \
		INSTALL_OWNER2= \
		CC=$(TARGET_CC) \
		INCFT2LIB=$(STAGING_DIR)/usr/include \
		LDFLAGS="-L$(PKG_BUILD_DIR)/src/lib -L$(STAGING_DIR)/usr/lib -lz -lfreetype" \
		install
	#$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/bin/nanowm $(PKG_INSTALL_DIR)/usr/bin/
endef

#define Build/InstallDev
#	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib/pkgconfig
#	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
#	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
#endef

define Package/microwin/install
	echo "Compile $(PKG_NAME) only. No Installation necessary!"
endef

$(eval $(call BuildPackage,microwin))
