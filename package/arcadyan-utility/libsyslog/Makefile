#
# Copyright (C) 2009 Arcadyan Corporation
# All Rights Reserved.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libsyslog
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Utilities
  SECTION:=utils
  CATEGORY:=Arcadyan
  TITLE:=User space syslog lib and tools
  DEPENDS:=+lib-arc-com
endef

define Package/$(PKG_NAME)/description
 This package contains user space syslog utilities and lib for Arcadyan platforms.
endef

define Package/$(PKG_NAME)/config
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -Wall -I$(STAGING_DIR)/usr/include" \
		LDFLAGS="$(TARGET_LDFLAGS) -lArcCom -lpthread"
endef

define Package/$(PKG_NAME)/install
	# bootup files
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/etc/init.d/umng_syslogd  $(1)/etc/init.d/.

	# libs
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libumngsyslog.so* $(1)/usr/lib/

	#	syslog
	# tools
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/umng_syslogd       $(1)/usr/sbin/.
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/umng_syslog_cli    $(1)/usr/sbin/.
	#	end of syslog
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/syslog/*.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libumngsyslog.so*        $(1)/usr/lib
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
