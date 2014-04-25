#! /bin/sh

# This script toggles the FW Variants:
# VDSL2 only, ADSL Annex A, Annex B images
# The gzip compressed images shall be extracted into a temp-fs
# folder and used from there via link

scriptname=$0
fwdir=/opt/lantiq/firmware

usage()
{
	echo "Usage: $scriptname <dest-dir> V|A|B"
	exit 1
}

# unzip <file> <to-path> <link index>
unzip()
{
	BASEPATH=$2

	if [ -e ${fwdir}/$1 ]; then
		echo "link $1 to $BASEPATH"
		ln -snf ${fwdir}/$1  $BASEPATH
	else
		echo "copy and unzip $1 to $BASEPATH"
		cp ${fwdir}/$1.gz  $BASEPATH
		gzip -d $BASEPATH/$1.gz
	fi
	[ $# -eq 3 ] && ln -snf $BASEPATH/$1 $BASEPATH/$3
}

set_fw()
{
	BASEPATH=$1
	echo "linking FW from $BASEPATH variant $2"
	rm -f $BASEPATH/acpe_hw_*.bin
	rm -f $BASEPATH/vcpe_hw.bin
	case $2 in
	V|v)
		unzip vcpe_hw.bin $BASEPATH
		;;
	VA|va)
		unzip acpe_hw_a.bin $BASEPATH acpe_hw.bin
		unzip vcpe_hw.bin $BASEPATH
		;;
	A|a)
		unzip acpe_hw_a.bin $BASEPATH acpe_hw.bin
		;;
	VB|vb)
		unzip acpe_hw_b.bin $BASEPATH acpe_hw.bin
		unzip vcpe_hw.bin $BASEPATH
		;;
	B|b)
		unzip acpe_hw_b.bin $BASEPATH acpe_hw.bin
		;;
	*) usage
		;;
	esac
}

# check inputs
if [ "$2" != "" ]; then
	FW_DEST=$1
	FW_VARIANT=$2

	case $FW_VARIANT in
	V|v|A|a|VA|va|B|b|VB|vb)
		;;
	*) usage
		;;
	esac
else
	usage
fi

echo "==== using FW Variant $FW_VARIANT in $FW_DEST ===="

set_fw $FW_DEST $FW_VARIANT

exit 0
