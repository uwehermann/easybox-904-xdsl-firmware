#
# Copyright (C) 2010 OpenWrt.org
# Copyright (C) 2010 Lantiq Technologies
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $


# This is a template for board-specific base files
#
# To create a new board specific package, do the following steps:
#   - Set the variable IFX_BOARD_NAME to the board name.
#     It must be in lower case because of ipkg/opkg restrictions!
#   - Adapt the DEPENDS statement (e.g. remove @BROKEN)
#   - !!! Delete this comment block in your Makefile !!!

include $(TOPDIR)/rules.mk

IFX_BOARD_NAME:=template

PKG_NAME:=ifx-base-files-$(IFX_BOARD_NAME)
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Base Files
  SECTION:=base
  CATEGORY:=Lantiq
  TITLE:=Base files for the $(IFX_BOARD_NAME) board.
  URL:=http://www.Lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=+ifx-base-files-common @BROKEN
endef

#define Package/$(PKG_NAME)/conffiles
#endef

define Package/$(PKG_NAME)/description
 This package contains files and system scripts for OpenWrt,
 required for the $(IFX_BOARD_NAME) board.
endef

define Build/Prepare
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/install
	[ -d ./files ] && $(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

