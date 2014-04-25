
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "libArcComApi.h"


/*******************************************************************************
 * Description
 *		get the system version info
 *
 * Parameters
 *		stBuf:	system version to be retrieved.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* this function refers:
 *			1. sw version: uboot_env --get --name sw_version
 *			2. kernel version: uname -svr
 *			3. boot loader version: uboot_env --get --name bl_version
 *			4. hw version: uboot_env --get --name hw_version
 *			5. serial number: uboot_env --get --name serial
 *
 ******************************************************************************/
long drvSysVerGet( stSysVer* stBuf )
{
	char	sBuf[128];

	if (stBuf == ARC_COM_NULL)
		return ARC_COM_ERROR;

	osBZero( stBuf, sizeof(*stBuf));

	/* sw version */
	if (osSystem_GetOutput( "/usr/sbin/uboot_env.sh --get --name sw_version", sBuf, sizeof(sBuf) ) < 0)
		return ARC_COM_ERROR;
		/**/
	utilStrTrimChar( sBuf, " \t\r\n" );
	utilStrMCpy( stBuf->sSwVer, sBuf, sizeof(stBuf->sSwVer) );

	/* kernel version */
	if (osSystem_GetOutput( "uname -svr", sBuf, sizeof(sBuf) ) < 0)
		return ARC_COM_ERROR;
		/**/
	utilStrTrimChar( sBuf, " \t\r\n" );
	utilStrMCpy( stBuf->sKernelVer, sBuf, sizeof(stBuf->sKernelVer) );

	/* uboot version */
	if (osSystem_GetOutput( "/usr/sbin/uboot_env.sh --get --name bl_version", sBuf, sizeof(sBuf) ) < 0)
		return ARC_COM_ERROR;
		/**/
	utilStrTrimChar( sBuf, " \t\r\n" );
	utilStrMCpy( stBuf->sUbootVer, sBuf, sizeof(stBuf->sUbootVer) );

	/* hw version */
	if (osSystem_GetOutput( "/usr/sbin/uboot_env.sh --get --name hw_version", sBuf, sizeof(sBuf) ) < 0)
		return ARC_COM_ERROR;
		/**/
	utilStrTrimChar( sBuf, " \t\r\n" );
	utilStrMCpy( stBuf->sHwVer, sBuf, sizeof(stBuf->sHwVer) );

	/* serial number */
	if (osSystem_GetOutput( "/usr/sbin/uboot_env.sh --get --name serial", sBuf, sizeof(sBuf) ) < 0)
		return ARC_COM_ERROR;
		/**/
	utilStrTrimChar( sBuf, " \t\r\n" );
	utilStrMCpy( stBuf->sSerial, sBuf, sizeof(stBuf->sSerial) );

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		get the xDSL deriver info and status
 *
 * Parameters
 *		stBuf:	xDSL driver info to be retrieved.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* this function refers:
 *			1. /firmware/dsl_vr9_firmware_xdsl-X.X.X.X.bin
 *			2. /etc/config/rc.conf
 *			3. dsl_cpe_pipe.sh lsg
 *			4. dsl_cpe_pipe.sh g997csg
 *
 ******************************************************************************/
long drvDslInfoGet( stDslInfo* stBuf )
{
	char	sCmd[128];
	char	sTmpFn[32];
	FILE*	pFile;
	char	sLineBuf[128];
	char*	pPtr;

	if (stBuf == ARC_COM_NULL)
		return ARC_COM_ERROR;

	sprintf( sTmpFn, "/tmp/%ld.tmp", osProcId() );

	osBZero( stBuf, sizeof(*stBuf));

	/* xDSL version */
	sprintf( sCmd, "ls /firmware/dsl_vr9_firmware_xdsl-*.bin > %s", sTmpFn );
	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;
	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;
	if ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		/**/
		utilStrLTrim( sLineBuf );
		/**/
		if ( strlen( sLineBuf ) > 0)
		{
			pPtr = strstr( sLineBuf, ".bin" );
			*pPtr = '\0';
			pPtr = sLineBuf + strlen("/firmware/dsl_vr9_firmware_xdsl-");
			strcpy( stBuf->sVer, pPtr );
		}
	}
	fclose( pFile );

  #if 1

	sprintf( sCmd, "libmapi_dsl_cli > %s", sTmpFn );
	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;
	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;
	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		utilStrRTrimChar( sLineBuf, "\n\r" );
		if (strstr( sLineBuf, "DSL training mode: " ) != ARC_COM_NULL)
		{
			if ( (pPtr=strchr( sLineBuf, ':' )) != ARC_COM_NULL ) {
				strcpy( stBuf->sMode, pPtr+2 );
			}
		}
		if (strstr( sLineBuf, "DSL link state: " ) != ARC_COM_NULL)
		{
			if (strstr( sLineBuf, "DSL link state: 0x800" ) != ARC_COM_NULL
			 || strstr( sLineBuf, "DSL link state: 0x801" ) != ARC_COM_NULL )
			{
				stBuf->bConnected = ARC_COM_TRUE;
			}
			else
			{
				stBuf->bConnected = ARC_COM_FALSE;
				break;
			}
		}
		if (strstr( sLineBuf, "US current actual data rate: " ) != ARC_COM_NULL)
		{
			if ( (pPtr=strchr( sLineBuf, ':' )) != ARC_COM_NULL )
				stBuf->lLineRateUs = atol( pPtr+2 );
		}
		if (strstr( sLineBuf, "DS current actual data rate: " ) != ARC_COM_NULL)
		{
			if ( (pPtr=strchr( sLineBuf, ':' )) != ARC_COM_NULL )
				stBuf->lLineRateDs = atol( pPtr+2 );
		}
	}
	fclose( pFile );

  #else

	/* xDSL mode */
	sprintf( sCmd, "cat /etc/config/rc.conf > %s", sTmpFn );
	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;
	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;
	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		/**/
		utilStrTrim( sLineBuf );
		/**/
		if ((pPtr=strstr( sLineBuf, "xDSL_MODE_VRX=" )) != ARC_COM_NULL)
		{
			utilStrMCpy( stBuf->sMode, pPtr+strlen("xDSL_MODE_VRX="), sizeof(stBuf->sMode) );
			utilStrTrimChar( stBuf->sMode, "\"" );
			break;
		}
	}
	fclose( pFile );

	/* xDSL connection */
	sprintf( sCmd, "dsl_cpe_pipe.sh lsg > %s", sTmpFn );
	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;
	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;
	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		if (strstr( sLineBuf, "nLineState=0x800" ) != ARC_COM_NULL
		 || strstr( sLineBuf, "nLineState=0x801" ) != ARC_COM_NULL )
		{
			stBuf->bConnected = ARC_COM_TRUE;
			break;
		}
	}
	fclose( pFile );

	/* xDSL US actual rate */
	if (stBuf->bConnected == ARC_COM_TRUE)
	{
		sprintf( sCmd, "dsl_cpe_pipe.sh g997csg 0 0 > %s", sTmpFn );
		if (osSystem( sCmd ) != ARC_COM_OK)
			return ARC_COM_ERROR;
		if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
			return ARC_COM_ERROR;
		while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
		{
			/**/
			utilStrTrim( sLineBuf );
			/**/
			if ((pPtr=strstr( sLineBuf, "ActualDataRate" )) != ARC_COM_NULL)
			{
				if ( (pPtr=strstr( pPtr, ":" )) != ARC_COM_NULL )
					stBuf->lLineRateUs = atol( pPtr+1 );
			}
		}
		fclose( pFile );
	}

	/* xDSL DS actual rate */
	if (stBuf->bConnected == ARC_COM_TRUE)
	{
		sprintf( sCmd, "dsl_cpe_pipe.sh g997csg 0 1 > %s", sTmpFn );
		if (osSystem( sCmd ) != ARC_COM_OK)
			return ARC_COM_ERROR;
		if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
			return ARC_COM_ERROR;
		while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
		{
			/**/
			utilStrTrim( sLineBuf );
			/**/
			if ((pPtr=strstr( sLineBuf, "ActualDataRate" )) != ARC_COM_NULL)
			{
				if ( (pPtr=strstr( pPtr, ":" )) != ARC_COM_NULL )
					stBuf->lLineRateDs = atol( pPtr+1 );
			}
		}
		fclose( pFile );
	}

  #endif

	unlink( sTmpFn );

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		enable USB driver or shutdown it
 *
 * Parameters
 *		bEnable:	0-disable, others-enable.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* this function refers:
 *			1. /usr/sbin/diskmount.sh unmount-all
 *			2. USB/GPIO power on/off
 *		* this function ONLY SUPPORT VR9
 *			1. VR9 EVM USB - GPIO 29 & 48
 *			1. 723 USB - GPIO 32
 *
 ******************************************************************************/
long drvUsbAdmin( int bEnable )
{
	char	sCmd[128];
	char	sTmpFn[32];
	FILE*	pFile;
	char	sLineBuf[128];
	char*	pPtr;
	int		iGpio;
	int		iCnt;

	if (osSystem( "/usr/sbin/diskmount.sh unmount-all" ) != ARC_COM_OK)
		return ARC_COM_ERROR;

	sprintf( sTmpFn, "/tmp/%ld.tmp", osProcId() );

	/* GPIO registered modules */
	sprintf( sCmd, "cp /proc/driver/ifx_gpio/module > %s", sTmpFn );
	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;
	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;
	/* output sample:

IFX GPIO Pin Usage
    0         1         2         3         4         5         6         7
-------------------------------------------------------------------------------
P0  Available DECT      Available Available LEDC      LEDC      LEDC      Available
P1  SSC       SSC       SSC       Available Available PCI       Available Available
P2  USB       Available Available Available Available Available Available Available
P3  Available Available N/A       N/A       N/A       N/A       N/A       N/A

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    8         9         10        11        12        13        14        15
-------------------------------------------------------------------------------
P0  Available DECT      Available Available DECT      Available Available Available
P1  Available Available Available Available Available Available Available Available
P2  PAGE_BUTTONAvailable Available Available Available Available Available Available
P3  N/A       N/A       N/A       N/A       N/A       N/A       N/A       N/A
	 */
	iCnt = 0;
	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		iCnt++;
		if ( ! ( (iCnt>=5 && iCnt<=8) || (iCnt>=14 && iCnt<=17) ) )
			continue;
		pPtr = sLineBuf;
		/**/
		while ((pPtr=strstr( pPtr+1, "USB" )) != ARC_COM_NULL)
		{
			if (iCnt>=5 && iCnt<=8)
				iGpio = (iCnt-5) * 16;
			else
				iGpio = (iCnt-11) * 16 + 8;
			iGpio += ( ( (int)(pPtr - sLineBuf) - 4) / 10 );
			/**/
			sprintf( sCmd, "echo dataout-%s %d > /proc/driver/ifx_gpio/setup"
						, (bEnable == ARC_COM_FALSE ? "clear" : "set"), iGpio );
			if (osSystem( sCmd ) != ARC_COM_OK)
			{
				fclose( pFile );
				unlink( sTmpFn );
				return ARC_COM_ERROR;
			}
		}
	}
	fclose( pFile );

	unlink( sTmpFn );

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		get the link status of an Ethernet port
 *
 * Parameters
 *		stBuf:	buffer to be filled with Ethernet port status. stBuf->iPortId is neccessary.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* this function refers:
 *			1. switch_utility PHY_AddrGet
 *			3. switch_utility MDIO_DataRead
 *
 ******************************************************************************/
long drvEthPhyStatus( stEthPhyStat* stBuf )
{
	char	sBuf[256];

	if ( stBuf == ARC_COM_NULL || stBuf->iPortId < LACD_ETH_ID_MIN || stBuf->iPortId > LACD_ETH_ID_MAX )
		return ARC_COM_ERROR;

	sprintf( sBuf, "switch_utility MDIO_DataRead `switch_utility PHY_AddrGet %d | awk '{ print $8 }'` 0x1 | grep Data | awk '{ print $3 }'"
				, (int)stBuf->iPortId );

	if (osSystem_GetOutput( sBuf, sBuf, sizeof(sBuf) ) < 0)
		return ARC_COM_ERROR;

	stBuf->iLinkStat = ( strtoul( sBuf, ARC_COM_NULL, 0 ) & 0x0004 ? 1 : 0 );

	stBuf->iLinkSpeed = 0;
	if (stBuf->iLinkStat == 1) {
		sprintf( sBuf, "switch_utility MDIO_DataRead `switch_utility PHY_AddrGet %d | awk '{ print $8 }'` 0x18 | grep Data | awk '{ print $3 }'"
					, (int)stBuf->iPortId );
		if (osSystem_GetOutput( sBuf, sBuf, sizeof(sBuf) ) < 0)
			return ARC_COM_ERROR;
		switch (strtoul( sBuf, ARC_COM_NULL, 0 ) & 0x3) {
		  case 0:		stBuf->iLinkSpeed = 10;		break;
		  case 1:		stBuf->iLinkSpeed = 100;	break;
		  case 2:		stBuf->iLinkSpeed = 1000;	break;
		}
	}

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		get the LTE driver info and status
 *
 * Parameters
 *		stBuf:	LTE driver info to be retrieved.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* this function refers:
 *			1. /tmp/lte/ltestatus
 *
 ******************************************************************************/
long drvLteInfoGet( stLteInfo* stBuf )
{
	FILE*	pFile;
	char	sLineBuf[128];

	if (stBuf == ARC_COM_NULL)
		return ARC_COM_ERROR;

	osBZero( stBuf, sizeof(*stBuf));

	if ( (pFile=fopen("/tmp/lte/ltestatus","r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;
	if ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		/**/
		utilStrLTrim( sLineBuf );
		/**/
		if ( strstr( sLineBuf, "9" ) != ARC_COM_NULL )
		{
			stBuf->bConnected=1;
			stBuf->sig_strength=100;
		}
	}
	fclose( pFile );

	return ARC_COM_OK;
}
