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

PKG_NAME:=ifx-voice-cpe-tapi
PKG_VERSION:=4.7.1.2
PKG_MD5SUM:=52e0ace8b7517ad362857a53222f1862
PKG_RELEASE:=1

# IMPORTANT - include the version-override here after PKG_VERSION definition
# include $(INCLUDE_DIR)/package-version-override.mk
# IMPORTANT - ... and before using PKG_VERSION below

PKG_SOURCE:=drv_tapi-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://localhost
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/drv_tapi-$(PKG_VERSION)
PKG_START_SCRIPT:=ltq_load_voice_tapi.sh
PKG_BUILD_DEPENDS:=ifx-os
IFX_BIN_DIR=opt/ifx/bin
LANTIQ_BIN_DIR=opt/lantiq/bin

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=Kernel Module
	SECTION:=libs
	CATEGORY:=Lantiq
	TITLE:=TAPI subsystem
	URL:=http://www.lantiq.com/
	MAINTAINER:=Lantiq Deutschland GmbH
	MENU:=1
	DEPENDS:=+ifx-os @TARGET_ifxcpe_platform_ar9||TARGET_ifxcpe_platform_vr9||TARGET_ifxcpe_platform_danube||TARGET_ltqcpe_platform_ar9||TARGET_ltqcpe_platform_vr9||TARGET_ltqcpe_platform_danube
endef

define Package/$(PKG_NAME)/description
	Voice Subsystem Telephony API High Level Driver
endef

define Package/$(PKG_NAME)/config
	$(call Package/$(PKG_NAME)/override_version)
	$(call Package/$(PKG_NAME)/override_source_path)
	source "$(SOURCE)/config/Config.in"
    ##### 20100726 ctc added to support DT 723 model ######
	choice
		depends on PACKAGE_$(PKG_NAME)
		prompt "Target board"
		default $(PKG_NAME)_DT_723
	config $(PKG_NAME)_XRX200
		bool "XRX-200 EVM"
	config $(PKG_NAME)_DT_723
		bool "DT 723 model, including DECT, SLIC and USB"
	endchoice
    ###################################################
endef

    ##### 20100726 ctc added to support DT 723 model ######
IVCT_DIR=XX
ifeq ($(CONFIG_$(PKG_NAME)_XRX200),y)
	IVCT_DIR=src.xrx200
endif
ifeq ($(CONFIG_$(PKG_NAME)_DT_723),y)
	IVCT_DIR=src.dt723
endif
    ###################################################

# set TAPI API interface
CONFIGURE_ARGS += --enable-tapi3

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_FAX),y)
# it should be save to enable-t38 for all platforms here, the configure/driver
# will ignore the --enable-t38 on older platforms
### ctc merged from charles ###
#CONFIGURE_ARGS += --enable-fax --enable-t38
CONFIGURE_ARGS += --disable-fax --enable-t38
###########
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_CID),y)
CONFIGURE_ARGS += --enable-cid
else
CONFIGURE_ARGS += --disable-cid
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_DECT),y)
CONFIGURE_ARGS += --enable-dect
else
CONFIGURE_ARGS += --disable-dect
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_KPI),y)
CONFIGURE_ARGS += --enable-kpi
else
CONFIGURE_ARGS += --disable-kpi
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_QOS),y)
CONFIGURE_ARGS += --enable-qos
else
CONFIGURE_ARGS += --disable-qos
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_LT_GR909),y)
CONFIGURE_ARGS += --enable-nlt
else
CONFIGURE_ARGS += --disable-nlt
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_STATISTICS),y)
CONFIGURE_ARGS += --enable-statistics
else
CONFIGURE_ARGS += --disable-statistics
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_METERING),y)
CONFIGURE_ARGS += --enable-metering
else
CONFIGURE_ARGS += --disable-metering
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_HDLC),y)
CONFIGURE_ARGS += --enable-hdlc
else
CONFIGURE_ARGS += --disable-hdlc
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_TRACES),y)
CONFIGURE_ARGS += --enable-trace
else
CONFIGURE_ARGS += --disable-trace
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_PHONE_DETECTION),y)
CONFIGURE_ARGS += --enable-phone-detection
else
CONFIGURE_ARGS += --disable-phone-detection
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_CONT_MEASUREMENT),y)
CONFIGURE_ARGS += --enable-cont-measurement
else
CONFIGURE_ARGS += --disable-cont-measurement
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_PMC),y)
CONFIGURE_ARGS += --enable-pmc
else
CONFIGURE_ARGS += --disable-pmc
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_EVENT_LOGGER),y)
CONFIGURE_ARGS += --enable-el-debug --with-el-incl=$(STAGING_DIR)/usr/include/event_logger
endif

CONFIGURE_ARGS += --with-kernel-incl="$(LINUX_DIR)/include"
CONFIGURE_ARGS += --with-ifxos-incl=$(STAGING_DIR)/usr/include/ifxos
CONFIGURE_ARGS += --with-ifxos-lib=$(STAGING_DIR)/usr/lib

ifneq ($(CONFIG_LTQ_VOICE_CPE_TAPI_ADDITIONAL_FLAGS),"")
CONFIGURE_ARGS += \
   $(subst ",, $(CONFIG_LTQ_VOICE_CPE_TAPI_ADDITIONAL_FLAGS))
endif

ifeq ($(CONFIG_LINUX_2_6),y)
CONFIGURE_ARGS += --enable-linux-26 --with-kernel-build="$(LINUX_DIR)" ARCH=$(LINUX_KARCH)
PKG_MODULE_EXTENSION=.ko
endif

#define Build/Configure
#	#@echo "#################### Tapi SUBTARGET: $(SUBTARGET)"
#	$(call Build/Configure/Default)
#endef

define Build/Prepare
	$(call Build/Prepare/Default)
    ##### 20100726 ctc added to support 723 model ######
	if [ -d "./src" ] ; then \
		$(CP) ./src/* $(PKG_BUILD_DIR)/. ; \
	fi
	if [ -d "./$(IVCT_DIR)" ] ; then \
		$(CP) ./$(IVCT_DIR)/* $(PKG_BUILD_DIR)/. ; \
	fi
    ###################################################
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	mkdir -p $(1)/usr/include/drv_tapi
	$(CP) --dereference $(PKG_BUILD_DIR)/include/* $(1)/usr/include/drv_tapi
	(cd $(1)/usr/include/drv_tapi && ln -s . include && ln -s ../ifxos/ifx_types.h .)
endef

define Build/UninstallDev
	rm -rf $(BUILD_DIR)/drv_tapi
endef

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
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/drv_tapi$(PKG_MODULE_EXTENSION) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_DIR) $(1)/$(MODULES_SUBDIR)
	ln -snf /$(LANTIQ_BIN_DIR)/drv_tapi$(PKG_MODULE_EXTENSION) $(1)/$(MODULES_SUBDIR)/drv_tapi.$(LINUX_KMOD_SUFFIX)
	(cd $(1)/$(IFX_BIN_DIR); ln -snf ../../lantiq/bin/drv_tapi.$(LINUX_KMOD_SUFFIX) .)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
