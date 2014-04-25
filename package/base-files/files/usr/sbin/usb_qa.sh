#!/bin/sh

start()
{
	mem -s 0x1e101440 -u -w 0x00001000
	mem -s 0x1e101440 -u -w 0x00001100
	mem -s 0x1e101440 -u -w 0x00001000
	mem -s 0x1e101440 -u -w 0x00009000
	mem -s 0x1e10103c -u -w 0x00000004
}

stop()
{
	mem -s 0x1e101440 -u -w 0x00001000
	mem -s 0x1e10103c -u -w 0x00000000
	mem -s 0x1e101440 -u -w 0x00001000
}

case $1 in
	"start")
			start ;;
	"stop")
			stop ;;
	*)
			echo "$0 start - start USB eye pattern test"
			echo "$0 stop  - stop USB eye pattern test"
			;;
esac
