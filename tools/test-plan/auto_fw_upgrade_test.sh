#!/bin/sh

FW_NAME="fullimage.root.squash.signed"
LOCK_FILE="/tmp/__auto_fw_upgrade.lock"

[[ -f $LOCK_FILE ]] && exit 1 || touch $LOCK_FILE

cd /tmp
tftp -gr $FW_NAME 192.168.2.100

if MACverify.sh fullN /tmp/$FW_NAME; then
        if upgrade.sh fullN /tmp/$FW_NAME; then
                reboot.sh
        else
                echo "upgrade.sh failed. Return $?" > /dev/console
                echo -e "Dump FW Upgrade Log\n" > /dev/console
                cat /tmp/etc/config/last_fw_upgrade_log.txt > /dev/console
        fi
else
        echo "MACverify.sh failed. Return $?" > /dev/console
fi

