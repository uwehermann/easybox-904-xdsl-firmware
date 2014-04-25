#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

CONFIG_NO_STRIP = N

include $(TOPDIR)/rules.mk

PKG_NAME:=ltrace
PKG_REV:=163765561621a9127cee3243dd94011e70a2c580
PKG_VERSION:=0.5.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://git.debian.org/git/collab-maint/ltrace.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_VERSION)

PKG_INSTALL:=1


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk


define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libelf
  TITLE:=ltracer
endef

define Package/$(PKG_NAME)/description
  ltrace is a debugging program which runs a specified command until it exits.
While the command is executing, ltrace intercepts and records the dynamic library
calls which are called by the executed process and the signals received by that
process.

  It can also intercept and print the system calls executed by the program.
The program to be traced need not be recompiled for this, so you can use it on
binaries for which you don't have the source handy.

  Remember to add "CONFIG_NO_STRIP = N" in package's makefile to stop the strip
process.


  Have to install libelfg0-dev.deb
endef

EXTRA_CPPFLAGS += -I$(STAGING_DIR)/usr/include/libelf



define Build/Prepare
	echo [htf] $(PKG_NAME) Build/Prepare
	$(call Build/Prepare/Default)
	cp -Rf ./files/*  $(PKG_BUILD_DIR)
endef


define Build/Configure
	echo [htf] $(PKG_NAME) Build/Configure
	$(call Build/Configure/Default)
endef



define Build/Compile
	echo [htf] $(PKG_NAME) Build/Compile
#	$(call Build/Compile/Default, TOOLCHAIN_DIR=$(TOOLCHAIN_DIR) )
	$(call Build/Compile/Default)
endef



define Package/$(PKG_NAME)/install
	echo [htf] $(PKG_NAME) Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ltrace $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/etc/ltrace.conf $(1)/etc/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
