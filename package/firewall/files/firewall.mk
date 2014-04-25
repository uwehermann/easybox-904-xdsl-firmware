
define Package/firewall/gitinstall
	echo [htf firewall] Package/firewall/gitinstall
	$(INSTALL_DIR) $(1)/lib/firewall
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/uci_firewall.sh $(1)/lib/firewall
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/firewall.config $(1)/etc/config/firewall
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/firewall.init $(1)/etc/init.d/firewall
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/20-firewall $(1)/etc/hotplug.d/iface
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/firewall.user $(1)/etc
endef

