
INSTALL_BIN:=install -m0755
INSTALL_DIR:=install -d -m0755
INSTALL_DATA:=install -m0644
INSTALL_CONF:=install -m0600

INSTALL_DIR:=$(shell pwd)/../ipkg/firewall

$(warning  INSTALL_DIR:[$(INSTALL_DIR)])

all: install



install:
	echo [htf firewall] Package/firewall/gitinstall
	$(INSTALL_DIR) $(INSTALL_DIR)/lib/firewall
	echo [$(INSTALL_DATA) ./uci_firewall.sh $(INSTALL_DIR)/lib/firewall]
	$(INSTALL_DATA) ./uci_firewall.sh $(INSTALL_DIR)/lib/firewall
	$(INSTALL_DIR) $(INSTALL_DIR)/etc/config
	$(INSTALL_DATA) ./firewall.config $(INSTALL_DIR)/etc/config/firewall
	$(INSTALL_DIR) $(INSTALL_DIR)/etc/init.d/
	$(INSTALL_BIN) ./firewall.init $(INSTALL_DIR)/etc/init.d/firewall
	$(INSTALL_DIR) $(INSTALL_DIR)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./20-firewall $(INSTALL_DIR)/etc/hotplug.d/iface
	$(INSTALL_DIR) $(INSTALL_DIR)/etc
	$(INSTALL_DATA) ./firewall.user $(INSTALL_DIR)/etc
	#create a folder for external module scripts
	$(INSTALL_DIR) $(INSTALL_DIR)/etc/config/firewallExt
	$(INSTALL_DATA) ./firewallv6.user $(INSTALL_DIR)/etc


