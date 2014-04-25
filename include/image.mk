#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

override TARGET_BUILD=
include $(INCLUDE_DIR)/prereq.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/host.mk

.NOTPARALLEL:
override MAKEFLAGS=
override MAKE:=$(SUBMAKE)
KDIR=$(KERNEL_BUILD_DIR)

IMG_PREFIX:=openwrt-$(BOARD)$(if $(SUBTARGET),-$(SUBTARGET))

ifneq ($(CONFIG_BIG_ENDIAN),)
  JFFS2OPTS     :=  --pad --big-endian --squash -v
  SQUASHFS_OPTS :=  -be
else
  JFFS2OPTS     :=  --pad --little-endian --squash -v
  SQUASHFS_OPTS :=  -le
endif

ifneq ($(CONFIG_LINUX_2_4)$(CONFIG_LINUX_2_6_25),)
  USE_SQUASHFS3 := y
endif

ifneq ($(USE_SQUASHFS3),)
  MKSQUASHFS_CMD := $(STAGING_DIR_HOST)/bin/mksquashfs-lzma
else
  MKSQUASHFS_CMD := $(STAGING_DIR_HOST)/bin/mksquashfs4
  SQUASHFS_OPTS  := -comp lzma -processors 1
endif

JFFS2_BLOCKSIZE ?= 64k 128k

define add_jffs2_mark
	echo -ne '\xde\xad\xc0\xde' >> $(1)
endef

# pad to 4k, 8k, 64k, 128k and add jffs2 end-of-filesystem mark
define prepare_generic_squashfs
	dd if=$(1) of=$(KDIR)/tmpfile.0 bs=4k conv=sync
	$(call add_jffs2_mark,$(KDIR)/tmpfile.0)
	dd if=$(KDIR)/tmpfile.0 of=$(KDIR)/tmpfile.1 bs=4k conv=sync
	$(call add_jffs2_mark,$(KDIR)/tmpfile.1)
	dd if=$(KDIR)/tmpfile.1 of=$(KDIR)/tmpfile.2 bs=64k conv=sync
	$(call add_jffs2_mark,$(KDIR)/tmpfile.2)
	dd if=$(KDIR)/tmpfile.2 of=$(1) bs=64k conv=sync
	$(call add_jffs2_mark,$(1))
	rm -f $(KDIR)/tmpfile.*
endef


ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)

  define Image/BuildKernel
		cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
		$(call Image/Build/Initramfs)
  endef

else

  ifneq ($(CONFIG_TARGET_ROOTFS_JFFS2),)
    define Image/mkfs/jffs2/sub
		# FIXME: removing this line will cause strange behaviour in the foreach loop below
		$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(JFFS2OPTS) -e $(patsubst %k,%KiB,$(1)) -o $(KDIR)/root.jffs2-$(1) -d $(TARGET_DIR)
		###### modified by ctc ######
#		$(call add_jffs2_mark,$(KDIR)/root.jffs2-$(1))
		$(STAGING_DIR_HOST)/bin/mkimage -A MIPS -O Linux -C lzma -T filesystem -e 0x00 -a 0x00 -n "LTQCPE RootFS" -d $(KDIR)/root.jffs2-$(1) $(KDIR)/root.jffs2-$(1).img
		##########################
		$(call Image/Build,jffs2-$(1))
		###### added by ctc ######
		$(CP) $(KDIR)/root.jffs2-$(1).img $(BIN_DIR)/$(SUBTARGET)/root.jffs2-$(1).nor
		##########################
    endef
    define Image/mkfs/jffs2
		###### added by ctc ######
		rm -rf $(TARGET_DIR)/jffs
		##########################
		@chmod 755 -R $(TARGET_DIR)/
		@rm -rf $(TARGET_DIR)/../dftconfig
		@mv $(TARGET_DIR)/etc/config $(TARGET_DIR)/../dftconfig
		@ln -sf /tmp/etc/config $(TARGET_DIR)/etc/config
		$(foreach SZ,$(JFFS2_BLOCKSIZE),$(call Image/mkfs/jffs2/sub,$(SZ)))
		###### added by ctc ######
		$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(JFFS2OPTS) -n -e 0x4000  -o $(KDIR)/root.jffs2-16k.nand.tmp  -d $(TARGET_DIR)
		$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(JFFS2OPTS) -n -e 0x20000 -o $(KDIR)/root.jffs2-128k.nand.tmp -d $(TARGET_DIR)
		$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(JFFS2OPTS) -n -e 0x40000 -o $(KDIR)/root.jffs2-256k.nand.tmp -d $(TARGET_DIR)
		$(STAGING_DIR_HOST)/bin/mkimage -A MIPS -O Linux -C lzma -T filesystem -e 0x00 -a 0x00 -n "LTQCPE RootFS" -d $(KDIR)/root.jffs2-16k.nand.tmp  $(KDIR)/root.jffs2-16k.nand
		$(STAGING_DIR_HOST)/bin/mkimage -A MIPS -O Linux -C lzma -T filesystem -e 0x00 -a 0x00 -n "LTQCPE RootFS" -d $(KDIR)/root.jffs2-128k.nand.tmp $(KDIR)/root.jffs2-128k.nand
		$(STAGING_DIR_HOST)/bin/mkimage -A MIPS -O Linux -C lzma -T filesystem -e 0x00 -a 0x00 -n "LTQCPE RootFS" -d $(KDIR)/root.jffs2-256k.nand.tmp $(KDIR)/root.jffs2-256k.nand
		$(CP) $(KDIR)/root.jffs2-*k.nand $(BIN_DIR)/$(SUBTARGET)/
		if [ -f "$(TARGET_DIR)/etc/dftconfig.tgz" ] ; then \
			mkdir dftconfig; $(CP) $(TARGET_DIR)/etc/dftconfig.tgz dftconfig/config.tgz ; \
			$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(JFFS2OPTS) -n -e 0x4000  -o $(KDIR)/sysconfig.jffs2-16k.nand  -d dftconfig ; \
			$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(JFFS2OPTS) -n -e 0x20000 -o $(KDIR)/sysconfig.jffs2-128k.nand -d dftconfig ; \
			$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(JFFS2OPTS) -n -e 0x40000 -o $(KDIR)/sysconfig.jffs2-256k.nand -d dftconfig ; \
			rm -rf dftconfig ; \
			$(CP) $(KDIR)/sysconfig.jffs2-*k.nand $(BIN_DIR)/$(SUBTARGET)/. ; \
		fi
		##########################
		@rm $(TARGET_DIR)/etc/config
		@mv $(TARGET_DIR)/../dftconfig $(TARGET_DIR)/etc/config
    endef
  endif

  ifneq ($(CONFIG_TARGET_ROOTFS_SQUASHFS),)
    define Image/mkfs/squashfs
		###### modified by ctc ######
#		@mkdir -p $(TARGET_DIR)/overlay
#		$(MKSQUASHFS_CMD) $(TARGET_DIR) $(KDIR)/root.squashfs -nopad -noappend -root-owned $(SQUASHFS_OPTS)
		@mkdir -p $(TARGET_DIR)/jffs
		@chmod 755 -R $(TARGET_DIR)/
		@rm -rf $(TARGET_DIR)/../dftconfig
		@mv $(TARGET_DIR)/etc/config $(TARGET_DIR)/../dftconfig
		@ln -sf /tmp/etc/config $(TARGET_DIR)/etc/config
		$(MKSQUASHFS_CMD) $(TARGET_DIR) $(KDIR)/root.squashfs -nopad -noappend -root-owned $(SQUASHFS_OPTS)
		$(STAGING_DIR_HOST)/bin/mkimage -A MIPS -O Linux -C lzma -T filesystem -e 0x00 -a 0x00 -n "LTQCPE RootFS" -d $(KDIR)/root.squashfs $(KDIR)/rootfs.img
		##########################
		$(call Image/Build,squashfs)
		@rm $(TARGET_DIR)/etc/config
		@mv $(TARGET_DIR)/../dftconfig $(TARGET_DIR)/etc/config
    endef
  endif

  ifneq ($(CONFIG_TARGET_ROOTFS_UBIFS),)
    define Image/mkfs/ubifs
		$(CP) ./ubinize.cfg $(KDIR)
		$(STAGING_DIR_HOST)/bin/mkfs.ubifs $(UBIFS_OPTS) -o $(KDIR)/root.ubifs -d $(TARGET_DIR)
		(cd $(KDIR); \
		$(STAGING_DIR_HOST)/bin/ubinize $(UBINIZE_OPTS) -o $(KDIR)/root.ubi ubinize.cfg)
		$(call Image/Build,ubi)
    endef
  endif

  ###### added by ctc ######
  define Image/mkfs/firmware
		$$(call Image/Build,firmware)
  endef

  define Image/mkfs/prepare/arcdftconf
  	[ ! -L $(TARGET_DIR)/etc/passwd ] && { \
		mv $(TARGET_DIR)/etc/passwd $(TARGET_DIR)/etc/config/passwd; \
	} || echo "Skip moving $(TARGET_DIR)/etc/passwd to default configuration"
	[ ! -L $(TARGET_DIR)/etc/group ] && { \
		mv $(TARGET_DIR)/etc/group $(TARGET_DIR)/etc/config/group; \
	} || echo "Skip moving $(TARGET_DIR)/etc/group to default configuration"
	[ ! -L $(TARGET_DIR)/etc/hosts ] && { \
		mv $(TARGET_DIR)/etc/hosts $(TARGET_DIR)/etc/config/hosts; \
	} || echo "Skip moving $(TARGET_DIR)/etc/hosts to default configuration"
	tar -zcvf $(TARGET_DIR)/etc/dftconfig.tgz -C $(TARGET_DIR)/etc config
  endef

  define Image/mkfs/prepare/arcmid
	# For forward compatibility with new added middle handler/initializer
	# For backward compatibility without new added middle handler/initializer, keep original registration DIR
#	@mkdir -p $(TARGET_DIR)/etc/arc-middle/register/
#	@cp -rf $(TARGET_DIR)/etc/config/arc-middle/register/. $(TARGET_DIR)/etc/arc-middle/register/.
  endef

  define Image/mkfs/prepare/arcsymlink
	[ -f $(TARGET_DIR)/etc/config/passwd ] && { \
		ln -sf /tmp/etc/config/passwd $(TARGET_DIR)/etc/passwd; \
	} || echo "Skip creating symlinks for $(TARGET_DIR)/etc/passwd"
	[ -f $(TARGET_DIR)/etc/config/group ] && { \
		ln -sf /tmp/etc/config/group $(TARGET_DIR)/etc/group; \
	} || echo "Skip creating symlinks for $(TARGET_DIR)/etc/group"
	[ -f $(TARGET_DIR)/etc/config/hosts ] && { \
		ln -sf /tmp/etc/config/hosts $(TARGET_DIR)/etc/hosts; \
	} || echo "Skip creating symlinks for $(TARGET_DIR)/etc/hosts"
	tar -zcvf $(TARGET_DIR)/etc/dftconfig.tgz -C $(TARGET_DIR)/etc config
	[ -f $(TARGET_DIR)/etc/iproute2/rt_tables ] && { \
		rm -f $(TARGET_DIR)/etc/iproute2/rt_tables; \
		ln -sf /etc/config/iproute2/rt_tables $(TARGET_DIR)/etc/iproute2/rt_tables; \
	} || echo "Skip creating symlinks for $(TARGET_DIR)/etc/iproute2/rt_tables"
	[ -f $(TARGET_DIR)/opt/twonky_server/twonkyserver-default.ini ] && { \
		mv -f $(TARGET_DIR)/opt/twonky_server/twonkyserver-default.ini $(TARGET_DIR)/opt/twonky_server/twonkyserver-default.ini.tmp; \
		ln -sf /tmp/twonkyserver-default.ini $(TARGET_DIR)/opt/twonky_server/twonkyserver-default.ini; \
	} || echo "Skip creating symlinks for $(TARGET_DIR)/opt/twonky_server/twonkyserver-default.ini"
  endef

#  define Image/mkfs/prepare/arccert
#	@mkdir -p $(TARGET_DIR)/usr/sbin
#	@cp $(TOPDIR)/target/linux/$(BOARD)/image/vf904Cert.pem  $(TARGET_DIR)/usr/sbin
#  endef

  define Image/mkfs/fullimage
		$$(call Image/Build,fullimage)
		rm -f $(TARGET_IMG_DIR)/root.squash
		ln -s $(TARGET_IMG_DIR)/rootfs.img $(TARGET_IMG_DIR)/root.squash
		$$(call Image/Build/fullimage_arcadyan,root.squash)
		$$(call Image/Build/fullimage_arcadyan,root.jffs2-128k.nor)
		$$(call Image/Build/fullimage_arcadyan,root.jffs2-16k.nand)
		$$(call Image/Build/fullimage_arcadyan,root.jffs2-128k.nand)
		$$(call Image/Build/fullimage_arcadyan,root.jffs2-256k.nand)
  endef
  ##########################

endif

ifneq ($(CONFIG_TARGET_ROOTFS_CPIOGZ),)
  define Image/mkfs/cpiogz
		( cd $(TARGET_DIR); find . | cpio -o -H newc | gzip -9 >$(BIN_DIR)/$(IMG_PREFIX)-rootfs.cpio.gz )
  endef
endif

ifneq ($(CONFIG_TARGET_ROOTFS_TARGZ),)
  define Image/mkfs/targz
		$(TAR) -zcf $(BIN_DIR)/$(IMG_PREFIX)-rootfs.tar.gz --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .
  endef
endif

ifneq ($(CONFIG_TARGET_ROOTFS_EXT2FS),)
  E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024)))

  define Image/mkfs/ext2
		$(STAGING_DIR_HOST)/bin/genext2fs -U -b $(E2SIZE) -N $(CONFIG_TARGET_ROOTFS_MAXINODE) -d $(TARGET_DIR)/ $(KDIR)/root.ext2
		$(call Image/Build,ext2)
  endef
endif

ifneq ($(CONFIG_TARGET_ROOTFS_ISO),)
  define Image/mkfs/iso
		$(call Image/Build,iso)
  endef
endif

define Image/mkfs/prepare/arcadyan
	$(call Image/mkfs/prepare/arcdftconf)
	$(call Image/mkfs/prepare/arcmid)
	$(call Image/mkfs/prepare/arcsymlink)
#	$(call Image/mkfs/prepare/arccert)
endef

define Image/mkfs/prepare/default
	- $(FIND) $(TARGET_DIR) -type f -not -perm +0100 -not -name 'ssh_host*' -print0 | $(XARGS) -0 chmod 0644
	- $(FIND) $(TARGET_DIR) -type f -perm +0100 -print0 | $(XARGS) -0 chmod 0755
	- $(FIND) $(TARGET_DIR) -type d -print0 | $(XARGS) -0 chmod 0755
	$(INSTALL_DIR) $(TARGET_DIR)/tmp
	chmod 0777 $(TARGET_DIR)/tmp
endef

define Image/mkfs/prepare
	$(call Image/mkfs/prepare/default)
	$(call Image/mkfs/prepare/arcadyan)
endef


define Image/Checksum
	( cd ${BIN_DIR} ; \
		$(FIND) -maxdepth 1 -type f \! -name 'md5sums'  -printf "%P\n" | sort | xargs \
		md5sum --binary > md5sums \
	)
endef


define BuildImage

  download:
  prepare:

  ifeq ($(IB),)
    compile: compile-targets FORCE
		$(call Build/Compile)
  else
    compile:
  endif


  ifeq ($(IB),)
    install: compile install-targets FORCE
		$(call Image/Prepare)
		$(call Image/mkfs/prepare)
		$(call Image/BuildKernel)
		$(call Image/mkfs/cpiogz)
		$(call Image/mkfs/targz)
		$(call Image/mkfs/ext2)
		$(call Image/mkfs/iso)
		$(call Image/mkfs/jffs2)
		$(call Image/mkfs/squashfs)
		$(call Image/mkfs/ubifs)
		###### added by ctc ######
		$(call Image/mkfs/firmware)
		$(call Image/mkfs/fullimage)
		##########################
		$(call Image/Checksum)
  else
    install: compile install-targets
		$(call Image/BuildKernel)
		$(call Image/mkfs/cpiogz)
		$(call Image/mkfs/targz)
		$(call Image/mkfs/ext2)
		$(call Image/mkfs/iso)
		$(call Image/mkfs/jffs2)
		$(call Image/mkfs/squashfs)
		$(call Image/mkfs/ubifs)
		###### added by ctc ######
		$(call Image/mkfs/firmware)
		$(call Image/mkfs/fullimage)
		##########################
		$(call Image/Checksum)
  endif

  ###### added by ctc ######
  firmware:
	$(call Image/mkfs/firmware)

  fullimage:
	$(call Image/mkfs/fullimage)
  ##########################

  ifeq ($(IB),)
    clean: clean-targets
		$(call Build/Clean)
  else
    clean:
  endif

  compile-targets:
  install-targets:
  clean-targets:

endef
