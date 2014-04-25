
include $(TOPDIR)/rules.mk

PKG_NAME:=lame
PKG_VERSION:=3.99.5
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://sourceforge.net/projects/lame/files/lame/3.99/
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/lame/Default
  TITLE:=LAME Ain't an MP3 Encoder
  URL:=http://lame.sourceforge.net/
endef

define Package/lame
  $(call Package/lame/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libsndfile
endef

define Package/lame/description
 LAME Ain't an MP3 Encoder
endef

define Build/Prepare
	$(call Build/Prepare/Default)
endef

define Build/Configure
	( cd $(PKG_BUILD_DIR); \
		./configure \
			--host=$(ARCH)-linux \
			--enable-static \
			--disable-shared \
			--with-fileio=sndfile \
			SNDFILE_CFLAGS=-I$(STAGING_DIR)/usr/include/libsndfile \
			SNDFILE_LIBS=-L$(STAGING_DIR)/usr/lib/ \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) BLD_DIR=$(PKG_BUILD_DIR)
endef

define Package/lame/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/frontend/lame $(1)/usr/bin/
#	$(INSTALL_DIR) $(1)/etc/wavsample
#	cp -r sample/* $(1)/etc/wavsample
endef

$(eval $(call BuildPackage,lame))
