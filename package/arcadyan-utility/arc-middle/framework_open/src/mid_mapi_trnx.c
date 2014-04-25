/*
 * Arcadyan middle layer IPC library (for Middle APIs)
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <stdio.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include "mid_sock.h"
#include "mid_mapi_trnx.h"

int msg_counter=0;

/*
 * -- Helper functions --
 */

/*
 * -- Public functions --
 */

int mapi_set_trnx_msghd(mid_msg_t *pmsg, int mid, unsigned short act_code, int mlen)
{
    if(pmsg==NULL){
        MID_TRACE("[%s] NULL pmsg\n", __FUNCTION__);
        return MID_FAIL;
    }
    
    pmsg->header.mid=mid;
    pmsg->header.act_rsp_code=act_code;
    pmsg->header.mlen=mlen;
    
    return MID_SUCCESS;
}

int mapi_start_transc(void)
{
    int                 sock;
    struct  sockaddr_un clntaddr;   //address of client
    struct  sockaddr_un servaddr;   //address of server
    mapi_transc_t       *pt;

    pt=(mapi_transc_t *)malloc(sizeof(mapi_transc_t));
    if(pt==NULL){
        MID_TRACE("[%s] mapi_transc_t allocation fail\n", __FUNCTION__);
        
        return MID_FAIL;
    }
    
    //create a UNIX tcp socket for the transaction
    if((sock=socket(AF_UNIX, SOCK_STREAM, 0))<0){
        perror("socket");
        
        return MID_FAIL;
    }
    pt->sock=sock;
    
    memset(&clntaddr, 0, sizeof(clntaddr));
    clntaddr.sun_family=AF_UNIX;
    sprintf(clntaddr.sun_path, "%s_%d_%08x", MID_CLIENT_PATH, getpid(), sock);
    unlink(clntaddr.sun_path);
        
    if(bind(sock, (struct sockaddr *)&clntaddr, sizeof(clntaddr))<0){
        close(sock);
        perror("bind");
        
        return MID_FAIL;
    }
    
    MID_TRACE("[%s] bind at %s\n", __FUNCTION__, clntaddr.sun_path);

    //connect to server
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family=AF_UNIX;
    strcpy(servaddr.sun_path, MID_CORE_TRNX_PATH);
    
    if(connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
        close(sock);
        perror("connect");
        unlink(clntaddr.sun_path);        
        
        return MID_FAIL;        
    }
    
    return (int)pt;
}

int mapi_tx_transc_req(int tid, mid_msg_t *pmsg)
{
    int                 send_len, slen, total_sent;
    char                *p;
    mapi_transc_t       *pt;
    
    pt=(mapi_transc_t *)tid;
    if(pt==NULL || pmsg==NULL){
        MID_TRACE("[%s] NULL pt or pmsg\n", __FUNCTION__);
        
        return MID_FAIL;
    }
    if(pmsg->header.mlen>MID_MSG_MAX_LEN){
        MID_TRACE("[%s] msg is too long, max=%d\n", __FUNCTION__, MID_MSG_MAX_LEN);
        
        return MID_FAIL;        
    }

    //set msg pid and rid
    //NOTICE: the caller of this function should take care of the content of pmsg->mid, pmsg->act_rsp_code, and pmsg->data
    pmsg->header.pid=getpid();
    pmsg->header.rid=msg_counter++;
    
    //send msg header first
    p=(char *)pmsg;
    send_len=sizeof(mid_msg_header_t);
    total_sent=0;
    while(total_sent<send_len){
        slen=send(pt->sock, (p+total_sent), (send_len-total_sent), 0);
        if(slen<0){
            MID_TRACE("[%s] write msg length fail\n", __FUNCTION__);
            
            return MID_FAIL;
        }
        total_sent+=slen;
    }
    
    //send data
    p=((char *)pmsg)+sizeof(mid_msg_header_t);
    send_len=pmsg->header.mlen-sizeof(mid_msg_header_t);
    total_sent=0;
    while(total_sent<send_len){
        slen=send(pt->sock, (p+total_sent), (send_len-total_sent), 0);
        if(slen<0){
            MID_TRACE("[%s] write fail\n", __FUNCTION__);
            
            return MID_FAIL;
        }
        total_sent+=slen;
    }
#if 0
    memcpy(&(pt->last_msg_hd), &(pmsg->header), sizeof(mid_msg_header_t));
#endif

    return sizeof(mid_msg_header_t)+total_sent;
}

int mapi_rx_transc_rsp(int tid, mid_msg_t *pmsg, int max_len)
{
    int                 rlen, total_recv, expect_len, msg_truncated=0;
    char                *p;
    mapi_transc_t       *pt;
    
    pt=(mapi_transc_t *)tid;
    if(pt==NULL || pmsg==NULL){
        MID_TRACE("[%s] NULL pt or pmsg\n", __FUNCTION__);
        
        return MID_FAIL;
    }

    //recv msg header first
    p=(char *)pmsg;
    expect_len=sizeof(mid_msg_header_t);
    total_recv=0;
    while(total_recv<expect_len){
        rlen=recv(pt->sock, (p+total_recv), (expect_len-total_recv), 0);
        if(rlen<=0){
            if(rlen<0)   MID_TRACE("[%s] read msg length fail\n", __FUNCTION__);
            return MID_FAIL;
        }
        total_recv+=rlen;
    }

    expect_len=pmsg->header.mlen-sizeof(mid_msg_header_t);
    //MID_TRACE("[%s] %d bytes data expected\n", __FUNCTION__, expect_len);
    if(expect_len>(max_len-sizeof(mid_msg_header_t))){
        MID_TRACE("[%s] expected data too long\n", __FUNCTION__);
#if 0
        return MID_FAIL;      
#else
        if((p=malloc(expect_len))==NULL){
            MID_TRACE("[%s] expected data too long, malloc() fail\n", __FUNCTION__);
            
            return MID_FAIL;
        }
        msg_truncated=1;
#endif
    }
    else{
        p=((char *)pmsg)+sizeof(mid_msg_header_t);
    }

    //recv msg
    total_recv=0;
    while(total_recv<expect_len){
        rlen=recv(pt->sock, (p+total_recv), (expect_len-total_recv), 0);
        if(rlen<=0){
            if(rlen<0)   MID_TRACE("[%s] read fail\n", __FUNCTION__);

            return MID_FAIL;
        }
        total_recv+=rlen;
    }

    if(msg_truncated){
        memcpy(((char *)pmsg)+sizeof(mid_msg_header_t), p, (max_len-sizeof(mid_msg_header_t)));
        pmsg->header.mlen=max_len;
        pmsg->header.act_rsp_code|=MID_MSG_RSP_TRUNCATED;                
        free(p);
    }

#if 0
    //verify the response
    if(pmsg->header.mid!=pt->last_msg_hd.mid || pmsg->header.pid!=pt->last_msg_hd.pid || pmsg->header.rid!=pt->last_msg_hd.rid){
        MID_TRACE("[%s] get unexpected response\n", __FUNCTION__);
        return MID_FAIL;
    }
#endif

    return pmsg->header.mlen;
}

int mapi_end_transc(int tid)
{
    mapi_transc_t       *pt;
    char                clntpath[256];
    
    pt=(mapi_transc_t *)tid;
    if(pt==NULL){
        MID_TRACE("[%s] NULL pt\n", __FUNCTION__);
        return MID_FAIL;
    }
    
    close(pt->sock);
    
    sprintf(clntpath, "%s_%d_%08x", MID_CLIENT_PATH, getpid(), pt->sock);
    unlink(clntpath);
    
    free(pt);
    
    return MID_SUCCESS;
}


int mapi_trx_transc2(int tid, mid_msg_t *pmsg, int max_len)
{
	int		len;

    if (pmsg == NULL) {
        MID_TRACE("[%s] pmsg is NULL\n", __FUNCTION__);
        return MID_FAIL;
    }

    if (max_len < sizeof(mid_msg_t)) {
        MID_TRACE("[%s] max_len %d too small\n", __FUNCTION__, max_len);
        return MID_FAIL;
    }

	len = mapi_tx_transc_req( tid, pmsg );
	if (len != MID_MSG_LEN(pmsg)) {
		MID_TRACE( "[%s] mapi_tx_transc_req() send %d, sent %d\n", __FUNCTION__, MID_MSG_LEN(pmsg), len );
		return MID_FAIL;
	}

	len = mapi_rx_transc_rsp( tid, pmsg, max_len );
	if (len <= 0) {
		MID_TRACE( "[%s] mapi_rx_transc_rsp() return %d\n", __FUNCTION__, len );
		return MID_FAIL;
	}

    return MID_SUCCESS;
}

#define	DEFAULT_DATA_SZ		644

// pData MUST be writable if dataSz is greater than 0
int mapi_trx_transc(int tid, int mid, unsigned short act_code, void *pData, int dataSz)
{
	char		buf[ sizeof(mid_msg_t)+DEFAULT_DATA_SZ ];
	mid_msg_t*	msg;
	void*		pNew;
	int			max_len;

	// check parameters
	if (tid == MID_FAIL) {
        MID_TRACE("[%s] incorrect tid\n", __FUNCTION__);
		return MID_FAIL;
	}
	if (pData == NULL)
		dataSz = 0;
	if (dataSz < 0) {
        MID_TRACE("[%s] dataSz too small\n", __FUNCTION__);
		return MID_FAIL;
	}

	// allocate memory if data size is over buffer size
	max_len = sizeof(mid_msg_t) + dataSz;

	if (dataSz > DEFAULT_DATA_SZ) {
		pNew = malloc( max_len );
		if (pNew == NULL) {
			MID_TRACE("[%s] malloc() failed, size %d\n", __FUNCTION__, dataSz);
			return MID_FAIL;
		}
		msg = (mid_msg_t*)pNew;
	} else {
		pNew = NULL;
		msg = (mid_msg_t*)buf;
	}

	// initialize message structure
	mapi_set_trnx_msghd( msg, mid, act_code, max_len );

	if (dataSz > 0)
		memcpy( MID_MSG_DAT(msg), pData, dataSz );

	// send request and get response
	if ( mapi_trx_transc2( tid, msg, max_len ) != MID_SUCCESS ) {
		if ( pNew != NULL)
			free( pNew );
        MID_TRACE("[%s] mapi_trx_transc2() failed\n", __FUNCTION__);
		return MID_FAIL;
	}

	// copy response
	if ( pNew != NULL)
		free( pNew );

	if (dataSz > 0)
		memcpy( pData, MID_MSG_DAT(msg), dataSz );

	return MID_SUCCESS;
}

// pData MUST be writable if dataSz is greater than 0
int mapi_one_transc(int mid, unsigned short act_code, void *pData, int dataSz)
{
	int		tid;

	// start transaction
	if ( (tid=mapi_start_transc()) == MID_FAIL ) {
        MID_TRACE("[%s] mapi_start_transc() failed\n", __FUNCTION__);
		return MID_FAIL;
	}

	// send request and get response
	if ( mapi_trx_transc( tid, mid, act_code, pData, dataSz ) != MID_SUCCESS ) {
		mapi_end_transc( tid );
        MID_TRACE("[%s] mapi_trx_transc() failed\n", __FUNCTION__);
		return MID_FAIL;
	}

	// end transaction
	mapi_end_transc( tid );

	return MID_SUCCESS;
}
