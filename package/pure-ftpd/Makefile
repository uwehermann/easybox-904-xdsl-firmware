#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pure-ftpd
PKG_VERSION:=1.0.27
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://download.pureftpd.org/pub/pure-ftpd/releases
PKG_MD5SUM:=7d18544384aa1649d483ba86518ed34e

include $(INCLUDE_DIR)/package.mk

define Package/pure-ftpd
  SUBMENU:=FTP
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libelf
  TITLE:=Secure FTP made easy!
  URL:=http://www.pureftpd.org/project/pure-ftpd
endef

define Package/pure-ftpd/description
 Pure-FTPd is a free (BSD), secure, production-quality and standard-conformant
 FTP server. It doesn't provide useless bells and whistles, but focuses on
 efficiency and ease of use.
 It provides simple answers to common needs, plus unique  useful features for
 personal users as well as hosting providers.
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/* $(PKG_BUILD_DIR)/src/.
endef

define Package/pure-ftpd/conffiles
/etc/pure-ftpd/pure-ftpd.dft
/etc/pure-ftpd/pure-ftpd.pem.dft
endef

CONFIGURE_ARGS += \
	--with-everything \
	--with-virtualchroot \
	--with-tls \
	--with-certfile=/tmp/pure-ftpd/pure-ftpd.pem

define Package/pure-ftpd/install
#	$(INSTALL_DIR) $(1)/tmp/pure-ftpd $(1)/etc/init.d $(1)/usr/sbin
#	$(INSTALL_CONF) ./files/$(PKG_NAME).config $(1)/tmp/pure-ftpd/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/pure-ftpd $(1)/etc/init.d $(1)/usr/sbin
	$(INSTALL_CONF) ./files/$(PKG_NAME).config $(1)/etc/pure-ftpd/$(PKG_NAME).dft
	$(INSTALL_CONF) ./files/$(PKG_NAME).pem $(1)/etc/pure-ftpd/$(PKG_NAME).pem.dft
	$(INSTALL_BIN) ./files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
	$(INSTALL_BIN) ./files/$(PKG_NAME)2 $(1)/etc/init.d/$(PKG_NAME)2
	$(INSTALL_BIN) ./files/ftp_folder_set.sh $(1)/usr/sbin/ftp_folder_set.sh
#	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ptracetest $(1)/usr/sbin
#	$(INSTALL_BIN) $(foreach i,ftpd pw ftpwho mrtginfo pwconvert quotacheck statsdecode uploadscript authd, \
#		$(PKG_BUILD_DIR)/src/pure-$(i)) $(1)/usr/sbin
	$(INSTALL_BIN) $(foreach i,ftpd pw ftpwho, $(PKG_BUILD_DIR)/src/pure-$(i)) $(1)/usr/sbin
endef

$(eval $(call BuildPackage,pure-ftpd))
