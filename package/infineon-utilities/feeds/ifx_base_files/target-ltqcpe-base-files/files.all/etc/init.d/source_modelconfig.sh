#!/bin/sh /etc/rc.common
#START=03
start() {
	if [ -r /etc/rc.d/model_config.sh ]; then
		. /etc/rc.d/model_config.sh
	fi
}
