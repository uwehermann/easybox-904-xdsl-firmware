#!/bin/sh 

if [ ! "$ENVLOADED" ]; then
	if [ -r /etc/rc.conf ]; then
		. /etc/rc.conf 2> /dev/null
		ENVLOADED="1"
	fi
fi

if [ -r /etc/rc.d/config.sh ]; then
	. /etc/rc.d/config.sh 2>/dev/null
fi


if [ "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A5_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D5_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A4_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_A4_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D4_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_D4_MOD" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_E5_BUILTIN" = "11" -o "1$CONFIG_PACKAGE_KMOD_LTQCPE_PPA_E5_MOD" = "11" ]; then

	/sbin/ppacmd $1 -i $2 2> /dev/null

fi
