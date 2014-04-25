/*
 * Arcadyan middle layer definition for misc system access
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_SYS_H_
#define _MID_SYS_H_


#include <mid_types.h>





#define MID_SYS_FW_UPGRADE			0x01
#define MID_SYS_LED_CONTROL			0x02
#define MID_SYS_REBOOT				0x03
#define MID_SYS_FW_TYPE				0x04


/* firmware upgrade */

#define	MID_SYS_FW_FILENAME_LEN		256	// including ending '\0'

enum mid_sys_fw_type_e {
	MID_SYS_FW_FULL			= 0,
	MID_SYS_FW_KERNEL		= 1,
	MID_SYS_FW_ROOTFS		= 2,
	MID_SYS_FW_FULL2		= 3,
	MID_SYS_FW_KERNEL2		= 4,
	MID_SYS_FW_ROOTFS2		= 5,
	MID_SYS_FW_MODULE		= 6,
	MID_SYS_FW_BOOTLOADER	= 7,
	MID_SYS_FW_FULLN		= 8,
	MID_SYS_FW_FULLUEN      = 9,
};

enum mid_sys_fw_err_e {
	MID_SYS_FW_NOERR		= 0,	// no error
	MID_SYS_FW_ERR_MTD		= 1 ,	// non-existent partition
	MID_SYS_FW_ERR_TYPE		= 2,	// image type error
	MID_SYS_FW_ERR_FORMAT	= 3,	// image format error
	MID_SYS_FW_ERR_FILE		= 4,	// file open failed
	MID_SYS_FW_ERR_SYS		= 5,	// system failed
	MID_SYS_FW_ERR_SIGNATURE    = 6,// firmware signature error
};

typedef struct mid_sys_fw_upgrade_s {	/* MID_SYS_FW_UPGRADE */
	enum mid_sys_fw_type_e 	fwType;							/* type of firmware */
	char					fwFile[MID_SYS_FW_FILENAME_LEN];/* file name of firmware */
	enum mid_sys_fw_err_e	execResult;						/* result of firmware upgrade */
} mid_sys_fw_upgrade_t;	/* MID_FW_UPGRADE */


/* system time */

#define	MID_SYS_NTP_SVR_MAX			5
#define	MID_SYS_NTP_SVR_NAME_LEN	65	// including ending '\0'
#define	MID_SYS_LOCAL_TIME_LEN		32	// including ending '\0'

#define	MID_SYS_TIME_ZONE_MAX		80	// max number of time zone entries
#define	MID_SYS_TIME_ZONE_TZSTR_LEN	32	// including ending '\0'
#define	MID_SYS_TIME_ZONE_DESCR_LEN	64	// including ending '\0'

typedef struct mid_sys_timezone_s {
	char	idx;			/* entry index */
	char	daylightSaving;	/* daylight saving, 0-no, 1-yes */
	short	bias;			/* bias to GMT, in minutes */
	char	tzstr[MID_SYS_TIME_ZONE_TZSTR_LEN];	/* TZ string */
	char	descr[MID_SYS_TIME_ZONE_DESCR_LEN];	/* description */
} mid_sys_timezone_t;

typedef struct mid_sys_timezone_list_s {
	unsigned long		number;			/* number of time zone entries in "tz" */
	mid_sys_timezone_t	tz[MID_SYS_TIME_ZONE_MAX];
} mid_sys_timezone_list_t;

typedef struct mid_sys_time_s {
	long	ntpEnable;
	long	ntpInterval;
	char	ntpServer[MID_SYS_NTP_SVR_MAX][MID_SYS_NTP_SVR_NAME_LEN];	/* NTP server name, RW */
	char	localTime[MID_SYS_LOCAL_TIME_LEN];							/* string of local time, RO */
	char	timezoneIndex;												/* index in time zone list, RW */
} mid_sys_time_t;


/* LED control */

#define	MID_SYS_LED_INTERVAL_MIN	1
#define	MID_SYS_LED_INTERVAL_MAX	255
#define	MID_SYS_LED_INTERVAL_DFT	50
#define	MID_SYS_LED_INTERVAL_NO		0

enum mid_sys_led_type_e {
	MID_SYS_LED_UPGRADE		= 0,
	MID_SYS_LED_WLAN		= 1,
	MID_SYS_LED_WPS			= 2,
	MID_SYS_LED_ADSL		= 3,
	MID_SYS_LED_POWER		= 4,
	MID_SYS_LED_WAN			= 5,
	MID_SYS_LED_VMAIL		= 6,
	MID_SYS_LED_UMTS		= 7,
	MID_SYS_LED_MESG		= 8,
	MID_SYS_LED_DECT		= 9,
	MID_SYS_LED_TAE1		= 10,
	MID_SYS_LED_TAE2		= 11,
	MID_SYS_LED_GWAN		= 12,
	MID_SYS_LED_ISDN		= 13,
	MID_SYS_LED_MAX
};

enum mid_sys_led_control_e {
	MID_SYS_LED_OFF		= 0,
	MID_SYS_LED_ON		= 1,
	MID_SYS_LED_BLINK	= 2,
	MID_SYS_LED_UNKNOWN	= 3,	/* for status */
};

enum mid_sys_led_color_e {
	MID_SYS_LED_DFT		= 0,	/* default color */
	MID_SYS_LED_R		= 1,
	MID_SYS_LED_G		= 2,
	MID_SYS_LED_B		= 4,
	MID_SYS_LED_RG		= 3,
	MID_SYS_LED_RB		= 5,
	MID_SYS_LED_GB		= 6,
	MID_SYS_LED_RGB		= 7,
};

typedef struct mid_sys_led_control_s {	/* MID_SYS_LED_CONTROL */
	enum mid_sys_led_type_e		ledType;		/* type of LED */
	enum mid_sys_led_control_e	ledControl;		/* LED control */
	enum mid_sys_led_color_e	ledColor;		/* MID_SYS_LED_DFT means default color */
	unsigned char				blinkInterval;	/* interval of blinking, in 20 msec, default 50, 0 means default value, ledColor must be set */
} mid_sys_led_control_t;	/* MID_SYS_LED_CONTROL */

#define SYS_SSH_DAEMON_STATE_NOT_RUNNING 0
#define SYS_SSH_DAEMON_STATE_RUNNING_BY_SESAME_FILE 1
#define SYS_SSH_DAEMON_STATE_ALWAYS_ON 2

typedef struct mid_sys_ssh_s {
	char	sys_ssh_daemon_state;
	int		nDataLength;
	unsigned char *pData; // public file data
} mid_sys_ssh_t;


#endif // _MID_SYS_H_
