#
# Copyright (C) 2009 Arcadyan Corporation
# All Rights Reserved.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ulogd
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/ulogd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ulog daemon for WAN traffic trace
  DEPENDS:=
endef

define Package/ulogd/description
 This package contains a ulog daemon program for capturing WAN port traffic.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
					CC="$(TARGET_CC)" LD="$(TARGET_CROSS)ld" \
					CFLAGS="$(TARGET_CPPFLAGS) -I$(PKG_BUILD_DIR)/include -I$(PKG_BUILD_DIR)/include/ulogd -I$(PKG_BUILD_DIR)/libipulog/include -DULOGD_LOGFILE_MAXSZ_DEFAULT=0x40000" \
					LDFLAGS="$(TARGET_LDFLAGS)"
	$(MAKE) -C $(PKG_BUILD_DIR)/pcap \
					CC="$(TARGET_CC)" LD="$(TARGET_CROSS)ld" \
					CFLAGS="$(TARGET_CPPFLAGS) -I$(PKG_BUILD_DIR)/include -I$(PKG_BUILD_DIR)/include/ulogd -I$(PKG_BUILD_DIR)/libipulog/include -I./ -DULOGD_PCAP_MAXSZ_DEFAULT=0x200000" \
					LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/ulogd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ulogd $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/extensions/ulogd_BASE_ebt.so  $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pcap/ulogd_PCAP_ebt.so        $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pcap/ulogd_PCAP_sock.so       $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/ulogd.conf $(1)/etc/config/ulogd.conf
endef

$(eval $(call BuildPackage,ulogd))
