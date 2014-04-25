#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
# re-define PATCH_DIR because kernel.mk will set it to kernel patches
PATCH_DIR:=./patches

PKG_NAME:=ifx-dsl-cpe-mei-vrx
PKG_VERSION:=1.2.0
PKG_BASE_NAME:=drv_mei_cpe
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://localhost
#PKG_MD5SUM:=
PKG_BUILD_DEPENDS:=ifx-os
LANTIQ_BIN_DIR=opt/lantiq/bin
FW_DIR=/lib/firmware/$(LINUX_VERSION)
# IFX_DSL_MAX_DEVICE=$(strip $(subst ",, $(CONFIG_IFX_DSL_MAX_DEVICE)))
# IFX_DSL_LINES_PER_DEVICE=$(strip $(subst ",, $(CONFIG_IFX_DSL_LINES_PER_DEVICE)))
# Set a default configuration, required setup is done after module load
IFX_DSL_MAX_DEVICE=1
### ctc ###
ifeq ($(FW_XDSL_VDSL_BOND_SUPPORT),)
IFX_DSL_LINES_PER_DEVICE=1
else
IFX_DSL_LINES_PER_DEVICE=2
endif
###########

include $(INCLUDE_DIR)/package-version-override.mk
include $(INCLUDE_DIR)/package.mk

ifeq ($(DUMP),)
   STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell ( \
      grep '^CONFIG_$(PKG_NAME)' $(TOPDIR)/.config) | md5s)
endif

define Package/$(PKG_NAME)
	SUBMENU:=Kernel Module
  SECTION:=driver
  CATEGORY:=Lantiq
  TITLE:=VRx MEI CPE Driver
  URL:=http://www.Lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=+ifx-os +kmod-ifx-dsl-cpe-mei-vrx
  MENU:=1
endef

define KernelPackage/$(PKG_NAME)
  SUBMENU:=Lantiq
  TITLE:=VRx MEI CPE Driver
  URL:=http://www.lantiq.com/
  MAINTAINER:=Lantiq Deutschland GmbH
  DEPENDS:=+ifx-os
  KCONFIG:=CONFIG_DSL_MEI_CPE_DRV=y
endef


# package specific configuration
define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
$(call Package/$(PKG_NAME)/override_source_path)
	menu "kernel build-in"
		depends PACKAGE_$(PKG_NAME)
	config $(PKG_NAME)_USE_KERNEL_BUILD_IN
		bool "Use kernel build-in"
		default n
	endmenu
endef 

define Package/$(PKG_NAME)/description
	Lantiq MEI CPE Driver
endef

define KernelPackage/$(PKG_NAME)/description
	Lantiq MEI CPE Kernel Module Driver
endef


CONFIGURE_ARGS += \
	--enable-kernelincl="$(LINUX_DIR)/include" \
	--enable-device=vr9 \
	--with-max-device=$(IFX_DSL_MAX_DEVICE) \
	--with-lines-per-device=$(IFX_DSL_LINES_PER_DEVICE) \
	--enable-debug --enable-error_print \
	--enable-ifxos-include="-I$(STAGING_DIR)/usr/include/ifxos" \
	--enable-ifxos-library="-L$(STAGING_DIR)/usr/lib" \
	--enable-add_drv_cflags="-DMEI_DRV_ATM_PTM_INTERFACE_ENABLE=1 -fno-pic -mno-abicalls -mlong-calls -O2 -g0"

ifeq ($(CONFIG_ifx-dsl-cpe-mei-vrx_USE_KERNEL_BUILD_IN),y)
CONFIGURE_ARGS += \
	--enable-ifxos_drv=no \
	--enable-lkm=no 
endif

ifeq ($(CONFIG_LINUX_2_6),y)
CONFIGURE_ARGS += \
	--enable-linux-26 \
	--enable-kernelbuild="$(LINUX_DIR)" \
	ARCH=$(LINUX_KARCH)
PKG_MODULE_EXTENSION=.ko
endif

define Build/Clean
	@- [ -e $(PKG_BUILD_DIR)/ipkg ] && rm -Rf $(PKG_BUILD_DIR)/ipkg
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include}
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_api_intern.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_api_atm_ptm_intern.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_interface.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/drv_mei_cpe_config.h $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/cmv_message_format.h $(1)/usr/include/
endef

define Build/UninstallDev
	rm -rf $(BUILD_DIR)/$(PKG_BASE_NAME)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ifx_load_cpe_mei_drv.sh $(1)/etc/init.d/
	lines=`expr $(IFX_DSL_LINES_PER_DEVICE) \* $(IFX_DSL_MAX_DEVICE)`; \
		$(SED) "s:@lines@:$$$$lines:g" $(1)/etc/init.d/ifx_load_cpe_mei_drv.sh; \
		$(SED) "s:@devices@:${IFX_DSL_MAX_DEVICE}:g" $(1)/etc/init.d/ifx_load_cpe_mei_drv.sh

	$(INSTALL_DIR) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/alias_drv_mei_cpe.sh $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/inst_drv_mei_cpe.sh $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/ifx_mei_cpe_drv_init.sh $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/enable_ARC_JTAG.sh $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/mei_cpe_drv_test $(1)/$(LANTIQ_BIN_DIR)

ifeq ($(CONFIG_ifx-dsl-cpe-mei-vrx_USE_KERNEL_BUILD_IN),y)
	$(INSTALL_DIR) $(1)/$(FW_DIR)
	$(CP) $(PKG_BUILD_DIR)/src/$(PKG_BASE_NAME).o $(LINUX_DIR)/drivers/char/
else
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/$(PKG_BASE_NAME)$(PKG_MODULE_EXTENSION) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_DIR) $(1)/$(MODULES_SUBDIR)
	ln -snf /$(LANTIQ_BIN_DIR)/$(PKG_BASE_NAME)$(PKG_MODULE_EXTENSION) $(1)/$(MODULES_SUBDIR)/$(PKG_BASE_NAME).$(LINUX_KMOD_SUFFIX)

	$(INSTALL_DIR) $(1)/$(FW_DIR)
endif
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
$(eval $(call KernelPackage,$(PKG_NAME)))
