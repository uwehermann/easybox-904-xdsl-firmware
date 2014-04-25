#ifndef _ARC_COM_CFG_H_
#define _ARC_COM_CFG_H_


#define	CFG_DB_MIN				1		/* min value of config database ID */
#define	CFG_DB_GLBCFG			1		/* config database in /etc/config/.glbcfg */
#define	CFG_DB_GLBCFGDFT		2		/* config database in /etc/config/glbcfg.dft */
#define	CFG_DB_DRVSTAT			3		/* config database in /tmp/fs/drvstat */
#define	CFG_DB_DISKINFO			4		/* config database in /tmp/fs/diskinfo */
#define	CFG_DB_VOLINFO			5		/* config database in /tmp/fs/volinfo */
#define	CFG_DB_MAX				5		/* max value of config database ID */
#define	CFG_DB_VALID( id )	( (id)>=CFG_DB_MIN && (id)<=CFG_DB_MAX )
#define	CFG_DB_NAMELEN_MAX		32		/* max length of system database file name, including ending '\0' */

#define	CFG_FTP_DISK_MAX		2		/* max number of FTP disk */
#define	CFG_FTP_USR_MAX			8		/* max number of FTP user per disk */
#define	CFG_FTP_USR_NAME_MAX	33		/* max length of FTP user name */
#define	CFG_FTP_USR_PWD_MAX		35		/* max length of FTP user password, orginally 33, but pure-pwd need 34+1 for hased valued */
#define	CFG_FTP_USR_PATH_MAX	512		/* max length of FTP user root path */


typedef struct {
	unsigned char	iDisk;							/* 1-based disk/driver ID, 1~CFG_FTP_DISK_MAX */
	unsigned char	iUserId;						/* 1-based user ID, 1~CFG_FTP_USR_MAX */
	unsigned char	bValid;							/* validity, 0-invalid, 1-valid */
	unsigned char	bEnable;						/* administrative state, 0-disable, 1-enable */
	char			sUser[CFG_FTP_USR_NAME_MAX];	/* user name, including ending '\0' */
	unsigned char	bHasPwd;						/* empty password or not, 0-no password, 1-has password */
	char			sPwd[CFG_FTP_USR_PWD_MAX];		/* user password, including ending '\0' */
	unsigned char	iPartition;						/* 1-based valid data partition ID in disk */
	char			sPath[CFG_FTP_USR_PATH_MAX];	/* user password, including ending '\0' */
	unsigned char	bWritable;						/* write access right or not, 0-read-only, 1-read-write */
} stFtpUser;


#ifdef __cplusplus
extern "C" {
#endif


extern long		cfgDataGet( char* sFile, char* sSect, char* sKey, char* sDataBuf, int iDataLen );
extern long		cfgDataSet( char* sFile, char* sSect, char* sKey, char* sData );

extern long		cfgWanIfName( char* sDataBuf, int iDataLen );
extern long		cfgWanIfSect( char* sDataBuf, int iDataLen );


#ifdef __cplusplus
}
#endif


#endif /* _ARC_COM_CFG_H_ */
