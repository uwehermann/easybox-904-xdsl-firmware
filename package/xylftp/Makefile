# 
# Copyright (C) 2009 FTP Server Package
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xylftp
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=FTP
  SECTION:=net
  CATEGORY:=Network
  TITLE:=XYL FTP Server
endef

define Package/$(PKG_NAME)/description
 FTP Server.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
#	rm -rf $(PKG_INSTALL_DIR)
#	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CC=$(TARGET_CC) \
		AR=$(TARGET_CROSS)ar \
		LD=$(TARGET_CROSS)ld \
		AS=$(TARGET_CROSS)as \
		STRIP=$(TARGET_CROSS)strip \
		NM=$(TARGET_CROSS)nm \
		RANLIB=$(TARGET_CROSS)ranlib \
		SIZE=$(TARGET_CROSS)size \
		all
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xylftp $(1)/usr/bin/
	#
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/xylftp.conf $(1)/etc/config
	$(INSTALL_DATA) ./files/xylftp.pass $(1)/etc/config
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
