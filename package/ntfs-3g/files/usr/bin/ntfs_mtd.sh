#!/bin/sh
#
# Copyright (C) 2009 Arcadyan
# All Rights Reserved.
#
# FTP configuration shell program
#
#
# $1 - device node, e.g. /dev/sda1, /dev/sdb2
# $2 - NTFS version of partition
# $3 - label name
#

NTFS_MTD_CFG="/tmp/ntfs_mtd.cfg"

if [ -z "$2" ] ; then
	exit 1
fi

if ! [ -f "${NTFS_MTD_CFG}" ] ; then
	echo "!" > ${NTFS_MTD_CFG}
fi

ccfg_cli -f ${NTFS_MTD_CFG} set version@$1=$2
ccfg_cli -f ${NTFS_MTD_CFG} set label@$1=$3

