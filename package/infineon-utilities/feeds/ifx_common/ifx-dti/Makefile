#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk

PKG_NAME:=ifx-dti
PKG_VERSION:=1.0.1
PKG_BASE_NAME:=lib_dti
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://localhost
LANTIQ_BIN_DIR=opt/lantiq/bin
LANTIQ_LIB_DIR=opt/lantiq/lib

PKG_BUILD_DEPENDS:=ifx-os \
	PACKAGE_ifx-dsl-vinax:ifx-dsl-vinax \
	PACKAGE_ifx-dsl-vinax2:ifx-dsl-vinax2 \
	PACKAGE_ifx-gpon-onu:ifx-gpon-onu \
	PACKAGE_ifx-dsl-geminaxd:ifx-dsl-geminaxd

include $(INCLUDE_DIR)/package-version-override.mk
include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Libs
  SECTION:=libs
  CATEGORY:=Lantiq
  TITLE:=Lantiq Debug and Trace Interface
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=+libpthread +ifx-os
  MENU:=1
endef

define Package/$(PKG_NAME)/description
  This library will be used by some Lantiq applications / drivers
endef

ifdef CONFIG_PACKAGE_ifx-dsl-vinax
CONFIGURE_ARGS += \
	--enable-vinax \
	--with-dti-max-lines-per-device=2\
	--with-dti-max-devices=32\
	--enable-dsl-driver-include="-I$(STAGING_DIR)/usr/include"
endif

ifdef CONFIG_PACKAGE_ifx-dsl-vinax2
CONFIGURE_ARGS += \
	--enable-vinax \
	--enable-dev-generic \
	--enable-dti-agent \
	--with-dti-max-lines-per-device=8\
	--with-dti-max-devices=8\
	--enable-dsl-driver-include="-I$(STAGING_DIR)/usr/include"
endif

ifdef CONFIG_PACKAGE_ifx-dsl-geminaxd
CONFIGURE_ARGS += \
	--enable-geminax \
	--enable-dti-agent \
	--with-dti-max-lines-per-device=$(IFX_DSL_LINES_PER_DEVICE)\
	--with-dti-max-devices=$(IFX_DSL_MAX_DEVICE)\
	--enable-dsl-driver-include="-I$(STAGING_DIR)/usr/include"
endif

CONFIGURE_ARGS += \
	--enable-targetincl=$(STAGING_DIR)/usr/include \
	--enable-target=$(STAGING_DIR)/usr/lib \
	--enable-debug --enable-error-print\
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos"\
	--enable-ifxos-library="-L$(STAGING_DIR)/usr/lib"\
	--with-dti-tcp-port=9000\
	--enable-dev-generic \
	--enable-dti-agent \
	--enable-cli

define Build/Clean
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include/dti}
	$(CP) $(PKG_BUILD_DIR)/src/include/* $(1)/usr/include/dti
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdti*.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdti*.la $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdti*.a $(1)/usr/lib/
endef

define Build/UninstallDev
	- rm -Rf $(1)/usr/include/dti
	- rm -f $(1)/usr/lib/libdti*.so*
endef

define Package/ifx-dti/install
	$(INSTALL_DIR) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_DIR) $(1)/usr/lib
	$(MAKE) -C $(PKG_BUILD_DIR) install DESTDIR=$(PKG_INSTALL_DIR)
ifeq ($(shell [ -f $(value $(PKG_INSTALL_DIR)/usr/bin/dti_agent) ] && echo exists),exists)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/dti_agent $(1)/$(LANTIQ_BIN_DIR)
endif
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdti_agent*.so.* $(1)/usr/lib/
ifeq ($(shell [ -f $(value $(PKG_INSTALL_DIR)/usr/lib/libdti_device*.so.*) ] && echo exists),exists)
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdti_device*.so.* $(1)/usr/lib/
endif
endef

$(eval $(call BuildPackage,ifx-dti))
