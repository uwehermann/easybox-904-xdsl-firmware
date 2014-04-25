#
# Copyright (C) 2010 Arcadyan Corporation
# All Rights Reserved.
#

ARC_MOD_NAME=mon-open
ARC_MOD_DESCR=MON
PKG_VERSION_MAJOR:=0
PKG_VERSION_MINOR:=1
PKG_RELEASE:=1
PKG_TEST_CLI:=
PKG_DEPENDS:=+lib-arc-com +arc-middle-framework-open +arc-middle-mod-ccfg-open

#####################################################################

PKG_NAME:=arc-middle-mod-$(ARC_MOD_NAME)
PKG_LDFLAGS:=-lArcCom -lmapi_ccfg

define Package/$(PKG_NAME)/post-install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmapi_mon.so*		$(1)/usr/lib
endef

define Package/$(PKG_NAME)/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_mapi_mon.h		$(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_mon.h			$(1)/usr/include

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmapi_mon.so*		$(1)/usr/lib
endef

include ../mod_package.mk

$(eval $(call BuildPackage,$(PKG_NAME)))
