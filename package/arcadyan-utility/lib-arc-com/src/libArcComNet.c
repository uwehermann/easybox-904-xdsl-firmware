
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <sys/sysctl.h>

#include "libArcComApi.h"


static unsigned char	byPppPseudoIp[4] = { 10, 64, 64, 64 };
static unsigned char	byPppPseudoSubMask[4] = { 255, 255, 255, 255 };
static unsigned char	byPppPseudoP2pIp[4] = { 10, 112, 112, 112 };

#pragma pack(1)
typedef struct arp_pkt
{
    // ethernet header
    unsigned char   dst_mac[6];
    unsigned char   src_mac[6];
    unsigned short  ether_type;
    // arp header
    unsigned short  h_type;
    unsigned short  proto_type;
    unsigned char   h_size;
    unsigned char   proto_size;
    unsigned short  arp_opcode;
    unsigned char   sender_mac[6];
    unsigned long   sender_ip;
    unsigned char   target_mac[6];
    unsigned long   target_ip;
    // paddign
    char            dummy[18];
} arp_pkt_t;

/*******************************************************************************
 * Description
 *		get the configuration of a specified or all network interface info
 *
 * Parameters
 *		sIfName:	a specified network interface info to be retrieved.
 *					It can be NULL to get all network interface info.
 *		lRecCnt:	network interface entries count of pstBuf
 *		pstBuf:		pointer to buffer to be filled with network interface info.
 *					It can be NULL.
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* others:				number of all matched network interface info in system
 *
 * Note
 *		* if entries count is more than lRecCnt, only lRecCnt entries are filled
 *			into pstBuf, but this function also returns all matched entries count
 *		* if lRecCnt is 0 or pstBuf is NULL, this function won't fill any entries
 *		* if sIfName does not exist, pstBuf will be reset to 0's
 *		* this function is equal to Linux commands:
 *			1. "ifconfig -a"
 *			2. "ifconfig <ifName>"
 *		* interface with IP 10.64.64.164 and P2P IP 10.112.112.212 and subnet 255.255.255.255
 *			is PPP pseudo config (on demand). It is excluded.
 *
 ******************************************************************************/
long osIfConfigGet( char* sIfName, unsigned long lRecCnt, stOsNetIfInfo* pstBuf )
{
	char			sCmd[64];
	char			sTmpFn[32];
	FILE*			pFile;
	char			sLineBuf[128];
	long			lCnt;
	char*			pPtr;
	char*			pLast;
	stOsNetIfInfo	stIfInfo;
	int             ppp_idx=0;
	//this is tricky, must be sure the pseudo addresses are consistent with those used in on-demand ppp (ipcp.c, ip_demand_conf())
    unsigned long	PppPseudoIp, PppIp;
    unsigned long	PppPseudoSubMask, PppSubMask;
    unsigned long	PppPseudoP2pIp, PppP2pIp;

    if(lRecCnt==1){
        return osIfConfigGet_S(sIfName, pstBuf);
    }

    utilIp2Val(byPppPseudoIp, &PppPseudoIp);
    utilIp2Val(byPppPseudoSubMask, &PppPseudoSubMask);
    utilIp2Val(byPppPseudoP2pIp, &PppPseudoP2pIp);

	if (lRecCnt == 0)
		pstBuf = ARC_COM_NULL;

	if (pstBuf == ARC_COM_NULL)
		lRecCnt = 0;

	sprintf( sTmpFn, "/tmp/%ld.tmp", (long)getpid() );

	if (sIfName == ARC_COM_NULL)
		sprintf( sCmd, "ifconfig -a > %s 2> /dev/null", sTmpFn );
	else
		sprintf( sCmd, "ifconfig %s > %s 2> /dev/null", sIfName, sTmpFn );

	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;

	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;

	if (pstBuf != ARC_COM_NULL)
		osBZero( pstBuf, lRecCnt*sizeof(pstBuf[0]));
	lCnt = 0;

	osBZero( &stIfInfo, sizeof(stIfInfo) );
	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		/**/
		utilStrTrim( sLineBuf );
		/**/
		if ( strlen( sLineBuf ) == 0) /* blank line, means end of one record */
		{
			utilIp2Val(stIfInfo.byIp, &PppIp);
			utilIp2Val(stIfInfo.bySubMask, &PppSubMask);
			utilIp2Val(stIfInfo.byP2pIp, &PppP2pIp);
			if ( PppP2pIp != (PppPseudoP2pIp + ppp_idx)
			 ||  PppSubMask != PppPseudoSubMask
			 ||  PppIp != (PppPseudoIp + ppp_idx)
			 ||  strncmp( stIfInfo.sName, "ppp", 3 ) != 0 )	
			{
				if (pstBuf != ARC_COM_NULL)
				{
					pstBuf[lCnt] = stIfInfo;
					osBZero( &stIfInfo, sizeof(stIfInfo) );
				}
				lCnt++;
				if (lCnt >= lRecCnt)
					pstBuf = ARC_COM_NULL;
			}
			ppp_idx=0;
			continue;
		}
		/**/
		if (strstr( sLineBuf, "Link encap:" ) != ARC_COM_NULL)
		{
			if ( (pPtr=osStrTok_r( sLineBuf, " ", &pLast )) == ARC_COM_NULL )
				continue;
			strcpy( stIfInfo.sName, pPtr );
			/**/
			if ( (pPtr=osStrTok_r( pLast, ":", &pLast )) == ARC_COM_NULL )
				continue;
			/**/
			if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
				continue;
			strcpy( stIfInfo.sLinkEncap, pPtr );
			while ( (pPtr=osStrTok_r( pLast, " ", &pLast )) != ARC_COM_NULL )
			{
				if ( strcmp( pPtr, "HWaddr" ) == 0 )
					break;
				strcat( stIfInfo.sLinkEncap, " " );
				strcat( stIfInfo.sLinkEncap, pPtr );
			}
			/**/
			utilMacStr2Mac( pLast, stIfInfo.byMac );
		}
		else if ( (pLast=strstr( sLineBuf, "inet addr:" )) != ARC_COM_NULL)
		{
			pPtr = pLast;
			osStrTok_r( pLast, ":", &pLast );
			do
			{
				if ( strcmp( pPtr, "inet addr" ) == 0 )
				{
					if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
						continue;
					utilIpStr2Ip( pPtr, stIfInfo.byIp );
				}
				else if ( strcmp( pPtr, "Bcast" ) == 0 )
				{
					if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
						continue;
					utilIpStr2Ip( pPtr, stIfInfo.byBcAddr );
				}
				else if ( strcmp( pPtr, "Mask" ) == 0 )
				{
					if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
						continue;
					utilIpStr2Ip( pPtr, stIfInfo.bySubMask );
				}
				else if ( strcmp( pPtr, "P-t-P" ) == 0 )
				{
					if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
						continue;
					utilIpStr2Ip( pPtr, stIfInfo.byP2pIp );
					if (stIfInfo.sName != NULL)
#if 1 /* Terry 20121228, fix crash problem if sIfName is NULL */
						/* TODO, Need to review the workardound */
						ppp_idx=atoi(stIfInfo.sName+3);
#else
					    ppp_idx=atoi(sIfName+3);
#endif
				}
				else
				{
					break;
				}
			} while ( (pPtr=osStrTok_r( pLast, " :", &pLast )) != ARC_COM_NULL );
		}
		else if (  strstr( sLineBuf, "UP " ) != ARC_COM_NULL
				&& strstr( sLineBuf, "MTU:" ) != ARC_COM_NULL
				&& strstr( sLineBuf, "Metric:" ) != ARC_COM_NULL )
		{
			stIfInfo.bEnabled = ARC_COM_TRUE;
			stIfInfo.iMtu = (unsigned short)atol(strstr( sLineBuf, "MTU:" ) + 4);
		}
		else if ( strstr( sLineBuf, "RX packets:" ) != ARC_COM_NULL)
		{
			stIfInfo.rx_packets = (unsigned long)atol(strstr( sLineBuf, "RX packets:" ) + 11);
		}
		else if (  strstr( sLineBuf, "TX packets:" ) != ARC_COM_NULL)
		{
			stIfInfo.tx_packets = (unsigned long)atol(strstr( sLineBuf, "TX packets:" ) + 11);
		}
		else if ( strstr( sLineBuf, "RX bytes:" ) != ARC_COM_NULL
				&& strstr( sLineBuf, "TX bytes" ) != ARC_COM_NULL )
		{
			stIfInfo.rx_bytes = (unsigned long)atol(strstr( sLineBuf, "RX bytes:" ) + 9);
			stIfInfo.tx_bytes = (unsigned long)atol(strstr( sLineBuf, "TX bytes:" ) + 9);
		}
	}

	fclose( pFile );

	unlink( sTmpFn );

	return lCnt;
}

long osIfConfigGet_S( char* sIfName, stOsNetIfInfo* pstBuf )
{
    struct ifreq        ifr;
    struct sockaddr_in  *sin_ptr;
    int                 i, fd, ifup=0, is_ppp=0, ppp_idx;
    //this is tricky, must be sure the pseudo addresses are consistent with those used in on-demand ppp
    unsigned long       PppPseudoIp, PppIp;
    //unsigned long       PppPseudoSubMask, PppSubMask;
    unsigned long       PppPseudoP2pIp, PppP2pIp;

    if(sIfName==NULL){
        return ARC_COM_ERROR;
    }

    fd=socket(AF_INET,SOCK_DGRAM,0);    
    if(fd==-1){
        return ARC_COM_ERROR;
    }

    osBZero(pstBuf, sizeof(*pstBuf));
    strncpy(pstBuf->sName, sIfName, 31);

    osBZero(&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, sIfName, IFNAMSIZ-1);

    //FLAGs
    if(ioctl(fd, SIOCGIFFLAGS, &ifr)==-1){
        goto IfConfigGet_S_Error;
    }
//printf("ifr.ifr_flags=0x%x\n", ifr.ifr_flags);
    if(ifr.ifr_flags&IFF_UP){
        pstBuf->bEnabled=ARC_COM_TRUE;
        ifup=1;
    }
    else{
        pstBuf->bEnabled=ARC_COM_FALSE;
    }

    //sLinkEncap
    if(ifr.ifr_flags&IFF_LOOPBACK){
        strcpy(pstBuf->sLinkEncap, "Local Loopback");
    }
    else if(ifr.ifr_flags&IFF_POINTOPOINT){
        strcpy(pstBuf->sLinkEncap, "Point-to-Point Protocol");
        is_ppp=1;
        ppp_idx=atoi(sIfName+3);
    }
    else if(strncmp(sIfName, "imq", 3)==0){
        strcpy(pstBuf->sLinkEncap, "UNSPEC");
    }
    else{
        strcpy(pstBuf->sLinkEncap, "Ethernet");
    }
 
    //HW addr
    if(ioctl(fd, SIOCGIFHWADDR, &ifr)==-1){
        goto IfConfigGet_S_Error;
    }
    for(i=0; i<6; i++){
        pstBuf->byMac[i]=(unsigned char)ifr.ifr_hwaddr.sa_data[i];
    }

    if(ifup==0){
        goto IfConfigGet_S_Return;
    }

    utilIp2Val(byPppPseudoIp, &PppPseudoIp);
    //utilIp2Val(byPppPseudoSubMask, &PppPseudoSubMask);
    utilIp2Val(byPppPseudoP2pIp, &PppPseudoP2pIp);

    //IPv4 addr
    ifr.ifr_addr.sa_family=AF_INET;
    if(ioctl(fd, SIOCGIFADDR, &ifr)==-1){
        goto IfConfigGet_S_Error;
    }
    sin_ptr=(struct sockaddr_in *)&ifr.ifr_addr;
    memcpy(&(pstBuf->byIp[0]), (char *)&(sin_ptr->sin_addr), 4);
	utilIp2Val(pstBuf->byIp, &PppIp);
    if(is_ppp==1 && PppIp==(PppPseudoIp+ppp_idx)){
        pstBuf->bEnabled=ARC_COM_FALSE;
        goto IfConfigGet_S_Return;
    }

    //IPv4 netmask
    if(ioctl(fd, SIOCGIFNETMASK, &ifr)==-1){
        goto IfConfigGet_S_Error;
    }
    sin_ptr=(struct sockaddr_in *)&ifr.ifr_addr;
    memcpy(&(pstBuf->bySubMask[0]), (char *)&(sin_ptr->sin_addr), 4);
//printf("bySubMask=%d.%d.%d.%d\n", pstBuf->bySubMask[0], pstBuf->bySubMask[1], pstBuf->bySubMask[2], pstBuf->bySubMask[3]);

    //PPP peer addr
    if(is_ppp==1){
        if(ioctl(fd, SIOCGIFDSTADDR, &ifr)==-1){
            goto IfConfigGet_S_Error;
        }
        sin_ptr=(struct sockaddr_in *)&ifr.ifr_dstaddr;
        memcpy(&(pstBuf->byP2pIp[0]), (char *)&(sin_ptr->sin_addr), 4);
        utilIp2Val(pstBuf->byP2pIp, &PppP2pIp);
        if(PppP2pIp==(PppPseudoP2pIp+ppp_idx)){
            pstBuf->bEnabled=ARC_COM_FALSE;
            goto IfConfigGet_S_Return;
        }
    }

    //Broadcast addr
    if(is_ppp==0){
        for(i=0; i<4; i++){
            pstBuf->byBcAddr[i]=pstBuf->byIp[i]|~(pstBuf->bySubMask[i]);
        }
//printf("byBcAddr=%d.%d.%d.%d\n", pstBuf->byBcAddr[0], pstBuf->byBcAddr[1], pstBuf->byBcAddr[2], pstBuf->byBcAddr[3]);
    }
        

    //MTU
    if(ioctl(fd, SIOCGIFMTU, &ifr)==-1){
        goto IfConfigGet_S_Error;
    }
    pstBuf->iMtu=(unsigned short)ifr.ifr_mtu;
//printf("iMtu=%d\n", pstBuf->iMtu);

    //rx_packets, tx_packets, rx_bytes, tx_bytes are temporarily skipped, no one uses them so far

IfConfigGet_S_Return:
    close(fd);
    return ARC_COM_TRUE;

IfConfigGet_S_Error:
    close(fd);
    return ARC_COM_ERROR;
}

/*******************************************************************************
 * Description
 *		get the configuration of routing table
 *
 * Parameters
 *		lRecCnt:	routing entries count of pstBuf
 *		pstBuf:		pointer to buffer to be filled with routing entries.
 *					It can be NULL.
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* others:				number of all routing entries in system
 *
 * Note
 *		* if entries count is more than lRecCnt, only lRecCnt entries are filled
 *			into pstBuf, but this function also returns all entries count
 *		* if lRecCnt is 0 or pstBuf is NULL, this function won't fill any entries
 *		* this function is equal to Linux commands:
 *			1. "route -n"
 *		* routing entry with gateway 10.112.112.212 and subnet 255.255.255.255
 *			is PPP pseudo config (on demand). It is excluded.
 *
 ******************************************************************************/
long osRouteGet( unsigned long lRecCnt, stOsNetRoute* pstBuf )
{
	char			sCmd[64];
	char			sTmpFn[32];
	FILE*			pFile;
	char			sLineBuf[128];
	long			lCnt;
	char*			pPtr;
	char*			pLast;
	stOsNetRoute	stRoute;
	//this is tricky, must be sure the pseudo addresses are consistent with those used in on-demand ppp
    unsigned long	PppPseudoP2pIp, PppP2pIp;
    int             ppp_idx;

	if (lRecCnt == 0)
		pstBuf = ARC_COM_NULL;

	if (pstBuf == ARC_COM_NULL)
		lRecCnt = 0;

	sprintf( sTmpFn, "/tmp/%ld.tmp", (long)getpid() );

	sprintf( sCmd, "route -n > %s", sTmpFn );

	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;

	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;

	if (pstBuf != ARC_COM_NULL)
		osBZero( pstBuf, lRecCnt*sizeof(pstBuf[0]));
	lCnt = 0;

	osBZero( &stRoute, sizeof(stRoute) );
	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		/**/
		utilStrTrim( sLineBuf );
		/* subnet address */
		if ( (pPtr=osStrTok_r( sLineBuf, " ", &pLast )) == ARC_COM_NULL )
			continue;
		utilIpStr2Ip( pPtr, stRoute.bySubAddr );
		/* gateway */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		utilIpStr2Ip( pPtr,stRoute.byGateway );
		/* subnet mask */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		utilIpStr2Ip( pPtr, stRoute.bySubMask );
		/* Flags */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		/* Metric */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		stRoute.iMetric = (unsigned char)atoi( pPtr );
		/* Ref */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		/* Use */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		/* Iface */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		strcpy( stRoute.sIfName, pPtr );

	    utilIp2Val(byPppPseudoP2pIp, &PppPseudoP2pIp);
		if(strncmp( stRoute.sIfName, "ppp", 3 )==0){
		    ppp_idx=atoi(stRoute.sIfName+3);
		    PppPseudoP2pIp+=ppp_idx;
		}
		
		/**/
		utilIp2Val(stRoute.byGateway, &PppP2pIp);
		//if ( utilIpCmp( stRoute.byGateway, byPppPseudoP2pIp ) != 0
		// ||  strncmp( stRoute.sIfName, "ppp", 3 ) != 0 )
		if ( strncmp( stRoute.sIfName, "ppp", 3 ) != 0
		 || PppP2pIp != PppPseudoP2pIp )
		{
			if (pstBuf != ARC_COM_NULL)
			{
				pstBuf[lCnt] = stRoute;
				osBZero( &stRoute, sizeof(stRoute) );
			}
			lCnt++;
			if (lCnt >= lRecCnt)
				pstBuf = ARC_COM_NULL;
		}
	}

	fclose( pFile );

	unlink( sTmpFn );

	return lCnt;
}

/*******************************************************************************
 * Description
 *		get the configuration of default route entries
 *
 * Parameters
 *		lRecCnt:	default route entries count of pstBuf
 *		pstBuf:		pointer to buffer to be filled with default route entries.
 *					It can be NULL.
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* others:				number of default route entries in system
 *
 * Note
 *		* if entries count is more than lRecCnt, only lRecCnt entries are filled
 *			into pstBuf, but this function also returns all default route entries count
 *		* if lRecCnt is 0 or pstBuf is NULL, this function won't fill any entries
 *		* this function is equal to Linux commands:
 *			1. "route -n | awk '{ if ( $1 == "0.0.0.0" ) print $1,$2,$3,$5,$8 }'"
 *
 ******************************************************************************/
long osRouteDefaultGet( unsigned long lRecCnt, stOsNetRoute* pstBuf )
{
	char	sCmd[64];
	char	sTmpFn[32];
	FILE*	pFile;
	char	sLineBuf[128];
	long	lCnt;
	char*	pPtr;
	char*	pLast;

	if (lRecCnt == 0)
		pstBuf = ARC_COM_NULL;

	if (pstBuf == ARC_COM_NULL)
		lRecCnt = 0;

	sprintf( sTmpFn, "/tmp/%ld.tmp", (long)getpid() );

	sprintf( sCmd, "route -n | awk '{ if ( $1 == \"0.0.0.0\" ) print $1,$2,$3,$5,$8 }' > %s", sTmpFn );

	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;

	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;

	if (pstBuf != ARC_COM_NULL)
		osBZero( pstBuf, lRecCnt*sizeof(pstBuf[0]));
	lCnt = 0;

	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		/**/
		utilStrTrim( sLineBuf );
		/* subnet address */
		if ( (pPtr=osStrTok_r( sLineBuf, " ", &pLast )) == ARC_COM_NULL )
			continue;
		if (pstBuf != ARC_COM_NULL)
			utilIpStr2Ip( pPtr, pstBuf[lCnt].bySubAddr );
		/* gateway */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		if (pstBuf != ARC_COM_NULL)
			utilIpStr2Ip( pPtr, pstBuf[lCnt].byGateway );
		/* subnet mask */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		if (pstBuf != ARC_COM_NULL)
			utilIpStr2Ip( pPtr, pstBuf[lCnt].bySubMask );
		/* Metric */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		pstBuf[lCnt].iMetric = (unsigned char)atoi( pPtr );
		/* Iface */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		if (pstBuf != ARC_COM_NULL)
			strcpy( pstBuf[lCnt].sIfName, pPtr );
		/**/
		lCnt++;
		if (lCnt >= lRecCnt)
			pstBuf = ARC_COM_NULL;
	}

	fclose( pFile );

	unlink( sTmpFn );

	return lCnt;
}

/*******************************************************************************
 * Description
 *		get the configuration of listened TCP/UDP socket ports
 *
 * Parameters
 *		iType:		type of socket ports; 0-both tcp & udp, 1-tcp, 2-udp.
 *					If bit VAL_OS_SOCK_BY_IP is bit-wise-OR'ed,
 *					this function filters socket port by bySrcIp.
 *		lRecCnt:	socket ports entries count of pstBuf
 *		pstBuf:		pointer to buffer to be filled with socket ports info.
 *					It can be NULL.
 *		bySrcIp:	pointer to buffer to be filled with source IP address.
 *					It is ignored if iType::VAL_OS_SOCK_BY_IP bit is cleared.
 *					And, it is cannot be NULL is iType::VAL_OS_SOCK_BY_IP bit is set.
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* others:				number of all matched TCP/UDP socket ports info in system
 *
 * Note
 *		* if entries count is more than lRecCnt, only lRecCnt entries are filled
 *			into pstBuf, but this function also returns all matched entries count
 *		* if lRecCnt is 0 or pstBuf is NULL, this function won't fill any entries
 *		* this function is equal to Linux commands:
 *			1. "awk '{ print $2 " " $4 }' /proc/net/tcp | grep " 0A""
 *			2. "awk '{ print $2 " " $4 }' /proc/net/udp | grep " 07""
 *
 ******************************************************************************/
long osSockPortGet ( int iType, unsigned long lRecCnt, stOsSocketPortInfo* pstBuf, unsigned char* bySrcIp )
{
	char	sCmd[64];
	char	sTmpFn[32];
	FILE*	pFile;
	char	sLineBuf[128];
	long	lCnt;
	char*	pPtr;
	char*	pLast;
	char	sIpHex[9];
	int		bCheckIp;

	bCheckIp = ( (iType & VAL_OS_SOCK_BY_IP) != 0);
	iType &= ~VAL_OS_SOCK_BY_IP;

	if ( bCheckIp && bySrcIp == NULL)
		return ARC_COM_ERROR;

	if (pstBuf == ARC_COM_NULL)
		lRecCnt = 0;

	sprintf( sTmpFn, "/tmp/%ld.tmp", (long)getpid() );

	osBZero( pstBuf, lRecCnt*sizeof(pstBuf[0]));

	lCnt = 0;

	if (bCheckIp)
		sprintf( sIpHex, "%02X%02X%02X%02X", bySrcIp[0], bySrcIp[1], bySrcIp[2], bySrcIp[3] );

	/* TCP */
	if (iType == 0 || iType == 1)
	{
		sprintf( sCmd, "cat /proc/net/tcp > %s", sTmpFn );
		if (osSystem( sCmd ) != ARC_COM_OK)
			return ARC_COM_ERROR;
		if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
			return ARC_COM_ERROR;
		while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
		{
			/*  sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode
				0: 00000000:0203 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 9629 1 8305c8e0 3000 0 0 2 -1 */
			/* check state */
			if ( strstr( sLineBuf, " 0A " ) == ARC_COM_NULL ) /* 0A is LISTEN state */
				continue;
			/* split seq number */
			if ( (pPtr=osStrTok_r( sLineBuf, " ", &pLast )) == ARC_COM_NULL )
				continue;
			/* split local address */
			if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
				continue;
			/* split source IP address & socket port */
			if ( (pPtr=osStrTok_r( pPtr, ":", &pLast )) == ARC_COM_NULL )
				continue;
			/* check source IP address */
			if ( bCheckIp && strcmp(pPtr,sIpHex) != 0 )
				continue;
			/* fill data */
			if (lCnt < lRecCnt)
			{
				pstBuf[lCnt].iType = 1;
				utilVal2Ip( strtoul( pPtr, 0, 16 ), pstBuf[lCnt].byIp );
				pstBuf[lCnt].iPort = (unsigned short)strtoul( pLast, 0, 16 );
			}
			/**/
			lCnt++;
		}

		fclose( pFile );

	}

	/* UDP */
	if (iType == 0 || iType == 2)
	{
		sprintf( sCmd, "cat /proc/net/udp > %s", sTmpFn );
		if (osSystem( sCmd ) != ARC_COM_OK)
			return ARC_COM_ERROR;
		if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
			return ARC_COM_ERROR;
		while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
		{
			/*  sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode
				0: 00000000:0203 00000000:0000 07 00000000:00000000 00:00000000 00000000     0        0 9629 1 8305c8e0 3000 0 0 2 -1 */
			/* check state */
			if ( strstr( sLineBuf, " 07 " ) == ARC_COM_NULL ) /* 07 is LISTEN state */
				continue;
			/* split seq number */
			if ( (pPtr=osStrTok_r( sLineBuf, " ", &pLast )) == ARC_COM_NULL )
				continue;
			/* split local address */
			if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
				continue;
			/* split source IP address & socket port */
			if ( (pPtr=osStrTok_r( pPtr, ":", &pLast )) == ARC_COM_NULL )
				continue;
			/* check source IP address */
			if ( bCheckIp && strcmp(pPtr,sIpHex) != 0 )
				continue;
			/* fill data */
			if (lCnt < lRecCnt)
			{
				pstBuf[lCnt].iType = 2;
				utilVal2Ip( strtoul( pPtr, 0, 16 ), pstBuf[lCnt].byIp );
				pstBuf[lCnt].iPort = (unsigned short)strtoul( pLast, 0, 16 );
			}
			/**/
			lCnt++;
		}

		fclose( pFile );

	}

	unlink( sTmpFn );

	return lCnt;
}

/*******************************************************************************
 * Description
 *		get the configuration of listened TCP/UDP wan socket ports
 *
 * Parameters
 *		iType:		type of socket ports; 0-both tcp & udp, 1-tcp, 2-udp.
 *					If bit VAL_OS_SOCK_BY_IP is bit-wise-OR'ed,
 *					this function filters socket port by bySrcIp.
 *		ports:		return a port list for the query
 *		numPorts:	specify the max size of ports array
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* others:				number of all matched TCP/UDP socket ports info in ports
 *
 * Note
 *
 ******************************************************************************/
long osWANSockPortGet ( int iType, int *ports, int numPorts  ){

	stOsNetIfInfo		stWanInfo;
	stOsSocketPortInfo*	stSockInfo = NULL; // modified by frank_lin 2012-05-29
	long				wSockCnt;
	long				wCnt, wTcpUdp; //, wTcp, wUdp;
	long				numDefIf = 0;
	stOsNetRoute		pstBuf;

	//if_name = IsDefaultRouteExisted2(NULL);
	numDefIf = osRouteDefaultGet( 1, &pstBuf );
	//get first default route name
	if (numDefIf > 0)
		osIfConfigGet( pstBuf.sIfName, 1, &stWanInfo );
	else
		utilIpZero( stWanInfo.byIp );

	wSockCnt = osSockPortGet( 0, 0, 0, stWanInfo.byIp );
	if (wSockCnt > 0)
		stSockInfo = malloc( sizeof(stSockInfo[0]) * wSockCnt );

//	printf("osWANSockPortGet> IfName=%s, wan_ip=%d.%d.%d.%d\n", pstBuf.sIfName, stWanInfo.byIp[0]
//																				, stWanInfo.byIp[1]
//																				, stWanInfo.byIp[2]
//																				, stWanInfo.byIp[3]
//																				);

	if (stSockInfo != NULL)
	{
		wCnt = osSockPortGet( 0, wSockCnt, stSockInfo, stWanInfo.byIp );
		if (wCnt < wSockCnt)
			wSockCnt = wCnt;
	}
	else
	{
		wSockCnt = 0;
	}

	for (wCnt=0, wTcpUdp=0; wCnt<wSockCnt; wCnt++)
	{
		if ((utilIpIsZero(stSockInfo[wCnt].byIp)==1) || (utilIpCmp(stSockInfo[wCnt].byIp,stWanInfo.byIp)==0) ){  // listening at WAN side
//			printf("osWANSockPortGet> stSockInfo[%d].byIp:%d.%d.%d.%d iType:%d iPort:%d\n", wCnt
//																				, stSockInfo[wCnt].byIp[0]
//																				, stSockInfo[wCnt].byIp[1]
//																				, stSockInfo[wCnt].byIp[2]
//																				, stSockInfo[wCnt].byIp[3]
//																				, stSockInfo[wCnt].iType
//																				, htons(stSockInfo[wCnt].iPort) );
			if (iType == stSockInfo[wCnt].iType)
			{
				ports[wTcpUdp++] = htons(stSockInfo[wCnt].iPort);
			}
			if (wTcpUdp >= numPorts)
				break;
		}
	}

	if (stSockInfo)
		free( stSockInfo );

	return wTcpUdp;
}

/*******************************************************************************
 * Description
 *		get the configuration of all DNS server IPs
 *
 * Parameters
 *		lRecCnt:	DNS server entries count of byIp
 *		byIp:		pointer to buffer to be filled with DNS server IPs.
 *					It can be NULL.
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* others:				number of all DNS server IPs in system
 *
 * Note
 *		* if entries count is more than lRecCnt, only lRecCnt entries are filled
 *			into pstBuf, but this function also returns all matched entries count
 *		* if lRecCnt is 0 or pstBuf is NULL, this function won't fill any entries
 *		* this function is equal to Linux commands:
 *			1. "cat /tmp/resolv.conf /tmp/resolv.conf.auto"
 *
 ******************************************************************************/
long osDnsSvrGet( unsigned long lRecCnt, unsigned char* byIp )
{
	char	sCmd[128];
	char	sTmpFn[32];
	FILE*	pFile;
	char	sLineBuf[128];
	long	lCnt;

	if (byIp == 0)
		lRecCnt = 0;

	sprintf( sTmpFn, "/tmp/%ld.tmp", (long)getpid() );

	sprintf( sCmd, "cat /tmp/resolv.conf.auto | grep -v -e '^search ' -e ' 127.0.0.1$' > %s", sTmpFn );

	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;

	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;

	osBZero( byIp, lRecCnt*4);
	lCnt = 0;

	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		/**/
		utilStrTrim( sLineBuf );
		/**/
		if ( strlen( sLineBuf ) == 0)
			continue;
		/**/
		if ( strncmp( sLineBuf, "nameserver ", 11 ) != 0)
			continue;
		/**/
		if (lCnt < lRecCnt)
		{
			utilIpStr2Ip( sLineBuf+11, &byIp[lCnt*4] );
		}
		lCnt++;
	}

	fclose( pFile );

	unlink( sTmpFn );

	return lCnt;
}

/*******************************************************************************
 * Description
 *		get the configuration of all leased DHCP client IPs
 *
 * Parameters
 *		lRecCnt:	DHCP client entries count of byIp
 *		stBuf:		pointer to buffer to be filled with DHCP client info.
 *					It can be NULL.
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* others:				number of all DHCP clients in system
 *
 * Note
 *		* if entries count is more than lRecCnt, only lRecCnt entries are filled
 *			into pstBuf, but this function also returns all matched entries count
 *		* if lRecCnt is 0 or pstBuf is NULL, this function won't fill any entries
 *		* this function is equal to Linux commands:
 *			1. "cat /tmp/dhcp.leases"
 *		* PLEASE AVOID TO BE STACK OVERFLOW
 *
 ******************************************************************************/
long osDhcpClientGet ( unsigned long lRecCnt, stOsDhcpClientInfo* stBuf )
{
	char	sTmpFn[32];
	FILE*	pFile;
	char	sLineBuf[128];
	long	lCnt;
	char*	pPtr;
	char*	pLast;
	stOsDhcpClientInfo*	pData;

	if (stBuf == ARC_COM_NULL)
		lRecCnt = 0;

	strcpy( sTmpFn, "/tmp/dhcp.leases" );

	if ( (pFile=fopen(sTmpFn,"r")) == 0 )
		return ARC_COM_ERROR;

	pData = stBuf;
	lCnt = 0;

	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		osStrTok_r( sLineBuf, " \r\n", &pLast);
		/**/
		if ( pLast == ARC_COM_NULL || strlen( pLast ) == 0)
			continue;
		/**/
		if (lCnt < lRecCnt) {
			osBZero( pData, sizeof(*pData) );
			if ( (pPtr=osStrTok_r( pLast, " \r\n", &pLast)) != ARC_COM_NULL)
				utilMacStr2Mac( pPtr, pData->byMac );
			if ( (pPtr=osStrTok_r( pLast, " \r\n", &pLast)) != ARC_COM_NULL)
				utilIpStr2Ip( pPtr, pData->byIp );
			if ( (pPtr=osStrTok_r( pLast, " \r\n", &pLast)) != ARC_COM_NULL)
				utilStrMCpy( pData->sName, pPtr, sizeof(pData->sName) );
			if ( pLast != ARC_COM_NULL)
				utilStrMCpy( pData->sClientId, pLast, sizeof(pData->sClientId) );
			pData++;
		}
		lCnt++;
	}

	fclose( pFile );

	return lCnt;
}

/*******************************************************************************
 * Description
 *		get the info of a specific ARP entry
 *
 * Parameters
 *		stBuf:		pointer to buffer to be filled with the specified ARP entry.
 *					It can not be NULL and stBuf->byIp MUST be set to the host IP
 *					address by caller.
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed or stBuf is NULL
 *		* others:				number of matched ARP entry in system
 *
 * Note
 *		* stBuf MUST not be NULL and stBuf->byIp MUST be set to the host IP address.
 *		* this function is equal to Linux commands:
 *			1. "arp | awk '{ if ( $1 == "<stBuf->byIp>" ) print $4,$6 }'"
 *
 ******************************************************************************/
long osArpEntryGetOne ( stOsArpEntry* pstBuf )
{
	char	sCmd[64];
	char	sTmpFn[32];
	FILE*	pFile;
	char	sLineBuf[128];
	long	lCnt;
	char*	pPtr;
	char*	pLast;

	if (pstBuf == ARC_COM_NULL)
		return ARC_COM_ERROR;

	sprintf( sTmpFn, "/tmp/%ld.tmp", (long)getpid() );

	sprintf( sCmd, "arp | awk '{ if ( $1 == \"%u.%u.%u.%u\" ) print $4,$6 }' > %s", pstBuf->byIp[0], pstBuf->byIp[1], pstBuf->byIp[2], pstBuf->byIp[3], sTmpFn );

	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;

	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;

	lCnt = 0;

	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		/**/
		utilStrTrim( sLineBuf );
		/* HW address */
		if ( (pPtr=osStrTok_r( sLineBuf, " ", &pLast )) == ARC_COM_NULL )
			continue;
		if (pstBuf != ARC_COM_NULL)
			utilMacStr2Mac( pPtr, pstBuf[lCnt].byMac );
		/* Device */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		if (pstBuf != ARC_COM_NULL)
			strcpy( pstBuf[lCnt].sIfName, pPtr );
		/**/
		lCnt++;
		if (lCnt >= 1)
			pstBuf = ARC_COM_NULL;
	}

	fclose( pFile );

	return lCnt;
}

/*******************************************************************************
 * Description
 *		get the info of all ARP entries
 *
 * Parameters
 *		lRecCnt:	DHCP client entries count of byIp
 *		stBuf:		pointer to buffer to be filled with ARP entries.
 *					It can be NULL.
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* others:				number of all ARP entries in system
 *
 * Note
 *		* if entries count is more than lRecCnt, only lRecCnt entries are filled
 *			into pstBuf, but this function also returns all matched entries count
 *		* if lRecCnt is 0 or pstBuf is NULL, this function won't fill any entries
 *		* this function is equal to Linux commands:
 *			1. "arp | awk '{ if ( $1 != "IP" ) print $1,$4,$6 }'"
 *
 ******************************************************************************/
long osArpEntryGet ( unsigned long lRecCnt, stOsArpEntry* pstBuf )
{
	char	sCmd[64];
	char	sTmpFn[32];
	FILE*	pFile;
	char	sLineBuf[128];
	long	lCnt;
	char*	pPtr;
	char*	pLast;

	if (pstBuf == ARC_COM_NULL)
		lRecCnt = 0;

	sprintf( sTmpFn, "/tmp/%ld.tmp", (long)getpid() );

	sprintf( sCmd, "arp | awk '{ if ( $1 != \"IP\" ) print $1,$4,$6 }' > %s", sTmpFn );

	if (osSystem( sCmd ) != ARC_COM_OK)
		return ARC_COM_ERROR;

	if ( (pFile=fopen(sTmpFn,"r")) == ARC_COM_NULL )
		return ARC_COM_ERROR;

	lCnt = 0;

	while ( fgets( sLineBuf, sizeof(sLineBuf), pFile ) != ARC_COM_NULL )
	{
		/**/
		utilStrTrim( sLineBuf );
		/* IP address */
		if ( (pPtr=osStrTok_r( sLineBuf, " ", &pLast )) == ARC_COM_NULL )
			continue;
		if (pstBuf != ARC_COM_NULL)
			utilIpStr2Ip( pPtr, pstBuf[lCnt].byIp );
		/* HW address */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		if (pstBuf != ARC_COM_NULL)
			utilMacStr2Mac( pPtr, pstBuf[lCnt].byMac );
		/* Device */
		if ( (pPtr=osStrTok_r( pLast, " ", &pLast )) == ARC_COM_NULL )
			continue;
		if (pstBuf != ARC_COM_NULL)
			strcpy( pstBuf[lCnt].sIfName, pPtr );
		/**/
		lCnt++;
		if (lCnt >= lRecCnt)
			pstBuf = ARC_COM_NULL;
	}

	fclose( pFile );

	return lCnt;
}

/*******************************************************************************
 * Description
 *		update ARP entries of an interface
 *
 * Parameters
 *		ifname:     device name of the interface
 *
 * Returns
 *		none
 *
 * Note
 *		* this function sends ARP requests in short time, intends to update the arp table by the ARP response
 *
 ******************************************************************************/
void osArpTableRefresh ( char *ifname )
{
    int                 retVal;
    int                 if_fd, sock_fd;
    struct ifreq        ifr;
    char                mac_addr[6];
    struct sockaddr_in  *sin;
    struct sockaddr_ll  sa = {0};	// bug fixed by Fallen, uninitialized structure causes arp sendto() failed.
    arp_pkt_t           arp_req;
    unsigned long       if_ip_addr, if_ip_mask, target_ip_addr;
    unsigned long       i, range;
    char                sCmd[64];
   
    if (ifname == NULL)
    {
        return;
    }
    
    /* open socket to get mac, ip4 address, and mask of the specified Interface */
    //if_fd = socket(AF_INET, SOCK_STREAM, 0);
    if_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if( if_fd < 0 )
    {
        printf("if_fd socket error\n");
        return;
    }
    
    strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
    
    /* get mac */
    retVal = ioctl(if_fd, SIOCGIFHWADDR, &ifr, sizeof(ifr));
    if( retVal < 0 )
    {
        printf("ioctl SIOCGIFHWADDR error\n");
        close(if_fd);
        return;
    }
    mac_addr[0] = ifr.ifr_hwaddr.sa_data[0]&0xff;
    mac_addr[1] = ifr.ifr_hwaddr.sa_data[1]&0xff;
    mac_addr[2] = ifr.ifr_hwaddr.sa_data[2]&0xff;
    mac_addr[3] = ifr.ifr_hwaddr.sa_data[3]&0xff;
    mac_addr[4] = ifr.ifr_hwaddr.sa_data[4]&0xff;
    mac_addr[5] = ifr.ifr_hwaddr.sa_data[5]&0xff;
    
    /* get ip4 address */
    retVal = ioctl(if_fd, SIOCGIFADDR, &ifr, sizeof(ifr));
    if( retVal < 0 )
    {
        printf("ioctl SIOCGIFADDR error\n");
        close(if_fd);
        return;
    }
    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    if_ip_addr = ntohl(sin->sin_addr.s_addr);

    /* get subnet mask */
    retVal = ioctl(if_fd, SIOCGIFNETMASK, &ifr, sizeof(ifr));
    if( retVal < 0 )
    {
        printf("ioctl SIOCGIFNETMASK error\n");
        close(if_fd);
        return;
    }
    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    if_ip_mask = ntohl(sin->sin_addr.s_addr);
//printf("if_ip_mask=0x%x\n", if_ip_mask);

    range = (~(if_ip_mask))&0xffffffff;
    if( range<=0 ){
        printf("target ip range=%d\n", range);
        close(if_fd);
        return;
    }        

    sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if( sock_fd == -1 )
    {
        printf("sock_fd socket error\n");
        close(if_fd);
        return;
    }
    
    /* set arp request fixed content */
    memset(arp_req.dst_mac, 0xff, 6);
    arp_req.src_mac[0] = mac_addr[0];
    arp_req.src_mac[1] = mac_addr[1];
    arp_req.src_mac[2] = mac_addr[2];
    arp_req.src_mac[3] = mac_addr[3];
    arp_req.src_mac[4] = mac_addr[4];
    arp_req.src_mac[5] = mac_addr[5];
    arp_req.ether_type = htons(0x0806);
    arp_req.h_type = htons(0x0001);
    arp_req.proto_type = htons(0x0800);
    arp_req.h_size = 0x06;
    arp_req.proto_size = 0x04;
    arp_req.arp_opcode = htons(0x0001);
    arp_req.sender_mac[0] = mac_addr[0];
    arp_req.sender_mac[1] = mac_addr[1];
    arp_req.sender_mac[2] = mac_addr[2];
    arp_req.sender_mac[3] = mac_addr[3];
    arp_req.sender_mac[4] = mac_addr[4];
    arp_req.sender_mac[5] = mac_addr[5];
    arp_req.sender_ip = htonl(if_ip_addr);
    memset(arp_req.target_mac, 0 , 6);
    memset(arp_req.dummy, 0 , 18);
    
    /* set target ip and send */    
    retVal = ioctl(if_fd, SIOCGIFINDEX, &ifr, sizeof(ifr));
    if( retVal < 0 )
    {
        printf("ioctl SIOCGIFINDEX error\n");
        close(sock_fd);
        close(if_fd);
        return;
    }
    
    /* flush arp cache */
    sprintf(sCmd, "/usr/sbin/ip neigh flush dev %s", ifname);
    osSystem(sCmd);

    /* to allow gratuitous ARP response temporarily */
    sprintf(sCmd, "echo 1 > /proc/sys/net/ipv4/conf/all/arp_accept");
    if (osSystem( sCmd ) != ARC_COM_OK){
        printf("set /proc/sys/net/ipv4/conf/all/arp_accept error\n");
        close(sock_fd);
        close(if_fd);

        return;
    }        
    
    sa.sll_family = AF_PACKET;
    sa.sll_ifindex = ifr.ifr_ifindex;
    sa.sll_protocol = htons(ETH_P_ARP);
	sa.sll_hatype   = 1;
	sa.sll_halen = 6;

    for(i = 0; i < range; i++){
        target_ip_addr=(if_ip_addr&if_ip_mask)+i;
        if(target_ip_addr != if_ip_addr){
            arp_req.target_ip = htonl(target_ip_addr);
            retVal = sendto(sock_fd, &arp_req, sizeof(arp_req), 0,(struct sockaddr *)&sa, sizeof(sa));
            if( retVal < 0 )
            {
                printf("sendto error, target ip=0x%x\n", target_ip_addr);
            }
            else{
                usleep(1000);
            }
        }
    }
    
    close(sock_fd);
    close(if_fd);

    sprintf(sCmd, "echo 0 > /proc/sys/net/ipv4/conf/all/arp_accept");
    if (osSystem( sCmd ) != ARC_COM_OK){
        printf("reset /proc/sys/net/ipv4/conf/all/arp_accept error\n");
    }

    return;
}

void osArpCurrentTableRefresh ( char *ifname )
{
    int                 retVal;
    int                 if_fd, sock_fd;
    struct ifreq        ifr;
    char                mac_addr[6];
    struct sockaddr_in  *sin;
    struct sockaddr_ll  sa;
    arp_pkt_t           arp_req;
    unsigned long       if_ip_addr, if_ip_mask, target_ip_addr;
    int                 i, range;
    char                sCmd[64];
   
    if (ifname == NULL)
    {
        return;
    }
    
    /* open socket to get mac, ip4 address, and mask of the specified Interface */
    //if_fd = socket(AF_INET, SOCK_STREAM, 0);
    if_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if( if_fd < 0 )
    {
        printf("if_fd socket error\n");
        return;
    }
    
    strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
    
    /* get mac */
    retVal = ioctl(if_fd, SIOCGIFHWADDR, &ifr, sizeof(ifr));
    if( retVal < 0 )
    {
        printf("ioctl SIOCGIFHWADDR error\n");
        close(if_fd);
        return;
    }
    mac_addr[0] = ifr.ifr_hwaddr.sa_data[0]&0xff;
    mac_addr[1] = ifr.ifr_hwaddr.sa_data[1]&0xff;
    mac_addr[2] = ifr.ifr_hwaddr.sa_data[2]&0xff;
    mac_addr[3] = ifr.ifr_hwaddr.sa_data[3]&0xff;
    mac_addr[4] = ifr.ifr_hwaddr.sa_data[4]&0xff;
    mac_addr[5] = ifr.ifr_hwaddr.sa_data[5]&0xff;
    
    /* get ip4 address */
    retVal = ioctl(if_fd, SIOCGIFADDR, &ifr, sizeof(ifr));
    if( retVal < 0 )
    {
        printf("ioctl SIOCGIFADDR error\n");
        close(if_fd);
        return;
    }
    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    if_ip_addr = ntohl(sin->sin_addr.s_addr);

    /* get subnet mask */
    retVal = ioctl(if_fd, SIOCGIFNETMASK, &ifr, sizeof(ifr));
    if( retVal < 0 )
    {
        printf("ioctl SIOCGIFNETMASK error\n");
        close(if_fd);
        return;
    }
    sin = (struct sockaddr_in *)&ifr.ifr_addr;

    sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if( sock_fd == -1 )
    {
        printf("sock_fd socket error\n");
        close(if_fd);
        return;
    }
    
    /* set arp request fixed content */
    memset(arp_req.dst_mac, 0xff, 6);
    arp_req.src_mac[0] = mac_addr[0];
    arp_req.src_mac[1] = mac_addr[1];
    arp_req.src_mac[2] = mac_addr[2];
    arp_req.src_mac[3] = mac_addr[3];
    arp_req.src_mac[4] = mac_addr[4];
    arp_req.src_mac[5] = mac_addr[5];
    arp_req.ether_type = htons(0x0806);
    arp_req.h_type = htons(0x0001);
    arp_req.proto_type = htons(0x0800);
    arp_req.h_size = 0x06;
    arp_req.proto_size = 0x04;
    arp_req.arp_opcode = htons(0x0001);
    arp_req.sender_mac[0] = mac_addr[0];
    arp_req.sender_mac[1] = mac_addr[1];
    arp_req.sender_mac[2] = mac_addr[2];
    arp_req.sender_mac[3] = mac_addr[3];
    arp_req.sender_mac[4] = mac_addr[4];
    arp_req.sender_mac[5] = mac_addr[5];
    arp_req.sender_ip = htonl(if_ip_addr);
    memset(arp_req.target_mac, 0 , 6);
    memset(arp_req.dummy, 0 , 18);
    
    /* set target ip and send */    
    retVal = ioctl(if_fd, SIOCGIFINDEX, &ifr, sizeof(ifr));
    if( retVal < 0 )
    {
        printf("ioctl SIOCGIFINDEX error\n");
        close(sock_fd);
        close(if_fd);
        return;
    }
    
    /* flush arp cache */
    sprintf(sCmd, "/usr/sbin/ip neigh flush dev %s", ifname);
    osSystem(sCmd);

    /* to allow gratuitous ARP response temporarily */
    sprintf(sCmd, "echo 1 > /proc/sys/net/ipv4/conf/all/arp_accept");
    if (osSystem( sCmd ) != ARC_COM_OK){
        printf("set /proc/sys/net/ipv4/conf/all/arp_accept error\n");
        close(sock_fd);
        close(if_fd);

        return;
    }        
    
    sa.sll_family = AF_PACKET;
    sa.sll_ifindex = ifr.ifr_ifindex;
    sa.sll_protocol = htons(ETH_P_ARP);
	sa.sll_hatype   = 1;
	sa.sll_halen = 6;

    FILE                *pArpFile;
    char			    sLineBuf[128];
    char                arp_mac[20], arp_ip4[16], arp_mask[8], arp_dev[24];
    int                 arp_hw_type, arp_flags;    

	if((pArpFile=fopen("/proc/net/arp", "r"))==NULL)
		return;

    fgets(sLineBuf, sizeof(sLineBuf), pArpFile);   //skip the title line
	while(fgets(sLineBuf, sizeof(sLineBuf), pArpFile)!=NULL)
    {
	    sscanf(sLineBuf, "%s 0x%x 0x%x %s %s %s",
	           arp_ip4, &arp_hw_type, &arp_flags, arp_mac, arp_mask, arp_dev);
        if(strcmp(ifname, arp_dev) == 0)
        {
            arp_req.target_ip = inet_addr(arp_ip4);
            retVal = sendto(sock_fd, &arp_req, sizeof(arp_req), 0,(struct sockaddr *)&sa, sizeof(sa));
            if( retVal < 0 )
            {
                printf("sendto error, target ip=0x%x\n", target_ip_addr);
            }
            else{
                usleep(1000);
            }        
	    }
	}
    fclose(pArpFile);
    close(sock_fd);
    close(if_fd);

    sprintf(sCmd, "echo 0 > /proc/sys/net/ipv4/conf/all/arp_accept");
    if (osSystem( sCmd ) != ARC_COM_OK){
        printf("reset /proc/sys/net/ipv4/conf/all/arp_accept error\n");
    }

    return;
}


/*******************************************************************************
 * Description
 *		check if Samba server starts
 *
 * Returns
 *		* ARC_COM_FALSE(0):	disabled
 *		* ARC_COM_TRUE(1):	enabled
 *		* ARC_COM_ERROR(-1):	failed
 *
 * Note
 *		* this function refers:
 *///			1. "cat /proc/*[0-9]/stat 2>&- | grep -e '(smbd)'"
/*
 ******************************************************************************/
long osSambaIsEnable( void )
{
	char	sBuf[16];

	if (osSystem_GetOutput( "cat /proc/*[0-9]/stat 2>&- | grep -c '(smbd)'", sBuf, sizeof(sBuf) ) <= 0)
		return ARC_COM_ERROR;

	if (atoi(sBuf) <= 0)
		return ARC_COM_FALSE;

	return ARC_COM_TRUE;
}

/*******************************************************************************
 * Description
 *		enable, shutdown or restart Samba server
 *
 * Parameters
 *		bEnable:	0-disable, 1-enable, 2-restart.
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* ARC_COM_OK(0):		successfully
 *
 * Note
 *		* this function refers:
 *			1. /usr/sbin/diskmount.sh unmount-all
 *			2. USB/GPIO power on/off
 *		* this function ONLY SUPPORT VR9
 *			1. VR9 EVM USB - GPIO 33
 *			1. 723 USB - GPIO 32
 *
 ******************************************************************************/
long osSambaAdmin( int iAction )
{
	if (iAction == 0) /* disable Samba */
	{
		if (osRunTask( "/etc/init.d/samba stop" ) != ARC_COM_OK)
			return ARC_COM_ERROR;
	}
	else if (iAction == 1) /* enable Samba */
	{
		if (osRunTask( "/etc/init.d/samba start" ) != ARC_COM_OK)
			return ARC_COM_ERROR;
	}
	else if (iAction == 2) /* restart Samba */
	{
		if (osRunTask( "/etc/init.d/samba stop; /etc/init.d/samba start" ) != ARC_COM_OK)
			return ARC_COM_ERROR;
	}

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Get infomation of Ntp
 *
 * Parameters
 *		stBuf->get_time_state:	0-Ntp never get time, 1-Ntp have got time successful.
 *	    stBuf->fail_num:  The number of Ntp fail to get time. 
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* ARC_COM_OK(0):		successfully
 *
 * Note
 *
 ******************************************************************************/
long osNtpStateGet( stOsNtpInfo* stBuf )
{
	FILE 	*fp;
	char	sLineBuf[32];
	char	title[32];
	char	num[32];
	int 	i, j , get;

	if((fp=fopen("/tmp/ntp_status","r"))==NULL)
		return ARC_COM_ERROR;

	while(fgets(sLineBuf, sizeof(sLineBuf), fp)!=NULL)
    {
		memset( title, 0, sizeof(title) );
		memset( num, 0, sizeof(num) );

		i = 0;
		j = 0;
		get = 0;

		//Get title
		while( sLineBuf[i] != '=')
		{
			i++;
		}

		memcpy(title, sLineBuf, i);

		i++;

		if( strcmp(title, "get_time_state")==0 )
			get	= 1;
		else if( strcmp(title, "fail_num")==0 )
			get = 2;

		if( (get == 1) || (get == 2) )
		{	//Get number
			while( sLineBuf[i] != NULL )
			{
				num[j] = sLineBuf[i];
				i++;
				j++;
			}
			printf("title = %s, ", title);
			printf("num = %s", num);

			switch(get)
			{
				case 1:	
					stBuf->get_time_state = atoi(num);
					break;
				case 2:
					stBuf->fail_num = atoi(num);
					break;
				default:
					break;
			}
		}
	}
	
	fclose(fp);

	return ARC_COM_OK;
}
