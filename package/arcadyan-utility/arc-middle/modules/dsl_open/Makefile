#
# Copyright (C) 2010 Arcadyan Corporation
# All Rights Reserved.
#

ARC_MOD_NAME=dsl-open
ARC_MOD_DESCR=DSL
PKG_VERSION_MAJOR:=0
PKG_VERSION_MINOR:=1
PKG_RELEASE:=1
PKG_TEST_CLI:=

include $(TOPDIR)/rules.mk

#####################################################################

PKG_NAME:=arc-middle-mod-$(ARC_MOD_NAME)

define Package/$(PKG_NAME)/post-install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmapi_dsl.so*		$(1)/usr/lib
endef

define Package/$(PKG_NAME)/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_mapi_dsl.h		$(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/mid_dsl.h			$(1)/usr/include

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libmapi_dsl.so*		$(1)/usr/lib
endef

include ../mod_package.mk

$(eval $(call BuildPackage,$(PKG_NAME)))
