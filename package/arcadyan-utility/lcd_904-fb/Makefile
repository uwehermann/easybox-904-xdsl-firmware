#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=lcd_904-fb
PKG_VERSION:=0.0.1
PKG_RELEASE:=2

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
	SUBMENU:=Kernel Module
	SECTION:=libs
	CATEGORY:=Arcadyan
	TITLE:=LCD driver of 904
endef

define Package/$(PKG_NAME)/description
	LCD driver of 904
endef

define Package/$(PKG_NAME)/config
	config LCD_LITE
		depends on PACKAGE_$(PKG_NAME)
		bool "lite version"
		default no
	config LCD_GPIO_RST
		depends on PACKAGE_$(PKG_NAME)
		int "reset GPIO"
		default 6
	config LCD_GPIO_BL
		depends on PACKAGE_$(PKG_NAME)
		int "backlight GPIO"
		default 28
	config LCD_GPIO_LED
		depends on PACKAGE_$(PKG_NAME)
		int "LED GPIO"
		default 31
endef

LITE_FLAGS=
ifeq ($(CONFIG_LCD_LITE),y)
	LITE_FLAG=y
endif

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) drv_lcd_904.ko $(PKG_BUILD_DIR)/
	$(CP) lcd_cli $(PKG_BUILD_DIR)/
	$(CP) lcd_demo $(PKG_BUILD_DIR)/
	$(CP) lcd_image $(PKG_BUILD_DIR)/
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	$(CP) $(LINUX_DIR)/drivers/video/fb*.h	$(PKG_BUILD_DIR)/
	$(CP) $(LINUX_DIR)/drivers/video/sys*.c	$(PKG_BUILD_DIR)/
endef

define Build/Compile

endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/$(MODULES_SUBDIR)/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/drv_lcd_904.ko $(1)/$(MODULES_SUBDIR)/
	if [ "$(LITE_FLAG)" != "y" ] ; then \
		$(INSTALL_DIR) $(1)/etc/init.d/								; \
		$(INSTALL_BIN) ./files/etc/init.d/lcd904 $(1)/etc/init.d/.	; \
		$(INSTALL_DIR) $(1)/usr/sbin								; \
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/lcd_cli  $(1)/usr/sbin/		; \
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/lcd_demo $(1)/usr/sbin/		; \
		$(CP) ./files/usr/sbin/* $(1)/usr/sbin/.					; \
		$(INSTALL_DIR) $(1)/etc										; \
	fi
	### LCD bootup animation LZMA ###
	if [ "$(LITE_FLAG)" != "y" ] ; then \
		cat ./files/etc/lcd_boot_image/*.raw | $(STAGING_DIR_HOST)/bin/lzma e $(PKG_BUILD_DIR)/lcd_boot_image.lzma -si -a0	; \
	fi
	### LCD bootup animation header, 160 bytes ###
	#    16 bytes: identifier (lcd_boot_image)
	#    12 bytes: LZMA compressed image size
	#     4 bytes: amount of files
	#    16 bytes: original image size
	#     4 bytes: frame time slot, in msec
	#   108 bytes: bit vector format of display images, lsb
	if [ "$(LITE_FLAG)" != "y" ] ; then \
		mkdir -p $(BIN_DIR)/$(SUBTARGET)																						; \
		printf "%s" "lcd_boot_image" > $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image													; \
			./pad2align.sh -n  16 $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image >& /dev/null											; \
		printf "%lu" `ls -l $(PKG_BUILD_DIR)/lcd_boot_image.lzma | cut -d ' ' -f 5` >> $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image	; \
			./pad2align.sh -n  28 $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image >& /dev/null											; \
		printf "%lu" `ls -1 ./files/etc/lcd_boot_image/*.raw | wc -l` >> $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image					; \
			./pad2align.sh -n  32 $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image >& /dev/null											; \
		printf "%lu" `du -b -c files/etc/lcd_boot_image/*.raw | awk '{ if ( $$$$2 == "total" ) print $$$$1 }'` >> $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image ; \
			./pad2align.sh -n  48 $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image >& /dev/null											; \
		echo "10" >> $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image																		; \
			./pad2align.sh -n  52 $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image >& /dev/null											; \
		echo "1111111111111111111111111111111111111111111111111111111111111101010101010101010110000000000" >> $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image ; \
			./pad2align.sh -n 160 $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image >& /dev/null											; \
		cat $(PKG_BUILD_DIR)/lcd_boot_image.lzma >> $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image										; \
			./pad2align.sh -n 131072 $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image >& /dev/null										; \
		$(CP) $(BIN_DIR)/$(SUBTARGET)/lcd_boot_image $(1)/etc/																	; \
	fi
	### LCD demo ###
	if [ "$(LITE_FLAG)" != "y" ] ; then \
		$(CP) ./files/etc/lcd_image	$(PKG_BUILD_DIR)/.				; \
		cd $(PKG_BUILD_DIR)/ ; tar zcvf lcd_image.tgz lcd_image/	; \
		$(CP) $(PKG_BUILD_DIR)/lcd_image.tgz $(1)/etc/.				; \
	fi
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/lcd_904.h $(1)/usr/include/lcd_904.h
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
