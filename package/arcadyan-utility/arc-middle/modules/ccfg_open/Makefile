#
# Copyright (C) 2010 Arcadyan Corporation
# All Rights Reserved.
#

ARC_MOD_NAME=ccfg-open
ARC_MOD_DESCR=common configuration
PKG_VERSION_MAJOR:=0
PKG_VERSION_MINOR:=1
PKG_RELEASE:=1
PKG_TEST_CLI:=
PKG_DEPENDS:=+libumng +arc-middle-framework +arc-middle-framework-open

#####################################################################

PKG_NAME:=arc-middle-mod-$(ARC_MOD_NAME)
PKG_MAPI_LDFLAGS:=
PKG_HDL_LDFLAGS:=
PKG_TEST_CLI_LDFLAGS:=
DEBUG:=0
MAPFILE:=1

#define Package/$(PKG_NAME)/pre-install
#endef

define Package/$(PKG_NAME)/post-install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmapi_ccfg.so*		$(1)/usr/lib
endef

define Package/$(PKG_NAME)/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_mapi_ccfg.h		$(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_ccfg.h			$(1)/usr/include

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmapi_ccfg.so*		$(1)/usr/lib
endef

include ../mod_package.mk

$(eval $(call BuildPackage,$(PKG_NAME)))
