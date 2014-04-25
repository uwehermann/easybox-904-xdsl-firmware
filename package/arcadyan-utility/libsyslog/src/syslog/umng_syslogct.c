#include "etcpip.h"
#include "ip.h"
#include "if.h"
#include "config.h"
#include "sockapi.h"
#include "dhcp.h"
#include "syslogct.h"
#define SYSLOGMBX	1

unsigned long syslog_idx = 0;
struct sockaddr_in SyslogSock;
int sock_id = -1;
//static struct sockaddr_in SyslogSock;
//static int sock_id = -1;
int SyslogSrv_running = 0;
char syslog_buf[512];

//int syslog_flag = 0;
// timeout == ticks count

void syslog(int priority, char *title, char *fmt, ...) {
	int nbytes, nbytes1, status;
	va_list argptr;

	//dprintf("syslog is called, sock_id = %d\n", sock_id);
	if (sock_id < 0)
		return;

#if 0
	if (gSetting.syslog_ds.syslog_enable == 0) {
		//dprintf("syslog now is disabled\n");
		if (sock_id > 0) {
			if (closesocket(sock_id) == SOCKET_ERROR)
				forceclosesocket(sock_id);

			//dprintf("syslog be disabled, closesocket(%d)\n", sock_id);
			sock_id = -1;
		}
		return;
	}

	if (sock_id < 0) {
	    sock_id = socket(AF_INET, SOCK_DGRAM, 0);
	    if (sock_id < 0) return;

	    memset((char *)&SyslogSock, 0, sizeof(SyslogSock));
	    SyslogSock.sin_family	= AF_INET;
	    SyslogSock.sin_port		= htons(514); // syslog
	    SyslogSock.sin_addr.s_addr  = htonl(INADDR_ANY);
	    if (bind(sock_id, (struct sockaddr *) &SyslogSock,
						sizeof(SyslogSock)) < 0) {
		//dprintf("[syslog]bind error\n");
		return;
	    }
	}
#endif
	//while (syslog_flag == 1)
	//	YIELD();

	if(gSetting.syslog_ds.syslog_enable==0)
		return;
	//syslog_flag = 1;
	nbytes = mt_sprintf(syslog_buf, "<%d>%s %s[%lu]:", priority, gSetting.Host_Name,
		(title==NULL)?"":title, syslog_idx++);
	va_start(argptr, fmt);
	nbytes1 = mt_vsprintf(syslog_buf+nbytes, fmt, argptr);
	va_end(argptr);
	//dprintf("nbytes %d : %s\n", nbytes1, syslog_buf+nbytes);

	//SyslogSock.sin_addr.s_addr = inet_addr(gSetting.syslog_ds.syslog_ip);
	// sendto(sock_id, syslog_buf, nbytes+nbytes1, 0, (struct sockaddr *)&SyslogSock,
        //		sizeof(SyslogSock));
	status=sndpkt(SYSLOGMBX, syslog_buf, 100, nbytes+nbytes1+1, COLOR0);
	#ifdef _SYS_LOG_DEBUG
	dprintf("[syslog] sndpkt() %d return %d\n", syslog_idx-1, status);
	#endif
	//syslog_flag = 0;
}

#ifndef _OMRON
void syslog_dprintf(char *fmt, ...) {
	int nbytes, nbytes1;
	va_list argptr;
	char syslog_buf[256];

	if (sock_id < 0)
		return;

#if 0
	while (syslog_flag == 1)
		YIELD();

	syslog_flag = 1;
#endif
	nbytes = mt_sprintf(syslog_buf, "<%d>dprintf[%lu]:", LOG_SYSLOG+LOG_DEBUG, syslog_idx++);
	va_start(argptr, fmt);
	nbytes1 = mt_vsprintf(syslog_buf+nbytes, fmt, argptr);
	va_end(argptr);
#if 0
	SyslogSock.sin_addr.s_addr = inet_addr(gSetting.syslog_ds.syslog_ip);
        sendto(sock_id, syslog_buf, nbytes+nbytes1, 0, (struct sockaddr *)&SyslogSock,
        		sizeof(SyslogSock));
	syslog_flag = 0;
#endif
	sndpkt(SYSLOGMBX, syslog_buf, 100, nbytes+nbytes1+1, COLOR0);
}
#endif

void syslog_client(void) {
	char *msg_addr;
	PKT_HDR *pkt;
	int nbytes, status, i;

	sock_id = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock_id < 0) {
		dprintf("syslog client can't create socket, ret = %d\n", sock_id);
		return;
	}

	SyslogSrv_running = 1;

	#ifdef _SYS_LOG_DEBUG
	dprintf(" ===================================================================\n");
	dprintf("syslog client, sock_id %d\n", sock_id);
	#endif
	memset((char *)&SyslogSock, 0, sizeof(SyslogSock));

	SyslogSock.sin_family	= AF_INET;
	SyslogSock.sin_port	= htons(514); // syslog
	SyslogSock.sin_addr.s_addr  = htonl(INADDR_ANY);
	if (bind(sock_id, (struct sockaddr *) &SyslogSock,
					sizeof(SyslogSock)) < 0) {
		closesocket(sock_id);
		sock_id = -1;
		dprintf("[syslog] bind error\n");
		return;
	}

    for(i=0; i<20; i++){    //wait at most 1 min for the interface to become ready
	    if(get_ip_by_route(inet_addr(gSetting.syslog_ds.syslog_ip))==0){
	        //dprintf("[syslog] find no route to syslog server %s\n", gSetting.syslog_ds.syslog_ip);
		    dlytsk(cur_task, DLY_SECS, 3);
		}
		else{
		    break;
		}
	}

	while (1) {
	    //if(get_ip_by_route(inet_addr(gSetting.syslog_ds.syslog_ip))==0){
	        //dprintf("[syslog] find no route to syslog server %s\n", gSetting.syslog_ds.syslog_ip);
		//    dlytsk(cur_task, DLY_SECS, 3);
		//    continue;
		//}
	    /*
		pkt = (PKT_HDR *)rcvmsg(SYSLOGMBX | PKTRCV, CLOCKHZ);
		if(pkt==NULL){
		    dprintf("[syslog] rcvmsg timeout\n");
		    YIELD();
		    continue;
		}
		*/
		pkt = (PKT_HDR *)rcvmsg(SYSLOGMBX | PKTRCV, 0);
		msg_addr = (char *) &pkt[1];
		nbytes = strlen(msg_addr);
		#ifdef _SYS_LOG_DEBUG
		dprintf("[syslog] nbytes %d : %s\n", nbytes, msg_addr);
        #endif
        //SyslogSock.sin_family	= AF_INET;
        //SyslogSock.sin_port	= htons(514); // syslog
		SyslogSock.sin_addr.s_addr = inet_addr(gSetting.syslog_ds.syslog_ip);
		//SyslogSock.sin_addr.s_addr = inet_addr("192.168.2.100");
		#ifdef _SYS_LOG_DEBUG
		dprintf("[syslog] log server %08x\n", SyslogSock.sin_addr.s_addr);
		#endif
	    status=sendto(sock_id, msg_addr, nbytes, 0, (struct sockaddr *)&SyslogSock, sizeof(SyslogSock));
	    if(status==SOCKET_ERROR){
		    dprintf("[syslog] sendto error, sock_id=%d, error code=%d\n", sock_id, GetLastError());
		}
	    relpkt(pkt);
	}
}

#ifndef _OMRON
/********************************
IPPROTO_IP	// IP packet
IPPROTO_TCP	// TCP packet
IPPROTO_UDP	// UDP packet
IPPROTO_ICMP	// ICMP packet
GRE_PROTO	// PPTP GRE packet
AH_PROTO	// IPSec AH packet
ESP_PROTO	// IPSec ESP packet
*********************************/

char *getProtoName(int p) {
	switch (p) {
	  case IPPROTO_IP:
		return "IP";
	  case IPPROTO_TCP:
		return "TCP";
	  case IPPROTO_UDP:
		return "UDP";
	  case IPPROTO_ICMP:
		return "ICMP";
#ifdef _PPTP
	  case GRE_PROTO:
		return "PPTP GRE";
#endif // _PPTP
#ifdef _IPSEC
	  case AH_PROTO:
		return "IPSec AH";
	  case ESP_PROTO:
		return "IPSec ESP";
#endif // _IPSEC
	  default:
	  	return "Unknown Protocol";
	}
}

struct _serv_t {
	char *name;
	int port;
};

struct _serv_t serv_db[] = {
"TCPMUX"	,1,
"ECHO"		,7,
"DISCARD"	,9,
"SYSTAT"	,11,
"DAYTIME"	,13,
"NETSTAT"	,15,
"QOTD"		,17,
"MSP"		,18,
"CHARGEN"	,19,
"FTP-DATA"      ,20,
"FTP"           ,21,
"SSH"           ,22,
"TELNET"        ,23,
"SMTP"		,25,
"TIME"		,37,
"RLP"		,39,
"NAMESERVER"	,42,
"WHOIS"		,43,
"DOMAIN"	,53,
"MTP"		,57,
"BOOTPS"	,67,
"BOOTPC"	,68,
"TFTP"		,69,
"GOPHER"	,70,
"RJE"		,77,
"FINGER"	,79,
"WWW"		,80,
"LINK"		,87,
"KERBEROS"	,88,
"SUPDUP"	,95,
"HOSTNAMES"	,101,
"ISO-TSAP"	,102,
"CSNET-NS"	,105,
"RTELNET"	,107,
"POP2"		,109,
"POP3"		,110,
"SUNRPC"	,111,
"AUTH"		,113,
"SFTP"		,115,
"UUCP-PATH"	,117,
"NNTP"		,119,
"NTP"		,123,
"NETBIOS-NS"	,137,
"NETBIOS-DGM"	,138,
"NETBIOS-SSN"	,139,
"IMAP2"		,143,
"SNMP"		,161,
"SNMP-TRAP"	,162,
"CMIP-MAN"	,163,
"CMIP-AGENT"	,164,
"XDMCP"		,177,
"NEXTSTEP"	,178,
"BGP"		,179,
"PROSPERO"	,191,
"IRC"		,194,
"SMUX"		,199,
"AT-RTMP"	,201,
"AT-NBP"	,202,
"AT-ECHO"	,204,
"AT-ZIS"	,206,
"Z3950"		,210,
"IPX"		,213,
"IMAP3"		,220,
"ULISTSERV"	,372,
"EXEC"		,512,
"BIFF"		,512,
"LOGIN"		,513,
"WHO"		,513,
"SHELL"		,514,
"SYSLOG"	,514,
"PRINTER"	,515,
"TALK"		,517,
"NTALK"		,518,
"ROUTE"		,520,
"TIMED"		,525,
"TEMPO"		,526,
"COURIER"	,530,
"CONFERENCE"	,531,
"NETNEWS"	,532,
"NETWALL"	,533,
"UUCP"		,540,
"REMOTEFS"	,556,
"KLOGIN"	,543,
"KSHELL"	,544,
"KERBEROS-ADM"	,749,
"WEBSTER"	,765,
"INGRESLOCK"	,1524,
"INGRESLOCK"	,1524,
"PROSPERO-NP"	,1525,
"RFE"		,5002,
"KRBUPDATE"	,760,
"KPASSWD"	,761,
"EKLOGIN"	,2105,
"SUPFILESRV"	,871,
"SUPFILEDBG"	,1127
};

char *getServName(int s) {
	int i;
	int SizeOfService = sizeof(serv_db)/sizeof(struct _serv_t);

	for (i=0; i<SizeOfService; i++) {
		if (serv_db[i].port == s)
			return serv_db[i].name;
	}

	return "Unknown Service";
}

struct _serv_t dhcpAct_db[] = {
"DHCP Discovery",	DHCPDISCOVER,
"DHCP Offer", 		DHCPOFFER,
"DHCP Request",		DHCPREQUEST,
"DHCP Decline",		DHCPDECLINE,
"DHCP Ack",		DHCPACK,
"DHCP Nak",		DHCPNAK,
"DHCP Release",		DHCPRELEASE,
"DHCP Inform",		DHCPINFORM
};


char *getDhcpActionName(int act) {
	int i;
	int SizeOfAct = sizeof(dhcpAct_db)/sizeof(struct _serv_t);

	for (i=0; i<SizeOfAct; i++) {
		if (dhcpAct_db[i].port == act)
			return dhcpAct_db[i].name;
	}

	return "Unknown Action";
}

char HACKERLOG_FMT[] = "%d/%d/%d %d:%d:%d, %s, SIP:%s:%d, DIP:%s:%d";

void HackerLog(char *s_ip, u_short s_port, char *d_ip, u_short d_port, int proto) {
	struct tm now;
	GetCurDateTime(&now);

	syslog(LOG_DAEMON+LOG_ALERT, "Hacker Log", HACKERLOG_FMT, now.tm_year+1900,
	 now.tm_mon+1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec,
	 (proto==6)?"PROTO_TCP":"PROTO_UDP", s_ip, s_port, d_ip, d_port );
}

void HackerLog_Socket(int sd, int proto) {
	struct sockaddr_in sIpAddr;
	struct sockaddr_in dIpAddr;
	char s_ip[16], d_ip[16];
	int len = sizeof(struct sockaddr);

	memset(&sIpAddr, 0, len);
	memset(&dIpAddr, 0, len);
	if ((getpeername(sd, (struct sockaddr *) &sIpAddr, &len)) < 0) {
		return;
	}
	if ((getsockname(sd, (struct sockaddr *)&dIpAddr, &len)) < 0) {
		return;
	}

	strcpy(s_ip, inet_ntoa(sIpAddr.sin_addr));
	strcpy(d_ip, inet_ntoa(dIpAddr.sin_addr));
	HackerLog(s_ip, ntohs(sIpAddr.sin_port), d_ip, ntohs(dIpAddr.sin_port), proto);
	return;
}

static char sHex[512];
char hex_db[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a',
	'b', 'c', 'd', 'e', 'f' };
char *hexToString(char *data, int len) {
	int i, k=0;

	if (len > 255) len = 255;
	for (i=0; i<len; i++) {
		sHex[k] = hex_db[ ((data[i]>>4) & 0xf) ];
		sHex[k+1] = hex_db[ (data[i] & 0xf) ];
		k += 2;
	}

	sHex[k] = '\0';
	return sHex;
}
#endif
