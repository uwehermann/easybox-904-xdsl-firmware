#!/bin/sh

usage()
{
	echo "Usage"
	echo "        redirect_console.sh <on/off>    Enable/Disable console redirect feature"
}

if [ -z "$1" ] ; then
	usage;
else
	if [ "$1" == "on" ] ; then
		echo "Enable console redirect feature"
		uboot_env --set --name log2file --value "1" >&-
	elif [ "$1" == "off" ] ; then
		echo "Disable console redirect feature"
		uboot_env --set --name log2file --value "" >&-
	else
		usage;
	fi
fi
