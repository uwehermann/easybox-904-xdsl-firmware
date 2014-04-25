#!/bin/sh

for i in $*; do
	version=`strings $i | grep "@(#)"`
	echo $i : $version
done

