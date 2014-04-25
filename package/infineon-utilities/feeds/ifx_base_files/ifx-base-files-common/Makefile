#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk

PKG_NAME:=ifx-base-files-common
PKG_RELEASE:=1

# TBR
LANTIQ_BIN_DIR=opt/lantiq/bin
IFX_BIN_DIR=/opt/ifx/bin

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Base Files
  SECTION:=base
  CATEGORY:=Lantiq
  TITLE:=Common files for Lantiq systems
  URL:=http://www.Lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
endef

#define Package/$(PKG_NAME)/conffiles
#endef

define Package/$(PKG_NAME)/description
 This package contains files and system scripts for OpenWrt,
 common for all Lantiq targets.
endef

define Build/Prepare
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(CP) ./files/* $(1)/

	# TBR
	$(INSTALL_DIR) $(1)/$(IFX_BIN_DIR)
	(cd $(1)/$(IFX_BIN_DIR); ln -snf ../../lantiq/bin/inst_driver.sh .)

	$(SED) 's,$$$$R,$(REVISION),g' $(1)/etc/banner.ifx
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
