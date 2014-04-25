#
# Copyright (C) 2009 Arcadyan Corporation
# All Rights Reserved.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libumng
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Utilities
  SECTION:=utils
  CATEGORY:=Arcadyan
  TITLE:=User space management lib and tools
  DEPENDS:=+lib-arc-com
endef

define Package/$(PKG_NAME)/description
 This package contains user space management utilities and lib for Arcadyan platforms.
endef

define Package/$(PKG_NAME)/config
	##### 20101216 ctc added to support different glbcfg ######
	choice
		depends on PACKAGE_$(PKG_NAME)
		prompt "Target board"
		default $(PKG_NAME)_904LTE
	config $(PKG_NAME)_904LTE
		bool "904LTE"
	endchoice
	###################################################
endef

	##### 20100723 ctc added to support different projects ######
GLBCFG_FILE=glbcfg.dft
ifeq ($(CONFIG_$(PKG_NAME)_904LTE),y)
	GLBCFG_FILE=glbcfg.dft.904lte
endif
    ###################################################

GLBCFG_WHITELIST=glbcfg.wl
ifeq ($(CONFIG_$(PKG_NAME)_904LTE),y)
	GLBCFG_WHITELIST=glbcfg.wl.904lte
endif

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

EXTRA_LDFLAGS:=-lArcCom

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include -I$(LINUX_DIR)/include

define Build/Compile

endef

define Package/$(PKG_NAME)/install

endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/cfg_stat/*.h $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/system/*.h   $(1)/usr/include
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
