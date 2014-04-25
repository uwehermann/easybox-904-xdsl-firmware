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

PKG_NAME:=ifx-voice-cpe-vmmc
PKG_VERSION:=1.10.2.2
PKG_MD5SUM:=e6e562872a4ce7fa00fa962909eef50d
PKG_RELEASE:=1

# IMPORTANT - include the version-override here after PKG_VERSION definition
# include $(INCLUDE_DIR)/package-version-override.mk
# IMPORTANT - ... and before using PKG_VERSION below

PKG_SOURCE:=drv_vmmc-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://localhost
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/drv_vmmc-$(PKG_VERSION)
PKG_START_SCRIPT:=ltq_load_voice_vmmc.sh
PKG_BUILD_DEPENDS:=ifx-os ifx-voice-cpe-tapi \
                   PACKAGE_ifx-evtlog:ifx-evtlog
IFX_BIN_DIR=opt/ifx/bin
LANTIQ_BIN_DIR=opt/lantiq/bin

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=Kernel Module
	SECTION:=libs
	CATEGORY:=Lantiq
	TITLE:=TAPI LL driver for Voice Macro
	URL:=http://www.lantiq.com/
	MAINTAINER:=Lantiq Deutschland GmbH
	DEPENDS:=ifx-voice-cpe-tapi  ifx-os
	MENU:=1
endef

define Package/$(PKG_NAME)/description
	Voice Subsystem Low Level Driver for Danube, AR9, VR9 device families
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
	config $(PKG_NAME)_904DSL
		bool "904 DSL model, including SLIC and USB"
	endchoice
    ###################################################
endef

    ##### 20100726 ctc added to support DT 723 model ######
IVCV_DIR=XX
ifeq ($(CONFIG_$(PKG_NAME)_XRX200),y)
	IVCV_DIR=src.xrx200
endif
ifeq ($(CONFIG_$(PKG_NAME)_DT_723),y)
	IVCV_DIR=src.dt723
endif
ifeq ($(CONFIG_$(PKG_NAME)_904DSL),y)
	IVCV_DIR=src.904dsl
endif
    ###################################################

ifeq ($(CONFIG_LTQ_VOICE_CPE_VMMC_EVENT_LOGGER),y)
CONFIGURE_ARGS += --enable-el_debug --with-cflags=-I$(STAGING_DIR)/usr/include/event_logger
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_VMMC_PMC),y)
CONFIGURE_ARGS += --enable-pmc
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_VMMC_PMU),y)
CONFIGURE_ARGS += --enable-pmu
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_VMMC_DISABLE_DECT_NIBBLE_SWAP),y)
CONFIGURE_ARGS += --disable-dect-nibble-swap
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_VMMC_PHONE_DETECTION),y)
CONFIGURE_ARGS += --enable-phone-detection
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_VMMC_CONT_MEASUREMENT),y)
CONFIGURE_ARGS += --enable-cont-measurement
endif

ifneq ($(CONFIG_LTQ_VOICE_CPE_VMMC_MPS_HISTORY_SIZE),128)
CONFIGURE_ARGS += --enable-history-buf=$(CONFIG_LTQ_VOICE_CPE_VMMC_MPS_HISTORY_SIZE)
endif

### ctc ###
WITH_DEVICE=
###########

ifeq ($(CONFIG_LTQ_VOICE_CPE_VMMC_WITH_DEVICE_DANUBE),y)
CONFIGURE_ARGS += --with-device=DANUBE
### ctc ###
WITH_DEVICE=y
###########
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_VMMC_WITH_DEVICE_AR9),y)
CONFIGURE_ARGS += --with-device=AR9
### ctc ###
WITH_DEVICE=y
###########
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_VMMC_WITH_DEVICE_VR9),y)
CONFIGURE_ARGS += --with-device=VR9
### ctc ###
WITH_DEVICE=y
###########
endif

### ctc ###
ifneq ($(WITH_DEVICE),y)
CONFIGURE_ARGS += --with-device=VR9
endif
###########

ifeq ($(CONFIG_LTQ_VOICE_CPE_VMMC_SSLIC_FXO_ACTIVATE),y)
CONFIGURE_ARGS += --enable-fxo-act
else
CONFIGURE_ARGS += --disable-fxo-act
endif


# configure options derived from TAPI configuration

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_FAX),y)
# it should be save to enable-t38 for all platforms here, the configure/driver
# will ignore the --enable-t38 on older platforms
CONFIGURE_ARGS += --enable-fax --enable-t38
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

#ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_QOS),y)
#CONFIGURE_ARGS += --enable-qos
#else
#CONFIGURE_ARGS += --disable-qos
#endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_LT_GR909),y)
CONFIGURE_ARGS += --enable-lt --enable-calibration
else
CONFIGURE_ARGS += --disable-lt
endif

#ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_STATISTICS),y)
#CONFIGURE_ARGS += --enable-statistics
#else
#CONFIGURE_ARGS += --disable-statistics
#endif

#ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_METERING),y)
#CONFIGURE_ARGS += --enable-metering
#else
#CONFIGURE_ARGS += --disable-metering
#endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_HDLC),y)
CONFIGURE_ARGS += --enable-hdlc --enable-hdlc-idle-patt=0xff
else
CONFIGURE_ARGS += --disable-hdlc
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPI_TRACES),y)
CONFIGURE_ARGS += --enable-trace
else
CONFIGURE_ARGS += --disable-trace
endif


DRV_TAPI_INCLUDE:=$(STAGING_DIR)/usr/include/drv_tapi

CONFIGURE_ARGS += --with-kernel-incl="$(LINUX_DIR)/include"
CONFIGURE_ARGS += --with-tapi-incl="$(DRV_TAPI_INCLUDE)"
CONFIGURE_ARGS += --with-ifxos-incl=$(STAGING_DIR)/usr/include/ifxos

ifneq ($(CONFIG_LTQ_VOICE_CPE_VMMC_ADDITIONAL_FLAGS),"")
CONFIGURE_ARGS += \
   $(subst ",, $(CONFIG_LTQ_VOICE_CPE_VMMC_ADDITIONAL_FLAGS))
endif

ifeq ($(CONFIG_LINUX_2_6),y)
CONFIGURE_ARGS += --enable-linux-26 --with-kernel-build="$(LINUX_DIR)" ARCH=$(LINUX_KARCH)
PKG_MODULE_EXTENSION=.ko
endif

#define Build/Configure
#	@echo "#################### Tapi SUBTARGET: $(SUBTARGET)"
#	$(call Build/Configure/Default)
#endef

define Build/Prepare
	$(call Build/Prepare/Default)
    ##### 20100726 ctc added to support 723 model ######
	if [ -d "./$(IVCV_DIR)" ] ; then \
		$(CP) ./$(IVCV_DIR)/* $(PKG_BUILD_DIR)/. ; \
	fi
    ###################################################
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	mkdir -p $(1)/usr/include/drv_vmmc
	$(CP) -v --dereference $(PKG_BUILD_DIR)/include/* $(1)/usr/include/drv_vmmc
	(cd $(1)/usr/include/drv_vmmc && ln -snf . include)
endef

define Build/UninstallDev
	rm -rf $(BUILD_DIR)/drv_vmmc
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
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/drv_vmmc$(PKG_MODULE_EXTENSION) $(1)/$(LANTIQ_BIN_DIR)
	$(INSTALL_DIR) $(1)/$(MODULES_SUBDIR)
	ln -snf /$(LANTIQ_BIN_DIR)/drv_vmmc$(PKG_MODULE_EXTENSION) $(1)/$(MODULES_SUBDIR)/drv_vmmc.$(LINUX_KMOD_SUFFIX)
	(cd $(1)/$(IFX_BIN_DIR); ln -snf ../../lantiq/bin/drv_vmmc.$(LINUX_KMOD_SUFFIX) .)
	$(INSTALL_DIR) $(1)/$(IFX_DWLD_DIR)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
