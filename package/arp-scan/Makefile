
include $(TOPDIR)/rules.mk

PKG_NAME:=arp-scan
PKG_VERSION:=1.7
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/arp-scan
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ARP sweeping/scanning utility
  DEPENDS:=+libpcap
endef

define Package/arp-scan/description
 This package contains a utility to sweep/scan hosts by use of ARP packets
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/arp-scan/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/arp-scan $(1)/usr/bin/
endef

$(eval $(call BuildPackage,arp-scan))
