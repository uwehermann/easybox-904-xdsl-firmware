#!/bin/sh

if [ -f /tmp/adsl_status ] && [ `grep -c 801 /tmp/adsl_status` -ge 1 ] ; then
	libmapi_dsl_cli
else
	echo link is down
fi
