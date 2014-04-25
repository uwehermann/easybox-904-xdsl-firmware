#
# Copyright (C) 2010 Arcadyan Corporation
# All Rights Reserved.
#

include $(TOPDIR)/rules.mk

PKG_VERSION:=$(PKG_VERSION_MAJOR).$(PKG_VERSION_MINOR)

PKG_BUILD_DIR := $(BUILD_DIR)/arc-middle/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Middle Layer
  SECTION:=utils
  CATEGORY:=Arcadyan
  TITLE:=module - $(ARC_MOD_DESCR)
  DEPENDS:=+arc-middle-framework $(PKG_DEPENDS)
endef

define Package/$(PKG_NAME)/description
 This package contains $(ARC_MOD_DESCR) APIs.
endef

#define Package/$(PKG_NAME)/config2
#	config $(PKG_NAME)_CLI2
#		depends on PACKAGE_$(PKG_NAME)
#		bool "test CLI2 for $(PKG_NAME)"
#		default n
#endef

ifneq ($(PKG_TEST_CLI),)
ifdef Package/$(PKG_NAME)/config2
define Package/$(PKG_NAME)/config
	config $(PKG_NAME)_CLI
		depends on PACKAGE_$(PKG_NAME)
		bool "test CLI for $(PKG_NAME)"
		default n
	$(call Package/$(PKG_NAME)/config2)
endef
else
define Package/$(PKG_NAME)/config
	config $(PKG_NAME)_CLI
		depends on PACKAGE_$(PKG_NAME)
		bool "test CLI for $(PKG_NAME)"
		default n
endef
endif
ifneq ($(CONFIG_$(PKG_NAME)_CLI),y)
	PKG_TEST_CLI=
endif
else
ifdef Package/$(PKG_NAME)/config2
	$(call Package/$(PKG_NAME)/config2)
endif
endif

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(CP) ../mod_make.mk $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall -I$(STAGING_DIR)/usr/include" \
		LDFLAGS="$(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib" \
		ARC_MOD_NAME=$(ARC_MOD_NAME) \
		PKG_VERSION_MAJOR=$(PKG_VERSION_MAJOR) \
		PKG_VERSION_MINOR=$(PKG_VERSION_MINOR) \
		PKG_TEST_CLI=$(PKG_TEST_CLI) \
		PKG_MAPI_LDFLAGS=$(PKG_MAPI_LDFLAGS) \
		PKG_HDL_LDFLAGS=$(PKG_HDL_LDFLAGS) \
		PKG_TEST_CLI_LDFLAGS=$(PKG_TEST_CLI_LDFLAGS) \
		MAPFILE=$(MAPFILE) \
		DEBUG=$(DEBUG) \
		NO_HDL=$(NO_HDL)
endef

REG_SEQ=$(shell awk '{ print $$1 }' ./files/mid_$(ARC_MOD_NAME).reg )
REG_SEQ_3=$(shell printf '%03d' $(REG_SEQ))

define Package/$(PKG_NAME)/install
	# pre-install
  ifdef Package/$(PKG_NAME)/pre-install
	$(call Package/$(PKG_NAME)/pre-install,$(1))
  endif

	# libs
	$(INSTALL_DIR) $(1)/usr/lib
	if [ -n "`ls $(PKG_BUILD_DIR)/libmapi_$(ARC_MOD_NAME).so*`" ] ; then \
		$(CP) $(PKG_BUILD_DIR)/libmapi_$(ARC_MOD_NAME).so* $(1)/usr/lib/. ; \
	fi
	if [ -n "`ls $(PKG_BUILD_DIR)/libmhdl_$(ARC_MOD_NAME).so*`" ] ; then \
		$(CP) $(PKG_BUILD_DIR)/libmhdl_$(ARC_MOD_NAME).so* $(1)/usr/lib/. ; \
	fi
	rm -f $(1)/usr/lib/libm???_$(ARC_MOD_NAME).so.depend
	rm -f $(1)/usr/lib/libm???_$(ARC_MOD_NAME).so.map

	# default config files
	if [ "$(NO_HDL)" != "1" ] && [ -f ./files/mid_$(ARC_MOD_NAME).reg ] ; then \
		$(INSTALL_DIR) $(1)/etc/config/arc-middle/register ; \
		$(INSTALL_DATA) ./files/mid_$(ARC_MOD_NAME).reg $(1)/etc/config/arc-middle/register/$(REG_SEQ_3)-mid_$(ARC_MOD_NAME).reg ; \
	fi

	# test program
  ifneq ($(PKG_TEST_CLI),)
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_TEST_CLI) $(1)/usr/sbin/
  endif

	# post-install
  ifdef Package/$(PKG_NAME)/post-install
	$(call Package/$(PKG_NAME)/post-install,$(1))
  endif
endef


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	if [ -n "`ls $(PKG_BUILD_DIR)/mid_mapi_$(ARC_MOD_NAME)*.h`" ] ; then \
		$(INSTALL_DATA) $(PKG_BUILD_DIR)/mid_mapi_$(ARC_MOD_NAME)*.h $(1)/usr/include ; \
	fi
	if [ -n "`ls $(PKG_BUILD_DIR)/mid_$(ARC_MOD_NAME)*.h`" ] ; then \
		$(INSTALL_DATA) $(PKG_BUILD_DIR)/mid_$(ARC_MOD_NAME)*.h $(1)/usr/include ; \
	fi

	$(INSTALL_DIR) $(1)/usr/lib
	if [ -n "`ls $(PKG_BUILD_DIR)/libmapi_$(ARC_MOD_NAME).so*`" ] ; then \
		$(CP) $(PKG_BUILD_DIR)/libmapi_$(ARC_MOD_NAME).so* $(1)/usr/lib/. ; \
	fi
	if [ -n "`ls $(PKG_BUILD_DIR)/libmhdl_$(ARC_MOD_NAME).so*`" ] ; then \
		$(CP) $(PKG_BUILD_DIR)/libmhdl_$(ARC_MOD_NAME).so* $(1)/usr/lib/. ; \
	fi
	rm -f $(1)/usr/lib/libm???_$(ARC_MOD_NAME).so.depend
	rm -f $(1)/usr/lib/libm???_$(ARC_MOD_NAME).so.map
	
  ifdef Package/$(PKG_NAME)/InstallDev
	$(call Package/$(PKG_NAME)/InstallDev,$(1))
  endif
endef
