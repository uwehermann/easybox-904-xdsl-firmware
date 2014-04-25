#!/bin/sh

FLASHARGS='setenv bootargs root=$(rootfsmtd) rw rootfstype=squashfs,jffs2'
FLASHARGS2='setenv bootargs root=$(rootfsmtd2) rw rootfstype=squashfs,jffs2 init=/etc/preinit'

PROJ_OP_MODE_CAL="bridge mptest calibrate"
PROJ_OP_MODE_MPT="mptest"
PROJ_OP_MODE_EMI="bridge emi"
PROJ_OP_MODE_RTD_NOVLAN="novlan"
PROJ_OP_MODE_RTD=""
PROJ_OP_MODE_BDG_MPT="bridge mptest"

# $1 - prompt
input_string()
{
	IN_SEL=
	read -p "$1 --> " IN_SEL
}

display_project_mode()
{
	echo

	DATA=`uboot_env --get --name flashargs 2>&-`
	DATA=${DATA/$FLASHARGS /}
	if [ "$DATA" == "$PROJ_OP_MODE_CAL" ] ; then
		echo " current operation mode : WiFi calibration"
	elif [ "$DATA" == "$PROJ_OP_MODE_MPT" ] ; then
		echo " current operation mode : production test"
	elif [ "$DATA" == "$PROJ_OP_MODE_EMI" ] ; then
		echo " current operation mode : EMI test"
	elif [ "$DATA" == "$PROJ_OP_MODE_RTD_NOVLAN" ] ; then
		echo " current operation mode : normal routed without VLAN"
	elif [ "$DATA" == "$PROJ_OP_MODE_RTD" ] ; then
		echo " current operation mode : normal routed"
	elif [ "$DATA" == "$PROJ_OP_MODE_BDG_MPT" ] ; then
		echo " current operation mode : production test with bridge"
	fi

	echo
}

display_project_selection_opmode()
{
	echo
	echo "  1. switch to WiFi calibration mode"
	echo "  2. switch to production test  mode"
	echo "  3. switch to EMI test         mode"
	echo "  4. switch to noraml routed    mode without VLAN"
	echo "  5. switch to noraml routed    mode"
	echo "  6. switch to production test  mode 2" 
	echo "---------------------------------------------------"
	echo "  x. exit"
	echo
}

while [ "1" ] ; do
	echo "==================================================="
	display_project_selection_opmode

	display_project_mode

	input_string "Please select an item"

	SEL=$IN_SEL
	case "${SEL:0:1}" in
		"1") # switch to WiFi calibration mode
			uboot_env --set --name flashargs --value "$FLASHARGS $PROJ_OP_MODE_CAL" >&-
			uboot_env --set --name flashargs2 --value "$FLASHARGS2 $PROJ_OP_MODE_CAL" >&-
			;;
		"2") # switch to production test mode
			uboot_env --set --name flashargs --value "$FLASHARGS $PROJ_OP_MODE_MPT" >&-
			uboot_env --set --name flashargs2 --value "$FLASHARGS2 $PROJ_OP_MODE_MPT" >&-
			;;
		"3") # switch to EMI test mode
			uboot_env --set --name flashargs --value "$FLASHARGS $PROJ_OP_MODE_EMI" >&-
			uboot_env --set --name flashargs2 --value "$FLASHARGS2 $PROJ_OP_MODE_EMI" >&-
			;;
		"4") # switch to normal mode without VLAN
			uboot_env --set --name flashargs --value "$FLASHARGS $PROJ_OP_MODE_RTD_NOVLAN" >&-
			uboot_env --set --name flashargs2 --value "$FLASHARGS2 $PROJ_OP_MODE_RTD_NOVLAN" >&-
			;;
		"5") # switch to normal mode
			uboot_env --set --name flashargs --value "$FLASHARGS $PROJ_OP_MODE_RTD" >&-
			uboot_env --set --name flashargs2 --value "$FLASHARGS2 $PROJ_OP_MODE_RTD" >&-
			;;
		"6") # switch to production mode with bridge
			uboot_env --set --name flashargs --value "$FLASHARGS $PROJ_OP_MODE_BDG_MPT" >&-
			uboot_env --set --name flashargs2 --value "$FLASHARGS2 $PROJ_OP_MODE_BDG_MPT" >&-
			;;
		#---------------------------------------------------------
		"x"|"X") # exit
			break
			;;
		*) # bad choice
			echo "BAD CHOICE !!!"
			;;
	esac
done
