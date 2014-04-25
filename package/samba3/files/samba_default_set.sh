#!/bin/sh

DRV_STAT_PATH="/tmp/fs"
VOL_INFO_FILE="${DRV_STAT_PATH}/volinfo"

local MAX_CFG_KEEP
local CFGID=0
local DRV_DATA
local CHECK=0
local VALID_ID=-1
local MAX_SEQ=0
local MIN_SEQ=99
local MIN_SEQ_ID
local VENDOR
local MODEL
local DEV
local PARTITION
local PARTITION2
local PARTITION3
local PARTITION4
local MOUNT_NUM

#set to User-Mode for 904-MFR
ccfg_cli set mode@samba=1

MAX_CFG_KEEP=`ccfg_cli get max_cfg_keep@samba`

echo "samba_default_set" > /dev/console

VENDOR="$1"
MODEL="$2"
DEV="$3"

echo VENDOR=$VENDOR MODEL=$MODEL DEV=$DEV > /dev/console

dev_id=`ccfg_cli -f "$VOL_INFO_FILE" get ${DEV}_id@volinfo`
#echo dev_id = $dev_id > /dev/console

MOUNT_NUM=`ccfg_cli -f "$VOL_INFO_FILE" get ${DEV}_mount_num@volinfo`
PARTITION=`ccfg_cli -f "$VOL_INFO_FILE" get volLabelMix@disk${dev_id}_1`
#echo PARTITION= $PARTITION > /dev/console
if [ "${MOUNT_NUM}" -ge "2" ]; then
		PARTITION2=`ccfg_cli -f "$VOL_INFO_FILE" get volLabelMix@disk${dev_id}_2`
fi
if [ "${MOUNT_NUM}" -ge "3" ]; then
		PARTITION3=`ccfg_cli -f "$VOL_INFO_FILE" get volLabelMix@disk${dev_id}_3`
fi
if [ "${MOUNT_NUM}" -ge "4" ]; then
		PARTITION4=`ccfg_cli -f "$VOL_INFO_FILE" get volLabelMix@disk${dev_id}_4`
fi

if [ -z "${dev_id}" ] || [ -z "${PARTITION}" ]; then
	return
fi

#echo "VENDOR =" $1 >/dev/console
#echo "MODEL = " $2 >/dev/console
#echo "DEV = " $3 >/dev/console

VALID_ID=-1
CHECK=0

while [ "${CFGID}" -le "${MAX_CFG_KEEP}" ]
do
	DRV_DATA=`ccfg_cli get smb${CFGID}_drive@samba`
	#echo DRVDATA= $DRV_DATA >/dev/console

	VALID=`echo "$DRV_DATA"  | cut -d ':' -f 1`
	SEQ=`echo "$DRV_DATA"  | cut -d ':' -f 3`
	CFGVENDOR=`echo "$DRV_DATA"  | cut -d ':' -f 4`
	CFGMODEL=`echo "$DRV_DATA"  | cut -d ':' -f 5`

	if [ "${SEQ}" -ge "${MAX_SEQ}" ]; then
		MAX_SEQ=$SEQ
	fi

	if [ "${SEQ}" -lt "${MIN_SEQ}" ]; then
		MIN_SEQ_ID=$CFGID
		MIN_SEQ=$SEQ
	fi

	if [ "${VALID_ID}" == "-1" ] && [ "${VALID}" -eq "0" ]; then
		VALID_ID=$CFGID
	fi

	if [ "${VENDOR}" == "${CFGVENDOR}" ] && [ "${MODEL}" == "${CFGMODEL}" ]; then
		CHECK=1
	fi

	let CFGID=$CFGID+1
done

#echo CHECK = $CHECK, MIN_SEQ_ID = $MIN_SEQ_ID, VALID_ID = $VALID_ID, MAX_SEQ = $MAX_SEQ >/dev/console
if [ -z "$MODEL" ]; then
	MODEL="#"
fi
 
if [ "${CHECK}" == "0" ]; then
	let MAX_SEQ=$MAX_SEQ+1

	if [ "${VALID_ID}" == "-1" ]; then
		ccfg_cli set smb${MIN_SEQ_ID}_drive@samba="1:1:${MAX_SEQ}:${VENDOR}:${MODEL}:"
		ccfg_cli set smb${MIN_SEQ_ID}_folder0@samba="1:${VENDOR}:1:/:0:::${PARTITION}"

		ccfg_cli set smb_account${MIN_SEQ_ID}@samba="0:0:#:#:Partition:${VENDOR}:${MODEL}:|||||||:|||||||:0:${DEV}:0:1:SN"		

		if [ "${MOUNT_NUM}" -ge "2" ]; then
			ccfg_cli set smb${MIN_SEQ_ID}_folder1@samba="1:${VENDOR}_1:2:/:2:::${PARTITION2}"
		fi
		if [ "${MOUNT_NUM}" -ge "3" ]; then
			ccfg_cli set smb${MIN_SEQ_ID}_folder2@samba="1:${VENDOR}_2:3:/:2:::${PARTITION3}"
		fi
		if [ "${MOUNT_NUM}" -ge "4" ]; then
			ccfg_cli set smb${MIN_SEQ_ID}_folder3@samba="1:${VENDOR}_3:4:/:2:::${PARTITION4}"
		fi
	else
		ccfg_cli set smb${VALID_ID}_drive@samba="1:1:${MAX_SEQ}:${VENDOR}:${MODEL}:"
		ccfg_cli set smb${VALID_ID}_folder0@samba="1:${VENDOR}:1:/:0:::${PARTITION}"

		ccfg_cli set smb_account${VALID_ID}@samba="0:0:#:#:Partition:${VENDOR}:${MODEL}:|||||||:|||||||:0:${DEV}:0:1:SN"

		if [ "${MOUNT_NUM}" -ge "2" ]; then
			ccfg_cli set smb${VALID_ID}_folder1@samba="1:${VENDOR}_1:2:/:2:::${PARTITION2}"
		fi
		if [ "${MOUNT_NUM}" -ge "3" ]; then
			ccfg_cli set smb${VALID_ID}_folder2@samba="1:${VENDOR}_2:3:/:2:::${PARTITION3}"
		fi
		if [ "${MOUNT_NUM}" -ge "4" ]; then
			ccfg_cli set smb${VALID_ID}_folder3@samba="1:${VENDOR}_3:4:/:2:::${PARTITION4}"
		fi
	fi

	/usr/sbin/sambacfg.sh smb_user_cfg_set
	/etc/init.d/samba start
fi

