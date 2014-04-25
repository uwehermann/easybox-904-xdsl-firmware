#!/bin/sh

[ -z "$1" ] && echo "Usage $0 <mount/umount> <dir1> <dir2> ... <dirN>" && exit 1

_CMD=$1

shift

for i in $@; do
        [ "$_CMD" = "mount" ] && mkdir -p /tmp/$i && mount -t mini_fo -o base=$i,sto=/tmp$i $i $i
	[ "$_CMD" = "umount" ] && umount $i && (cd /tmp; rmdir -p $i; cd -)
done

