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

typedef struct mapi_transc {
	int                     sock;
	//mid_msg_header_t        last_msg_hd;
} mapi_transc_t;

extern int mapi_set_trnx_msghd(mid_msg_t *pmsg, int mid, unsigned short act_code, int mlen);
extern int mapi_start_transc(void);
extern int mapi_tx_transc_req(int tid, mid_msg_t *pmsg);
extern int mapi_rx_transc_rsp(int tid, mid_msg_t *pmsg, int max_len);
extern int mapi_end_transc(int tid);

extern int mapi_trx_transc(int tid, int mid, unsigned short act_code, void *pData, int dataSz);
extern int mapi_trx_transc2(int tid, mid_msg_t *pmsg, int max_len);
extern int mapi_one_transc(int mid, unsigned short act_code, void *pData, int dataSz);

#endif // _mid_mapi_trnx_h_
