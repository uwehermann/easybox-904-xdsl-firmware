#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gutenprint
PKG_VERSION:=5.2.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	http://sourceforge.net/projects/gimp-print/files/gutenprint-5.2/$(PKG_VERSION) \
PKG_MD5SUM:=b19029972bf28f6efd693270daf8f5de

TARGET_LDFLAGS+=-Wl,-rpath-link=$(STAGING_DIR)/usr/lib

include $(INCLUDE_DIR)/package.mk

define Package/gutenprint
  SECTION:=net
  CATEGORY:=Network
  ### ctc ###
#  DEPENDS:=+zlib +libpthread +libpng +libjpeg +libstdcpp
#  DEPENDS:=+zlib +libpthread +libstdcpp
  DEPENDS:=+cups +libiconv
  ###########
  TITLE:=Gimp-Print High quality printer drivers
  URL:=http://gimp-print.sourceforge.net/index.php
endef

define Package/gutenprint/description
	High quality drivers for Canon, Epson, Lexmark, Sony, Olympus, and PCL printers for use with CUPS, Ghostscript, Foomatic, and GIMP
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	if [ -d ./src ] ; then \
		$(CP) ./src/* $(PKG_BUILD_DIR)/ ; \
	fi
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--with-foomatic=no \
		--with-foomatic3=no \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LIBS="$(TARGET_LDFLAGS) -lz" \
	)
	### ctc ###
#	CPPFLAGS="-I$(PKG_BUILD_DIR)/../cups-1.4.8/ipkg-install/usr/include -I$(PKG_BUILD_DIR)/../libiconv/include"
	###########
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		STRIP="/bin/true" \
		all install
endef

define Package/gutenprint/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgutenprint.* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cups $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/* $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/share/cups
	$(CP) $(PKG_INSTALL_DIR)/usr/share/cups/* $(1)/usr/share/cups/
	$(INSTALL_DIR) $(1)/usr/share/locale
	$(CP) $(PKG_INSTALL_DIR)/usr/share/locale/* $(1)/usr/share/locale/
	rm -rf $(1)/usr/share/gutenprint
	$(INSTALL_DIR) $(1)/usr/share/gutenprint
	$(CP) $(PKG_INSTALL_DIR)/usr/share/gutenprint/5.2 $(1)/usr/share/gutenprint/
endef

#define Build/InstallDev
#	rm -f /tmp/1
#endef

$(eval $(call BuildPackage,gutenprint))
