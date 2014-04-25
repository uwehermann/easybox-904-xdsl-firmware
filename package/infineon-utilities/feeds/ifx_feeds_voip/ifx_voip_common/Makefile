#
# Openwrt Makefile for ifx-voip-common
#

#### Includes ###################################
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

#### Package Info ###############################
PKG_NAME:=ifx-voip-common
PKG_VERSION:=2.3.1.8
PKG_RELEASE:=1
#PKG_MD5SUM:=27a19252419b288fd35382397e65c1ed
PKG_MD5SUM:=6e178797831b2d876b9b9cf0e9770dde
PKG_BASE_NAME:=ifx_voip_common
PKG_START_SCRIPT:=ifx_load_voip_common_drivers.sh
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=http://localhost
PKG_NO_AUTORECONF:=y
PKG_DEFAULT_CUSTOM_SOURCE_DIR:=$(TOPDIR)/../../../repo_voip_source/
include $(INCLUDE_DIR)/package.mk
# include $(INCLUDE_DIR)/package-version-override.mk

define Package/$(PKG_NAME)
	SUBMENU:=Utilities
	SECTION:=utils
	CATEGORY:=Lantiq
	TITLE:=Lantiq common package for Dect & VoIP Subsystem on UGW 4.2 onwards
	URL:=http://www.lantiq.com
	MAINTAINER:=Lantiq
#	MENU:=1
endef

define Package/$(PKG_NAME)/description
	This ifx-voip-common package is Lantiq common package for Dect & VoIP Subsystem on UGW 4.2 onwards
endef

#### Directory Paths ############################

IFX_UBOOT_DIR:=$(KERNEL_BUILD_DIR)/open_uboot/open_uboot
IFX_TAPI_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_tapi*)
#IFX_TAPI_DIR:=$(wildcard $(BUILD_DIR)/drv_tapi*)
IFX_VMMC_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_vmmc*)
#IFX_VMMC_DIR:=$(wildcard $(BUILD_DIR)/drv_vmmc*)
DSL_API_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_dsl_cpe_api*)
#DSL_CONTROL_DIR:=$(wildcard $(KERNEL_BUILD_DIR)/dsl_cpe_control*)
#DSL_API_DIR:=$(wildcard $(BUILD_DIR)/drv_dsl_cpe_api*)
#DSL_CONTROL_DIR:=$(wildcard $(BUILD_DIR)/dsl_cpe_control*)
DRV_TEREDIAN_DIR:=$(wildcard $(STAGIND_DIR)/usr/include/drv_ter*)
#DRV_TEREDIAN_DIR:=$(wildcard $(BUILD_DIR)/drv_ter*)

export UBOOT_DIR=$(IFX_UBOOT_DIR)
export IFX_DRV_TEREDIAN_DIR=$(DRV_TEREDIAN_DIR)
export TAPI_DIR=$(IFX_TAPI_DIR)
export VMMC_DIR=$(IFX_VMMC_DIR)
export IFX_PRODUCT_ROOT=$(PKG_BUILD_DIR)
export IFX_MAPI_DIR:=$(wildcard $(STAGING_DIR)/usr/include/ifx_IFXAPIs)
export IFX_DSL_API_DIR=$(DSL_API_DIR)
#export IFX_DSL_API_DIR=$(BUILD_DIR)/drv_dsl_cpe_api
#export IFX_DSL_CONTROL_DIR=$(DSL_CONTROL_DIR)
export IFX_HTTPD_DIR=$(wildcard $(STAGING_DIR)/usr/include/ifx_httpd)
export IFX_OAM_DIR=$(BUILD_DIR)/ifx_oam
export IFX_STAGING_DIR=$(STAGING_DIR)
export CONFIG_FEATURE_IFX_TR104


### Menuconfig ##################################
define Package/$(PKG_NAME)/config
$(call Package/$(PKG_NAME)/override_version)
$(call Package/$(PKG_NAME)/override_source_path)
#	menu "DECT features"
#		depends on PACKAGE_$(PKG_NAME)
#		source "$(SOURCE)/config/Config.in.dect"
#	endmenu
#	menu "VoIP features"
#		depends on PACKAGE_$(PKG_NAME)
#		source "$(SOURCE)/config/Config.in.voip"
#	endmenu
endef

#ifeq ($(CONFIG_IFX_DRV_TAPI_EVENT_LOGGER),y)
#CONFIGURE_ARGS += --enable-el-debug
#endif

CFLAGS += -Werror
#ifeq ($(CONFIG_IFX_DECT_SS_CATIQ_2.0),y)
#CFLAGS +=  -DCAT_IQ2_0
#endif

#ifeq ($(CONFIG_IFX_VOIP_PRACK),y)
#CFLAGS +=  -DLINUX
#export PRACK_SUPPORT  1
#endif

### Configurations depends on Menuconfig ########
IFX_CFLAGS_y:=
IFX_LDFLAGS_y:=

IFX_CFLAGS_y +=$(TARGET_CFLAGS) -Wall -Werror

IFX_CFLAGS_y += -DIFX_CONFIG_MEMORY_SIZE=$(CONFIG_IFX_CONFIG_MEMORY_SIZE) -DIFX_CONFIG_FLASH_SIZE=${CONFIG_IFX_CONFIG_FLASH_SIZE} -DCPU_${CONFIG_IFX_CONFIG_CPU} -DCONFIG_AMAZON_S

IFX_CFLAGS:=$(IFX_CFLAGS_y)
IFX_LDFLAGS:=$(IFX_LDFLAGS_y)

#EXTRA_CFLAGS :=  -I$(IFX_PRODUCT_ROOT)/ifx_pagebutton_drv
#EXTRA_CFLAGS += -I$(KERNEL)/include 

#CFLAGS +=  -DLINUX -DDECT_DEMO -O2 -g -Wunused -Wpointer-arith 

#ifdef CONFIG_TARGET_ifxcpe_platform_ar9
#CFLAGS += -DCONFIG_AR9
#CFLAGS += -DIFX_T38_FW
#CFLAGS += -DTEREDIAN
#CFLAGS += -DENABLE_FXO
#CFLAGS += -I${IFX_DRV_TEREDIAN_DIR}
#endif

#ifdef CONFIG_TARGET_ifxcpe_platform_danube 
#CFLAGS += -DCONFIG_DANUBE
#endif

#ifdef CONFIG_TARGET_ifxcpe_platform_vr9 
#CFLAGS += -DCONFIG_VR9
#CFLAGS += -DIFX_T38_FW
#CFLAGS += -DTEREDIAN
#CFLAGS += -DENABLE_FXO
#CFLAGS += -I${IFX_DRV_TEREDIAN_DIR}
#endif

#ifdef CONFIG_TARGET_ifxcpe_platform_amazon_se
#CFLAGS += -DCONFIG_AMAZON_S
#endif

#export CONFIGURE_ARGS

#### Target Rules ###############################
define Build/Prepare
$(call Build/Prepare/Default)
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/bin
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/lib
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/$(PKG_NAME)
	$(INSTALL_DIR) $(STAGING_DIR)/usr/lib
endef


define Build/Compile
	$(MAKE) ARCH=$(LINUX_KARCH) $(TARGET_CONFIGURE_OPTS) -C $(LINUX_DIR) M=$(IFX_PRODUCT_ROOT)/voip_ifx_timer_driver modules
	$(MAKE) LINUX_DIR="$(LINUX_DIR)" $(TARGET_CONFIGURE_OPTS) -C $(PKG_BUILD_DIR)/voip_ifx_common/make/ all IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)"
	cp -f $(IFX_PRODUCT_ROOT)/lib/* $(STAGING_DIR)/usr/lib
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/lib/modules
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_START_SCRIPT) $(1)/etc/init.d/
	cp -f `find $(PKG_BUILD_DIR)/voip_ifx_common/ -follow -type f -name "*.h"` $(STAGING_DIR)/usr/include/$(PKG_NAME)/
endef


define Build/Clean
	if test -d $(IFX_PRODUCT_ROOT)/voip_ifx_common/make; then \
	$(MAKE) LINUX_DIR="$(LINUX_DIR)" $(TARGET_CONFIGURE_OPTS) -C $(PKG_BUILD_DIR)/voip_ifx_common/make/ clean IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)"; \
	fi;
	if test -d $(IFX_PRODUCT_ROOT)/voip_ifx_timer_driver; then \
	$(MAKE) -C $(LINUX_DIR) M=$(IFX_PRODUCT_ROOT)/voip_ifx_timer_driver clean ;\
	fi;
	 rm -rf $(IFX_PRODUCT_ROOT)/lib/*
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
