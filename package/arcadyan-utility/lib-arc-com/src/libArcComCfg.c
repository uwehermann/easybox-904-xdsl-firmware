#include <stdio.h>
#include <string.h>
#include "libArcComApi.h"

/*******************************************************************************
 * Description
 *		get a configuration data from a database file
 *
 * Parameters
 *		sFile:		configuration file name. It can be NULL to access global configuration file.
 *		sSect:		section name of key-value pair.
 *		sKey:		key name of key-value pair.
 *		sDataBuf:	buffer to be filled with returned data value.
 *		iDataLen:	length of sDataBuf.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* this function refers:
 *			1. umngcli [-f filename] get key@section
 *
 ******************************************************************************/
long cfgDataGet( char* sFile, char* sSect, char* sKey, char* sDataBuf, int iDataLen )
{
	char	sCmd[128];

	if (sSect == ARC_COM_NULL || sKey == ARC_COM_NULL || sDataBuf == ARC_COM_NULL || iDataLen <= 0)
		return ARC_COM_ERROR;

	if (sFile != ARC_COM_NULL)
			snprintf( sCmd, sizeof(sCmd), "umngcli -f '%s' get %s@%s", sFile, sKey, sSect );
	  else	snprintf( sCmd, sizeof(sCmd), "umngcli get %s@%s", sKey, sSect );

	if (osSystem_GetOutput( sCmd, sDataBuf, iDataLen ) < 0)
		return ARC_COM_ERROR;

	utilStrTrim( sDataBuf );

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		set a configuration data into a database file
 *
 * Parameters
 *		sFile:	configuration file name. It can be NULL to access global configuration file.
 *		sSect:	section name of key-value pair.
 *		sKey:	key name of key-value pair.
 *		sData:	new data value.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* this function refers:
 *			1. umngcli [-f filename] set key@section=data
 *
 ******************************************************************************/
long cfgDataSet( char* sFile, char* sSect, char* sKey, char* sData )
{
	char	sCmd[128];

	if (sSect == ARC_COM_NULL || sKey == ARC_COM_NULL || sData == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if (sFile != ARC_COM_NULL)
			snprintf( sCmd, sizeof(sCmd), "umngcli -f '%s' set %s@%s='%s'", sFile, sKey, sSect, sData );
	  else	snprintf( sCmd, sizeof(sCmd), "umngcli set %s@%s='%s'", sKey, sSect, sData );

	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		get the WAN main interface name (ifname of WAN section in configuration file)
 *
 * Parameters
 *		sDataBuf:	buffer to be filled with returned data value.
 *		iDataLen:	length of sDataBuf.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* this function refers:
 *			1. wandataif.sh
 *
 ******************************************************************************/
long cfgWanIfName( char* sDataBuf, int iDataLen )
{
	if (sDataBuf == ARC_COM_NULL || iDataLen <= 0)
		return ARC_COM_ERROR;

	if (osSystem_GetOutput( "wandataif.sh", sDataBuf, iDataLen ) < 0)
		return ARC_COM_ERROR;

	utilStrTrim( sDataBuf );

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		get the WAN main section name (name of WAN section in configuration file)
 *
 * Parameters
 *		sDataBuf:	buffer to be filled with returned data value.
 *		iDataLen:	length of sDataBuf.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* this function refers:
 *			1. wandataif.sh
 *			2. if2sect.sh
 *
 ******************************************************************************/
long cfgWanIfSect( char* sDataBuf, int iDataLen )
{
	char	sCmd[64];

	if (sDataBuf == ARC_COM_NULL || iDataLen <= 0)
		return ARC_COM_ERROR;

	if (osSystem_GetOutput( "wandataif.sh", sDataBuf, iDataLen ) < 0)
		return ARC_COM_ERROR;

	utilStrTrim( sDataBuf );

	if (sDataBuf[0] == '\0')
		return ARC_COM_OK;

	snprintf( sCmd, sizeof(sCmd), "if2sect.sh %s", sDataBuf );

	if (osSystem_GetOutput( sCmd, sDataBuf, iDataLen ) < 0)
		return ARC_COM_ERROR;

	utilStrTrim( sDataBuf );

	return ARC_COM_OK;
}
