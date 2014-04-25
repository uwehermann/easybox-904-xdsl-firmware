#
# Openwrt Makefile for ifx-dect
#

#### Includes ###################################
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

#### Package Info ###############################
PKG_NAME:=ifx-dect
PKG_VERSION:=3.2.0.9
PKG_RELEASE:=1
#PKG_MD5SUM:=27a19252419b288fd35382397e65c1ed
#PKG_MD5SUM:=e971a9b71a0efe261276283264f7d8df
PKG_MD5SUM:=35ca26ca348703430789e9e1a9ee4981
PKG_BASE_NAME:=ifx_dect
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_START_SCRIPT:=ifx_load_dect_drivers.sh
PKG_SOURCE_URL:=http://localhost
PKG_NO_AUTORECONF:=y
PKG_DEFAULT_CUSTOM_SOURCE_DIR:=$(TOPDIR)/../../../repo_dect_source/
include $(INCLUDE_DIR)/package.mk
# include $(INCLUDE_DIR)/package-version-override.mk

define Package/$(PKG_NAME)
	SUBMENU:=Utilities
	SECTION:=utils
	CATEGORY:=Lantiq
	TITLE:=Lantiq Dect Sub Sytem Package for GW 4.2 onwards
	URL:=http://www.lantiq.com
	MAINTAINER:=Lantiq
	DEPENDS:=+ifx-voice-cpe-tapi +ifx-voip-common @TARGET_ltqcpe_platform_ar9||TARGET_ltqcpe_platform_danube||TARGET_ltqcpe_platform_vr9
	MENU:=1
endef

define Package/$(PKG_NAME)/description
	This ifx-dect package is DECT subsytem used to realize DECT & VoIP functionality on GW 4.2 onwards.
endef

define KernelPackage/dect_usif_spi
  SUBMENU:=LTQCPE support
  TITLE:=Enable USIP_SPI Support in kernel for DECT-SS
	DEPENDS:=@TARGET_ltqcpe_platform_vr9
  KCONFIG:= \
		CONFIG_IFX_USIF_SPI=y \
		CONFIG_IFX_USIF_SPI_ASYNCHRONOUS=y 
endef

define KernelPackage/dect_usif_spi/description
	This is used to enable USIF_SPI module in the kernel when DECT-SS uses USIF_SPI
endef



#### Directory Paths ############################

IFX_UBOOT_DIR:=$(KERNEL_BUILD_DIR)/open_uboot/open_uboot
IFX_TAPI_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_tapi*)
IFX_VMMC_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_vmmc*)
#DSL_API_DIR:=$(wildcard $(BUILD_DIR)/drv_dsl_cpe_api*)
#DSL_CONTROL_DIR:=$(wildcard $(BUILD_DIR)/dsl_cpe_control*)
DSL_API_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_dsl_cpe_api*)
#DSL_CONTROL_DIR:=$(wildcard $(KERNEL_BUILD_DIR)/dsl_cpe_control*)
DRV_TEREDIAN_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_ter*)
VOIP_COMMON_DIR:=$(wildcard $(BUILD_DIR)/ifx_voip_common*)
MAPI_DIR:=$(wildcard $(STAGING_DIR)/usr/include/ifx_IFXAPIs)

export UBOOT_DIR=$(IFX_UBOOT_DIR)
export IFX_DRV_TEREDIAN_DIR=$(DRV_TEREDIAN_DIR)
export TAPI_DIR=$(IFX_TAPI_DIR)
export VMMC_DIR=$(IFX_VMMC_DIR)
export IFX_PRODUCT_ROOT=$(PKG_BUILD_DIR)
export IFX_MAPI_DIR=$(MAPI_DIR)
export IFX_DSL_API_DIR=$(DSL_API_DIR)
#export IFX_DSL_CONTROL_DIR=$(DSL_CONTROL_DIR)
export IFX_HTTPD_DIR=$(wildcard $(STAGING_DIR)/usr/include/ifx_httpd)
export IFX_OAM_DIR=$(BUILD_DIR)/ifx_oam
export IFX_STAGING_DIR=$(STAGING_DIR)
export CONFIG_FEATURE_IFX_TR104
export IFX_VOIP_COMMON_DIR=$(VOIP_COMMON_DIR)

### Menuconfig ##################################
define Package/$(PKG_NAME)/config
	$(call Package/$(PKG_NAME)/override_version)
	$(call Package/$(PKG_NAME)/override_source_path)
	menu "DECT features"
		depends on PACKAGE_$(PKG_NAME)
		source "$(SOURCE)/config/Config.in.dect"
	endmenu
#	menu "VoIP features"
#		depends on PACKAGE_$(PKG_NAME)
#		source "$(SOURCE)/config/Config.in.voip"
#	endmenu
	##### 20100830 ctc added to support DT 723 project ######
	choice
		depends on PACKAGE_$(PKG_NAME)
		prompt "Target board"
		default $(PKG_NAME)_DT_VOX2
	config $(PKG_NAME)_XRX200
		bool "XRX-200 EVM"
	config $(PKG_NAME)_DT_723
		bool "DT 723 model, including DECT, SLIC and USB"
	config $(PKG_NAME)_VOX2
		bool "VOX 2.0 model, including DECT, SLIC and USB"
	endchoice
    ###################################################
endef


ifeq ($(CONFIG_IFX_DECT_SS_CATIQ_2.0),y)
CFLAGS +=  -DCAT_IQ2_0
endif


### Configurations depends on Menuconfig ########
IFX_CFLAGS_y:=
IFX_LDFLAGS_y:=

IFX_CFLAGS_y +=$(TARGET_CFLAGS) -Wall -Werror

IFX_CFLAGS_y += -DIFX_CONFIG_MEMORY_SIZE=$(CONFIG_IFX_CONFIG_MEMORY_SIZE) -DIFX_CONFIG_FLASH_SIZE=${CONFIG_IFX_CONFIG_FLASH_SIZE} -DCPU_${CONFIG_IFX_CONFIG_CPU} -DCONFIG_AMAZON_S

    ##### 20100726 ctc added to support DT 723 model ######
ID_DIR=XX
ifeq ($(CONFIG_$(PKG_NAME)_XRX200),y)
	ID_DIR=src.xrx200
endif
ifeq ($(CONFIG_$(PKG_NAME)_DT_723),y)
	ID_DIR=src.723
endif
ifeq ($(CONFIG_$(PKG_NAME)_VOX2),y)
	ID_DIR=src.vox2
endif
    ###################################################

IFX_CFLAGS:=$(IFX_CFLAGS_y)
IFX_LDFLAGS:=$(IFX_LDFLAGS_y)

EXTRA_CFLAGS :=  -I$(IFX_PRODUCT_ROOT)/dect_ifx_pagebutton_drv
EXTRA_CFLAGS += -I$(KERNEL)/include 

CFLAGS +=  -DLINUX -Wall -DDECT_DEMO -O2 -g -Wunused -Wpointer-arith   

ifdef CONFIG_TARGET_ltqcpe_platform_ar9
CFLAGS += -DCONFIG_AR9
endif

ifdef CONFIG_TARGET_ltqcpe_platform_danube 
CFLAGS += -DCONFIG_DANUBE
endif

ifdef CONFIG_TARGET_ltqcpe_platform_vr9 
CFLAGS += -DCONFIG_VR9
ifdef CONFIG_IFX_DECT_SS_FEATURE_USIF_SPI
CFLAGS += -DDECT_USE_USIF
endif
endif

ifdef CONFIG_TARGET_ltqcpe_platform_amazon_se
CFLAGS += -DCONFIG_AMAZON_S
endif
ifeq ($(CONFIG_IFX_MODEL_NAME),"ARX182_GW_EL_FXS_DECT")
FW_PATH=ramdisk_copy/flash/
CFLAGS += -DCOSIC_BMC_FW_ON_RAM
else
FW_PATH=opt/ifx/downloads/
endif

export CONFIGURE_ARGS

#### Target Rules ###############################
define Build/Prepare
$(call Build/Prepare/Default)
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/lib
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/$(PKG_NAME)
	$(INSTALL_DIR) $(STAGING_DIR)/usr/lib

    ##### 20100824 ctc added to add debug msg ######
	if [ -d "./src" ] ; then \
		$(CP) ./src/*  $(PKG_BUILD_DIR)/. ; \
	fi
    ###################################################
    ##### 20100824 ctc added to support 723 model ######
	if [ -d "./$(ID_DIR)" ] ; then \
		$(CP) ./$(ID_DIR)/* $(PKG_BUILD_DIR)/. ; \
	fi
    ###################################################
endef


define Build/Compile

	echo $(CFLAGS)

	$(MAKE) ARCH=$(LINUX_KARCH) $(TARGET_CONFIGURE_OPTS) -C $(LINUX_DIR) M=$(IFX_PRODUCT_ROOT)/dect_ifx_timer modules
	cp -rf $(IFX_PRODUCT_ROOT)/dect_ifx_timer/drv_timer.ko $(IFX_PRODUCT_ROOT)/lib/
#cp -f `find $(PKG_BUILD_DIR)/dect_ifx_timer -follow -type f -iname "*.h"` $(STAGING_DIR)/usr/include/$(PKG_NAME)/

	$(MAKE) ARCH=$(LINUX_KARCH) $(TARGET_CONFIGURE_OPTS) -C $(LINUX_DIR) M=$(IFX_PRODUCT_ROOT)/dect_ifx_pagebutton_drv modules IFX_CFLAGS="$(IFX_CFLAGS)"
	cp -rf $(IFX_PRODUCT_ROOT)/dect_ifx_pagebutton_drv/danube_paging.ko $(IFX_PRODUCT_ROOT)/lib/paging.ko

	$(MAKE) ARCH=$(LINUX_KARCH) $(TARGET_CONFIGURE_OPTS) -C $(LINUX_DIR) M=$(IFX_PRODUCT_ROOT)/dect_ifx_cosic_driver modules IFX_CFLAGS="$(CFLAGS)" IFX_VOIP_DECT_COMMON_DIR=$(VOIP_COMMON_DIR) IFX_MAPI_DIR=$(IFX_MAPI_DIR) TAPI_DIR=$(TAPI_DIR) KBUILD_VERBOSE=1
	cp -rf $(IFX_PRODUCT_ROOT)/dect_ifx_cosic_driver/cosic.ko $(IFX_PRODUCT_ROOT)/lib/drv_dect.ko
#cp -f `find $(PKG_BUILD_DIR)/dect_ifx_cosic_driver -follow -type f -iname "*.h"` $(STAGING_DIR)/usr/include/$(PKG_NAME)/

	if test -d $(IFX_PRODUCT_ROOT)/dect_ifx_dsaa ; then \
		$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(IFX_PRODUCT_ROOT)/dect_ifx_dsaa all \
		IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" ; \
		cp $(IFX_PRODUCT_ROOT)/dect_ifx_dsaa/DSAA.obj $(IFX_PRODUCT_ROOT)/lib;\
		cp $(IFX_PRODUCT_ROOT)/dect_ifx_dsaa/DSAA.obj $(IFX_PRODUCT_ROOT)/dect_ifx_catiq_stack/DSAA_OBJS ;\
	fi;

	if test -d $(IFX_PRODUCT_ROOT)/dect_ifx_stack_toolkit/make; then \
		$(MAKE) LINUX_DIR=$(LINUX_DIR) $(TARGET_CONFIGURE_OPTS) -C $(IFX_PRODUCT_ROOT)/dect_ifx_stack_toolkit/make \
		-I $(IFX_PRODUCT_ROOT)/dect_ifx_stack_toolkit/stack_h -I $(BUILD_DIR)/ifx_voip_common/ifx_common/h all \
		IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" \
		DTK_ROOT_DIR=$(IFX_PRODUCT_ROOT)/dect_ifx_stack_toolkit/  IFX_VOIP_DECT_COMMON_DIR=$(IFX_VOIP_COMMON_DIR); \
	fi;
#cp -f `find $(PKG_BUILD_DIR)/dect_ifx_stack_toolkit -follow -type f -iname "*.h"` $(STAGING_DIR)/usr/include/$(PKG_NAME)/

	if test -f $(IFX_PRODUCT_ROOT)/dect_ifx_catiq_stack/Makefile; then \
		$(MAKE) LINUX_DIR=$(LINUX_DIR) $(TARGET_CONFIGURE_OPTS) -C $(IFX_PRODUCT_ROOT)/dect_ifx_catiq_stack all \
		IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" ; \
	fi;
#cp -f `find $(PKG_BUILD_DIR)/dect_ifx_catiq_stack -follow -type f -iname "*.h"` $(STAGING_DIR)/usr/include/$(PKG_NAME)/

	cp -f $(IFX_PRODUCT_ROOT)/lib/DSAA.obj $(STAGING_DIR)/usr/lib/ ;\
	cp -f $(IFX_PRODUCT_ROOT)/lib/libdtk.a $(STAGING_DIR)/usr/lib/ ;\
	cp -f $(IFX_PRODUCT_ROOT)/lib/libdectstack.a $(STAGING_DIR)/usr/lib/ ;\

endef

define Package/$(PKG_NAME)/install
#$(INSTALL_BIN) -p $(shell find $(PKG_BUILD_DIR)/dect_ifx_timer -follow -type f -iname "*.h") $(STAGING_DIR)/usr/include/$(PKG_NAME)/
	cp -pf `find $(PKG_BUILD_DIR)/dect_ifx_timer -follow -type f -iname "*.h"` $(STAGING_DIR)/usr/include/$(PKG_NAME)/
#$(INSTALL_BIN) -p $(shell find $(PKG_BUILD_DIR)/dect_ifx_cosic_driver -follow -type f -iname "*.h") $(STAGING_DIR)/usr/include/$(PKG_NAME)/
	cp -pf `find $(PKG_BUILD_DIR)/dect_ifx_cosic_driver -follow -type f -iname "*.h"` $(STAGING_DIR)/usr/include/$(PKG_NAME)/
#$(INSTALL_BIN) -p $(shell find $(PKG_BUILD_DIR)/dect_ifx_stack_toolkit -follow -type f -iname "*.h") $(STAGING_DIR)/usr/include/$(PKG_NAME)/
	cp -pf `find $(PKG_BUILD_DIR)/dect_ifx_stack_toolkit -follow -type f -iname "*.h"` $(STAGING_DIR)/usr/include/$(PKG_NAME)/
#$(INSTALL_BIN) -p $(shell find $(PKG_BUILD_DIR)/dect_ifx_catiq_stack -follow -type f -iname "*.h") $(STAGING_DIR)/usr/include/$(PKG_NAME)/
	cp -pf `find $(PKG_BUILD_DIR)/dect_ifx_catiq_stack -follow -type f -iname "*.h"` $(STAGING_DIR)/usr/include/$(PKG_NAME)/
	$(INSTALL_DIR) $(1)/usr/drivers
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/$(FW_PATH)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/drv_timer.ko  $(1)/usr/drivers
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/drv_dect.ko   $(1)/usr/drivers
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/paging.ko  $(1)/usr/drivers/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dect_ifx_catiq_stack/COSICFw.BIN  $(1)/$(FW_PATH)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dect_ifx_catiq_stack/BMCFw.BIN  $(1)/$(FW_PATH)
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_START_SCRIPT) $(1)/etc/init.d/
	echo "DECT Sub System    $(PKG_VERSION)"  >>  $(1)/usr/sbin/DECT_VERSION
endef


define Build/Clean

	$(MAKE) -C $(LINUX_DIR) M=$(IFX_PRODUCT_ROOT)/dect_ifx_timer clean

	$(MAKE) -C $(LINUX_DIR) M=$(IFX_PRODUCT_ROOT)/dect_ifx_pagebutton_drv clean

	$(MAKE) -C $(LINUX_DIR) M=$(IFX_PRODUCT_ROOT)/dect_ifx_cosic_driver clean

	if test -d $(IFX_PRODUCT_ROOT)/dect_ifx_dsaa ; then \
		$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(IFX_PRODUCT_ROOT)/dect_ifx_dsaa clean \
		IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" ; \
	fi;

	if test -d $(IFX_PRODUCT_ROOT)/dect_ifx_stack_toolkit/make; then \
		$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(IFX_PRODUCT_ROOT)/dect_ifx_stack_toolkit/make \
		-I $(IFX_PRODUCT_ROOT)/dect_ifx_stack_toolkit/stack_h clean \
		IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" \
		DTK_ROOT_DIR=$(IFX_PRODUCT_ROOT)/dect_ifx_stack_toolkit/ ; \
	fi;

	if test -f $(IFX_PRODUCT_ROOT)/dect_ifx_catiq_stack/Makefile; then \
		$(MAKE) LINUX_DIR=$(LINUX_DIR) $(TARGET_CONFIGURE_OPTS) -C $(IFX_PRODUCT_ROOT)/dect_ifx_catiq_stack clean \
		IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)" ; \
	fi;
	rm -rf $(IFX_PRODUCT_ROOT)/lib/*

endef
$(eval $(call BuildPackage,$(PKG_NAME)))
$(eval $(call KernelPackage,dect_usif_spi))
