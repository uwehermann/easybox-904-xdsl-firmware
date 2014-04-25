/* ulogd_PCAP.c, Version $Revision: 6016 $
 *
 * ulogd output target for writing pcap-style files (like tcpdump)
 *
 * FIXME: descr.
 * 
 *
 * (C) 2002 by Harald Welte <laforge@gnumonks.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 
 *  as published by the Free Software Foundation
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: ulogd_PCAP.c 6016 2005-09-23 13:37:46Z laforge $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pcap.h>
#include <ulogd/ulogd.h>
#include <ulogd/conffile.h>

/*
 * This is a timeval as stored in disk in a dumpfile.
 * It has to use the same types everywhere, independent of the actual
 * `struct timeval'
 */

struct pcap_timeval {
    int32_t tv_sec;		/* seconds */
    int32_t tv_usec;		/* microseconds */
};

/*
 * How a `pcap_pkthdr' is actually stored in the dumpfile.
 *
 * Do not change the format of this structure, in any way (this includes
 * changes that only affect the length of fields in this structure),
 * and do not make the time stamp anything other than seconds and
 * microseconds (e.g., seconds and nanoseconds).  Instead:
 *
 *	introduce a new structure for the new format;
 *
 *	send mail to "tcpdump-workers@tcpdump.org", requesting a new
 *	magic number for your new capture file format, and, when
 *	you get the new magic number, put it in "savefile.c";
 *
 *	use that magic number for save files with the changed record
 *	header;
 *
 *	make the code in "savefile.c" capable of reading files with
 *	the old record header as well as files with the new record header
 *	(using the magic number to determine the header format).
 *
 * Then supply the changes to "patches@tcpdump.org", so that future
 * versions of libpcap and programs that use it (such as tcpdump) will
 * be able to read your new capture file format.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

struct pcap_sf_pkthdr {
    struct pcap_timeval ts;	/* time stamp */
    uint32_t caplen;		/* length of portion present */
    uint32_t len;		/* length this packet (off wire) */
};

#ifndef ULOGD_PCAP_DEFAULT
#define ULOGD_PCAP_DEFAULT	"/var/log/ulogd.sock"
#endif

#ifndef ULOGD_PCAP_SYNC_DEFAULT
#define ULOGD_PCAP_SYNC_DEFAULT	0
#endif

#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
        ((unsigned char *)&addr)[2], \
        ((unsigned char *)&addr)[3]

static config_entry_t pcapsock_ce = { 
	.key = "sock", 
	.type = CONFIG_TYPE_STRING, 
	.options = CONFIG_OPT_NONE,
	.u = { .string = ULOGD_PCAP_DEFAULT }
};

static int					sock = -1;
static struct sockaddr_un	sock_addr;

struct intr_id {
	char* name;
	unsigned int id;		
};

#define INTR_IDS 	4
static struct intr_id intr_ids[INTR_IDS] = {
	{ "raw.pkt", 0 },
	{ "raw.pktlen", 0 },
	{ "oob.time.sec", 0 },
	{ "oob.time.usec", 0 },
};

#define GET_VALUE(x)	ulogd_keyh[intr_ids[x].id].interp->result[ulogd_keyh[intr_ids[x].id].offset].value
#define GET_FLAGS(x)	ulogd_keyh[intr_ids[x].id].interp->result[ulogd_keyh[intr_ids[x].id].offset].flags

#define ntohl(x)	((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
					(((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
#define ntohs(x)	((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

static int pcap_output(ulog_iret_t *res)
{
	int						send_len, slen, total_sent;
	char*					p;
	struct pcap_sf_pkthdr	pchdr;

	if ( sock < 0 )
		return 1;

	pchdr.len = ntohl(GET_VALUE(1).ui32);
	pchdr.caplen = pchdr.len;

	if (GET_FLAGS(2) & ULOGD_RETF_VALID
	    && GET_FLAGS(3) & ULOGD_RETF_VALID) {
		pchdr.ts.tv_sec = GET_VALUE(2).ui32;
		pchdr.ts.tv_usec = GET_VALUE(3).ui32;
	} else {
		/* use current system time */
		struct timeval tv;
		gettimeofday(&tv, NULL);

		pchdr.ts.tv_sec = tv.tv_sec;
		pchdr.ts.tv_usec = tv.tv_usec;
	}

	pchdr.ts.tv_sec = ntohl(pchdr.ts.tv_sec);
	pchdr.ts.tv_usec = ntohl(pchdr.ts.tv_usec);

	p = (char*)&pchdr;
	send_len = sizeof(pchdr);
	total_sent = 0;
	while ( total_sent < send_len )
	{
		slen = send(sock, (p+total_sent), (send_len-total_sent), 0);
		if ( slen < 0 )
			return 1;
		total_sent += slen;
	}

	p = (char*)GET_VALUE(0).ptr;
	send_len = (int)GET_VALUE(1).ui32;
	total_sent = 0;
	while ( total_sent < send_len )
	{
		slen = send(sock, (p+total_sent), (send_len-total_sent), 0);
		if ( slen < 0 )
			return 1;
		total_sent += slen;
	}

	return 0;
}

/* stolen from libpcap savefile.c */
#define LINKTYPE_RAW		101
#define	LINKTYPE_ETHERNET	1
#define TCPDUMP_MAGIC		0xa1b2c3d4

static int write_pcap_header(void)
{
	int						send_len, slen, total_sent;
	char*					p;
	struct pcap_file_header pcfh;

	if ( sock < 0 )
		return 0;

	pcfh.magic = ntohl(TCPDUMP_MAGIC);
	pcfh.version_major = ntohs(PCAP_VERSION_MAJOR);
	pcfh.version_minor = ntohs(PCAP_VERSION_MINOR);
	pcfh.thiszone = ntohl(timezone);
	pcfh.sigfigs = ntohl(0);
	pcfh.snaplen = ntohl(65535); /* we don't know the length in advance */
	pcfh.linktype = ntohl(LINKTYPE_ETHERNET);

	p = (char*)&pcfh;
	send_len = sizeof(pcfh);
	total_sent = 0;
	while ( total_sent < send_len )
	{
		slen = send(sock, (p+total_sent), (send_len-total_sent), 0);
		if ( slen < 0 )
			return 0;
		total_sent += slen;
	}

	return send_len;
}
	
/* get all key id's for the keys we are intrested in */
static int get_ids(void)
{
	int i;
	struct intr_id *cur_id;

	for (i = 0; i < INTR_IDS; i++) {
		cur_id = &intr_ids[i];
		cur_id->id = keyh_getid(cur_id->name);
		if (!cur_id->id) {
			ulogd_log(ULOGD_ERROR, 
				"Cannot resolve keyhash id for %s\n", 
				cur_id->name);
			return 1;
		}
	}	
	return 0;
}

void append_create_sock(void)
{
	//create a UNIX tcp socket for the transaction
	if ((sock=socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return;

	memset( &sock_addr, 0, sizeof(sock_addr) );
	sock_addr.sun_family = AF_UNIX;
	strcpy( sock_addr.sun_path, pcapsock_ce.u.string );
        
	if ( connect( sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr) ) < 0 )
	{
		close(sock);
		sock = -1;
		return;
	}

	if (!write_pcap_header())
	{
		close(sock);
		sock = -1;
		return;
	}
}

static void pcap_signal_handler(int signal)
{
	switch (signal) {
	case SIGHUP:
		ulogd_log(ULOGD_NOTICE, "pcap: reopening capture file\n");
		if (sock >= 0)
		{
			close(sock);
			sock = -1;
		}
		append_create_sock();
		break;
	default:
		break;
	}
}

static int pcap_init(void)
{
	/* FIXME: error handling */
	config_parse_file("PCAPSOCK", &pcapsock_ce);

	append_create_sock();

	return 0;
}

static void pcap_fini(void)
{
	if (sock >= 0)
	{
		close( sock );
		sock = -1;
	}
}

static ulog_output_t pcap_op = {
	.name = "pcap", 
	.init = &pcap_init,
	.fini = &pcap_fini,
	.output = &pcap_output,
	.signal = &pcap_signal_handler,
};

void _init(void)
{
	if (get_ids()) {
		ulogd_log(ULOGD_ERROR, "can't resolve all keyhash id's\n");
	}

	register_output(&pcap_op);
}
