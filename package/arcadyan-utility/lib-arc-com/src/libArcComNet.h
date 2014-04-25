#ifndef _ARC_COM_NET_H_
#define _ARC_COM_NET_H_


#define		VAL_OS_SOCK_BY_IP			0x8000


typedef struct {
	char			sName[32];		/* interface name */
	char			sLinkEncap[32];	/* link encapsulation type */
	unsigned char	byMac[6];		/* MAC address */
	unsigned char	byIp[4];		/* IP4 interface address */
	unsigned char	byBcAddr[4];	/* IP4 broadcast */
	unsigned char	bySubMask[4];	/* IP4 sunnet mask */
	unsigned char	byP2pIp[4];		/* IP4 Ponit-to-point address */
	unsigned short	iMtu;			/* MTU */
	unsigned char	bEnabled;		/* interface up or down, 0-down, 1-up */
	unsigned long	rx_packets;		/* total packets received       */
	unsigned long	tx_packets;		/* total packets transmitted    */
	unsigned long	rx_bytes;		/* total bytes received         */
	unsigned long	tx_bytes;		/* total bytes transmitted      */
} stOsNetIfInfo;

typedef struct {
	unsigned char	bySubAddr[4];	/* IP4 subnet address */
	unsigned char	bySubMask[4];	/* IP4 sunnet mask */
	unsigned char	byGateway[4];	/* IP4 gateway address */
	unsigned char	iMetric;		/* metric */
	char			sIfName[32];	/* interface name */
} stOsNetRoute;

typedef struct {
	unsigned char	byIp[4];		/* IP4 address, option 50 */
	unsigned char	byMac[6];		/* IP4 MAC address */
	char			sName[32];		/* client host name, option 12 */
	char			sClientId[66];	/* client ID, option 61 */
} stOsDhcpClientInfo;

typedef struct {
	unsigned short	iType;			/* 1-TCP, 2-UDP */
	unsigned char	byIp[4];		/* IP4 address */
	unsigned short	iPort;			/* socket port address */
} stOsSocketPortInfo;

typedef struct {
	unsigned char	byIp[4];		/* IP4 address */
	unsigned char	byMac[6];		/* IP4 MAC address */
	char			sIfName[32];	/* interface name */
} stOsArpEntry;

typedef struct {
	int	get_time_state;	/*Ntp get time state*/
	int fail_num; /*Number of Ntp fail to get time*/
}stOsNtpInfo;


#ifdef __cplusplus
extern "C" {
#endif


extern long		osIfConfigGet ( char* sIfName, unsigned long lRecCnt, stOsNetIfInfo* pstBuf );
extern long     osIfConfigGet_S( char* sIfName, stOsNetIfInfo* pstBuf );
extern long		osSockPortGet ( int iType, unsigned long lRecCnt, stOsSocketPortInfo* pstBuf, unsigned char* bySrcIp );
extern long		osWANSockPortGet ( int iType, int* ports, int numPorts  );
extern long		osRouteGet ( unsigned long lRecCnt, stOsNetRoute* pstBuf );
extern long		osRouteDefaultGet ( unsigned long lRecCnt, stOsNetRoute* pstBuf );

extern long		osDnsSvrGet ( unsigned long lRecCnt, unsigned char* byIp );

extern long		osDhcpClientGet ( unsigned long lRecCnt, stOsDhcpClientInfo* stBuf );

extern long		osArpEntryGetOne ( stOsArpEntry* stBuf );
extern long		osArpEntryGet ( unsigned long lRecCnt, stOsArpEntry* stBuf );
extern void     osArpTableRefresh ( char *ifname );
extern void     osArpCurrentTableRefresh ( char *ifname );

extern long		osSambaIsEnable( void );
extern long		osSambaAdmin( int iAction );

extern long		osNtpStateGet( stOsNtpInfo* stBuf  );

#ifdef __cplusplus
}
#endif


#endif /* _ARC_COM_NET_H_ */
