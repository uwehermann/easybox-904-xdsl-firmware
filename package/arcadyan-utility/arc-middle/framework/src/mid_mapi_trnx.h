/*
 * Arcadyan middle layer transaction header file (for Middle APIs)
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _mid_mapi_trnx_h_
#define _mid_mapi_trnx_h_

#include "mid_fmwk.h"

#define MID_TCP_TRANSC		0
#define MID_UDP_TRANSC		1
typedef struct mapi_transc {
	int                     sock;
	int                     type;	//0:tcp, 1:udp
	//mid_msg_header_t        last_msg_hd;
} mapi_transc_t;

typedef enum {
	MID_MSG_MOD_DSL = 2,
	MID_MSG_MOD_SWITCH = 3,
	MID_MSG_MOD_QOS = 4,
	MID_MSG_MOD_VOIP = 5,
	MID_MSG_MOD_SYS = 6,
	MID_MSG_MOD_WLAN = 7,
	MID_MSG_MOD_USB = 8,
	MID_MSG_MOD_LTE = 9,
	MID_MSG_MOD_MON = 10,
	MID_MSG_MOD_ASM = 11,
	MID_MSG_MOD_IPTV = 13
} mid_msg_mid_t;

extern int mapi_set_trnx_msghd(mid_msg_t *pmsg, int mid, unsigned short act_code, int mlen);
extern int mapi_start_transc(void);
extern int mapi_start_transc_u(void);
extern int mapi_tx_transc_req(int tid, mid_msg_t *pmsg);
extern int mapi_rx_transc_rsp(int tid, mid_msg_t *pmsg, int max_len);

extern void* mapi_rx_transc_whole_section_rsp(int tid, mid_msg_t *msg);

extern int mapi_end_transc(int tid);

extern int mapi_trx_transc(int tid, int mid, unsigned short act_code, void *pData, int dataSz);
extern int mapi_trx_transc2(int tid, mid_msg_t *pmsg, int max_len);
extern int mapi_one_transc(int mid, unsigned short act_code, void *pData, int dataSz);

#endif // _mid_mapi_trnx_h_
