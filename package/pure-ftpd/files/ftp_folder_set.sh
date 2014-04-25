#!/bin/sh

DRV_STAT_PATH="/tmp/fs"
DRV_STAT_FILE="${DRV_STAT_PATH}/drvstat"
VOL_INFO_FILE="${DRV_STAT_PATH}/volinfo"

FTP_PROGRAM="/etc/init.d/pure-ftpd2"
FTP_PW_PROG="pure-pw"
FTP_CFG_DIR="/tmp/pure-ftpd"
FTP_PWD_FILE="$FTP_CFG_DIR/pure-ftpd.pwd"
FTP_PDB_FILE="$FTP_CFG_DIR/pure-ftpd.pdb"


#for dir in `df | grep -w "/tmp/ftp1" |cut -d '%' -f 2 |cut -d ' ' -f 2`;
#do
#  a=`fuser -m "${dir}"`
#  echo dir="${dir}", process=$a >/dev/console

#  umount -l "${dir}"
#done

#for dir in `df | grep -w "/tmp/ftp2" |cut -d '%' -f 2 |cut -d ' ' -f 2`;
#do
#  a=`fuser -m "${dir}"`
#  echo dir="${dir}", process=$a >/dev/console

#  umount -l "${dir}"
#done

#sleep 3

rm -rf /tmp/ftp1
mkdir -m 0755 -p /tmp/ftp1

rm -rf /tmp/ftp2
mkdir -m 0755 -p /tmp/ftp2

ftp_disk_online_check()
{
	local DRIVE_ID

	DRIVE_ID=0

	ONLINE1=`ccfg_cli -f "$DRV_STAT_FILE" get online1@drive`
	VENDOR1=`ccfg_cli -f "$DRV_STAT_FILE" get vendor1@drive`
	MODEL1=`ccfg_cli -f "$DRV_STAT_FILE" get model1@drive`
	SERIAL_NUM1=`ccfg_cli -f "$DRV_STAT_FILE" get serial_num1@drive`
	DEVNAME1=`ccfg_cli -f "$DRV_STAT_FILE" get devname1@drive`

	ONLINE2=`ccfg_cli -f "$DRV_STAT_FILE" get online2@drive`
	VENDOR2=`ccfg_cli -f "$DRV_STAT_FILE" get vendor2@drive`
	MODEL2=`ccfg_cli -f "$DRV_STAT_FILE" get model2@drive`
	SERIAL_NUM2=`ccfg_cli -f "$DRV_STAT_FILE" get serial_num2@drive`
	DEVNAME2=`ccfg_cli -f "$DRV_STAT_FILE" get devname2@drive`

	#echo $ONLINE1 $VENDOR1 $MODEL1 $DEVNAME1> /dev/console	
	#echo $ONLINE2 $VENDOR2 $MODEL2 $DEVNAME2> /dev/console	

		MAX_DRIVE=`ccfg_cli get max_drive@drive`

		while [ "${DRIVE_ID}" -lt "${MAX_DRIVE}" ];
		do
			DRV_DATA=`ccfg_cli get ftp_disk${DRIVE_ID}@drive`

			#echo DRV_DATA = $DRV_DATA > /dev/console
			online=`echo "$DRV_DATA"  | cut -d ':' -f 1`
			enable_sharing=`echo "$DRV_DATA"  | cut -d ':' -f 2`
			share_all_folder=`echo "$DRV_DATA"  | cut -d ':' -f 3`
			vendor=`echo "$DRV_DATA"  | cut -d ':' -f 4`
			model=`echo "$DRV_DATA"  | cut -d ':' -f 5`
			serial_num=`echo "$DRV_DATA"  | cut -d ':' -f 6`
			dev_name=`echo "$DRV_DATA"  | cut -d ':' -f 7`
			username=`echo "$DRV_DATA"  | cut -d ':' -f 8`
			password=`echo "$DRV_DATA"  | cut -d ':' -f 9`

			#echo online = $online > /dev/console
			#echo enable_sharing = $enable_sharing > /dev/console
			#echo share_all_folder = $share_all_folder > /dev/console
			#echo vendor = $vendor > /dev/console
			#echo model = $model > /dev/console
			#echo serial_num = $serial_num > /dev/console
			#echo dev_name = $dev_name > /dev/console
			#echo username = $username > /dev/console
			#echo password = $password > /dev/console

		if [ -n "$DRV_DATA" ]; then
				if [ "${ONLINE1}" == "1" ] && [ "${vendor}" == "${VENDOR1}" ] && [ "${model}" == "${MODEL1}" ] && [ "${serial_num}" == "${SERIAL_NUM1}" ]; then
				ccfg_cli set ftp_disk${DRIVE_ID}@drive="1:${enable_sharing}:${share_all_folder}:${vendor}:${model}:${serial_num}:${dev_name}:${username}:${password}"
				elif [ "${ONLINE2}" == "1" ] && [ "${vendor}" == "${VENDOR2}" ] && [ "${model}" == "${MODEL2}" ] && [ "${serial_num}" == "${SERIAL_NUM2}" ]; then
				ccfg_cli set ftp_disk${DRIVE_ID}@drive="1:${enable_sharing}:${share_all_folder}:${vendor}:${model}:${serial_num}:${dev_name}:${username}:${password}"
			else
				ccfg_cli set ftp_disk${DRIVE_ID}@drive="0:${enable_sharing}:${share_all_folder}:${vendor}:${model}:${serial_num}:${dev_name}:${username}:${password}"
			fi
			fi
								
			let DRIVE_ID=$DRIVE_ID+1
		done
}

#$1 0:mount, 1:umount
#$2 vendor
#$3	model
#$4 serial number
#$5	device name

ftp_default_folder_set()
{
	local DRIVE_ID
	local VALID_ID
	local CHECK
	local DEV_NAME

	echo "[ftp_folder_set] setting ftp default folder >>>"> /dev/console	
	#echo $2 $3 $4 $5> /dev/console			

	

	CHECK=0
	DRIVE_ID=0
	VALID_ID=-1

	if [ "$1" == "0" ]; then
		ftp_disk_online_check

		MAX_DRIVE=`ccfg_cli get max_drive@drive`

		MAX_FOLDER=`ccfg_cli get max_folder@drive`

		while [ "${DRIVE_ID}" -lt "${MAX_DRIVE}" ];
		do
			ONLINE=0

			DRV_DATA=`ccfg_cli get ftp_disk${DRIVE_ID}@drive`

			#echo DRV_DATA = $DRV_DATA > /dev/console

			online=`echo "$DRV_DATA"  | cut -d ':' -f 1`
			enable_sharing=`echo "$DRV_DATA"  | cut -d ':' -f 2`
			share_all_folder=`echo "$DRV_DATA"  | cut -d ':' -f 3`	
			vendor=`echo "$DRV_DATA"  | cut -d ':' -f 4`
			model=`echo "$DRV_DATA"  | cut -d ':' -f 5`
			serial_num=`echo "$DRV_DATA"  | cut -d ':' -f 6`
			dev_name=`echo "$DRV_DATA"  | cut -d ':' -f 7`
			username=`echo "$DRV_DATA"  | cut -d ':' -f 8`
			password=`echo "$DRV_DATA"  | cut -d ':' -f 9`

			#echo online = $online > /dev/console
			#echo enable_sharing = $enable_sharing > /dev/console
			#echo share_all_folder = $share_all_folder > /dev/console
			#echo vendor = $vendor > /dev/console
			#echo model = $model > /dev/console
			#echo serial_num = $serial_num > /dev/console
			#echo dev_name = $dev_name > /dev/console
			#echo username = $username > /dev/console
			#echo password = $password > /dev/console
			
			if [ -n "${DRV_DATA}" ]; then
				if [ "${vendor}" == "${2}" ] && [ "${model}" == "${3}" ] && [ "${serial_num}" == "${4}" ]; then
					ccfg_cli set ftp_disk${DRIVE_ID}@drive="1:${enable_sharing}:${share_all_folder}:${vendor}:${model}:${serial_num}:${5}:${username}:${password}"
					CHECK=1
				fi
				else
				if [ "${VALID_ID}" == "-1" ]; then
				VALID_ID=$DRIVE_ID
				fi
			fi

			if [ "${online}" == "0" ] && [ "${VALID_ID}" == "-1" ]; then
				VALID_ID=$DRIVE_ID
			fi
								
			let DRIVE_ID=$DRIVE_ID+1
		done

		if [ "${CHECK}" == "0" ]; then
				ccfg_cli set ftp_disk${VALID_ID}@drive="1:0:1:${2}:${3}:${4}:${5}:root${VALID_ID}:"
				
				FOLDER_ID=0

				while [ "${FOLDER_ID}" -lt "${MAX_FOLDER}" ];
				do
					ccfg_cli unset ftp${VALID_ID}_folder${FOLDER_ID}@drive
					ccfg_cli unset ftp${VALID_ID}_folder${FOLDER_ID}_display_name@drive

					let FOLDER_ID=$FOLDER_ID+1
				done
		fi
	else
		ftp_disk_online_check
	fi

	ftp_folder_add
}

ftp_folder_add()
{
	local DRIVE_ID=0
	local FOLDER_ID=0
	local ONLINE1=0
	local VENDOR1
	local MODEL1
	local DEVNAME1
	local ONLINE2=0
	local VENDOR2
	local MODEL2
	local DEVNAME2
	local DEV_NAME
	local LAN_IP
	local SET_STR
	local FOLDER_DATA
	local USR_PASSWORD
	local nolabel_count=0
	local tmp_nolabel_count=0
	local display_name
	local directory_type=1  # type 1= /vol_label/display_name , type 2= complete directory structure.


	MAX_DRIVE=`ccfg_cli get max_drive@drive`
	MAX_FOLDER=`ccfg_cli get max_folder@drive`
	CONTENT_SHARING=`ccfg_cli get content_sharing@drive`

	rm -rf $FTP_PWD_FILE

	#FTP_HOME=/tmp/ftp

	if [ "${CONTENT_SHARING}" == "0" ]; then
		"$FTP_PROGRAM" stop
		exit
	fi

	ONLINE1=`ccfg_cli -f "$DRV_STAT_FILE" get online1@drive`
	VENDOR1=`ccfg_cli -f "$DRV_STAT_FILE" get vendor1@drive`
	MODEL1=`ccfg_cli -f "$DRV_STAT_FILE" get model1@drive`
	SERIAL_NUM1=`ccfg_cli -f "$DRV_STAT_FILE" get serial_num1@drive`
	DEVNAME1=`ccfg_cli -f "$DRV_STAT_FILE" get devname1@drive`

	ONLINE2=`ccfg_cli -f "$DRV_STAT_FILE" get online2@drive`
	VENDOR2=`ccfg_cli -f "$DRV_STAT_FILE" get vendor2@drive`
	MODEL2=`ccfg_cli -f "$DRV_STAT_FILE" get model2@drive`
	SERIAL_NUM2=`ccfg_cli -f "$DRV_STAT_FILE" get serial_num2@drive`
	DEVNAME2=`ccfg_cli -f "$DRV_STAT_FILE" get devname2@drive`

	CHECK=0

	while [ "${DRIVE_ID}" -lt "${MAX_DRIVE}" ];
	do
		FOLDER_ID=0
		nolabel_count=0

		DRV_DATA=`ccfg_cli get ftp_disk${DRIVE_ID}@drive`

		#echo DRV_DATA = $DRV_DATA > /dev/console

		online=`echo "$DRV_DATA"  | cut -d ':' -f 1`
		enable_sharing=`echo "$DRV_DATA"  | cut -d ':' -f 2`
		share_all_folder=`echo "$DRV_DATA"  | cut -d ':' -f 3`
		vendor=`echo "$DRV_DATA"  | cut -d ':' -f 4`
		model=`echo "$DRV_DATA"  | cut -d ':' -f 5`
		serial_num=`echo "$DRV_DATA"  | cut -d ':' -f 6`
		dev_name=`echo "$DRV_DATA"  | cut -d ':' -f 7`
		username=`echo "$DRV_DATA"  | cut -d ':' -f 8`
		password=`echo "$DRV_DATA"  | cut -d ':' -f 9`

		#echo online = $online > /dev/console
		#echo enable_sharing = $enable_sharing > /dev/console
		#echo share_all_folder = $share_all_folder > /dev/console
		#echo vendor = $vendor > /dev/console
		#echo model = $model > /dev/console
		#echo serial_num = $serial_num > /dev/console
		#echo dev_name = $dev_name > /dev/console
		#echo username = $username > /dev/console
		#echo password = $password > /dev/console

		if [ "${ONLINE1}" == "1" ] && [ "${vendor}" == "${VENDOR1}" ] && [ "${model}" == "${MODEL1}" ] && [ "${serial_num}" == "${SERIAL_NUM1}" ]; then
			DEV_NAME=$DEVNAME1
		elif [ "${ONLINE2}" == "1" ] && [ "${vendor}" == "${VENDOR2}" ] && [ "${model}" == "${MODEL2}" ] && [ "${serial_num}" == "${SERIAL_NUM2}" ]; then
			DEV_NAME=$DEVNAME2
		fi

		#echo DEV_NAME = $DEV_NAME > /dev/console

		if [ "${DEV_NAME}" == "sda" ]; then
			dev_id=0
			FTP_HOME="/tmp/ftp1"
		elif [ "${DEV_NAME}" == "sdb" ]; then
			dev_id=1
			FTP_HOME="/tmp/ftp2"
		fi

		#echo dev_id = $dev_id > /dev/console

		if [ -n "${DRV_DATA}" ] && [ -n "${DEV_NAME}" ]; then
			if [ "${online}" == "1" ] && [ "${enable_sharing}" == "1" ]; then
				CHECK=1
				#FTP_HOME=/tmp/usb/ftp${DRIVE_ID}

				#rm -rf ${FTP_HOME}
				#mkdir ${FTP_HOME}

				USR_PASSWORD=${password}

				if [ -n "${USR_PASSWORD}" ]; then
					USR_PASSWORD=`echo $USR_PASSWORD | pure-pw crypt | cut -d ' ' -f 3`
				else
					USR_PASSWORD=`echo | pure-pw crypt | cut -d ' ' -f 3`
				fi

				#echo USR_PASSWORD $USR_PASSWORD > /dev/console

				echo "$username:${USR_PASSWORD}:root:root::${FTP_HOME}:::::::::::" >> $FTP_PWD_FILE

				$FTP_PW_PROG mkdb $FTP_PDB_FILE -f $FTP_PWD_FILE

				if [ "${share_all_folder}" == "1" ]; then

					dev_mount_num=`ccfg_cli -f "$VOL_INFO_FILE" get ${DEV_NAME}_mount_num@volinfo`
					#echo dev_mount_num = $dev_mount_num > /dev/console

					part_id=1
					
					while [ "${part_id}" -le "${dev_mount_num}" ];
					do
						vol_type=`ccfg_cli -f "$VOL_INFO_FILE" get volType@disk${dev_id}_${part_id}`
						if [ "${vol_type}" == "Unknown" ]; then
							let part_id=$part_id+1
							continue
						fi
							
						dev_name=`ccfg_cli -f "$VOL_INFO_FILE" get devName@disk${dev_id}_${part_id}`
						#echo dev_name = $dev_name > /dev/console

						mount_folder=${dev_name:2}
						#echo mount_folder = $mount_folder > /dev/console

						ALLPATH="/tmp/usb/"${mount_folder}
						#echo ALLPATH = $ALLPATH > /dev/console

						vol_label=`ccfg_cli -f "$VOL_INFO_FILE" get volLabel@disk${dev_id}_${part_id}`

						if [ "$vol_label" == "<no_label>" ]; then
							if [ "$nolabel_count" == "0" ]; then
								ln -sf ${ALLPATH} "${FTP_HOME}"/DATA
								#mkdir -m 0755 -p "${FTP_HOME}"/DATA
								#mount -o bind "${ALLPATH}" "${FTP_HOME}"/DATA
								let nolabel_count=$nolabel_count+2
							else
								ln -sf ${ALLPATH} "${FTP_HOME}"/DATA-${nolabel_count}
								#mkdir -m 0755 -p "${FTP_HOME}"/DATA-${nolabel_count}
								#mount -o bind "${ALLPATH}" "${FTP_HOME}"/DATA-${nolabel_count}
								let nolabel_count=$nolabel_count+1
							fi
						else
							ln -sf ${ALLPATH} "${FTP_HOME}"/${vol_label}
							#mkdir -m 0755 -p "${FTP_HOME}"/${vol_label}
							#mount -o bind "${ALLPATH}" "${FTP_HOME}"/${vol_label}
						fi

						#ln -sf "${ALLPATH}" "${FTP_HOME}"/partition${part_id}
						#mount --bind /tmp/ftp /tmp/usb
					
						let part_id=$part_id+1

					done
				else
					dev_id=`ccfg_cli -f "$VOL_INFO_FILE" get ${DEV_NAME}_id@volinfo`
					if [ "$dev_id" == "1" ]; then
						nolabel_count=$tmp_nolabel_count
					fi
					mount_num=`ccfg_cli -f "$VOL_INFO_FILE" get ${DEV_NAME}_mount_num@volinfo`
					#echo mount_num = $mount_num > /dev/console
					part_id=1
					while [ "${part_id}" -le "${mount_num}" ];
					do
						vol_label=`ccfg_cli -f "$VOL_INFO_FILE" get volLabel@disk${dev_id}_${part_id}`
						if [ "$vol_label" == "<no_label>" ]; then
							if [ "$nolabel_count" == "0" ]; then
								#ln -sf ${ALLPATH} "/tmp/nas/DATA"
								vol_label_1="DATA"
								let nolabel_count=$nolabel_count+2
							else
								#ln -sf ${ALLPATH} "/tmp/nas/DATA-${nolabel_count}"
								if [ "$nolabel_count" == "2" ]; then
									vol_label_2="DATA-${nolabel_count}"
								fi
								if [ "$nolabel_count" == "3" ]; then
									vol_label_3="DATA-${nolabel_count}"
								fi
								if [ "$nolabel_count" == "4" ]; then
									vol_label_4="DATA-${nolabel_count}"
								fi
								if [ "$nolabel_count" == "5" ]; then
									vol_label_5="DATA-${nolabel_count}"
								fi
								if [ "$nolabel_count" == "6" ]; then
									vol_label_6="DATA-${nolabel_count}"
								fi
								if [ "$nolabel_count" == "7" ]; then
									vol_label_7="DATA-${nolabel_count}"
								fi
								if [ "$nolabel_count" == "8" ]; then
									vol_label_8="DATA-${nolabel_count}"
								fi

								let nolabel_count=$nolabel_count+1
							fi
						fi
						let part_id=$part_id+1
					done

					while [ "${FOLDER_ID}" -lt "${MAX_FOLDER}" ];
					do
						FOLDER_DATA=`ccfg_cli get ftp${DRIVE_ID}_folder${FOLDER_ID}@drive`
	
						if [ -n "${FOLDER_DATA}" ]; then
							partition_idx=`echo "$FOLDER_DATA"  | cut -d ':' -f 1`
							path=`echo "$FOLDER_DATA"  | cut -d ':' -f 2`

							#echo partition_idx= $partition_idx> /dev/console
							#echo path= $path> /dev/console		

							#dev_name=`ccfg_cli -f "$VOL_INFO_FILE" get devName@disk${dev_id}_${partition_idx}`
							dev_name=${DEV_NAME}${partition_idx}	# sdaX or sdbX
							#echo dev_name = $dev_name > /dev/console

							mount_folder=${dev_name:2}
							#echo mount_folder = $mount_folder > /dev/console

							mount_num=`ccfg_cli -f "$VOL_INFO_FILE" get ${DEV_NAME}_mount_num@volinfo`
							#echo mount_num = $mount_num > /dev/console

							if [ "$dev_id" == "0" ]; then
								tmp_nolabel_count=$nolabel_count
								#echo tmp_nolabel_count = $tmp_nolabel_count > /dev/console
							fi

							part_id=1

							if [ "$dev_id" == "0" ]; then
								nolabel_count=0
							else
								nolabel_count=$tmp_nolabel_count
							fi

							while [ "${part_id}" -le "${mount_num}" ];
							do
								dev_name_cmp=`ccfg_cli -f "$VOL_INFO_FILE" get devName@disk${dev_id}_${part_id}`
								vol_label=`ccfg_cli -f "$VOL_INFO_FILE" get volLabel@disk${dev_id}_${part_id}`
								#echo dev_name_cmp = $dev_name_cmp > /dev/console
								if [ "$dev_name_cmp" == "$dev_name" ]; then
									#echo nolabel_count = $nolabel_count > /dev/console
									if [ "$vol_label" == "<no_label>" ]; then
										if [ "$nolabel_count" == "0" ]; then
											vol_label="$vol_label_1"
											#echo vol_label 0 = $vol_label_1 > /dev/console
										fi
										if [ "$nolabel_count" == "2" ]; then
											vol_label="$vol_label_2"
											#echo vol_label 2 = $vol_label_2 > /dev/console
										fi
										if [ "$nolabel_count" == "3" ]; then
											vol_label="$vol_label_3"
											#echo vol_label 3 = $vol_label_3 > /dev/console
										fi
										if [ "$nolabel_count" == "4" ]; then
											vol_label="$vol_label_4"
											#echo vol_label 4 = $vol_label_4 > /dev/console
										fi
										if [ "$nolabel_count" == "5" ]; then
											vol_label="$vol_label_5"
											#echo vol_label 5 = $vol_label_5 > /dev/console
										fi
										if [ "$nolabel_count" == "6" ]; then
											vol_label="$vol_label_6"
											#echo vol_label 6 = $vol_label_6 > /dev/console
										fi
										if [ "$nolabel_count" == "7" ]; then
											vol_label="$vol_label_7"
											#echo vol_label 7 = $vol_label_7 > /dev/console
										fi
										if [ "$nolabel_count" == "8" ]; then
											vol_label="$vol_label_8"
											#echo vol_label 8 = $vol_label_8 > /dev/console
										fi
									fi
									let part_id=$mount_num
								fi

								if [ "$vol_label" == "<no_label>" ]; then
									if [ "$nolabel_count" == "0" ]; then
										let nolabel_count=$nolabel_count+2
									else
										let nolabel_count=$nolabel_count+1
									fi
								fi

								let part_id=$part_id+1
							done

							#vol_label=`ccfg_cli -f "$VOL_INFO_FILE" get volLabel@disk${dev_id}_${partition_idx}`
							#echo "vol_label = "$vol_label

							#echo vol_label = $vol_label > /dev/console

							display_name=`ccfg_cli get ftp"$dev_id"_folder"$FOLDER_ID"_display_name@drive`
							
							echo dev_id = $dev_id,  FOLDER_ID = $FOLDER_ID > /dev/console
							echo display_name = $display_name > /dev/console

							ALLPATH="/tmp/usb/"${mount_folder}"${path}"
							#echo ALLPATH = $ALLPATH > /dev/console

							if [ "$path" == "/" ]; then
								ln -s "$ALLPATH" "/tmp/ftp/${vol_label}"
								#mkdir -m 0755 -p "$FTP_HOME"/"${vol_label}"
								#mount -o bind "$ALLPATH" "$FTP_HOME"/"${vol_label}"
							else
								cd "$FTP_HOME"
								mkdir $vol_label
								cd $vol_label

								if [ "$directory_type" == "1" ]; then
									ln -s "$ALLPATH" $display_name
									#mkdir -m 0755 -p "$display_name"
									#mount -o bind "$ALLPATH" "$display_name"
								elif [ "$directory_type" == "2" ]; then

								token_idx=2  # first token always is empty (ex: /xxx/yyy)
								path_token=`echo "$path"  | cut -d '/' -f $token_idx`

								while [ "$path_token" != "" ]
								do
									let token_idx=$token_idx+1
									path_token_next=`echo "$path"  | cut -d '/' -f $token_idx`
									if [ "$path_token_next" == "" ]; then
											ln -s "$ALLPATH" "$path_token"
											#mkdir -m 0755 -p "$path_token"
											#mount -o bind "$ALLPATH" "$path_token"
									else
										mkdir $path_token
										cd $path_token
									fi
									path_token=$path_token_next
								done
							fi
							fi

							SHAREPATH="$FTP_HOME"/"${share_folder}"
							#echo SHAREPATH = $SHAREPATH > /dev/console

							#mount --bind /tmp/ftp/${share_folder} $ALLPATH

							if [ "$path" == "/" ]; then
								total_file_name="${ALLPATH}""${mount_folder}"
							else
								total_file_name="${ALLPATH}"/"${share_folder}"
							fi

							#echo total_file_name = $total_file_name > /dev/console
							if [ -h "$total_file_name" ]; then
								rm -Rf "$total_file_name"
							fi

							#ln -s "${ALLPATH}" "${FTP_HOME}"	
						fi
						let FOLDER_ID=$FOLDER_ID+1
					done
				fi
			fi
		fi
		let DRIVE_ID=$DRIVE_ID+1
	done

	if [ "${CHECK}" == "0" ]; then
		"$FTP_PROGRAM" stop
		exit
	fi
	
	/usr/sbin/ftpcfg.sh syncsvc 
	"$FTP_PROGRAM" restart
}


case "$1" in
	"ftp_folder_add")		ftp_folder_add;;
	"ftp_default_folder_set")		ftp_default_folder_set "$2" "$3" "$4" "$5" "$6";;
	*)
				echo $0 "ftp_folder_add  - FTP folder set function"
				;;
esac
