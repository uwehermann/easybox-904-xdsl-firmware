#!/bin/sh

echo "#ifndef __ARC_OPENWRT_CONFIG_H" > $2
echo "#define __ARC_OPENWRT_CONFIG_H" >> $2
echo "/*"  >> $2
echo " * Automatically generated from .config. Do NOT EDIT!!"  >> $2
echo " * If .config is modified, change_project.sh MUST be ran again to update this header file" >> $2
echo " */" >> $2
grep "^#" -v $1 | sed '/^$/d' | sed 's/=/:=/' | awk -F":=" '{ gsub(/-/,"_",$1); if ($2 == "y") print "#define " $1 " 1" ; else print "#define " $1 " " $2}' >> $2
echo "" >> $2
echo "#endif" >> $2
