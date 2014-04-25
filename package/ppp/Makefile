#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 12369 2008-08-22 05:03:37Z nico $

include $(TOPDIR)/rules.mk

PKG_NAME:=ppp
PKG_VERSION:=2.4.3
PKG_RELEASE:=10

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.samba.org/pub/ppp/
PKG_MD5SUM:=848f6c3cafeb6074ffeb293c3af79b7c

PKG_BUILD_DEPENDS:=libpcap arc-middle-framework-open arc-middle-mod-ccfg-open

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/ppp/Default
  SECTION:=net
  CATEGORY:=Network
  URL:=http://ppp.samba.org/
endef

define Package/ppp
$(call Package/ppp/Default)
#  DEPENDS:=+kmod-ppp   # Ifx PPA compilation issue
  DEPENDS:=
  TITLE:=PPP daemon
  MENU:=1
  DEPENDS:=+libumng +libsyslog
endef

define Package/ppp/description
 This package contains the PPP (Point-to-Point Protocol) daemon.
endef

define Package/ppp-mod-pppoa
$(call Package/ppp/Default)
  DEPENDS:=ppp +linux-atm
  TITLE:=PPPoA plugin
endef

define Package/ppp-mod-pppoa/description
 This package contains a PPPoA (PPP over ATM) plugin for ppp.
endef

define Package/ppp-mod-pppoe
$(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=PPPoE plugin
endef

define Package/ppp-mod-pppoe/description
 This package contains a PPPoE (PPP over Ethernet) plugin for ppp.
endef

define Package/ppp-mod-radius
$(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=RADIUS plugin
endef

define Package/ppp-mod-radius/description
 This package contains a RADIUS (Remote Authentication Dial-In User
 Service) plugin for ppp.
endef

define Package/chat
$(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=Establish conversation with a modem
endef

define Package/chat/description
 This package contains an utility to establish conversation with other
 PPP servers (via a modem).
endef

define Package/pppdump
$(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=Read PPP record file
endef

define Package/pppdump/description
 This package contains an utility to read PPP record file.
endef

define Package/pppstats
$(call Package/ppp/Default)
  DEPENDS:=ppp
  TITLE:=Report PPP statistics
endef

define Package/pppstats/description
 This package contains an utility to report PPP statistics.
endef


define Build/Configure
$(call Build/Configure/Default,, \
	UNAME_S="Linux" \
	UNAME_R="$(LINUX_VERSION)" \
	UNAME_M="$(ARCH)" \
)
endef

define Build/Prepare
	$(call Build/Prepare/Default,)
	$(CP) ./files/code/include/linux/if_ppp.h $(PKG_BUILD_DIR)/include/linux
	$(CP) ./files/code/include/net/if_ppp.h $(PKG_BUILD_DIR)/include/net
	$(CP) ./files/code/pppd/plugins/rp-pppoe/Makefile.linux $(PKG_BUILD_DIR)/pppd/plugins/rp-pppoe
	$(CP) ./files/code/pppd/plugins/rp-pppoe/discovery.c $(PKG_BUILD_DIR)/pppd/plugins/rp-pppoe
	$(CP) ./files/code/pppd/plugins/rp-pppoe/if.c $(PKG_BUILD_DIR)/pppd/plugins/rp-pppoe
	$(CP) ./files/code/pppd/plugins/rp-pppoe/plugin.c $(PKG_BUILD_DIR)/pppd/plugins/rp-pppoe
	$(CP) ./files/code/pppd/plugins/rp-pppoe/pppoe.h $(PKG_BUILD_DIR)/pppd/plugins/rp-pppoe
	$(CP) ./files/code/pppd/plugins/rp-pppoe/pppoe-discovery.c $(PKG_BUILD_DIR)/pppd/plugins/rp-pppoe
	$(CP) ./files/code/pppd/plugins/rp-pppoe/utils.c $(PKG_BUILD_DIR)/pppd/plugins/rp-pppoe
	$(CP) ./files/code/pppd/plugins/rp-pppoe/common.c $(PKG_BUILD_DIR)/pppd/plugins/rp-pppoe
	$(CP) ./files/code/pppd/plugins/radius/Makefile.linux $(PKG_BUILD_DIR)/pppd/plugins/radius
	$(CP) ./files/code/pppd/plugins/radius/radius.c $(PKG_BUILD_DIR)/pppd/plugins/radius
	$(CP) ./files/code/pppd/auth.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/cbcp.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/ccp.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/chap-new.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/chap-new.h $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/demand.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/eap.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/ecp.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/fsm.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/fsm.h $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/ipcp.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/ipcp.h $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/ipv6cp.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/ipv6cp.h $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/lcp.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/main.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/multilink.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/options.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/pppd.h $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/sys-linux.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/tty.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/upap.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/utils.c $(PKG_BUILD_DIR)/pppd
	$(CP) ./files/code/pppd/Makefile.linux $(PKG_BUILD_DIR)/pppd
	if [ -e ./src ] ; then \
		$(CP)  ./src/*  $(PKG_BUILD_DIR)/. ; \
	fi
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		COPTS="$(TARGET_CFLAGS)" \
		PRECOMPILED_FILTER=1 \
		STAGING_DIR="$(STAGING_DIR)" \
		DESTDIR="$(PKG_INSTALL_DIR)/usr" \
		LDFLAGS1="-lumngsyslog -lmapitrnx -lmapi_ccfg -lArcCom -L$(STAGING_DIR)/usr/lib" \
		all install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/pppd $(1)/usr/include/
endef

define Package/ppp/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_VERSION)
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/pppd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_BIN) ./files/ppp.sh $(1)/lib/network/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_CONF) ./files/etc/ppp/chap-secrets $(1)/etc/ppp/
	$(INSTALL_DATA) ./files/etc/ppp/filter $(1)/etc/ppp/
	$(INSTALL_BIN) ./files/etc/ppp/ip-up $(1)/etc/ppp/
	$(INSTALL_DIR) $(1)/etc/ppp/ip-up.d
	$(INSTALL_BIN) ./files/etc/ppp/ip-down $(1)/etc/ppp/
	$(INSTALL_DIR) $(1)/etc/ppp/ip-down.d
	$(INSTALL_BIN) ./files/etc/ppp/ipv6-up $(1)/etc/ppp/
	$(INSTALL_BIN) ./files/etc/ppp/ipv6-down $(1)/etc/ppp/
#	$(INSTALL_DATA) ./files/etc/ppp/options $(1)/etc/ppp/
	$(INSTALL_DATA) ./files/etc/ppp/options $(1)/etc/ppp/ppp.options
	ln -sf /tmp/resolv.conf.auto $(1)/etc/ppp/resolv.conf

	$(INSTALL_BIN) ./files/etc/ppp/ip-up-named   $(1)/etc/ppp/ip-up.d
	$(INSTALL_BIN) ./files/etc/ppp/ip-down-named $(1)/etc/ppp/ip-down.d
endef

define Package/ppp-mod-pppoa/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_VERSION)/pppoatm.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_BIN) ./files/pppoa.sh $(1)/lib/network/
endef

define Package/ppp-mod-pppoe/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_VERSION)/rp-pppoe.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_BIN) ./files/pppoe.sh $(1)/lib/network/
endef

define Package/ppp-mod-radius/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PKG_VERSION)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/pppd/$(PKG_VERSION)/radius.so \
		$(1)/usr/lib/pppd/$(PKG_VERSION)/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_DATA) ./files/etc/ppp/radius.conf $(1)/etc/ppp/
	$(INSTALL_DIR) $(1)/etc/ppp/radius
	$(INSTALL_DATA) ./files/etc/ppp/radius/dictionary* \
		$(1)/etc/ppp/radius/
	$(INSTALL_CONF) ./files/etc/ppp/radius/servers \
		$(1)/etc/ppp/radius/
endef

define Package/chat/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/chat $(1)/usr/sbin/
endef

define Package/pppdump/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/pppdump $(1)/usr/sbin/
endef

define Package/pppstats/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/pppstats $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ppp))
$(eval $(call BuildPackage,ppp-mod-pppoa))
$(eval $(call BuildPackage,ppp-mod-pppoe))
$(eval $(call BuildPackage,ppp-mod-radius))
$(eval $(call BuildPackage,chat))
$(eval $(call BuildPackage,pppdump))
$(eval $(call BuildPackage,pppstats))
