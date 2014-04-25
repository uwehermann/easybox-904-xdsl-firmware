#!/bin/sh

NTP_DIR=/etc/config
NTP_HTML=${NTP_DIR}/www.pool.ntp.org_zone_de.html

mkdir $NTP_DIR
#wget http://www.pool.ntp.org/zone/de --directory-prefix=$NTP_DIR

NTP_COUNT=$(ccfg_cli get servercnt@ntp)

if [ -z "$NTP_COUNT" ] ; then
	NTP_COUNT=0
fi

i=$NTP_COUNT

#delete type "1" server
while [ $i -ge 1 ] ; do

	if [ "`ccfg_cli get servertype${i}@ntp`" == 1 ] ; then
		/etc/init.d/ntpclient2 del-svr `ccfg_cli get server${i}@ntp`
	fi

	let i=$i-1

done

#parse HTML
ALLSVR=`awk 'BEGIN { head=-1; tail=-1 } $1~/<pre>/{head=NR} $1~/<\/pre>/{tail=NR} { if ( head != -1 && NR > head && $1 !~/<\/pre>/ && tail==-1 ) { print $2} }' ${NTP_HTML}`

#add server with type "1"
for SVR in $ALLSVR ; do
	/etc/init.d/ntpclient2 add-svr $SVR "1"
done
