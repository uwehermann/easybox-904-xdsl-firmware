#
# Copyright (C) 2008-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=firewall

PKG_VERSION:=2
PKG_RELEASE:=58

include $(INCLUDE_DIR)/package.mk

define Package/firewall
  SECTION:=net
  CATEGORY:=Base system
  URL:=http://openwrt.org/
  TITLE:=OpenWrt firewall
  MAINTAINER:=Jo-Philipp Wich <xm@subsignal.org>
  DEPENDS:=+iptables +iptables-mod-conntrack +iptables-mod-nat
  PKGARCH:=all
endef

define Package/firewall/description
 UCI based firewall for OpenWrt
endef

define Build/Compile
	true
endef

define Package/firewall/conffiles
/etc/config/firewall
#/etc/firewall.user
endef

define Package/firewall/install
	$(INSTALL_DIR) $(1)/etc/config/firewallExt
	$(INSTALL_DIR) $(1)/lib/firewall
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DATA) ./files/lib/*.sh $(1)/lib/firewall
	$(INSTALL_BIN) ./files/rstFirewallFst.sh $(1)/usr/sbin/rstFirewallFst.sh
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) ./files/bin/fw $(1)/sbin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/firewall.config $(1)/etc/config/firewall
	$(INSTALL_DATA) ./files/firewall_adsl.config $(1)/lib/firewall/firewall_adsl
	$(INSTALL_DATA) ./files/firewall_vdsl.config $(1)/lib/firewall/firewall_vdsl
	$(INSTALL_DATA) ./files/firewall_ether.config $(1)/lib/firewall/firewall_ether
#	$(INSTALL_DATA) ./files/firewall_3g.config $(1)/lib/firewall/firewall_3g
	$(INSTALL_DATA) ./files/firewall_lte.config $(1)/lib/firewall/firewall_lte
	$(INSTALL_DATA) ./files/firewall.user $(1)/lib/firewall/firewall.user
	$(INSTALL_DATA) ./files/firewallv6.user $(1)/lib/firewall/firewallv6.user
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/firewall.init $(1)/etc/init.d/firewall
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/firewall.hotplug $(1)/etc/hotplug.d/iface/20-firewall
	$(INSTALL_DIR) $(1)/etc/hotplug.d/firewall
	$(INSTALL_DATA) ./files/reflection.hotplug $(1)/etc/hotplug.d/firewall/10-nat-reflection
	$(INSTALL_DIR) $(1)/etc
#	$(INSTALL_DATA) ./files/firewall.user $(1)/etc
	$(INSTALL_DIR) $(1)/lib/upgrade/keep.d
	$(INSTALL_DATA) ./files/firewall.upgrade $(1)/lib/upgrade/keep.d/firewall
endef

$(eval $(call BuildPackage,firewall))
