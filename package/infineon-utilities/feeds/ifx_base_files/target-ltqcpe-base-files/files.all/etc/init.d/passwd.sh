#!/bin/sh /etc/rc.common

START=03

#Need to enable later on once utils are in place, remove it from rcS

start() {
        touch /ramdisk/flash/passwd
        i=0
        while [ $i -lt $passFileLineCount ]
        do
                eval passVar='$passFileLineCount'$i
                echo $passVar >> /ramdisk/flash/passwd
       		i=$(( $i + 1 ))
	done

	if ! [ -f /etc/rc.conf ]; then
		echo "root:\$1\$\$CoERg7ynjYLsj2j4glJ34.:0:0:root:/root:/bin/sh" > /ramdisk/flash/passwd
		echo "admin:\$1\$\$CoERg7ynjYLsj2j4glJ34.:0:0:root:/root:/bin/sh" >> /ramdisk/flash/passwd
	fi
}
