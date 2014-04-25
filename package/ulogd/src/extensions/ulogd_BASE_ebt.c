/* ulogd_MAC.c, Version $Revision: 5239 $
 *
 * ulogd interpreter plugin for 
 * 	o MAC addresses
 * 	o NFMARK field
 * 	o TIME
 * 	o Interface names
 *
 * (C) 2000-2001 by Harald Welte <laforge@gnumonks.org>
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
 
 * $Id: ulogd_BASE.c 5239 2005-02-12 21:22:56Z laforge $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <ulogd/ulogd.h>
#include <linux/netfilter_bridge/ebt_ulog.h>

/***********************************************************************
 * 			Raw header
 ***********************************************************************/
static ulog_iret_t raw_rets[] = {
	{ .type = ULOGD_RET_STRING, 
	  .flags = ULOGD_RETF_FREE, 
	  .key = "raw.mac",
	},
	{ .type = ULOGD_RET_RAW,
	  .flags = ULOGD_RETF_NONE,
	  .key = "raw.pkt",
	},
	{ .type = ULOGD_RET_UINT32, 
	  .flags = ULOGD_RETF_NONE, 
	  .key = "raw.pktlen",
	},
};

static ulog_iret_t *_interp_raw(ulog_interpreter_t *mac, 
				ulog_packet_msg_t *pkt)
{
	unsigned char *p;
	int i;
	ebt_ulog_packet_msg_t* frame = (ebt_ulog_packet_msg_t*)pkt;
	char *buf, *oldbuf = NULL;
	ulog_iret_t *ret = mac->result;

	if (frame->data_len >= 12) {
		buf = (char *) malloc(2 * 3 * 6/*MAC len*/ + 1);
		if (!buf) {
			ulogd_log(ULOGD_ERROR, "OOM!!!\n");
			return NULL;
		}
		*buf = '\0';

		p = frame->data;
		oldbuf = buf;
		for (i = 0; i < 5; i++)
			sprintf(buf, "%s%02x:", oldbuf, *p++);
		sprintf(buf, "%s%02x-", oldbuf, *p++);
		for (i = 0; i < 5; i++)
			sprintf(buf, "%s%02x:", oldbuf, *p++);
		sprintf(buf, "%s%02x", oldbuf, *p++);
		ret[0].value.ptr = buf;
		ret[0].flags |= ULOGD_RETF_VALID;
	}

	/* include pointer to raw MAC packet */
	ret[1].value.ptr = frame->data;
	ret[1].flags |= ULOGD_RETF_VALID;
	ret[2].value.ui32 = frame->data_len;
	ret[2].flags |= ULOGD_RETF_VALID;

	return ret;
}

/***********************************************************************
 * 			OUT OF BAND
 ***********************************************************************/

static ulog_iret_t oob_rets[] = {
	{ .type = ULOGD_RET_STRING, 
	  .flags = ULOGD_RETF_NONE, 
	  .key = "oob.prefix",
	},
	{ .type = ULOGD_RET_UINT32, 
	  .flags = ULOGD_RETF_NONE, 
	  .key = "oob.time.sec", 
	},
	{ .type = ULOGD_RET_UINT32, 
	  .flags = ULOGD_RETF_NONE, 
	  .key = "oob.time.usec", 
	},
	{ .type = ULOGD_RET_UINT32, 
	  .flags = ULOGD_RETF_NONE, 
	  .key = "oob.mark", 
	},
	{ .type = ULOGD_RET_STRING, 
	  .flags = ULOGD_RETF_NONE, 
	  .key = "oob.in", 
	}, 
	{ .type = ULOGD_RET_STRING, 
	  .flags = ULOGD_RETF_NONE, 
	  .key = "oob.out", 
	},
	{ .type = ULOGD_RET_UINT32, 
	  .flags = ULOGD_RETF_NONE, 
	  .key = "oob.version",
	},
	{ .type = ULOGD_RET_STRING, 
	  .flags = ULOGD_RETF_NONE, 
	  .key = "oob.physin", 
	}, 
	{ .type = ULOGD_RET_STRING, 
	  .flags = ULOGD_RETF_NONE, 
	  .key = "oob.physout", 
	},
};

static ulog_iret_t *_interp_oob(struct ulog_interpreter *mac, 
				ulog_packet_msg_t *pkt)
{
	ebt_ulog_packet_msg_t* frame = (ebt_ulog_packet_msg_t*)pkt;
	ulog_iret_t *ret = mac->result;

	ret[0].value.ptr = frame->prefix;
	ret[0].flags |= ULOGD_RETF_VALID;

	if (frame->stamp.tv_sec) {
		ret[1].value.ui32 = frame->stamp.tv_sec;
		ret[1].flags |= ULOGD_RETF_VALID;
		ret[2].value.ui32 = frame->stamp.tv_usec;
		ret[2].flags |= ULOGD_RETF_VALID;
	} else {
		ret[1].flags &= ~ULOGD_RETF_VALID;
		ret[2].flags &= ~ULOGD_RETF_VALID;
	}

	ret[3].value.ui32 = frame->mark;
	ret[3].flags |= ULOGD_RETF_VALID;
	ret[4].value.ptr = frame->indev;
	ret[4].flags |= ULOGD_RETF_VALID;
	ret[5].value.ptr = frame->outdev;
	ret[5].flags |= ULOGD_RETF_VALID;
	
	ret[6].value.ui32 = frame->version;
	ret[6].flags |= ULOGD_RETF_VALID;
	ret[7].value.ptr = frame->physindev;
	ret[7].flags |= ULOGD_RETF_VALID;
	ret[8].value.ptr = frame->physoutdev;
	ret[8].flags |= ULOGD_RETF_VALID;

	return ret;
}


static ulog_interpreter_t base_mac[] = {
	{ .name = "raw", 
	  .interp = &_interp_raw, 
	  .key_num = 3, 
	  .result = raw_rets },
	{ .name = "oob", 
	  .interp = &_interp_oob, 
	  .key_num = 9, 
	  .result = oob_rets },
	{ NULL, "", 0, NULL, 0, NULL }, 
};

void _base_reg_mac(void)
{
	ulog_interpreter_t *mac = base_mac;
	ulog_interpreter_t *p;

	for (p = mac; p->interp; p++) {
		register_interpreter(p);
	}
}

void _init(void)
{
	_base_reg_mac();
}
