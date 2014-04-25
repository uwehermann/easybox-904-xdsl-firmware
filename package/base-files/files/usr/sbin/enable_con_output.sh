#!/bin/sh

echo "0" > /tmp/etc/config/no_con_output.txt

uboot_env --set --name console --value 'ttyS0'

ccfg_cli commitcfg

