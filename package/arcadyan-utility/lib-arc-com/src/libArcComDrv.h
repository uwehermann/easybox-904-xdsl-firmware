#ifndef _ARC_COM_DRV_H_
#define _ARC_COM_DRV_H_


#define		LACD_ETH_ID_MIN		0
#define		LACD_ETH_ID_MAX		5


typedef struct {
	char	sSwVer[32];
	char	sKernelVer[80];
	char	sUbootVer[80];
	char	sHwVer[32];
	char	sSerial[32];
} stSysVer;

typedef struct {
	char	sVer[64];
	long	bConnected;
	char	sMode[16];
	long	lLineRateUs; /* in Kbps */
	long	lLineRateDs; /* in Kbps */
} stDslInfo;

typedef struct {
	long	iPortId;	/* port number, 0-6 */
	long	iLinkStat;	/* 0 - link down, 1 - link up*/
	long	iLinkSpeed;	/* link speed, in Mbps, 0/10/100/1000 */
} stEthPhyStat;

typedef struct {
	long	bConnected;
    long    sig_strength;
} stLteInfo;

#ifdef __cplusplus
extern "C" {
#endif


extern long		drvSysVerGet( stSysVer* stBuf );
extern long		drvDslInfoGet( stDslInfo* stBuf );
extern long		drvUsbAdmin( int bEnable );
extern long		drvEthPhyStatus( stEthPhyStat* stBuf );
extern long     drvLteInfoGet( stLteInfo* stBuf );


#ifdef __cplusplus
}
#endif


#endif /* _ARC_COM_DRV_H_ */
