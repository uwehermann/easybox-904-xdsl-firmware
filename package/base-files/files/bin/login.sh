#!/bin/sh
# Copyright (C) 2006-2010 OpenWrt.org

### ctc ###
#if grep -qs '^root:[^!]' /etc/passwd /etc/shadow && [ -z "$FAILSAFE" ]; then
#	echo "Login failed."
#	exit 0
#else
#cat << EOF
# === IMPORTANT ============================
#  Use 'passwd' to set your login password
#  this will disable telnet and enable SSH
# ------------------------------------------
#EOF
#fi
#
#exec /bin/ash --login
exec login
###########
