#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk
# re-define PATCH_DIR because kernel.mk will set it to kernel patches
PATCH_DIR:=./patches

PKG_NAME:=ifx-voice-cpe-tapidemo
PKG_VERSION:=5.1.1.0
PKG_MD5SUM:=ed4b55ddb405f23078502c20b6f0ec6f
PKG_RELEASE:=1

# IMPORTANT - include the version-override here after PKG_VERSION definition
# include $(INCLUDE_DIR)/package-version-override.mk
# IMPORTANT - ... and before using PKG_VERSION below

PKG_SOURCE:=tapidemo-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/tapidemo-$(PKG_VERSION)

PKG_BUILD_DEPENDS:=ifx-voice-cpe-tapi \
                   PACKAGE_ifx-voice-cpe-vmmc:ifx-voice-cpe-vmmc \
#                   PACKAGE_teridian-voice-daa-ter1x66:teridian-voice-daa-ter1x66 \
#                   PACKAGE_ifx-voice-cpe-tapi-duslic_xt:ifx-voice-cpe-tapi-duslic_xt
DRV_TAPI_INCLUDE:=$(STAGING_DIR)/usr/include
PKG_SOURCE_URL:=http://localhost
PKG_START_SCRIPT:=bringup_tapidemo
IFX_BIN_DIR=/opt/ifx/bin
LANTIQ_BIN_DIR=/opt/lantiq/bin
LANTIQ_FIRMWARE_DIR=/lib/firmware

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=Applications
	SECTION:=utils
	CATEGORY:=Lantiq
	TITLE:=TAPIdemo application
	URL:=http://www.lantiq.com/
	MAINTAINER:=Lantiq Deutschland GmbH
#	DEPENDS:=+ifx-voice-cpe-tapi
	DEPENDS:=+ifx-voice-cpe-tapi +ifx-dect
	MENU:=1
endef

define Package/$(PKG_NAME)/description
	Voice Access mini-PBX Demo Application
endef


define Package/$(PKG_NAME)/config
	$(call Package/$(PKG_NAME)/override_version)
	$(call Package/$(PKG_NAME)/override_source_path)
    source "$(SOURCE)/config/Config.in"
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
	config $(PKG_NAME)_904DSL
		bool "904 DSL model, including SLIC and USB"
	endchoice
    ###################################################
endef

    ##### 20100726 ctc added to support DT 723 model ######
ITDF_DIR=XX
ifeq ($(CONFIG_$(PKG_NAME)_XRX200),y)
	ITDF_DIR=files.xrx200
endif
ifeq ($(CONFIG_$(PKG_NAME)_DT_723),y)
	ITDF_DIR=files.723
endif
ifeq ($(CONFIG_$(PKG_NAME)_VOX2),y)
	ITDF_DIR=files.vox2
endif
ifeq ($(CONFIG_$(PKG_NAME)_904DSL),y)
	ITDF_DIR=files.904dsl
endif
ITDS_DIR=XX
ifeq ($(CONFIG_$(PKG_NAME)_XRX200),y)
	ITDS_DIR=src.xrx200
endif
ifeq ($(CONFIG_$(PKG_NAME)_DT_723),y)
	ITDS_DIR=src.723
endif
ifeq ($(CONFIG_$(PKG_NAME)_VOX2),y)
	ITDS_DIR=src.vox2
endif
ifeq ($(CONFIG_$(PKG_NAME)_904DSL),y)
	ITDS_DIR=src.904dsl
endif
    ###################################################

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_BOARD_EASY50712),y)
CONFIGURE_ARGS += --enable-boardname=EASY50712
endif
ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_BOARD_EASY50712_V3),y)
CONFIGURE_ARGS += --enable-boardname=EASY50712_V3
endif
ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_BOARD_EASY508xx),y)
CONFIGURE_ARGS += --enable-boardname=EASY508XX
endif
ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_BOARD_EASY80910),y)
CONFIGURE_ARGS += --enable-boardname=EASY80910
endif
#to be extended...


ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_QOS),y)
CONFIGURE_ARGS += --enable-qos
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_FAX_T_38_FW),y)
CONFIGURE_ARGS += --enable-fax-t38
else
#--enable-fax-t38 enables also functionality that is enabled by --enable-fax-modem ,
#when --enable-fax-t38 was use then using --enable-fax-modem doesn't change configuration
ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_FAX_MODEM),y)
CONFIGURE_ARGS += --enable-fax-modem
endif
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_DXT_EXT),y)
CONFIGURE_ARGS += --enable-dxt-ext
CONFIGURE_ARGS += --with-dxt-incl=$(STAGING_DIR)/usr/include/drv_duslic_xt
endif

ifeq ($(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_DECT),y)
CONFIGURE_ARGS += --enable-dect
CONFIGURE_ARGS += --with-dect-lib=$(STAGING_DIR)/usr/lib
CONFIGURE_ARGS += --with-dect-incl=$(STAGING_DIR)/usr/include
CONFIGURE_ARGS += --with-voip-common-incl=$(STAGING_DIR)/usr/include
CONFIGURE_ARGS += --with-voip-common-lib=$(STAGING_DIR)/usr/lib
endif

ifeq ($(CONFIG_PACKAGE_teridian-voice-daa-ter1x66),y)
CONFIGURE_ARGS += --with-teridian-incl=$(STAGING_DIR)/usr/include/drv_ter1x66
endif

CONFIGURE_ARGS +=  \
	--enable-trace --enable-fs  --with-kernel-incl=$(LINUX_DIR)/include \
	--with-tapi-incl=$(STAGING_DIR)/usr/include/drv_tapi \
	--with-vmmc-incl=$(STAGING_DIR)/usr/include/drv_vmmc --enable-warnings \
	--with-ifxos-incl=$(STAGING_DIR)/usr/include/ifxos \
	--with-ifxos-lib=$(STAGING_DIR)/usr/lib

ifneq ($(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_ADDITIONAL_FLAGS),"")
CONFIGURE_ARGS += \
   $(subst ",, $(CONFIG_LTQ_VOICE_CPE_TAPIDEMO_ADDITIONAL_FLAGS))
endif

ifeq ($(CONFIG_LINUX_2_6),y)
CONFIGURE_ARGS += --enable-linux-26 ARCH=$(LINUX_KARCH)
endif

define Build/Prepare
	$(call Build/Prepare/Default)
	### ctc ###
	if [ -d "./src" ] ; then \
		$(CP) ./src/* $(PKG_BUILD_DIR)/. ; \
	fi
	if [ -d "./$(ITDS_DIR)" ] ; then \
		$(CP) ./$(ITDS_DIR)/* $(PKG_BUILD_DIR)/. ; \
	fi
	###########
endef

define Build/Clean 
	@- [ -e $(PKG_BUILD_DIR)/ipkg ] && rm -Rf $(PKG_BUILD_DIR)/ipkg
	@- [ -e $(PKG_BUILD_DIR)/Makefile ] && $(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Clean/Default)
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)$(IFX_BIN_DIR)
	$(INSTALL_DIR) $(1)$(LANTIQ_BIN_DIR)
	$(INSTALL_DIR) $(1)/$(LANTIQ_FIRMWARE_DIR)
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/tapidemo $(1)$(LANTIQ_BIN_DIR)
	$(INSTALL_BIN) ./files/$(PKG_START_SCRIPT) $(1)/usr/sbin/
	(cd $(1)/usr/sbin; ln -snf ../../opt/lantiq/bin/tapidemo .)
	(cd $(1)$(IFX_BIN_DIR); ln -snf ../../lantiq/bin/tapidemo .)
	$(INSTALL_BIN) ./files/ter1x66_bbd.bin $(1)$(LANTIQ_FIRMWARE_DIR)
	### ctc ###
	if [ -d "./$(ITDF_DIR)" ] ; then \
		$(CP) ./$(ITDF_DIR)/* $(1)/. ; \
	fi
	###########
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
