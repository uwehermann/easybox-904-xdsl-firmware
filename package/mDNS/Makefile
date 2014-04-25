
include $(TOPDIR)/rules.mk

PKG_NAME:=mDNSResponder
PKG_VERSION:=320.5
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.opensource.apple.com/tarballs/mDNSResponder/
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/mDNS
  SECTION:=net
  CATEGORY:=Network
  TITLE:=mDNS
endef


define Package/mDNS/description
	mDNSResponder is application which support mDNS.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	if [ -d ./src ] ; then \
		$(CP) ./src/* $(PKG_BUILD_DIR)/. ; \
	fi
endef

define Build/Compile
	echo $(PKG_BUILD_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) BLD_DIR=$(PKG_BUILD_DIR)
endef

define Package/mDNS/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mDNSPosix/build/prod/mDNSClientPosix $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mDNS.init $(1)/etc/init.d/mDNS
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mDNSPosix/build/prod/mDNSClientPosix $(1)/usr/bin/
endef

$(eval $(call BuildPackage,mDNS))
