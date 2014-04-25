#!/bin/sh

for i in $*; do
	version=`zcat $i | strings | grep "@(#)"`
	echo $i : $version
done

