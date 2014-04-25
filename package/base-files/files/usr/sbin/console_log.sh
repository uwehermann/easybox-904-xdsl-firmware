#!/bin/sh

FILE_SIZE=102400	# if size of console.log exceed 100K, zip it as console_log_XXXXXXXX.gz
TIME_MIN=10			# keep zip files which create less 10 mins 
SOURCE_FILE="/tmp/consolelog/console.log"
BACKUP_DIR="/tmp/consolelog"
BACKUP_NAM="console_log_"
MAX_DIR_SIZE=1048576																															
	if [ $(stat -c %s $SOURCE_FILE) -ge $FILE_SIZE ] ; then	
		DATE=$(date +%m%d%H%M)
		cp $SOURCE_FILE $BACKUP_DIR/$BACKUP_NAM${DATE} 		
		killall cat
		echo "">$SOURCE_FILE
		cat /proc/kmsg >$SOURCE_FILE &
		gzip $BACKUP_DIR/$BACKUP_NAM${DATE}
		#find $BACKUP_DIR/$BACKUP_NAM* -mmin +$TIME_MIN -exec rm {} \;
		TSIZE=`du -s $BACKUP_DIR | awk '{ print $1 }'`
		if [ $size -ge $MAX_DIR_SIZE ] ; then
			OLDFILE=`find $BACKUP_DIR/$BACKUP_NAM* | sort | sed '2,$d'`
			rm -f $OLDFILE
			#find $BACKUP_DIR/$BACKUP_NAM* -mmin +$TIME_MIN -exec rm {} \;
		fi
	fi
