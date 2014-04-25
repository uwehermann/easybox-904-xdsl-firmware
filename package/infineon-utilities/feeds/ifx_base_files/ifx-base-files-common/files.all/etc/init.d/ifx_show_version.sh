#!/bin/sh /etc/rc.common
# Copyright (C) 2010 OpenWrt.org
# Copyright (C) 2010 Lantiq.com

START=99

bindir=/opt/lantiq/bin

start() {
   # start the dsl daemon
   [ -e ${bindir}/show_version.sh ] && ${bindir}/show_version.sh
	[ -e /opt/lantiq/image_version ] && cat /opt/lantiq/image_version
}
