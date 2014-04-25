#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
# re-define PATCH_DIR because kernel.mk will set it to kernel patches
PATCH_DIR:=./patches

PKG_NAME:=ifx-voice-cpe-tapi-kpi2udp
PKG_VERSION:=3.0.1
PKG_MD5SUM:=6696cc77b2be1e63342e688e751ac731
PKG_RELEASE:=1

# IMPORTANT - include the version-override here after PKG_VERSION definition
# include $(INCLUDE_DIR)/package-version-override.mk
# IMPORTANT - ... and before using PKG_VERSION below

PKG_SOURCE:=drv_kpi2udp-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://localhost
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/drv_kpi2udp-$(PKG_VERSION)
PKG_START_SCRIPT:=ltq_load_voice_kpi2udp.sh
PKG_BUILD_DEPENDS:=ifx-os ifx-voice-cpe-tapi
IFX_BIN_DIR=opt/ifx/bin
LANTIQ_BIN_DIR=opt/lantiq/bin

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=Kernel Module
	SECTION:=libs
	CATEGORY:=Lantiq
	TITLE:=TAPI KPI2UDP plug-in
	URL:=http://www.lantiq.com/
	MAINTAINER:=Lantiq Deutschland GmbH
	DEPENDS:=+ifx-voice-cpe-tapi +ifx-os
endef

define Package/$(PKG_NAME)/description
	RTP packet path acceleration into IP stack (strongly recommended)
endef

define Package/$(PKG_NAME)/config
	$(call Package/$(PKG_NAME)/override_version)
	$(call Package/$(PKG_NAME)/override_source_path)
#	menu "configuration"
#		depends on PACKAGE_$(PKG_NAME)
#		source "$(SOURCE)/config/Config.in"
#	endmenu
endef

DRV_TAPI_INCLUDE:=$(STAGING_DIR)/usr/include/drv_tapi

CONFIGURE_ARGS += --with-kernel-incl="$(LINUX_DIR)/include"
CONFIGURE_ARGS += --with-tapi-incl="$(DRV_TAPI_INCLUDE)"
CONFIGURE_ARGS += --with-ifxos-incl=$(STAGING_DIR)/usr/include/ifxos
CONFIGURE_ARGS += --enable-warning

ifeq ($(CONFIG_LINUX_2_6),y)
CONFIGURE_ARGS += --enable-linux-26 --with-kernel-build="$(LINUX_DIR)" ARCH=$(LINUX_KARCH)
PKG_MODULE_EXTENSION=.ko
endif

#define Build/InstallDev
#	$(INSTALL_DIR) $(1)/usr/include
#	mkdir -p $(1)/usr/include/drv_kpi2udp
#	$(CP) $(PKG_BUILD_DIR)/include/* $(1)/usr/include/drv_kpi2udp
#endef

#define Build/UninstallDev
#	rm -rf $(BUILD_DIR)/drv_kpi2udp
#endef

define Build/Clean
	@- [ -e $(PKG_BUILD_DIR)/ipkg ] && rm -Rf $(PKG_BUILD_DIR)/ipkg
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_START_SCRIPT) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/$(IFX_BIN_DIR)
	$(INSTALL_DIR) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/drv_kpi2udp$(PKG_MODULE_EXTENSION) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_DIR) $(1)/$(MODULES_SUBDIR)
	ln -snf /$(LANTIQ_BIN_DIR)/drv_kpi2udp$(PKG_MODULE_EXTENSION) $(1)/$(MODULES_SUBDIR)/drv_kpi2udp.$(LINUX_KMOD_SUFFIX)
	(cd $(1)/$(IFX_BIN_DIR); ln -snf ../../lantiq/bin/drv_kpi2udp.$(LINUX_KMOD_SUFFIX) .)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
