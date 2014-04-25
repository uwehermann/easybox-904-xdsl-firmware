#!/bin/sh

echo "1" > /tmp/etc/config/no_con_output.txt

uboot_env --set --name console --value ''

ccfg_cli commitcfg

