/*
 * Arcadyan middle layer APIs for DSL
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <libArcComApi.h>
#include <mid_fmwk.h>
#include <mid_mapi_ccfg.h>

#include "mid_mapi_sys_internal.h"
#include "mid_mapi_sys.h"


#define	MAPI_SYS_NTP_SECTION_NAME	"ntp"

#define	MAPI_SYS_TIMEZONE_FILE		"/etc/config/timezonelist"

#define	MAPI_SYS_LED_CFG_FILE		"/tmp/led.cfg"


/*
 * -- Helper functions --
 */
void mapi_sys_helper_cpu_stat_get(mapi_sys_cpu_stat_t *stat) 
{
    FILE *fp_stat ;
    char buff[128+1] ;
    
	fp_stat = fopen("/proc/stat","r");
	while( fgets(buff, 128, fp_stat) ) {
        if( strstr(buff, "cpu") ) {
            sscanf(buff, "cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu",
                         &stat->usr, &stat->sys, &stat->nice, &stat->idle, &stat->iowait, 
						 &stat->irq, &stat->softirq, &stat->stead, &stat->guest) ;
            break;
        }
    }
    fclose(fp_stat);
}

float mapi_sys_helper_cpu_usage_cal(mapi_sys_cpu_stat_t *s1, mapi_sys_cpu_stat_t *s2)
{
	float scale;
	unsigned long long diff, total;
	mapi_sys_cpu_stat_t s;

	s.usr = s2->usr - s1->usr;
	s.sys = s2->sys - s1->sys;
	s.nice = s2->nice - s1->nice;
	s.idle = ((diff = s2->idle - s1->idle) < 0) ? 0 : diff;
	s.iowait = s2->iowait - s1->iowait;
	s.irq = s2->irq - s1->irq;
	s.softirq = s2->softirq - s1->softirq;
	s.stead = s2->stead - s1->stead;
	s.guest = s2->guest - s1->guest;
	total = s.usr + s.sys + s.nice + s.idle + s.iowait + s.irq + s.softirq + s.stead + s.guest;
	scale = 100.0 / (float) total;

	return (float) ((total - s.idle) * scale);
}

void mapi_sys_helper_mem_stat_get(mapi_sys_mem_stat_t *stat) 
{
    FILE *fp_stat;
    char buff[128+1];
    
	fp_stat = fopen("/proc/meminfo","r");
	while (fgets(buff, 128, fp_stat)) {
        if(strstr(buff, "MemTotal:"))
            sscanf(buff, "MemTotal: %u", &stat->total);
		else
		if (strstr(buff, "MemFree:")) 
			sscanf(buff, "MemFree: %u", &stat->free);
		else
		if (strstr(buff, "Buffers:"))
			sscanf(buff, "Buffers: %u", &stat->buffers);
		else
		if (strstr(buff, "Cached:"))
			sscanf(buff, "Cached: %u", &stat->cached);
		else
			break;
    }
    fclose(fp_stat);
}

float mapi_sys_helper_mem_usage_cal(mapi_sys_mem_stat_t *m)
{
	return (((float) (m->total - (m->free + m->buffers + m->cached)) / (float) m->total) * 100.0);
}
/*
 * -- Public functions --
 */
/*******************************************************************************
 * Description
 *		Caculating the CPU usage currently
 * Parameters
 *		None
 * Returns
 *		* MID_FAIL(-1):	Cannot get the CPU Usage
 * Note
 *
 ******************************************************************************/
float mapi_sys_cpu_usage(void)
{
	mapi_sys_cpu_stat_t s1, s2;

	mapi_sys_helper_cpu_stat_get(&s1);
	sleep(1);
	mapi_sys_helper_cpu_stat_get(&s2);

	return mapi_sys_helper_cpu_usage_cal(&s1, &s2);
}

/*******************************************************************************
 * Description
 *		Determine where to store the uploading/downloading firware file according 
 *		to current memory usage
 * Parameters
 *		fw_size_bytes: The file size of the firmware going to be upgraded in bytes
 * Returns
 * 		MAPI_SYS_FWUG_STORE_MEM		Suggest to store the firmware in MEM
 * 		MAPI_SYS_FWUG_STORE_FLASH	Suggest to store firmware in FLASH
 * Note
 *
 ******************************************************************************/
int mapi_sys_fwug_store(int fw_size_bytes)
{
	int min_acceted_mem_size = fw_size_bytes + MAPI_SYS_FWUG_EXTRA_BUF;

	if (min_acceted_mem_size < (mapi_sys_tmp_free() * 1024) && min_acceted_mem_size < (mapi_sys_mem_free() * 1024))
		return MAPI_SYS_FWUG_STORE_MEM;
	else {
		mkdir("/mnt/misc/fw-upgrade", S_IRWXU | S_IRWXG | S_IRWXO);
		return MAPI_SYS_FWUG_STORE_FLASH;
	}
}

/*******************************************************************************
 * Description
 *		Caculating the Free space in /tmp currently in KB
 * Parameters
 *		None
 * Returns
 * 		None
 * Note
 *
 ******************************************************************************/
int mapi_sys_tmp_free(void)
{
	char free_tmp[8];
	char cmdbuf[128];

	snprintf(cmdbuf, sizeof(cmdbuf), "df | awk '/\\/tmp$/ {print $4}'");

	if ( osSystem_GetOutput( cmdbuf, free_tmp, sizeof(free_tmp)) == ARC_COM_ERROR ) {
		MID_TRACE( "[%s] get tmp free failed\n", __FUNCTION__ );
		return 0;
	}

	return atoi(free_tmp);
}

/*******************************************************************************
 * Description
 *		Caculating the Free Memory currently in KB
 * Parameters
 *		None
 * Returns
 * 		None
 * Note
 *
 ******************************************************************************/
int mapi_sys_mem_free(void)
{
	mapi_sys_mem_stat_t m;

	mapi_sys_helper_mem_stat_get(&m);

	return (m.free + m.buffers + m.cached);
}

/*******************************************************************************
 * Description
 *		Caculating the Memory usage currently
 * Parameters
 *		None
 * Returns
 *		* MID_FAIL(-1):	Cannot get the Memory Usage
 * Note
 *
 ******************************************************************************/
float mapi_sys_mem_usage(void)
{
	mapi_sys_mem_stat_t m;

	mapi_sys_helper_mem_stat_get(&m);

	return mapi_sys_helper_mem_usage_cal(&m);
}
/*******************************************************************************
 * Description
 *		firmware upgrade
 *
 * Parameters
 *		pstData:	pointer to data of firmware information to be upgraded.
 *						pstData->fwType and pstData->fwFile are required.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		pstData is NULL or failed
 *
 * Note
 *		* pstData->execResult returns the result of firmware upgrade
 *		* upgrade full image or rootfs will automatically reboot system
 *		* image file pstData->fwFile will be automatically removed after upgrading,
 *			even if failed to upgrade
 *
 ******************************************************************************/
int mapi_sys_fw_upgrade( int tid, mid_sys_fw_upgrade_t* pstData )
{
	if (pstData == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_SYS, MID_SYS_FW_UPGRADE, pstData, sizeof(*pstData) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		get firmware type
 *
 * Parameters
 *		pstData:	pointer to data of firmware information to be checked.
 *						pstData->fwFile is required.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		pstData is NULL or failed
 *
 * Note
 *		* pstData->execResult returns MID_SYS_FW_NOERR if firmware image recognizable
 *									or MID_SYS_FW_ERR_FORMAT if unknown firmware type
 *									or MID_SYS_FW_ERR_FILE if file open failed
 *									or MID_SYS_FW_ERR_SYS if execution failed
 *		* pstData->fwType returns firmware type if pstData->execResult is MID_SYS_FW_NOERR
 *
 ******************************************************************************/
int mapi_sys_fw_type( int tid, mid_sys_fw_upgrade_t* pstData )
{
	if (pstData == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_SYS, MID_SYS_FW_TYPE, pstData, sizeof(*pstData) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		get time zone list
 *
 * Parameters
 *		pstBuf:	pointer to buffer of system time data
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		pstBuf is NULL or failed
 *
 * Note
 *		* size of mid_sys_timezone_list_t is MID_SYS_TIME_ZONE_MAX*sizeof(mid_sys_timezone_list_t) bytes (6400)
 *
 ******************************************************************************/
int mapi_sys_timezone_list_get( int tid, mid_sys_timezone_list_t* pstBuf )
{
	FILE*	pFile;
	char	sLineBuf[128];
	char*	pPtr;
	char*	pNext;

	if (pstBuf == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	osBZero( pstBuf, sizeof(*pstBuf) );

	if ( (pFile=fopen(MAPI_SYS_TIMEZONE_FILE,"r")) == NULL ) {
		MID_TRACE( "[%s] fail to open %s\n", __FUNCTION__, MAPI_SYS_TIMEZONE_FILE );
		return MID_FAIL;
	}

	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != NULL )
	{
		utilStrTrim( sLineBuf );
		if ( strncmp( sLineBuf, "tz", 2 ) != 0 )
			continue;
		if ( sLineBuf[2] < '0' || sLineBuf[2] > '9' )
			continue;
		pNext = sLineBuf;
		//
		strtok_r( pNext, "=", &pNext );
		if ( pNext == NULL )
			continue;
		// index
		pstBuf->tz[ pstBuf->number ].idx = (char)atoi(sLineBuf+2);
		// bias
		pPtr = strtok_r( pNext, ",", &pNext );
		if ( pPtr == NULL )
			continue;
		pstBuf->tz[ pstBuf->number ].bias = (short)atoi(pPtr);
		// daylight saving
		pPtr = strtok_r( pNext, ",", &pNext );
		if ( pPtr == NULL )
			continue;
		pstBuf->tz[ pstBuf->number ].daylightSaving = (char)atoi(pPtr);
		// TZ string
		pPtr = strtok_r( pNext, ",", &pNext );
		if ( pPtr == NULL )
			continue;
		strcpy( pstBuf->tz[ pstBuf->number ].tzstr, pPtr );
		pPtr = pstBuf->tz[ pstBuf->number ].tzstr;
		while ( pPtr=strchr(pPtr+1,'@') )
			*pPtr = ',';
		// description
		strcpy( pstBuf->tz[ pstBuf->number++ ].descr, pNext );
		//
		if (pstBuf->number >= MID_SYS_TIME_ZONE_MAX)
			break;
	}

	fclose( pFile );

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		get system time information
 *
 * Parameters
 *		pstBuf:	pointer to buffer of system time data
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		pstBuf is NULL or failed
 *
 * Note
 *
 ******************************************************************************/
int mapi_sys_time_get( int tid, mid_sys_time_t* pstBuf )
{
	long	lCnt;
	long	lSvrCnt;
	char	sKey[16];

	if (pstBuf == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	osBZero( pstBuf, sizeof(*pstBuf) );

    // NTP client enable
	pstBuf->ntpEnable = mapi_ccfg_get_long( tid, MAPI_SYS_NTP_SECTION_NAME, "enable", 0 );

    // NTP sync interval
	pstBuf->ntpInterval = mapi_ccfg_get_long( tid, MAPI_SYS_NTP_SECTION_NAME, "interval", 600 );

    // NTP server count
	lSvrCnt = mapi_ccfg_get_long( tid, MAPI_SYS_NTP_SECTION_NAME, "servercnt", 0 );

	// NTP server
	for ( lCnt = 0 ; lCnt < lSvrCnt && lCnt < MID_SYS_NTP_SVR_MAX ; lCnt++ )
	{
		sprintf( sKey, "server%ld", lCnt + 1);
		mapi_ccfg_get_str( tid, MAPI_SYS_NTP_SECTION_NAME, sKey, pstBuf->ntpServer[lCnt], sizeof(pstBuf->ntpServer[lCnt]), "" );
	}

	// local time
	if ( osSystem_GetOutput( "date", pstBuf->localTime, sizeof(pstBuf->localTime) ) == ARC_COM_ERROR ) {
		MID_TRACE( "[%s] date failed\n", __FUNCTION__ );
		return MID_FAIL;
	}
	utilStrTrim( pstBuf->localTime );

	// time zone index
	pstBuf->timezoneIndex = (char)mapi_ccfg_get_long( tid, MAPI_SYS_NTP_SECTION_NAME, "tz", 0 );

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		set system NTP servers and time zone index
 *
 * Parameters
 *		pstData:	pointer to data of system time
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		pstData is NULL or failed
 *
 * Note
 *		* related sections: porder->pname
 *
 * Note
 *		* related sections: ntp
 *
 * Note
 *
 ******************************************************************************/
int mapi_sys_time_set( int tid, mid_sys_time_t* pstData )
{
	long	lCnt;
	char	sKey[16];

	if (pstData == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if ( pstData->timezoneIndex < 0 || pstData->timezoneIndex >= MID_SYS_TIME_ZONE_MAX ) {
		MID_TRACE( "[%s] timezoneIndex\n", __FUNCTION__ );
		return MID_FAIL;
	}

    // NTP client enable
	mapi_ccfg_set_long( tid, MAPI_SYS_NTP_SECTION_NAME, "enable", pstData->ntpEnable );

    // NTP sync interval
	mapi_ccfg_set_long( tid, MAPI_SYS_NTP_SECTION_NAME, "interval", pstData->ntpInterval );

    // NTP server count
	mapi_ccfg_set_long( tid, MAPI_SYS_NTP_SECTION_NAME, "servercnt", MID_SYS_NTP_SVR_MAX );

	// NTP server
	for ( lCnt = 0 ; lCnt < MID_SYS_NTP_SVR_MAX ; lCnt++ )
	{
		sprintf( sKey, "server%ld", lCnt + 1);
		mapi_ccfg_set_str( tid, MAPI_SYS_NTP_SECTION_NAME, sKey, pstData->ntpServer[lCnt] );
	}

	// time zone index
	mapi_ccfg_set_long( tid, MAPI_SYS_NTP_SECTION_NAME, "tz", pstData->timezoneIndex );

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		LED control
 *
 * Parameters
 *		pstData:	pointer to data of LED control.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		pstData is NULL or failed
 *
 * Note
 *		* pstData->ledColor takes effect only when pstData->ledControl is ON or BLINKING.
 *			MID_SYS_LED_DFT means to use default color.
 *		* pstData->blinkInterval takes effect only when BLINKING and not default color.
 *			MID_SYS_LED_INTERVAL_NO means to use default interval.
 *
 ******************************************************************************/
int mapi_sys_led_control( int tid, mid_sys_led_control_t* pstData )
{
	mid_sys_led_control_t	stData;

	if (pstData == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	stData = *pstData;

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_SYS, MID_SYS_LED_CONTROL, &stData, sizeof(stData) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		get LED status
 *
 * Parameters
 *		pstBuf:	pointer to data of LED status. It MUST be an array of MID_SYS_LED_MAX entries
 *
 * Returns
 *		* MID_FAIL(-1):		pstData is NULL or failed
 *		* others:			count of valid entries filled in pstBuf
 *
 * Note
 *		* pstBuf MUST be at least an array of MID_SYS_LED_MAX entries
 *
 ******************************************************************************/
int mapi_sys_led_status( int tid, mid_sys_led_control_t* pstBuf )
{
	FILE*	pFile;
	char	sLineBuf[128];
	char*	pPtr;
	char*	pNext;
	int		iCnt;

	if (pstBuf == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if ( (pFile=fopen(MAPI_SYS_LED_CFG_FILE,"r")) == NULL ) {
		MID_TRACE( "[%s] fail to open %s\n", __FUNCTION__, MAPI_SYS_LED_CFG_FILE );
		return MID_FAIL;
	}

	osBZero( pstBuf, sizeof(*pstBuf)*MID_SYS_LED_MAX );

	iCnt = 0;
	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != NULL )
	{
		utilStrTrim( sLineBuf );
		if ( sLineBuf[0]=='@' || sLineBuf[0]=='#' || sLineBuf[0]=='!' )
			continue;
		pNext = sLineBuf;
		// type
		strtok_r( pNext, "=", &pNext );
		if ( pNext == NULL )
			continue;
		if ( strcasecmp( pPtr, "upgrade" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_UPGRADE;
		else if ( strcasecmp( pPtr, "wlan" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_WLAN;
		else if ( strcasecmp( pPtr, "wps" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_WPS;
		else if ( strcasecmp( pPtr, "adsl" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_ADSL;
		else if ( strcasecmp( pPtr, "power" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_POWER;
		else if ( strcasecmp( pPtr, "wan" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_WAN;
		else if ( strcasecmp( pPtr, "vmail" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_VMAIL;
		else if ( strcasecmp( pPtr, "usb3g" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_UMTS;
		else if ( strcasecmp( pPtr, "mesg" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_MESG;
		else if ( strcasecmp( pPtr, "dect" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_DECT;
		else if ( strcasecmp( pPtr, "tae1" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_TAE1;
		else if ( strcasecmp( pPtr, "tae2" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_TAE2;
		else if ( strcasecmp( pPtr, "gwan" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_GWAN;
		else if ( strcasecmp( pPtr, "isdn" ) == 0 )
			pstBuf[iCnt].ledType = MID_SYS_LED_ISDN;
		else
			continue;
		// status
		pPtr = strtok_r( pNext, ",", &pNext );
		if ( pPtr == NULL || pPtr[0] == '\0' ) {
			pstBuf[iCnt].ledControl = MID_SYS_LED_UNKNOWN;
			goto mapi_sys_led_status_next;
		}
		if ( strcasecmp( pPtr, "on" ) == 0 )
			pstBuf[iCnt].ledControl = MID_SYS_LED_ON;
		else if ( strcasecmp( pPtr, "off" ) == 0 )
			pstBuf[iCnt].ledControl = MID_SYS_LED_OFF;
		else if ( strcasecmp( pPtr, "blink" ) == 0 )
			pstBuf[iCnt].ledControl = MID_SYS_LED_BLINK;
		else {
			pstBuf[iCnt].ledControl = MID_SYS_LED_UNKNOWN;
			goto mapi_sys_led_status_next;
		}
		// color
		pPtr = strtok_r( pNext, ",", &pNext );
		if ( pPtr == NULL || pPtr[0] == '\0' )
			goto mapi_sys_led_status_next;
		if ( strcasecmp( pPtr, "R" ) == 0 )
			pstBuf[iCnt].ledColor = MID_SYS_LED_R;
		else if ( strcasecmp( pPtr, "G" ) == 0 )
			pstBuf[iCnt].ledColor = MID_SYS_LED_G;
		else if ( strcasecmp( pPtr, "B" ) == 0 )
			pstBuf[iCnt].ledColor = MID_SYS_LED_B;
		else if ( strcasecmp( pPtr, "RG" ) == 0 )
			pstBuf[iCnt].ledColor = MID_SYS_LED_RG;
		else if ( strcasecmp( pPtr, "RB" ) == 0 )
			pstBuf[iCnt].ledColor = MID_SYS_LED_RB;
		else if ( strcasecmp( pPtr, "GB" ) == 0 )
			pstBuf[iCnt].ledColor = MID_SYS_LED_GB;
		else if ( strcasecmp( pPtr, "RGB" ) == 0 )
			pstBuf[iCnt].ledColor = MID_SYS_LED_RGB;
		else
			pstBuf[iCnt].ledColor = MID_SYS_LED_DFT;
		// blink interval
		pPtr = strtok_r( pNext, ",", &pNext );
		if ( pPtr != NULL )
			pstBuf[iCnt].blinkInterval = (unsigned char)atoi(pPtr);
		//
mapi_sys_led_status_next:
		iCnt++;
		if (iCnt >= MID_SYS_LED_MAX)
			break;
	}

	fclose( pFile );

	return iCnt;
}

/*******************************************************************************
 * Description
 *		system reboot
 *
 * Parameters
 *		delay: delay time to reboot, in seconds
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		pstData is NULL or failed
 *
 * Note
 *		* this function won't return due to system rebooting
 *
 ******************************************************************************/
int mapi_sys_reboot( int tid, unsigned short delay )
{
	unsigned short	iSecond;

	iSecond = delay;

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_SYS, MID_SYS_REBOOT, &iSecond, sizeof(iSecond) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

    return MID_SUCCESS;
}

#define	MAPI_SYS_SSH_SECTION_NAME	"ssh"

/*******************************************************************************
 * Description
 *		get ssh running information
 *
 * Parameters
 *		pstBuf:	pointer to buffer of system time data
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		pstBuf is NULL or failed
 *
 * Note
 *
 ******************************************************************************/
int mapi_sys_ssh_status_get( int tid, mid_sys_ssh_t* pstBuf )
{
	char strAlwaysOn[10] = {0};
	int bPIDFileFound = 0;
	char strCmd[64], strData[256];
    int     new_transc = 0;

	if (pstBuf == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

    if ( tid == 0 )
	{
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
		new_transc = 1;
	}

	osBZero( pstBuf, sizeof(*pstBuf) );

	mapi_ccfg_get_str( tid, MAPI_SYS_SSH_SECTION_NAME, "AlwaysOn", strAlwaysOn, sizeof(strAlwaysOn), "off" );
	MID_TRACE("[%s] strAlwaysOn=%s\n", __FUNCTION__, strAlwaysOn );
	if( strcmp(strAlwaysOn, "on") == 0)
	{
		pstBuf->sys_ssh_daemon_state = SYS_SSH_DAEMON_STATE_ALWAYS_ON;
	}
	else
	{
		sprintf(strCmd, "ls /var/run/dropbear*");
		if ( osSystem_GetOutput( strCmd, strData, sizeof(strData) ) == ARC_COM_ERROR ) {
			goto mapi_sys_ssh_status_get_fail;
		}
		if(strlen(strData) == 0)
			bPIDFileFound = 0;
		else
			bPIDFileFound = 1;
		utilStrTrim( strData );
		MID_TRACE("[%s] bPIDFileFound=%d, strData=%s\n", __FUNCTION__, bPIDFileFound, strData );
		if(bPIDFileFound)
		{
			pstBuf->sys_ssh_daemon_state = SYS_SSH_DAEMON_STATE_RUNNING_BY_SESAME_FILE;
		}
		else
		{
			pstBuf->sys_ssh_daemon_state = SYS_SSH_DAEMON_STATE_NOT_RUNNING;
		}
	}

	if ( new_transc )
	    mapi_end_transc(tid);

    return MID_SUCCESS;

mapi_sys_ssh_status_get_fail:
	if ( new_transc )
	    mapi_end_transc(tid);

    return MID_FAIL;
}

int mapi_sys_ssh_status_set( int tid, mid_sys_ssh_t data )
{
    int     new_transc = 0;
	FILE 	*fp;
	char strCmd[64], strData[256];

    if ( tid == 0 )
	{
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
		new_transc = 1;
	}

	fp = fopen("/etc/config/cert/authorized_keys","wb");
	fwrite(data.pData, 1, data.nDataLength, fp);
	fclose(fp);

	sprintf(strCmd, "cp /etc/config/cert/authorized_keys /etc/config/dropbear_key/authorized_keys");
	if ( osSystem_GetOutput( strCmd, strData, sizeof(strData) ) == ARC_COM_ERROR ) {
		goto mapi_sys_ssh_status_set_fail;
	}

	if ( new_transc )
	    mapi_end_transc(tid);

    return MID_SUCCESS;

mapi_sys_ssh_status_set_fail:
	if ( new_transc )
	    mapi_end_transc(tid);

    return MID_FAIL;
}

#define CUSTOMER_TYPE_ARCOR 1
#define PIN_APPLY 2
#define MIC_LENGTH 20

int IsSpecialPinCode(char *PINCode)
{
	int i = 0;

	for(i = 1; i< MIC_LENGTH-1; i++)
		if(PINCode[i-1] != PINCode[i])
			break;
	if (i == MIC_LENGTH -1)
		return 1;

	return 0;
}

void save_PIN_to_flash(int tid, char *PINCode)
{
	unsigned char tr69Username[256] = "";
	unsigned char tr69Password[256] = "";
	char arcor_customer;
	char arcor_pinConf;
	int isNewTid = 0;

	strncpy(tr69Username, PINCode, 10);
	tr69Username[10] = '\0';
	strncpy(tr69Password, PINCode+10, 10);
	tr69Password[10] = '\0';
	arcor_pinConf = PIN_APPLY;
	arcor_customer = CUSTOMER_TYPE_ARCOR;
	
	if (tid == NULL )
	{
		isNewTid = 1;
		if ( (tid = mapi_start_transc()) == MID_FAIL ) 
		{
			fprintf( stderr, "save_PIN_to_flash: Failed to start a transaction to arc middle\n" );
			return;
		}
	}
	
	if( mapi_ccfg_set_long(tid, "bootstrap", "arcor_pinConf", arcor_pinConf) == MID_FAIL )
	{
		printf("!save_PIN_to_flash: mapi_ccfg_set Fail [%d]\n", __LINE__);
	}


	if( mapi_ccfg_set_long(tid, "bootstrap", "arcor_customer", arcor_customer) == MID_FAIL )
	{
		printf("!save_PIN_to_flash: mapi_ccfg_set Fail [%d]\n", __LINE__);
	}	

	if( mapi_ccfg_set_str(tid, "tr69", "Username", tr69Username) == MID_FAIL )
	{
		printf("!save_PIN_to_flash: mapi_ccfg_set Fail [%d]\n", __LINE__);
	}
	
	if( mapi_ccfg_set_str(tid, "tr69", "Password", tr69Password) == MID_FAIL )
	{
		printf("!save_PIN_to_flash: mapi_ccfg_set Fail [%d]\n", __LINE__);
	}
	
	mapi_ccfg_commit(tid, MAPI_CCFG_COMMIT_FILE|MID_CCFG_COMMIT_FLASH, NULL );

	if (isNewTid == 1)
	{
		mapi_end_transc( tid );
	}
}

int mapi_sys_verify_MIC(int tid, char *PINCode)
{
	int i, oddSum = 0, evenSum = 0, rc = 0, oddtmp, eventmp, mult2, PinLen;

	PinLen = strlen(PINCode);
	if (PinLen != MIC_LENGTH)
		return -1;

	if (IsSpecialPinCode(PINCode) == 1)
		return -1;

	mult2 = 1;
	for (i = 0; i < MIC_LENGTH; i += 2)
	{
		oddtmp = PINCode[i] - '0';
		eventmp = PINCode[i + 1] - '0';
		if (oddtmp > 9 || oddtmp < 0 || eventmp > 9 || eventmp < 0)
			return -1;
		if (mult2 == 1)
		{
			oddtmp <<= 1;
			eventmp <<=1;
			if (oddtmp > 9)
				oddtmp -= 9;
			if (eventmp > 9)
				eventmp -= 9;
			mult2 = 0;
		}
		else
			mult2 = 1;
		oddSum += oddtmp;
		evenSum+= eventmp;
	}

	if ((oddSum%10) || (evenSum%10))
	{
		printf("oddSum = %d, evenSum = %d \n", oddSum, evenSum);
		rc = -1;
	}
	else
	{
		save_PIN_to_flash(tid, PINCode);
	}

	return rc;
}


