#
# Openwrt Makefile for ifx-voip
#

#### Includes ###################################
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

#### Package Info ###############################
PKG_NAME:=ifx-voip
PKG_VERSION:=2.4.0.15
PKG_RELEASE:=1
#PKG_MD5SUM:=27a19252419b288fd35382397e65c1ed
PKG_MD5SUM:=39a95fae1042489893cb016b2e05adf7
PKG_BASE_NAME:=ifx_voip
PKG_SOURCE:=$(PKG_BASE_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_BASE_NAME)-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=PACKAGE_teridian-voice-daa-ter1x66:teridian-voice-daa-ter1x66
PKG_SOURCE_URL:=http://localhost
PKG_FILE_DEPENDS:=$(PKG_BUILD_DIR)/.configured__*
PKG_NO_AUTORECONF:=y
PKG_DEFAULT_CUSTOM_SOURCE_DIR:=$(TOPDIR)/../../../repo_voip_source/
include $(INCLUDE_DIR)/package.mk
# include $(INCLUDE_DIR)/package-version-override.mk
LINK1:=ln -s $(TOPDIR)/../../../repo_dect_source/dect_ifx_agent $(TOPDIR)/../../../repo_voip_source/dect_ifx_agent
#to add .config feature dependency only for VoIP features
ifeq ($(DUMP),)
  STAMP_CONFIGURED:=$(strip $(STAMP_CONFIGURED))_$(shell grep -e CONFIG_IFX_VOIP_FAX -e CONFIG_IFX_VOIP_FXO -e CONFIG_IFX_DECT_SUPPORT $(TOPDIR)/.config | md5s)
	echo "STAMP_CONFIGURED=" $STAMP_CONFIGURED
endif

define Package/$(PKG_NAME)
	SUBMENU:=Utilities
	SECTION:=utils
	CATEGORY:=Lantiq
	TITLE:=Lantiq VoIP & Dect Sub Sytem Package for GW 4.2 onwards
	URL:=http://www.lantiq.com
	MAINTAINER:=Lantiq
	DEPENDS:=+ifx-voice-cpe-tapi +ifx-voice-cpe-vmmc +ifx-IFXAPIs @+FEATURE_IFX_VOIP +ifx-oam @+LTQ_VOICE_CPE_TAPI_DECT @TARGET_ltqcpe_platform_ar9||TARGET_ltqcpe_platform_danube||TARGET_ltqcpe_platform_vr9 +ifx-voice-cpe-tapi-kpi2udp +ifx-voip-common +ifx-dect +ifx-voice-cpe-libt38 
#	DEPENDS:=+ifx-voice-cpe-tapi +teridian-voice-daa-ter1x66 +ifx-voice-cpe-vmmc +ifx-IFXAPIs @+FEATURE_IFX_VOIP +ifx-oam @+IFX_VOICE_CPE_TAPI_DECT @TARGET_ltqcpe_platform_ar9||TARGET_ltqcpe_platform_danube||TARGET_ltqcpe_platform_vr9 +ifx-voice-cpe-tapi-kpi2udp +ifx-voip-common +ifx-dect +ifx-voice-cpe-libt38 
	MENU:=1
endef

define Package/$(PKG_NAME)/description
	This ifx-voip package is VoIP & DECT subsytem used to realize DECT & VoIP functionality on GW 4.2 onwards.
endef

#### Directory Paths ############################

IFX_UBOOT_DIR:=$(KERNEL_BUILD_DIR)/open_uboot/open_uboot
IFX_TAPI_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_tapi*)
IFX_VMMC_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_vmmc*)
DSL_API_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_dsl_cpe_api*)
#DSL_CONTROL_DIR:=$(wildcard $(KERNEL_BUILD_DIR)/dsl_cpe_control*)
#DSL_API_DIR:=$(wildcard $(BUILD_DIR)/drv_dsl_cpe_api*)
#DSL_CONTROL_DIR:=$(wildcard $(BUILD_DIR)/dsl_cpe_control*)
DRV_TEREDIAN_DIR:=$(wildcard $(STAGING_DIR)/usr/include/drv_ter*)
#MAPI_DIR:=$(wildcard $(BUILD_DIR)/ifx_IFXAPIs*)
MAPI_DIR:=$(wildcard $(STAGING_DIR)/usr/include/ifx_IFXAPIs) -DBUILD_FROM_LTQ_APPS
IFX_VOIP_COMMON_DIR:=$(wildcard $(BUILD_DIR)/ifx_voip_common*)

export UBOOT_DIR=$(IFX_UBOOT_DIR)
export IFX_DRV_TEREDIAN_DIR=$(DRV_TEREDIAN_DIR)
export TAPI_DIR=$(IFX_TAPI_DIR)
export VMMC_DIR=$(IFX_VMMC_DIR)
export IFX_PRODUCT_ROOT=$(PKG_BUILD_DIR)
export IFX_MAPI_DIR=$(MAPI_DIR)
export IFX_DSL_API_DIR=$(DSL_API_DIR)
#export IFX_DSL_CONTROL_DIR=$(DSL_CONTROL_DIR)
export IFX_HTTPD_DIR=$(wildcard $(STAGING_DIR)/usr/include/ifx_httpd*)
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
	menu "VoIP features"
		depends on PACKAGE_$(PKG_NAME)
		source "$(SOURCE)/config/Config.in.voip"
	endmenu
endef

### Configurations depends on Menuconfig ########
CFLAGS += -Werror 
ifdef CONFIG_LTQ_DT_FEATURES
CFLAGS += -DLTQ_DT_SUPPORT
endif
ifdef CONFIG_IFX_DECT_SUPPORT
CFLAGS += -DDECT_SUPPORT
export DECT_AGENT_DIR=1
endif
ifdef CONFIG_TARGET_ltqcpe_platform_ar9
CFLAGS += -DCONFIG_AR9
ifdef CONFIG_IFX_VOIP_FAX
#CFLAGS += -DFAX_SUPPORT
#CFLAGS += -DTAPI_FAX_T38
CFLAGS += -DIFX_T38_FW
endif
ifdef CONFIG_IFX_VOIP_FXO
ifdef CONFIG_IFX_GW188
CFLAGS += -DSLIC121
else
CFLAGS += -DTEREDIAN
CFLAGS += -I${IFX_DRV_TEREDIAN_DIR}
endif
CFLAGS += -DENABLE_FXO
endif
endif

ifdef CONFIG_TARGET_ltqcpe_platform_danube 
CFLAGS += -DCONFIG_DANUBE
ifdef CONFIG_IFX_VOIP_FAX
#CFLAGS += -DFAX_SUPPORT
export IFX_T38_LIB=1
endif
ifdef CONFIG_IFX_VOIP_FXO
ifdef CONFIG_IFX_GW188
CFLAGS += -DSLIC121
else
CFLAGS += -DTEREDIAN
CFLAGS += -I${IFX_DRV_TEREDIAN_DIR}
endif
CFLAGS += -DENABLE_FXO
endif
endif

ifdef CONFIG_TARGET_ltqcpe_platform_vr9 
CFLAGS += -DCONFIG_VR9
ifdef CONFIG_IFX_VOIP_FAX
#CFLAGS += -DFAX_SUPPORT
#CFLAGS += -DTAPI_FAX_T38
CFLAGS += -DIFX_T38_FW
endif
ifdef CONFIG_IFX_VOIP_FXO
CFLAGS += -DTEREDIAN
CFLAGS += -DENABLE_FXO
CFLAGS += -I${IFX_DRV_TEREDIAN_DIR}
endif

ifdef CONFIG_IFX_VOIP_FXO
#CFLAGS += -DTEREDIAN
#CFLAGS += -DENABLE_FXO
endif
endif

ifdef CONFIG_TARGET_ltqcpe_platform_amazon_se
CFLAGS += -DCONFIG_AMAZON_S
endif

ifeq ($(CONFIG_IFX_MODEL_NAME),"ARX182_GW_EL_FXS_DECT")
CFLAGS += -DCOSIC_BMC_FW_ON_RAM
endif
export CONFIGURE_ARGS

#### Target Rules ###############################
define Build/Prepare
	$(call Build/Prepare/Default)
#	ln -sf $(IFX_VOIP_COMMON_DIR)/ifx_common $(PKG_BUILD_DIR)/ifx_common
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/bin
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/lib
	### ctc ###
	if [ -d ./src ] ; then \
		$(CP) ./src/* $(PKG_BUILD_DIR)/. ; \
	fi
	###########
endef


define Build/Compile

	echo $(CFLAGS)

	if [ ! -d $(IFX_PRODUCT_ROOT)/dect_ifx_agent ] ; then \
	$(LINK1) ; \
	fi;
	if [ ! -d $(IFX_PRODUCT_ROOT)/voip_ifx_common ] ; then \
	ln -sf $(IFX_VOIP_COMMON_DIR)/voip_ifx_common $(PKG_BUILD_DIR)/voip_ifx_common ; \
	fi;

	$(MAKE) LINUX_DIR="$(LINUX_DIR)" $(TARGET_CONFIGURE_OPTS) -C $(PKG_BUILD_DIR)/voip_ifx_gateway_appln/ all IFX_CFLAGS="$(CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)"

	cp -f $(PKG_BUILD_DIR)/voip_ifx_gateway_appln/bin/ifxsip $(PKG_BUILD_DIR)/voip_ifx_gateway_appln/bin/ifxsip_wo_strip
	$(STRIP) $(PKG_BUILD_DIR)/voip_ifx_gateway_appln/bin/ifxsip 

#	cp -f $(IFX_PRODUCT_ROOT)/voip_ifx_gateway_appln/create_customer_package.sh  $(IFX_PRODUCT_ROOT)/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/lib
	$(INSTALL_DIR) $(1)/www
	$(INSTALL_DIR) $(1)/opt/ifx/downloads/ifx_country_settings

	cp -rf $(PKG_BUILD_DIR)/voip_ifx_country_settings/* $(1)/opt/ifx/downloads/ifx_country_settings
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/voip_ifx_webs/*  $(1)/www/

#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/libvmapimips-linux.so $(1)/lib

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/libxvmapimips-linux.so $(1)/lib

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/voip_ifx_gateway_appln/bin/ifxsip $(1)/usr/sbin
	echo "VoIP Sub System    $(PKG_VERSION)"  >>  $(1)/usr/sbin/VoIP_VERSION  
ifndef CONFIG_IFX_DECT_SUPPORT
	rm -f $(1)/www/*dect*.asp
	rm -f $(1)/www/diagnostics.asp 
	rm -f $(BUILD_DIR)/root-ltqcpe/www/*dect*.asp
	rm -f $(BUILD_DIR)/root-ltqcpe/www/diagnostics.asp 
endif
ifndef CONFIG_LTQ_DT_FEATURES
	if test -f $(BUILD_DIR)/root-ltqcpe/ramdisk_copy/flash/rssemail.txt; then \
		rm -f $(BUILD_DIR)/root-ltqcpe/ramdisk_copy/flash/rssemail.txt ; \
		rm -f $(BUILD_DIR)/root-ltqcpe/ramdisk_copy/flash/chanel*.txt; \
		rm -f $(BUILD_DIR)/root-ltqcpe/ramdisk_copy/flash/email*.txt; \
	fi;
endif
ifdef CONFIG_LTQ_DT_FEATURES
		cp -rf $(IFX_PRODUCT_ROOT)/dect_ifx_agent/config/*.txt $(BUILD_DIR)/root-ltqcpe/ramdisk_copy/flash/
endif

endef


define Build/Clean
	if test -f $(IFX_PRODUCT_ROOT)/voip_ifx_gateway_appln/Makefile; then \
	$(MAKE) LINUX_DIR="$(LINUX_DIR)" $(TARGET_CONFIGURE_OPTS) -C $(PKG_BUILD_DIR)/voip_ifx_gateway_appln/ clean IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)"; \
	fi;
	if test -d $(IFX_PRODUCT_ROOT)/voip_ifx_common/make; then \
        $(MAKE) LINUX_DIR="$(LINUX_DIR)" $(TARGET_CONFIGURE_OPTS) -C $(PKG_BUILD_DIR)/voip_ifx_common/make/ clean IFX_CFLAGS="$(IFX_CFLAGS)" IFX_LDFLAGS="$(IFX_LDFLAGS)"; \
        fi;
        if test -d $(IFX_PRODUCT_ROOT)/voip_ifx_timer_driver; then \
        $(MAKE) -C $(LINUX_DIR) M=$(IFX_PRODUCT_ROOT)/voip_ifx_timer_driver clean ;\
        fi;
	rm -rf $(IFX_PRODUCT_ROOT)/lib/*
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
