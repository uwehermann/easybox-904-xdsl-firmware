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

PKG_NAME:=lq-coc-app-pmd
PKG_VERSION:=1.0.3.0
PKG_MD5SUM:=b5a02933d548790c7d8151eeacaa3e12
PKG_RELEASE:=1

# IMPORTANT - include the version-override here after PKG_VERSION definition
# include $(INCLUDE_DIR)/package-version-override.mk
# IMPORTANT - ... and before using PKG_VERSION below

PKG_SOURCE:=lq_coc_app_pmd-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://localhost
PKG_BUILD_DIR:=$(BUILD_DIR)/lq_coc_app_pmd-$(PKG_VERSION)
PKG_START_SCRIPT:=pmd.sh
PKG_BUILD_DEPENDS:=
LANTIQ_BIN_DIR=opt/lantiq/bin

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=Utilities
	SECTION:=utils
	CATEGORY:=Lantiq
	TITLE:=Power Management Daemon (PMD)
	URL:=http://www.lantiq.com/
	MAINTAINER:=Lantiq Deutschland GmbH
	DEPENDS:=
	MENU:=1
endef

define Package/$(PKG_NAME)/description
	Power Management Daemon(PMD) adjust the clock frequency of the chip dependent on the current CPU load.
   The PMD evaluate the current idle time of the processor and if the idle time is below or above a predefined threshold,
   the clock frequency of the specific modules(e.g. CPU, DDRAM, Peripherals) are ajusted.
   Example: pmd -h    "print help menu"
   
endef

### Menuconfig ##################################
define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
$(call Package/$(PKG_NAME)/override_source_path)
	source "$(SOURCE)/config/Config.in"
endef


#### Target Rules ###############################
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(TARGET_CONFIGURE_OPTS) IFX_CFLAGS="$(IFX_CFLAGS)"
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_START_SCRIPT) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/pmd $(1)/opt/lantiq/bin/
endef

define Build/clean
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
