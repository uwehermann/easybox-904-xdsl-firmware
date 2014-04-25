#
# Copyright (C) 2010 Arcadyan Corporation
# All Rights Reserved.
#

ARC_MOD_NAME=wlan-open
ARC_MOD_DESCR=WLAN
PKG_VERSION_MAJOR:=0
PKG_VERSION_MINOR:=1
PKG_RELEASE:=1
PKG_TEST_CLI:=
PKG_DEPENDS:=+arc-middle-mod-ccfg-open +arc-middle-mod-ccfgsal +lib-arc-com

#####################################################################

PKG_NAME:=arc-middle-mod-$(ARC_MOD_NAME)
PKG_LDFLAGS:=-lArcCom -lmapi_ccfg -lmapi_ccfgsal

define Package/$(PKG_NAME)/post-install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmapi_wlan.so*		$(1)/usr/lib
endef

define Package/$(PKG_NAME)/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_mapi_wlan.h		$(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_wlan.h			$(1)/usr/include

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmapi_wlan.so*		$(1)/usr/lib
endef

include ../mod_package.mk

$(eval $(call BuildPackage,$(PKG_NAME)))
