#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=ifx-os
PKG_VERSION:=1.5.14
PKG_BASE_NAME:=lib_ifxos
# include $(INCLUDE_DIR)/package-version-override.mk

PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://localhost
#PKG_MD5SUM:=1111
LANTIQ_BIN_DIR=opt/lantiq/bin
# TBR
IFX_BIN_DIR=/opt/ifx/bin

# re-define PATCH_DIR because kernel.mk will set it to kernel patches
PATCH_DIR:=./patches

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=Kernel Module
	SECTION:=libs
	CATEGORY:=Lantiq
	TITLE:=OS abstraction library
	URL:=http://www.lantiq.com/
	MAINTAINER:=Lantiq GmbH
	DEPENDS:=+libpthread
	MENU:=1
endef

define Package/$(PKG_NAME)/description
	This library will be used by some Lantiq applications / drivers
endef

CONFIGURE_ARGS += --enable-kernelincl="$(LINUX_DIR)/include"

ifeq ($(CONFIG_LINUX_2_6),y)
CONFIGURE_ARGS += --enable-linux-26 --enable-kernelbuild="$(LINUX_DIR)" ARCH=$(LINUX_KARCH)
PKG_MODULE_EXTENSION=.ko
endif

ifeq ($(CONFIG_mips),y)
CONFIGURE_ARGS += --enable-add_drv_cflags="-fno-pic -mno-abicalls -mlong-calls -G 0"
#EXTRA_CFLAGS = -fno-pic -mno-abicalls -mlong-calls -G 0
endif

ifeq ($(CONFIG_mipsel),y)
CONFIGURE_ARGS += --enable-add_drv_cflags="-fno-pic -mno-abicalls -mlong-calls -G 0"
endif

define Build/Clean
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include/ifxos}
	$(CP) $(PKG_BUILD_DIR)/src/include/* $(1)/usr/include/ifxos
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/src/libifxos.a $(1)/usr/lib/libifxos.a
endef

define Build/UninstallDev
	- rm -Rf $(1)/usr/include/ifxos
	- rm -f $(1)/usr/lib/libifxos.a
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ifx_load_ifxos_drv.sh $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/drv_ifxos$(PKG_MODULE_EXTENSION) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_DIR) $(1)/$(MODULES_SUBDIR)
	ln -snf /$(LANTIQ_BIN_DIR)/drv_ifxos$(PKG_MODULE_EXTENSION) $(1)/$(MODULES_SUBDIR)/drv_ifxos.$(LINUX_KMOD_SUFFIX)
	# TBR
	$(INSTALL_DIR) $(1)/$(IFX_BIN_DIR)
	(cd $(1)/$(IFX_BIN_DIR); ln -snf ../../lantiq/bin/drv_ifxos.$(LINUX_KMOD_SUFFIX) .)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
