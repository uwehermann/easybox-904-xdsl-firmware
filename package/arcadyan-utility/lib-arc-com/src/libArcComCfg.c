
#include <stdio.h>
#include <string.h>
#include "libArcComApi.h"


static char _gsDbFileName[ CFG_DB_MAX ][ CFG_DB_NAMELEN_MAX ] = {
				  "/etc/config/.glbcfg"
				, "/etc/config/glbcfg.dft"
				, "/tmp/fs/drvstat"
				, "/tmp/fs/diskinfo"
				, "/tmp/fs/volinfo"
			};


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
 *		get the corresponding system database file name according to its ID
 *
 * Parameters
 *		iDbId:		system database file ID, CFG_DATABASE_MIN ~ CFG_DATABASE_MAX.
 *		sBuf:		pointer to be filled with corresponding system database file name. It can be NULL.
 *
 * Returns
 *		* ARC_COM_NULL(0):	iDbId is out of range
 *		* others:			pointer to corresponding system database file name
 *
 * Note
 *		* sBuf size MUST be at least CFG_DB_NAMELEN_MAX if it is not NULL.
 *
 ******************************************************************************/
char* cfgDbName( int iDbId, char* sBuf )
{
	static char	sFn[ CFG_DB_NAMELEN_MAX ];

	if ( ! CFG_DB_VALID( iDbId ) )
		return ARC_COM_NULL;

	if (sBuf == ARC_COM_NULL)
	{
		sFn[ sizeof(sFn)-1 ] = '\0';
		strncpy( sFn, _gsDbFileName[ iDbId - CFG_DB_MIN ], sizeof(sFn)-1 );
		return sFn;
	}
	else
	{
		strncpy( sBuf, _gsDbFileName[ iDbId - CFG_DB_MIN ], sizeof(sFn)-1 );
		return sBuf;
	}
}

/*******************************************************************************
 * Description
 *		get a configuration data from a system database file
 *
 * Parameters
 *		iDbId:		system database file ID, CFG_DATABASE_MIN ~ CFG_DATABASE_MAX.
 *		sSect:		section name of key-value pair.
 *		sKey:		key name of key-value pair.
 *		sDataBuf:	buffer to be filled with returned data value.
 *		iDataLen:	length of sDataBuf.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * See Also
 *		cfgDbName, cfgDataGet, cfgDbDataSet
 *
 ******************************************************************************/
long cfgDbDataGet( int iDbId, char* sSect, char* sKey, char* sDataBuf, int iDataLen )
{
	char	sDbName[ CFG_DB_NAMELEN_MAX ];

	if ( cfgDbName( iDbId, sDbName ) == ARC_COM_NULL )
		return ARC_COM_ERROR;

	return cfgDataGet( sDbName, sSect, sKey, sDataBuf, iDataLen );
}

/*******************************************************************************
 * Description
 *		set a configuration data into a database file
 *
 * Parameters
 *		iDbId:	system database file ID, CFG_DATABASE_MIN ~ CFG_DATABASE_MAX.
 *		sSect:	section name of key-value pair.
 *		sKey:	key name of key-value pair.
 *		sData:	new data value.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	failed
 *
 * See Also
 *		cfgDbName, cfgDataSet, cfgDbDataGet
 *
 ******************************************************************************/
long cfgDbDataSet( int iDbId, char* sSect, char* sKey, char* sData )
{
	char	sDbName[ CFG_DB_NAMELEN_MAX ];

	if ( cfgDbName( iDbId, sDbName ) == ARC_COM_NULL )
		return ARC_COM_ERROR;

	return cfgDataSet( sDbName, sSect, sKey, sData );
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
