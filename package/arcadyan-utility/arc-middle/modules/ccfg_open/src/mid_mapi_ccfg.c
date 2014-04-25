/*
 * Arcadyan middle layer APIs for common configuration
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/param.h>
#include <libArcComApi.h>
#include "mid_fmwk.h"
#include "mid_mapi_trnx.h"
#include "mid_mapi_ccfg.h"

#define CCFG_MSG_BUF_LEN    ((sizeof(mid_msg_t)+sizeof(mapi_ccfg_tuple_t)+sizeof(int))/sizeof(int))


#define		MAPI_CCFG_IDX_NAME_FMT_MAXLEN		32
#define		MAPI_CCFG_IDX_NAME_PREFIX			0			/* "%s*%u*" or "%s" */
#define		MAPI_CCFG_IDX_INDEX_PREFIX			1			/* "*%u*%s" or "%u" */
															/* %u - index, %s name */

#if 0
 #define	MAPI_CCFG_IDX_NAME_PATTERN_FMT		"%s%u"
 #define	MAPI_CCFG_IDX_NAME_PATTERN_TYPE		MAPI_CCFG_IDX_NAME_PREFIX
#else
 #define	MAPI_CCFG_IDX_NAME_PATTERN_FMT		"R%u_%s"
 #define	MAPI_CCFG_IDX_NAME_PATTERN_TYPE		MAPI_CCFG_IDX_INDEX_PREFIX
#endif

static char	_namePatternFmt[MAPI_CCFG_IDX_NAME_FMT_MAXLEN] = MAPI_CCFG_IDX_NAME_PATTERN_FMT;
static int	_namePatternType = MAPI_CCFG_IDX_NAME_PATTERN_TYPE; 

#define	MAPI_CCFG_IDX_PREPROC( key, name, idx, ret )							\
				char	key[ MID_CCFG_MAX_PARAM_NAME_LEN ];						\
				if (name == NULL)												\
					return ret;													\
				if ( _namePatternType == MAPI_CCFG_IDX_NAME_PREFIX )			\
					snprintf( key, sizeof(key), _namePatternFmt, name, idx );	\
				  else															\
					snprintf( key, sizeof(key), _namePatternFmt, idx, name );

static char _sNull[] = "";

/*
 * -- Helper functions --
 */

/*
 * -- Public functions --
 */
 
// Please refer to mid_mapi_ccfg.h for the usage of the input "mapi_ccfg_tuple_t* ptuple"
// in the below functions


/************************************************************/
/******************** CORE FUNCTIONS ************************/
/************************************************************/


/*******************************************************************************
 * Description
 *		Get a variable data from common configuration database
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		ptuple:	pointer to the buffer of data structure mapi_ccfg_tuple_t.
 *				Fields ptuple->section, ptuple->name and ptuple->type are required.
 *				Field ptuple->value will be filled with variable data.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or the variable does not exist
 *							or ptuple is NULL.
 *
 * Note
 *		* ptuple->type is one of MID_CCFG_INT8, MID_CCFG_UINT8, MID_CCFG_INT16, MID_CCFG_UINT16,
 *			MID_CCFG_INT32, MID_CCFG_UINT32, MID_CCFG_INET4, MID_CCFG_INET6 and MID_CCFG_STR.
 *			If it is unknown type, this function views it as MID_CCFG_STR.
 *
 * See Also
 *		mapi_ccfg_set, mapi_ccfg_del, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_get(int tid, mapi_ccfg_tuple_t* ptuple)
{
    int                 buf[CCFG_MSG_BUF_LEN];
    mid_msg_t           *msg;
    int                 type, slen, rlen;
    char                val[MID_CCFG_MAX_VALUE_LEN];

    if(ptuple==NULL){
        MID_TRACE("[%s] NULL ptuple\n", __FUNCTION__);
        
        return MID_FAIL;
    }

    type=ptuple->type;
    
    //set msg header
    msg=(mid_msg_t *)buf;
    mapi_set_trnx_msghd(msg, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_GET, sizeof(mid_msg_header_t)+sizeof(mapi_ccfg_tuple_t));
    //set msg data, may be a data structure for the handler's convenience to parse it
    memcpy(&(msg->data[0]), ptuple, sizeof(mapi_ccfg_tuple_t));
    
	((mapi_ccfg_tuple_t*)msg->data)->type = MID_CCFG_GET_THIS;
    //send the request
    slen=mapi_tx_transc_req(tid, msg);
    if(slen!=msg->header.mlen){
        MID_TRACE("[%s] mapi_tx_transc_req() to send %d, sent %d\n", __FUNCTION__, msg->header.mlen, slen);
        
        return MID_FAIL;
    }
    
    //get response
    memset(buf, 0, sizeof(mid_msg_header_t)+sizeof(mapi_ccfg_tuple_t));
    rlen=mapi_rx_transc_rsp(tid, msg, sizeof(buf));
    if(rlen<=0){
        MID_TRACE("[%s] mapi_rx_transc_rsp() return %d\n", __FUNCTION__, rlen);
        
        return MID_FAIL;
    }
    
    //parse content
    memcpy((char *)ptuple, &(msg->data[0]), sizeof(mapi_ccfg_tuple_t));
    MID_TRACE("[%s] %s@%s=%s\n", __FUNCTION__, ptuple->name, ptuple->section, ptuple->value);
    if((msg->header.act_rsp_code&MID_MSG_RSP_MASK)!=MID_MSG_RSP_SUCCESS){
        MID_TRACE("[%s] fail, rsp code=%u\n", __FUNCTION__, msg->header.act_rsp_code);
        
        return MID_FAIL;
    }
    
    //convert the value[] field according to type
    if(type!=MID_CCFG_STR){
        strcpy(val, (char*)ptuple->value);
        memset(ptuple->value, 0, MID_CCFG_MAX_VALUE_LEN);
        switch(type){
	        case MID_CCFG_INT8:
	            *(char*)(ptuple->value)=atoi(val);
	            break;
        
	        case MID_CCFG_UINT8:
	            *(unsigned char*)(ptuple->value)=strtoul(val, NULL, 0);
	            break;
        
	        case MID_CCFG_INT16:
	            *(short*)(ptuple->value)=atoi(val);
	            break;
        
	        case MID_CCFG_UINT16:
	            *(unsigned short*)(ptuple->value)=strtoul(val, NULL, 0);
	            break;
        
	        case MID_CCFG_INT32:
	            *(int32_t*)(ptuple->value)=atol(val);
	            break;
        
            case MID_CCFG_UINT32:
	            *(uint32_t*)(ptuple->value)=strtoul(val, NULL, 0);
	            break;
        
            case MID_CCFG_INET4:
                //notice: the parameter value should point to a well-prepared buffer of struct in_addr
                if(inet_pton(AF_INET, val, ptuple->value)<=0)
                    msg->header.act_rsp_code=MID_MSG_RSP_FAIL_UNSPECIFIED;
                break;
        
            case MID_CCFG_INET6:
                //notice: the parameter value should point to a well-prepared buffer of struct in6_addr
                if(inet_pton(AF_INET6, val, ptuple->value)<=0)
                    msg->header.act_rsp_code=MID_MSG_RSP_FAIL_UNSPECIFIED;
                break;                
                
            default:
                MID_TRACE("[%s] unknow type\n", __FUNCTION__);
                strcpy((char*)ptuple->value, val);
                break;
        }
    }        
    
    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Set a variable data into common configuration database
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		ptuple:	pointer to the buffer of data structure mapi_ccfg_tuple_t.
 *				Fields ptuple->section, ptuple->name, ptuple->type and ptuple->value
 *				are all required.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or ptuple is NULL.
 *
 * Note
 *		* If the variable does not exist, this function will add it.
 *		* ptuple->type is one of MID_CCFG_INT8, MID_CCFG_UINT8, MID_CCFG_INT16, MID_CCFG_UINT16,
 *			MID_CCFG_INT32, MID_CCFG_UINT32, MID_CCFG_INET4, MID_CCFG_INET6 and MID_CCFG_STR.
 *			If it is unknown type, this function views it as MID_CCFG_STR.
 *		* Please use mapi_ccfg_commit() to permanently commit variables.
 *
 * See Also
 *		mapi_ccfg_get, mapi_ccfg_del, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_set(int tid, mapi_ccfg_tuple_t* ptuple)
{
    int                 buf[CCFG_MSG_BUF_LEN];
    mid_msg_t           *msg;
    int                 slen, rlen;
    char                val[MID_CCFG_MAX_VALUE_LEN];
        
    if(ptuple==NULL){
        MID_TRACE("[%s] ptuple\n", __FUNCTION__);
        
        return MID_FAIL;
    }
    //convert the value[] field according to type
    if(ptuple->type!=MID_CCFG_STR){
        strcpy(val, (char*)ptuple->value);
        memset(ptuple->value, 0, MID_CCFG_MAX_VALUE_LEN);
        switch(ptuple->type){
	        case MID_CCFG_INT8:
	            snprintf((char*)ptuple->value, MID_CCFG_MAX_VALUE_LEN, "%d", *(char *)val);
	            break;
        
	        case MID_CCFG_UINT8:
	            snprintf((char*)ptuple->value, MID_CCFG_MAX_VALUE_LEN, "%u", *(unsigned char *)val);
	            break;
        
	        case MID_CCFG_INT16:
	            snprintf((char*)ptuple->value, MID_CCFG_MAX_VALUE_LEN, "%d", *(short *)val);
	            break;
        
	        case MID_CCFG_UINT16:
	            snprintf((char*)ptuple->value, MID_CCFG_MAX_VALUE_LEN, "%u", *(unsigned short *)val);
	            break;
        
	        case MID_CCFG_INT32:
	            snprintf((char*)ptuple->value, MID_CCFG_MAX_VALUE_LEN, "%d", *(int32_t *)val);
	            break;
        
            case MID_CCFG_UINT32:
	            snprintf((char*)ptuple->value, MID_CCFG_MAX_VALUE_LEN, "%u", *(uint32_t *)val);
	            break;
        
            case MID_CCFG_INET4:
                if(inet_ntop(AF_INET, val, (char*)ptuple->value, INET_ADDRSTRLEN)==NULL){
                    MID_TRACE("[%s] inet_ntop(AF_INET) fail\n", __FUNCTION__);
                    
                    return MID_FAIL;
                }
                break;
        
            case MID_CCFG_INET6:
                if(inet_ntop(AF_INET6, val, (char*)ptuple->value, INET6_ADDRSTRLEN)==NULL){
                    MID_TRACE("[%s] inet_ntop(AF_INET6) fail\n", __FUNCTION__);
                    
                    return MID_FAIL;
                }
                break;
                
            default:
                MID_TRACE("[%s] unknow type\n", __FUNCTION__);
                strcpy((char*)ptuple->value, val);
                break;
        }
    }
    
    //set msg header
    msg=(mid_msg_t *)buf;
    mapi_set_trnx_msghd(msg, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_SET, sizeof(mid_msg_header_t)+sizeof(mapi_ccfg_tuple_t));
    //set msg data, may be a data structure for the handler's convenience to parse it
    memcpy(&(msg->data[0]), ptuple, sizeof(mapi_ccfg_tuple_t));
    
    //send the request
    slen=mapi_tx_transc_req(tid, msg);
    if(slen!=msg->header.mlen){
        MID_TRACE("[%s] mapi_tx_transc_req() to send %d, sent %d\n", __FUNCTION__, msg->header.mlen, slen);
        return MID_FAIL;
    }
    
    //get response
    memset(buf, 0, sizeof(mid_msg_header_t)+sizeof(mapi_ccfg_tuple_t));
    rlen=mapi_rx_transc_rsp(tid, msg, sizeof(buf));
    if(rlen<=0){
        MID_TRACE("[%s] mapi_rx_transc_rsp() return %d\n", __FUNCTION__, rlen);
        return MID_FAIL;
    }
    
    //parse content
    memcpy((char *)ptuple, &(msg->data[0]), sizeof(mapi_ccfg_tuple_t));
    if(msg->header.act_rsp_code!=MID_CCFG_RSP_SUCCESS){
        MID_TRACE("[%s] fail, rsp code=%u\n", __FUNCTION__, msg->header.act_rsp_code);
        return MID_FAIL;
    }
    
    return MID_SUCCESS;   
}

/*******************************************************************************
 * Description
 *		Remove a variable data from common configuration database
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		ptuple:	pointer to the buffer of data structure mapi_ccfg_tuple_t.
 *				Fields ptuple->section and ptuple->name are required.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or variable does not exist or ptuple is NULL.
 *
 * Note
 *		* Please use mapi_ccfg_commit() to permanently commit variables.
 *
 * See Also
 *		mapi_ccfg_get, mapi_ccfg_set, mapi_ccfg_commit, mapi_ccfg_del_multi, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_del(int tid, mapi_ccfg_tuple_t* ptuple)
{
    int                 buf[CCFG_MSG_BUF_LEN];
    mid_msg_t           *msg;
    int                 slen, rlen;

    if(ptuple==NULL){
        MID_TRACE("[%s] NULL ptuple\n", __FUNCTION__);
        
        return MID_FAIL;
    }

    //set msg header
    msg=(mid_msg_t *)buf;
    mapi_set_trnx_msghd(msg, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_DEL, sizeof(mid_msg_header_t)+sizeof(mapi_ccfg_tuple_t));
    //set msg data, may be a data structure for the handler's convenience to parse it
    memcpy(&(msg->data[0]), ptuple, sizeof(mapi_ccfg_tuple_t));
    
    //send the request
    slen=mapi_tx_transc_req(tid, msg);
    if(slen!=msg->header.mlen){
        MID_TRACE("[%s] mapi_tx_transc_req() to send %d, sent %d\n", __FUNCTION__, msg->header.mlen, slen);
        
        return MID_FAIL;
    }
    
    //get response
    memset(buf, 0, sizeof(mid_msg_header_t)+sizeof(mapi_ccfg_tuple_t));
    rlen=mapi_rx_transc_rsp(tid, msg, sizeof(buf));
    if(rlen<=0){
        MID_TRACE("[%s] mapi_rx_transc_rsp() return %d\n", __FUNCTION__, rlen);
        
        return MID_FAIL;
    }
    
    //parse content
    //memcpy((char *)ptuple, &(msg->data[0]), sizeof(mapi_ccfg_tuple_t));
    //MID_TRACE("[%s] %s@%s=%s\n", __FUNCTION__, ptuple->name, ptuple->section, ptuple->value);
    if((msg->header.act_rsp_code&MID_MSG_RSP_MASK)!=MID_MSG_RSP_SUCCESS){
        MID_TRACE("[%s] fail, rsp code=%u\n", __FUNCTION__, msg->header.act_rsp_code);
        
        return MID_FAIL;
    }
    
    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Send commission request of some variable data in common configuration database
 *		to configuration file and/or flash
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *		commit_mask:	what kinds of committing variables; bitwise-OR values
 *							MID_CCFG_COMMIT_FILE (0x1)	- commit variables to file
 *							MID_CCFG_COMMIT_FLASH (0x2)	- commit variables to flash
 *							MID_CCFG_COMMIT_SYNC (0x4)	- synchronize to each service configuration file
 *		sSect:			which section to synchronize. It can be NULL if all sections are going to synchronize.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to send request.
 *
 * See Also
 *		mapi_ccfg_commit_recv, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_commit_send(int tid, char commit_mask, char* sSect)
{
    int                 buf[CCFG_MSG_BUF_LEN];
    mid_msg_t           *msg;
    int                 slen;
	mapi_ccfg_tuple_t	stTuple;

    //set msg header
    msg = (mid_msg_t *)buf;
    mapi_set_trnx_msghd(msg, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_COMMIT, sizeof(mid_msg_header_t)+sizeof(stTuple));
    //set msg data
	stTuple.value[0] = stTuple.value[ sizeof(stTuple.value) - 1 ] = '\0';
	if (sSect)
		strncpy( (char*)stTuple.value, sSect, sizeof(stTuple.value)-1 );
	stTuple.type = commit_mask;
    memcpy( msg->data, &stTuple, sizeof(stTuple) );
    //set msg length
    
    //send the request
    slen = mapi_tx_transc_req(tid, msg);
    if (slen != msg->header.mlen) {
        MID_TRACE("[%s] mapi_tx_transc_req() to send %d, sent %d\n", __FUNCTION__, msg->header.mlen, slen);
        
        return MID_FAIL;
    }

    return MID_SUCCESS;   
}

/*******************************************************************************
 * Description
 *		Receive commission response of some variable data in common configuration database
 *		to configuration file and/or flash
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to receive response.
 *
 * See Also
 *		mapi_ccfg_commit_send, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_commit_recv(int tid)
{
    int                 buf[CCFG_MSG_BUF_LEN];
    mid_msg_t           *msg;
    int                 rlen;
    //unsigned short      *response_code;
    
    //get response
    memset(buf, 0, sizeof(mid_msg_header_t));
    msg = (mid_msg_t *)buf;
    rlen=mapi_rx_transc_rsp(tid, msg, sizeof(buf));
    if(rlen<=0){
        MID_TRACE("[%s] mapi_rx_transc_rsp() return %d\n", __FUNCTION__, rlen);
        
        return MID_FAIL;
    }
    
    //parse content
    //response_code=(unsigned short *)&(msg->data[0]);
    if(msg->header.act_rsp_code!=MID_CCFG_RSP_SUCCESS){
        MID_TRACE("[%s] fail, rsp code=%u\n", __FUNCTION__, msg->header.act_rsp_code);
        
        return MID_FAIL;
    }
    
    return MID_SUCCESS;   
}

/*******************************************************************************
 * Description
 *		Commit some variable data in common configuration database
 *		to configuration file and/or flash
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *		commit_mask:	what kinds of committing variables; bitwise-OR values
 *							MID_CCFG_COMMIT_FILE (0x1)	- commit variables to file
 *							MID_CCFG_COMMIT_FLASH (0x2)	- commit variables to flash
 *							MID_CCFG_COMMIT_SYNC (0x4)	- synchronize to each service configuration file
 *		sSect:			which section to synchronize. It can be NULL if all sections are going to synchronize.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or ptuple is NULL.
 *
 * See Also
 *		mapi_ccfg_set, mapi_ccfg_del, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_commit(int tid, char commit_mask, char* sSect)
{
	if ( mapi_ccfg_commit_send( tid, commit_mask, sSect ) == MID_FAIL )
		return MID_FAIL;

	return mapi_ccfg_commit_recv( tid );   
}

/*******************************************************************************
 * Description
 *		Send lazy-commission request of some variable data in common configuration database
 *		to configuration file and/or flash.
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *		commit_mask:	what kinds of committing variables; bitwise-OR values
 *							MID_CCFG_COMMIT_FILE (0x1)	- commit variables to file
 *							MID_CCFG_COMMIT_FLASH (0x2)	- commit variables to flash
 *							MID_CCFG_COMMIT_SYNC (0x4)	- synchronize to each service configuration file
 *		sSect:			which section to synchronize. It can be NULL if all sections are going to synchronize.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to send request.
 *
 * See Also
 *		mapi_ccfg_commit_recv, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_commit_lazy_send(int tid, char commit_mask, char* sSect)
{
    int                 buf[CCFG_MSG_BUF_LEN];
    mid_msg_t           *msg;
    int                 slen;
	mapi_ccfg_tuple_t	stTuple;

    //set msg header
    msg = (mid_msg_t *)buf;
    mapi_set_trnx_msghd(msg, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_COMMIT_LAZY, sizeof(mid_msg_header_t)+sizeof(stTuple));
    //set msg data
	stTuple.value[0] = stTuple.value[ sizeof(stTuple.value) - 1 ] = '\0';
	if (sSect)
		strncpy( (char*)stTuple.value, sSect, sizeof(stTuple.value)-1 );
	stTuple.type = commit_mask;
    memcpy( msg->data, &stTuple, sizeof(stTuple) );
    //set msg length

    //send the request
    slen = mapi_tx_transc_req(tid, msg);
    if (slen != msg->header.mlen) {
        MID_TRACE("[%s] mapi_tx_transc_req() to send %d, sent %d\n", __FUNCTION__, msg->header.mlen, slen);

        return MID_FAIL;
    }

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Lazily commit some variable data in common configuration database
 *		to configuration file and/or flash
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *		commit_mask:	what kinds of committing variables; bitwise-OR values
 *							MID_CCFG_COMMIT_FILE (0x1)	- commit variables to file
 *							MID_CCFG_COMMIT_FLASH (0x2)	- commit variables to flash
 *							MID_CCFG_COMMIT_SYNC (0x4)	- synchronize to each service configuration file
 *		sSect:			which section to synchronize. It can be NULL if all sections are going to synchronize.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or ptuple is NULL.
 *
 * See Also
 *		mapi_ccfg_set, mapi_ccfg_del, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_commit_lazy(int tid, char commit_mask, char* sSect)
{
	if ( mapi_ccfg_commit_lazy_send( tid, commit_mask, sSect ) == MID_FAIL )
		return MID_FAIL;

	return mapi_ccfg_commit_recv( tid );
}

/*******************************************************************************
 * Description
 *		Remove multiple variable data from common configuration database
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		ptuple:	pointer to the buffer of data structure mapi_ccfg_tuple_t.
 *				Fields ptuple->section, ptuple->name and ptuple->type are required.
 *				ptuple->type is a bitwise-OR value of variables removal criteria.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or ptuple is NULL.
 *
 * Note
 *		* Please use UMNGCFG_MATCH_TYPE() to set removal criteria of ptuple->name.
 *			criteria is one of:
 *				UMNGCFG_MATCH_EXACT - exactly matching (string),
 *				UMNGCFG_MATCH_INFIX - middle string matching (*string*),
 *				UMNGCFG_MATCH_PREFIX - preceding string matching (string*),
 *				UMNGCFG_MATCH_POSTFIX - tailing string matching (*string)
 *
 * See Also
 *		mapi_ccfg_get, mapi_ccfg_set, mapi_ccfg_commit, mapi_ccfg_del, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_del_multi(int tid, mapi_ccfg_tuple_t* ptuple)
{
	if (ptuple == NULL) {
		MID_TRACE("[%s] NULL ptuple\n", __FUNCTION__);
		return MID_FAIL;
    }

	ptuple->type = UMNGCFG_MATCH_TYPE( UMNGCFG_MATCH_SECT_TYPE(ptuple->type),UMNGCFG_MATCH_NAME_TYPE(ptuple->type),UMNGCFG_MATCH_VAL_TYPE(ptuple->type));

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_DEL_MULTI, ptuple, sizeof(*ptuple) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Search a variable data in common configuration database
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		ptuple:	pointer to the buffer of data structure mapi_ccfg_tuple_t.
 *				Fields ptuple->section, ptuple->name, ptuple->value and ptuple->type are required.
 *				ptuple->type is a bitwise-OR value of variables removal criteria.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or ptuple is NULL.
 *
 * Note
 *		* Please use UMNGCFG_MATCH_TYPE() to set removal criteria of ptuple->section, ptuple->name
 *			and ptuple->value. Each field criteria is one of:
 *				UMNGCFG_MATCH_EXACT - exactly matching (string),
 *				UMNGCFG_MATCH_INFIX - middle string matching (*string*),
 *				UMNGCFG_MATCH_PREFIX - preceding string matching (string*),
 *				UMNGCFG_MATCH_POSTFIX - tailing string matching (*string)
 *		* If successful, variable data are filled in ptuple, including section, name and value.
 *
 * See Also
 *		mapi_ccfg_search, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_search(int tid, mapi_ccfg_tuple_t* ptuple)
{
	if (ptuple == NULL) {
		MID_TRACE("[%s] NULL ptuple\n", __FUNCTION__);
		return MID_FAIL;
    }

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_SEARCH, ptuple, sizeof(*ptuple) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Find out an unused variable data in common configuration database
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		ptuple:	pointer to the buffer of data structure mapi_ccfg_tuple_t.
 *				Fields ptuple->section, ptuple->name, ptuple->value and ptuple->type are required.
 *				ptuple->name is a print format string for sprintf() to check if a variable is not
 *				used yet. ptuple->type is the starting number to check.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or ptuple is NULL or ptuple->name format is wrong.
 *
 * Note
 *		* ptuple->type MUST be a zero or positive number.
 *		* ptuple->name MUST include one and only one character specificer %u for sprintf().
 *		* If successful, section and nanme of an unused variable data are filled in ptuple.
 *
 * See Also
 *		mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_find_unused( int tid, mapi_ccfg_tuple_t* ptuple)
{
	if (ptuple == NULL) {
		MID_TRACE("[%s] NULL ccfg\n", __FUNCTION__);
		return MID_FAIL;
    }

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_FIND_UNUSED, ptuple, sizeof(*ptuple) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

	return ptuple->type;
}

/*******************************************************************************
 * Description
 *		Backup all configuration database to a speficied file
 *
 * Parameters
 *		tid:		transaction ID returned by calling mapi_start_transc()
 *		sFileName:	backup file name, max length is ( MID_CCFG_MAX_FILENAME - 1 )
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or sFileName is NULL or oversize
 *
 * See Also
 *		mapi_ccfg_restore, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_backup( int tid, char* sFileName)
{
	int		iLen;
	char*	pBuf;

	if ( sFileName == NULL || (iLen=strlen(sFileName)) >= MID_CCFG_MAX_FILENAME)
		return MID_FAIL;

	if ( ( pBuf=osMalloc(iLen+1) ) == NULL )
		return MID_FAIL;

	strcpy( pBuf, sFileName );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_BACKUP, pBuf, iLen+1 ) != MID_SUCCESS) {
		osFree( pBuf );
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

	osFree( pBuf );

    return MID_SUCCESS;   
}

/*******************************************************************************
 * Description
 *		Restore all configuration database from a speficied file
 *
 * Parameters
 *		tid:		transaction ID returned by calling mapi_start_transc()
 *		sFileName:	restore file name, max length is ( MID_CCFG_MAX_FILENAME - 1 )
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or sFileName is NULL or oversize
 *
 * See Also
 *		mapi_ccfg_backup, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_restore( int tid, char* sFileName)
{
	int		iLen;
	char*	pBuf;

	if ( sFileName == NULL || (iLen=strlen(sFileName)) >= MID_CCFG_MAX_FILENAME)
		return MID_FAIL;

	if ( ( pBuf=osMalloc(iLen+1) ) == NULL )
		return MID_FAIL;

	strcpy( pBuf, sFileName );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_RESTORE, pBuf, iLen+1 ) != MID_SUCCESS) {
		osFree( pBuf );
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

	osFree( pBuf );

    return MID_SUCCESS;   
}

/*******************************************************************************
 * Description
 *		Backup main configuration file to a specified file
 *
 * Parameters
 *		tid:		transaction ID returned by calling mapi_start_transc()
 *		sFileName:	backup file name, max length is ( MID_CCFG_MAX_FILENAME - 1 )
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or sFileName is NULL or oversize
 *
 * See Also
 *		mapi_ccfg_restore, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_backup_partial( int tid, char* sFileName)
{
	int		iLen;
	char*	pBuf;

	if ( sFileName == NULL || (iLen=strlen(sFileName)) >= MID_CCFG_MAX_FILENAME)
		return MID_FAIL;

	if ( ( pBuf=osMalloc(iLen+1) ) == NULL )
		return MID_FAIL;

	strcpy( pBuf, sFileName );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_BACKUP_PARTIAL, pBuf, iLen+1 ) != MID_SUCCESS) {
		osFree( pBuf );
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

	osFree( pBuf );

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Restore main configuration file from a speficied file
 *
 * Parameters
 *		tid:		transaction ID returned by calling mapi_start_transc()
 *		sFileName:	restore file name, max length is ( MID_CCFG_MAX_FILENAME - 1 )
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or sFileName is NULL or oversize
 *
 * See Also
 *		mapi_ccfg_backup_partial, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_restore_partial( int tid, char* sFileName)
{
	int		iLen;
	char*	pBuf;

	if ( sFileName == NULL || (iLen=strlen(sFileName)) >= MID_CCFG_MAX_FILENAME)
		return MID_FAIL;

	if ( ( pBuf=osMalloc(iLen+1) ) == NULL )
		return MID_FAIL;

	strcpy( pBuf, sFileName );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_RESTORE_PARTIAL, pBuf, iLen+1 ) != MID_SUCCESS) {
		osFree( pBuf );
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

	osFree( pBuf );

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Duplicate a section in common configuration database
 *
 * Parameters
 *		tid:		transaction ID returned by calling mapi_start_transc()
 *		sDstSect:	the pointer to the new section name, max length is ( MID_CCFG_MAX_SECTION_NAME_LEN - 1 )
 *		sSrcSect:	the pointer to the source section name, max length is ( MID_CCFG_MAX_SECTION_NAME_LEN - 1 )
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or sFileName is NULL or oversize
 *
 * See Also
 *		mapi_ccfg_backup, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_dup( int tid, char* sDstSect, char* sSrcSect)
{
	mapi_ccfg_tuple_t	stCfg;

	if ( sDstSect == NULL || strlen(sDstSect) >= MID_CCFG_MAX_SECTION_NAME_LEN)
		return MID_FAIL;

	if ( sSrcSect == NULL || strlen(sSrcSect) >= MID_CCFG_MAX_SECTION_NAME_LEN)
		return MID_FAIL;

	strcpy( stCfg.section, sSrcSect );
	strcpy( stCfg.name, sDstSect );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_DUPLICATE, &stCfg, sizeof(stCfg) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

    return MID_SUCCESS;   
}


/************************************************************/
/****************** EXTENDED FUNCTIONS **********************/
/************************************************************/


/*******************************************************************************
 * Description
 *		Retrieve a variable data in common configuration database.
 *		If this variable does not exist, a specified default value is returned.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		ptuple:	pointer to the buffer of data structure mapi_ccfg_tuple_t.
 *				Fields ptuple->section, ptuple->name and ptuple->type are required.
 *		sDft:	default value (string) returned if the specified variable does not exist.
 *				It can not be NULL.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or ptuple is NULL or sDft is NULL.
 *
 * Note
 *		* ptuple->type is one of MID_CCFG_INT8, MID_CCFG_UINT8, MID_CCFG_INT16, MID_CCFG_UINT16,
 *			MID_CCFG_INT32, MID_CCFG_UINT32, MID_CCFG_INET4, MID_CCFG_INET6 and MID_CCFG_STR.
 *			If it is unknown type, this function views it as MID_CCFG_STR.
 *
 * See Also
 *		mapi_ccfg_get_dft, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_get_dft(int tid, mapi_ccfg_tuple_t* ptuple, char* sDft)
{
	if ( tid==MID_FAIL || ptuple == NULL || sDft == NULL )
		return MID_FAIL;

	if ( mapi_ccfg_get(tid,ptuple) == MID_SUCCESS )
		return MID_SUCCESS;

	switch (ptuple->type) {
		case MID_CCFG_STR:
			ptuple->value[ sizeof(ptuple->value)-1 ] = '\0';
			strncpy((char*)ptuple->value, sDft, sizeof(ptuple->value)-1);
			break;

		case MID_CCFG_INT8:
			*(I8*)(ptuple->value) = (I8)strtol(sDft, NULL, 0);
			break;
	
		case MID_CCFG_UINT8:
			*(U8*)(ptuple->value) = (U8)strtoul(sDft, NULL, 0);
			break;
	
		case MID_CCFG_INT16:
			*(I16*)(ptuple->value) = (I16)strtol(sDft, NULL, 0);
			break;
	
		case MID_CCFG_UINT16:
			*(U16*)(ptuple->value) = (U16)strtoul(sDft, NULL, 0);
			break;
	
		case MID_CCFG_INT32:
			*(I32*)(ptuple->value) = (I32)strtol(sDft, NULL, 0);
			break;
	
		case MID_CCFG_UINT32:
			*(U32*)(ptuple->value) = (U32)strtoul(sDft, NULL, 0);
			break;
	
		case MID_CCFG_INET4:
			//notice: the parameter value should point to a well-prepared buffer of struct in_addr
			if(inet_pton(AF_INET, sDft, ptuple->value)<=0)
				return MID_FAIL;
			break;
	
		case MID_CCFG_INET6:
			//notice: the parameter value should point to a well-prepared buffer of struct in6_addr
			if(inet_pton(AF_INET6, sDft, ptuple->value)<=0)
				return MID_FAIL;
			break;                
			
		default:
			MID_TRACE("[%s] unknow type\n", __FUNCTION__);
			ptuple->value[ sizeof(ptuple->value)-1 ] = '\0';
			strncpy((char*)ptuple->value, sDft, sizeof(ptuple->value)-1);
			break;
	}
    
    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Retrieve a variable data with string format in common configuration database.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		buf:	buffer to store the variable value. It MUST be prepared by caller function.
 *		bufSz:	size of buf, in byte.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction, or sect, name, buf are NULL,
 *							or bufSz is less than 2.
 *
 * Note
 *		* This function will write at most bufSz-1 of the characters into buf (the bufSz'th
 *			character then gets the terminating `\0'); if the variable value is greater than
 *			or equal to the bufSz argument, the string was too short and some of the characters
 *			were discarded. The output is always null-terminated.
 *
 * See Also
 *		mapi_ccfg_get, mapi_ccfg_get_str_dft, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_get_str2(int tid, char* sect, char* name, char* buf, int bufSz )
{
	mid_ccfg_tuple_t	stCfg;

	if ( tid==MID_FAIL || sect==NULL || name==NULL || buf==NULL || bufSz<=1 )
		return MID_FAIL;

	stCfg.type = MID_CCFG_STR;
	stCfg.section[ sizeof(stCfg.section)-1 ] = '\0';
	strncpy( stCfg.section, sect, sizeof(stCfg.section)-1 );
	stCfg.name[ sizeof(stCfg.name)-1 ] = '\0';
	strncpy(stCfg.name, name, sizeof(stCfg.name)-1);

	if (mapi_ccfg_get( tid, &stCfg ) == MID_FAIL)
		return MID_FAIL;

	buf[ bufSz-1 ] = '\0';
	if (bufSz < sizeof(stCfg.value))
		strncpy( buf, (char*)stCfg.value, bufSz-1 );
	else
		strcpy( buf, (char*)stCfg.value );

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Retrieve a variable data with string format in common configuration database.
 *		If this variable does not exist, a specified default value is returned.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		buf:	buffer to store the variable value. It MUST be prepared by caller function.
 *		bufSz:	size of buf, in byte.
 *		sDft:	default value (string) returned if the specified variable does not exist.
 *				It can not be NULL.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction, or sect, name, buf, sDft are NULL,
 *							or bufSz is less than 2.
 *
 * Note
 *		* This function will write at most bufSz-1 of the characters into buf (the bufSz'th
 *			character then gets the terminating `\0'); if the variable value is greater than
 *			or equal to the bufSz argument, the string was too short and some of the characters
 *			were discarded. The output is always null-terminated.
 *
 * See Also
 *		mapi_ccfg_get, mapi_ccfg_get_str2, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_get_str_dft(int tid, char* sect, char* name, char* buf, int bufSz, char* sDft )
{
	mid_ccfg_tuple_t	stCfg;

	if ( tid==MID_FAIL || sect==NULL || name==NULL || buf==NULL || bufSz<=1 || sDft==NULL || bufSz<=1 )
		return MID_FAIL;

	stCfg.type = MID_CCFG_STR;
	stCfg.section[ sizeof(stCfg.section)-1 ] = '\0';
	strncpy( stCfg.section, sect, sizeof(stCfg.section)-1 );
	stCfg.name[ sizeof(stCfg.name)-1 ] = '\0';
	strncpy(stCfg.name, name, sizeof(stCfg.name)-1);

	if (mapi_ccfg_get_dft( tid, &stCfg, sDft ) == MID_FAIL)
		return MID_FAIL;

	buf[ bufSz-1 ] = '\0';
	if (bufSz < sizeof(stCfg.value))
		strncpy( buf, (char*)stCfg.value, bufSz-1 );
	else
		strcpy( buf, (char*)stCfg.value );

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Retrieve a variable data with string format in common configuration database.
 *		If this variable does not exist, a specified default value is returned.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		buf:	buffer to store the variable value. It MUST be prepared by caller function.
 *		bufSz:	size of buf, in byte.
 *		sDft:	default value (string) returned if the specified variable does not exist.
 *				It can not be NULL.
 *
 * Returns
 *		* pointer to buf:				successfully
 *		* pointer to an empty string:	failed to process transaction, or sect, name, buf, sDft are NULL,
 *										or bufSz is less than 2.
 *
 * Note
 *		* This function will write at most bufSz-1 of the characters into buf (the bufSz'th
 *			character then gets the terminating `\0'); if the variable value is greater than
 *			or equal to the bufSz argument, the string was too short and some of the characters
 *			were discarded. The output is always null-terminated.
 *
 * See Also
 *		mapi_ccfg_set_str, mapi_ccfg_get, mapi_start_transc
 *
 ******************************************************************************/
char* mapi_ccfg_get_str(int tid, char* sect, char* name, char* buf, int bufSz, char* sDft )
{
	if ( tid==MID_FAIL || sect==NULL || name==NULL || buf==NULL || bufSz<=1 )
		return _sNull;

	if (sDft == NULL) {
		buf[0] = '\0';
		mapi_ccfg_get_str_dft( tid, sect, name, buf, bufSz, _sNull );
	} else {
		if (mapi_ccfg_get_str_dft( tid, sect, name, buf, bufSz, sDft ) == MID_FAIL) {
			buf[ bufSz-1 ] = '\0';
			strncpy(buf, sDft, bufSz-1);
		}
	}

	return buf;
}

/*******************************************************************************
 * Description
 *		Retrieve a variable data with long integer format in common configuration database.
 *		If this variable does not exist, a specified default value is returned.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		buf:	buffer to store the variable value. It MUST be prepared by caller function.
 *		bufSz:	size of buf, in byte.
 *		lDft:	default value (long integer) returned if the specified variable does not exist.
 *
 * Returns
 *		* lDft:		failed to process transaction, or sect, name, buf are NULL,
 *					or value format is wrong, or value is the same as lDft
 *		* others:	successfully
 *
 * See Also
 *		mapi_ccfg_set_long, mapi_ccfg_get, mapi_start_transc
 *
 ******************************************************************************/
long mapi_ccfg_get_long(int tid, char* sect, char* name, long lDft )
{
	char	sVal[16];
	long	lVal;
	char*	pEnd;

	mapi_ccfg_get_str( tid, sect, name, sVal, sizeof(sVal), "" );

	lVal = strtol( sVal, &pEnd, 0 );

	if ( pEnd == sVal )
		return lDft;

	return lVal;
}

/*******************************************************************************
 * Description
 *		Set a variable data with string format in common configuration database.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		sVal:	variable value (string)
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction, or sect, name, buf, sVal are NULL
 *
 * Note
 *		* If the variable does not exist, this function will add it.
 *		* Please use mapi_ccfg_commit() to permanently commit variables.
 *
 * See Also
 *		mapi_ccfg_get_str, mapi_ccfg_set, mapi_ccfg_unset, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_set_str(int tid, char* sect, char* name, char* sVal )
{
	mid_ccfg_tuple_t	stCfg;

	if ( tid==MID_FAIL || sect==NULL || name==NULL || sVal==NULL )
		return MID_FAIL;

	stCfg.type = MID_CCFG_STR;
	stCfg.section[ sizeof(stCfg.section)-1 ] = '\0';
	strncpy( stCfg.section, sect, sizeof(stCfg.section)-1 );
	stCfg.name[ sizeof(stCfg.name)-1 ] = '\0';
	strncpy(stCfg.name, name, sizeof(stCfg.name)-1);
	stCfg.value[ sizeof(stCfg.value)-1 ] = '\0';
	strncpy((char*)stCfg.value, sVal, sizeof(stCfg.value)-1);

	return mapi_ccfg_set( tid, &stCfg );
}

/*******************************************************************************
 * Description
 *		Set a variable data with long integer format in common configuration database.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		lVal:	variable value (long integer)
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction, or sect, name are NULL
 *
 * Note
 *		* If the variable does not exist, this function will add it.
 *		* Please use mapi_ccfg_commit() to permanently commit variables.
 *
 * See Also
 *		mapi_ccfg_get_long, mapi_ccfg_set, mapi_ccfg_unset, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_set_long( int tid, char* sect, char* name, long lVal )
{
	char	sVal[16];

	sprintf( sVal, "%ld", (long int)lVal );

	return mapi_ccfg_set_str( tid, sect, name, sVal );
}

/*******************************************************************************
 * Description
 *		Remove a variable data from common configuration database.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction, or sect, name are NULL
 *
 * Note
 *		* Please use mapi_ccfg_commit() to permanently commit variables.
 *
 * See Also
 *		mapi_ccfg_unset_multi, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_unset(int tid, char* sect, char* name )
{
	mid_ccfg_tuple_t	stCfg;

	if ( tid==MID_FAIL || sect==NULL || name==NULL )
		return MID_FAIL;

	stCfg.type = MID_CCFG_STR;
	stCfg.section[ sizeof(stCfg.section)-1 ] = '\0';
	strncpy( stCfg.section, sect, sizeof(stCfg.section)-1 );
	stCfg.name[ sizeof(stCfg.name)-1 ] = '\0';
	strncpy(stCfg.name, name, sizeof(stCfg.name)-1);
	stCfg.value[ sizeof(stCfg.value)-1 ] = '\0';

	return mapi_ccfg_del( tid, &stCfg );
}

/*******************************************************************************
 * Description
 *		Remove multiple variable data from common configuration database
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of these variables belonging to
 *		name:	partial or full name of these variables
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction, or sect, name are NULL
 *
 * Note
 *		* if 'name' is a partial name of variables, wild character '*' can be used as follows:
 *				exactly matching (string),
 *				middle string matching (*string*),
 *				preceding string matching (string*),
 *				tailing string matching (*string)
 *
 * See Also
 *		mapi_ccfg_unset, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_unset_multi(int tid, char* sect, char* name )
{
	mid_ccfg_tuple_t	stCfg;
	int					iSectLen, iNameLen;
	int					iSectType, iNameType;

	if ( tid==MID_FAIL || sect==NULL || name==NULL )
		return MID_FAIL;

	if (sect[0] == '\0' || name[0] == '\0')
		return MID_FAIL;

	memset( stCfg.section, 0, sizeof(stCfg.section));
	memset( stCfg.name, 0, sizeof(stCfg.name));
	stCfg.value[ 0 ] = '\0';
	iSectLen = strlen(sect);
	iNameLen = strlen(name);

	if (sect[0] == '*') {
		if (sect[1] != '\0' && sect[iSectLen-1] == '*') {
			strncpy( stCfg.section, sect+1, MIN(iSectLen-2,sizeof(stCfg.section)-1) );
			iSectType = UMNGCFG_MATCH_INFIX;
		} else {
			strncpy( stCfg.section, sect+1, MIN(iSectLen-1,sizeof(stCfg.section)-1) );
			iSectType = UMNGCFG_MATCH_POSTFIX;
		}
	} else if (sect[iSectLen-1] == '*') {
		strncpy( stCfg.section, sect, MIN(iSectLen-1,sizeof(stCfg.section)-1) );
		iSectType = UMNGCFG_MATCH_PREFIX;
	} else {
		strncpy( stCfg.section, sect, MIN(iSectLen,sizeof(stCfg.section)-1) );
		iSectType = UMNGCFG_MATCH_EXACT;
	}

	if (name[0] == '*') {
		if (name[1] != '\0' && name[iNameLen-1] == '*') {
			strncpy( stCfg.name, name+1, MIN(iNameLen-2,sizeof(stCfg.name)-1) );
			iNameType = UMNGCFG_MATCH_INFIX;
		} else {
			strncpy( stCfg.name, name+1, MIN(iNameLen-1,sizeof(stCfg.name)-1) );
			iNameType = UMNGCFG_MATCH_POSTFIX;
		}
	} else if (name[iNameLen-1] == '*') {
		strncpy( stCfg.name, name, MIN(iNameLen-1,sizeof(stCfg.name)-1) );
		iNameType = UMNGCFG_MATCH_PREFIX;
	} else {
		strncpy( stCfg.name, name, MIN(iNameLen,sizeof(stCfg.name)-1) );
		iNameType = UMNGCFG_MATCH_EXACT;
	}

	stCfg.type = UMNGCFG_MATCH_TYPE( iSectType, iNameType, 0 );

	return mapi_ccfg_del_multi( tid, &stCfg );
}

/*******************************************************************************
 * Description
 *		Retrieve a variable data with string format in common configuration database
 *		and compare it to a given string.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		buf:	buffer to store the variable value. It MUST be prepared by caller function.
 *		bufSz:	size of buf, in byte.
 *		sStr:	string to be compared to.
 *
 * Returns
 *		* -1:	variable value is less than sStr
 *		* 0:	variable value is equal to sStr
 *		* 1:	variable value is greater than sStr
 *
 * Note
 *		* If variable does not exist, this function views it as an empty string.
 *		* If sStr is NULL, this function always returns 1.
 *		* This function will write at most bufSz-1 of the characters into buf (the bufSz'th
 *			character then gets the terminating `\0'); if the variable value is greater than
 *			or equal to the bufSz argument, the string was too short and some of the characters
 *			were discarded. The output is always null-terminated.
 *
 * See Also
 *		mapi_ccfg_cmp_str, mapi_ccfg_cmp_str_dft, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_get_cmp_str(int tid, char* sect, char* name, char* buf, int bufSz, char* sStr )
{
	char*	sVal;

	if (sStr == NULL)
		return 1;

	sVal = mapi_ccfg_get_str( tid, sect, name, buf, bufSz, NULL );

	return strcmp( sVal, sStr );
}

/*******************************************************************************
 * Description
 *		Compare a variable data with string format to a given string.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		sStr:	string to be compared to.
 *
 * Returns
 *		* -1:	variable value is less than sStr
 *		* 0:	variable value is equal to sStr
 *		* 1:	variable value is greater than sStr
 *
 * Note
 *		* If variable does not exist, this function views it as an empty string.
 *		* If sStr is NULL, this function always returns 1.
 *
 * See Also
 *		mapi_ccfg_get_cmp_str, mapi_ccfg_cmp_str_dft, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_cmp_str(int tid, char* sect, char* name, char* sStr )
{
	char	sVal[MID_CCFG_MAX_VALUE_LEN];

	return mapi_ccfg_get_cmp_str( tid, sect, name, sVal, sizeof(sVal), sStr );
}

/*******************************************************************************
 * Description
 *		Compare a variable data with string format to a given string.
 *		If this variable does not exist, Comparison is to a specified default string.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		sStr:	string to be compared to.
 *		sDft:	specified default string to compare to if the variable does not exist.
 *
 * Returns
 *		* -1:	variable value is less than sStr
 *		* 0:	variable value is equal to sStr
 *		* 1:	variable value is greater than sStr
 *
 * Note
 *		* If variable does not exist, this function views it as an empty string.
 *		* If sStr is NULL, this function always returns 1.
 *		* If the variable does not exist and sDft is NULL, this function always returns -1.
 *
 * See Also
 *		mapi_ccfg_get_cmp_str, mapi_ccfg_cmp_str, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_cmp_str_dft(int tid, char* sect, char* name, char* sStr, char* sDft )
{
	char	sVal[MID_CCFG_MAX_VALUE_LEN];
	char*	pPtr;

	if (sStr == NULL)
		return 1;

	pPtr = mapi_ccfg_get_str( tid, sect, name, sVal, sizeof(sVal), NULL );

	if (pPtr != sVal)
		pPtr = sDft;

	if (pPtr == NULL)
		return -1;

	return strcmp( sVal, sStr );
}

/*******************************************************************************
 * Description
 *		Compare a variable data with long integer format to a given number.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		lVal:	long integer number to be compared to.
 *
 * Returns
 *		* -1:	variable value is less than lVal
 *		* 0:	variable value is equal to lVal
 *		* 1:	variable value is greater than lVal
 *
 * Note
 *		* If variable does not exist or it is an empty string, this function always returns -1.
 *
 * See Also
 *		mapi_ccfg_cmp_long_dft, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_cmp_long(int tid, char* sect, char* name, long lVal )
{
	char	sVal[16];
	long	lTmp;

	mapi_ccfg_get_str( tid, sect, name, sVal, sizeof(sVal), "" );

	if (sVal[0] == '\0')
		return -1;

	lTmp = strtol( sVal, NULL, 0 );

	if (lTmp == lVal)
		return 0;

	if (lTmp > lVal)
		return 1;

	return -1;
}

/*******************************************************************************
 * Description
 *		Compare a variable data with long integer format to a given number.
 *		If this variable does not exist, Comparison is to a specified default value.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		lVal:	long integer number to be compared to.
 *
 * Returns
 *		* -1:	variable value is less than lVal
 *		* 0:	variable value is equal to lVal
 *		* 1:	variable value is greater than lVal
 *
 * Note
 *		* If variable does not exist or it is an empty string, this function views it as lDft.
 *
 * See Also
 *		mapi_ccfg_cmp_long, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_cmp_long_dft(int tid, char* sect, char* name, long lVal, long lDft )
{
	char	sVal[16];
	long	lTmp;

	mapi_ccfg_get_str( tid, sect, name, sVal, sizeof(sVal), "" );

	if (sVal[0] == '\0')
		lTmp = lDft;
	else
		lTmp = strtol( sVal, NULL, 0 );

	if (lTmp == lVal)
		return 0;

	if (lTmp > lVal)
		return 1;

	return -1;
}

/*******************************************************************************
 * Description
 *		Compare and set a variable data with string format.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		sStr:	string to be compared to.
 *
 * Returns
 *		* 0:	variable value is equal to sStr, or failed to process transaction,
 *				or sect, name, sStr are NULL
 *		* 1:	variable value is updated with sStr
 *
 * Note
 *		* If variable does not exist, this function will add it.
 *
 * See Also
 *		mapi_ccfg_cmp_str, mapi_ccfg_set_str, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_update_str(int tid, char* sect, char* name, char* sStr )
{
	if (mapi_ccfg_cmp_str( tid, sect, name, sStr ) == 0)
		return 0;

	if (mapi_ccfg_set_str( tid, sect, name, sStr ) == MID_FAIL)
		return 0;

	return 1;
}

/*******************************************************************************
 * Description
 *		Compare and set a variable data with long integer format.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		lVal:	long integer number to be compared to.
 *
 * Returns
 *		* 0:	variable value is equal to sStr, or failed to process transaction,
 *				or sect, name are NULL
 *		* 1:	variable value is updated with sStr
 *
 * Note
 *		* If variable does not exist, this function will add it.
 *
 * See Also
 *		mapi_ccfg_cmp_long, mapi_ccfg_set_long, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_update_long(int tid, char* sect, char* name, long lVal )
{
	if (mapi_ccfg_cmp_long( tid, sect, name, lVal ) == 0)
		return 0;

	if (mapi_ccfg_set_long( tid, sect, name, lVal ) == MID_FAIL)
		return 0;

	return 1;
}


/************************************************************/
/***************** INDEX-STYPLE FUNCTIONS *******************/
/************************************************************/


/*******************************************************************************
 * Description
 *		Set index pattern of index-style variable name
 *
 * Parameters
 *		namePat:	index pattern. It can be NULL.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		namePat format is wrong
 *
 * Note
 *		* namePat is used to generate and verify a group of variables which have the same key name
 *			but own thier own index number (positive long integer number). System applies it as
 *			print character specifier of sprintf().
 *		* This function allows at most one "%u" and one "%s" specifiers in the pattern.
 *			Other "%" character specifier are not allowed except for "%%".
 *		* "%s" MUST be at the beginning or the end of pattern.
 *		* If namePat is NULL, this function will reset index pattern to "%s%u"
 *		* If multi-threaded application is used, please use one only one kind of pattern,
 *			or the effect will be unpredicable.
 *
 ******************************************************************************/
int mapi_ccfg_idx_set_pattern( char* namePat )
{
	char*	pIdx;
	char*	pName;

	if (namePat == NULL) { /* reset to default */
		strcpy( _namePatternFmt, MAPI_CCFG_IDX_NAME_PATTERN_FMT );
		_namePatternType = MAPI_CCFG_IDX_NAME_PATTERN_TYPE;
		return MID_SUCCESS;
	}

	pName = strchr( namePat, '%' );
	while (pName) {
		if (pName[1] != 'u' && pName[1] != 's' && pName[1] != '%')
			return MID_FAIL;
		pName = strchr( pName+2, '%' );
	}

	pIdx  = strstr( namePat, "%u" );
	pName = strstr( namePat, "%s" );

	if ( pName && pName != namePat && (pName+2) != (namePat+strlen(namePat)) )
		return MID_FAIL; /* ilegal pattern, partialName is not at the beginning or the end of pattern */

	if (pIdx > pName)
		_namePatternType = MAPI_CCFG_IDX_NAME_PREFIX;
	else
		_namePatternType = MAPI_CCFG_IDX_INDEX_PREFIX;

	strncpy( _namePatternFmt, namePat, MAPI_CCFG_IDX_NAME_FMT_MAXLEN );
	_namePatternFmt[ sizeof(_namePatternFmt)-1 ] = '\0';

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Retrieve an index-styple variable data with string format in common configuration database.
 *		If this variable does not exist, a specified default value is returned.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		buf:	buffer to store the variable value. It MUST be prepared by caller function.
 *		bufSz:	size of buf, in byte.
 *		sDft:	default value (string) returned if the specified variable does not exist.
 *				It can not be NULL.
 *
 * Returns
 *		* pointer to buf:				successfully
 *		* pointer to an empty string:	failed to process transaction, or sect, name, buf, sDft are NULL,
 *										or bufSz is less than 2.
 *
 * Note
 *		* This function will write at most bufSz-1 of the characters into buf (the bufSz'th
 *			character then gets the terminating `\0'); if the variable value is greater than
 *			or equal to the bufSz argument, the string was too short and some of the characters
 *			were discarded. The output is always null-terminated.
 *
 * See Also
 *		mapi_ccfg_idx_set_str, mapi_start_transc
 *
 ******************************************************************************/
char* mapi_ccfg_idx_get_str(int tid, char* sect, char* name, unsigned int idx, char* buf, int bufSz, char* sDft )
{
	MAPI_CCFG_IDX_PREPROC( key, name, idx, _sNull );
	return mapi_ccfg_get_str( tid, sect, key, buf, bufSz, sDft );
}

/*******************************************************************************
 * Description
 *		Retrieve an index-style variable data with long integer format in common configuration database.
 *		If this variable does not exist, a specified default value is returned.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		buf:	buffer to store the variable value. It MUST be prepared by caller function.
 *		bufSz:	size of buf, in byte.
 *		lDft:	default value (long integer) returned if the specified variable does not exist.
 *
 * Returns
 *		* lDft:		failed to process transaction, or sect, name, buf are NULL,
 *					or value format is wrong, or value is the same as lDft
 *		* others:	successfully
 *
 * See Also
 *		mapi_ccfg_idx_set_long, mapi_start_transc
 *
 ******************************************************************************/
long mapi_ccfg_idx_get_long( int tid, char* sect, char* name, unsigned int idx, long lDft )
{
	char	sVal[16];
	long	lVal;

	mapi_ccfg_idx_get_str( tid, sect, name, idx, sVal, sizeof(sVal), "" );

	lVal = strtol( sVal, NULL, 0 );

	if (lVal==0 && errno == EINVAL )
		return lDft;

	return lVal;
}

/*******************************************************************************
 * Description
 *		Set an index-style variable data with string format in common configuration database.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		sVal:	variable value (string)
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction, or sect, name, buf, sVal are NULL
 *
 * Note
 *		* If the variable does not exist, this function will add it.
 *		* Please use mapi_ccfg_commit() to permanently commit variables.
 *
 * See Also
 *		mapi_ccfg_idx_get_str, mapi_ccfg_idx_unset, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_set_str(int tid, char* sect, char* name, unsigned int idx, char* sVal )
{
	MAPI_CCFG_IDX_PREPROC( key, name, idx, MID_FAIL );
	return mapi_ccfg_set_str( tid, sect, key, sVal );
}

/*******************************************************************************
 * Description
 *		Set an index-style variable data with long integer format in common configuration database.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		lVal:	variable value (long integer)
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction, or sect, name are NULL
 *
 * Note
 *		* If the variable does not exist, this function will add it.
 *		* Please use mapi_ccfg_commit() to permanently commit variables.
 *
 * See Also
 *		mapi_ccfg_idx_get_long, mapi_ccfg_idx_unset, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_set_long( int tid, char* sect, char* name, unsigned int idx, long lVal )
{
	char	sVal[16];

	sprintf( sVal, "%ld", (long int)lVal );

	return mapi_ccfg_idx_set_str( tid, sect, name, idx, sVal );
}

/*******************************************************************************
 * Description
 *		Remove an index-style variable data from common configuration database.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction, or sect, name are NULL
 *
 * Note
 *		* Please use mapi_ccfg_commit() to permanently commit variables.
 *
 * See Also
 *		mapi_ccfg_idx_unset_multi, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_unset(int tid, char* sect, char* name, unsigned int idx )
{
	MAPI_CCFG_IDX_PREPROC( key, name, idx, MID_FAIL );
	return mapi_ccfg_unset( tid, sect, key );
}

/*******************************************************************************
 * Description
 *		Retrieve an index-style variable data with string format in common configuration database
 *		and compare it to a given string.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		buf:	buffer to store the variable value. It MUST be prepared by caller function.
 *		bufSz:	size of buf, in byte.
 *		sStr:	string to be compared to.
 *
 * Returns
 *		* -1:	variable value is less than sStr
 *		* 0:	variable value is equal to sStr
 *		* 1:	variable value is greater than sStr
 *
 * Note
 *		* If variable does not exist, this function views it as an empty string.
 *		* If sStr is NULL, this function always returns 1.
 *		* This function will write at most bufSz-1 of the characters into buf (the bufSz'th
 *			character then gets the terminating `\0'); if the variable value is greater than
 *			or equal to the bufSz argument, the string was too short and some of the characters
 *			were discarded. The output is always null-terminated.
 *
 * See Also
 *		mapi_ccfg_idx_cmp_str, mapi_ccfg_idx_cmp_str_dft, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_get_cmp_str(int tid, char* sect, char* name, unsigned int idx, char* buf, int bufSz, char* sStr )
{
	MAPI_CCFG_IDX_PREPROC( key, name, idx, -1 );
	return mapi_ccfg_get_cmp_str( tid, sect, key, buf, bufSz, sStr );
}

/*******************************************************************************
 * Description
 *		Compare an index-style variable data with string format to a given string.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		sStr:	string to be compared to.
 *
 * Returns
 *		* -1:	variable value is less than sStr
 *		* 0:	variable value is equal to sStr
 *		* 1:	variable value is greater than sStr
 *
 * Note
 *		* If variable does not exist, this function views it as an empty string.
 *		* If sStr is NULL, this function always returns 1.
 *
 * See Also
 *		mapi_ccfg_idx_get_cmp_str, mapi_ccfg_idx_cmp_str_dft, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_cmp_str(int tid, char* sect, char* name, unsigned int idx, char* sStr )
{
	MAPI_CCFG_IDX_PREPROC( key, name, idx, -1 );
	return mapi_ccfg_cmp_str( tid, sect, key, sStr );
}

/*******************************************************************************
 * Description
 *		Compare an index-style variable data with string format to a given string.
 *		If this variable does not exist, Comparison is to a specified default string.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		sStr:	string to be compared to.
 *		sDft:	specified default string to compare to if the variable does not exist.
 *
 * Returns
 *		* -1:	variable value is less than sStr
 *		* 0:	variable value is equal to sStr
 *		* 1:	variable value is greater than sStr
 *
 * Note
 *		* If variable does not exist, this function views it as an empty string.
 *		* If sStr is NULL, this function always returns 1.
 *		* If the variable does not exist and sDft is NULL, this function always returns -1.
 *
 * See Also
 *		mapi_ccfg_idx_get_cmp_str, mapi_ccfg_idx_cmp_str, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_cmp_str_dft(int tid, char* sect, char* name, unsigned int idx, char* sStr, char* sDft )
{
	MAPI_CCFG_IDX_PREPROC( key, name, idx, -1 );
	return mapi_ccfg_cmp_str_dft( tid, sect, key, sStr, sDft );
}

/*******************************************************************************
 * Description
 *		Compare an index-style variable data with long integer format to a given number.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		lVal:	long integer number to be compared to.
 *
 * Returns
 *		* -1:	variable value is less than lVal
 *		* 0:	variable value is equal to lVal
 *		* 1:	variable value is greater than lVal
 *
 * Note
 *		* If variable does not exist or it is an empty string, this function always returns -1.
 *
 * See Also
 *		mapi_ccfg_idx_cmp_long_dft, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_cmp_long(int tid, char* sect, char* name, unsigned int idx, long lVal )
{
	MAPI_CCFG_IDX_PREPROC( key, name, idx, -1 );
	return mapi_ccfg_cmp_long( tid, sect, key, lVal );
}

/*******************************************************************************
 * Description
 *		Compare an index-style variable data with long integer format to a given number.
 *		If this variable does not exist, Comparison is to a specified default value.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		lVal:	long integer number to be compared to.
 *
 * Returns
 *		* -1:	variable value is less than lVal
 *		* 0:	variable value is equal to lVal
 *		* 1:	variable value is greater than lVal
 *
 * Note
 *		* If variable does not exist or it is an empty string, this function views it as lDft.
 *
 * See Also
 *		mapi_ccfg_idx_cmp_long, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_cmp_long_dft(int tid, char* sect, char* name, unsigned int idx, long lVal, long lDft )
{
	MAPI_CCFG_IDX_PREPROC( key, name, idx, -1 );
	return mapi_ccfg_cmp_long_dft( tid, sect, key, lVal, lDft );
}

/*******************************************************************************
 * Description
 *		Compare and set an index-style variable data with string format.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		sStr:	string to be compared to.
 *
 * Returns
 *		* 0:	variable value is equal to sStr, or failed to process transaction,
 *				or sect, name, sStr are NULL
 *		* 1:	variable value is updated with sStr
 *
 * Note
 *		* If variable does not exist, this function will add it.
 *
 * See Also
 *		mapi_ccfg_idx_cmp_str, mapi_ccfg_idx_set_str, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_update_str(int tid, char* sect, char* name, unsigned int idx, char* sStr )
{
	MAPI_CCFG_IDX_PREPROC( key, name, idx, 0 );
	return mapi_ccfg_update_str( tid, sect, key, sStr );
}

/*******************************************************************************
 * Description
 *		Compare and set an index-style variable data with long integer format.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		lVal:	long integer number to be compared to.
 *
 * Returns
 *		* 0:	variable value is equal to sStr, or failed to process transaction,
 *				or sect, name are NULL
 *		* 1:	variable value is updated with sStr
 *
 * Note
 *		* If variable does not exist, this function will add it.
 *
 * See Also
 *		mapi_ccfg_idx_cmp_long, mapi_ccfg_idx_set_long, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_update_long(int tid, char* sect, char* name, unsigned int idx, long lVal )
{
	MAPI_CCFG_IDX_PREPROC( key, name, idx, 0 );
	return mapi_ccfg_update_long( tid, sect, key, lVal );
}

/*******************************************************************************
 * Description
 *		Remove all index-style variable data in a section which have the same index number
 *		regardless of what their names are
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of these variables belonging to
 *		idx:	index number, positive interger number
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction, or sect, name are NULL
 *
 * Note
 *		* This function will check and remove all index-style variables whose index number
 *			number are the same.
 *
 * See Also
 *		mapi_ccfg_idx_unset, mapi_ccfg_commit, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_unset_all(int tid, char* sect, unsigned int idx )
{
	char	key[ MID_CCFG_MAX_PARAM_NAME_LEN ];
	char	fmt[ MAPI_CCFG_IDX_NAME_FMT_MAXLEN ];
	char*	pPtr;

	if (sect == NULL)
		return MID_FAIL;

	strcpy( fmt, _namePatternFmt );
	if ( (pPtr=strstr(fmt,"%s")) != NULL ) {
		pPtr[0] = '*';
		strcpy( pPtr+1, pPtr+2 );
	}

	snprintf( key, sizeof(key), fmt, idx );

	return mapi_ccfg_unset_multi( tid, sect, key );
}

/*******************************************************************************
 * Description
 *		Search an index-style variable data in common configuration database by its value
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		sVal:	variable value to search
 *		iOrder:	which one to be retrieved if multiple variables match the sVal.
 *					UMNGCFG_MATCH_SPC_FIRST(4) - get the first matched variable, in alphabetical name order.
 *					UMNGCFG_MATCH_SPC_LAST(8) - get the last matched variable, in alphabetical name order.
 *					others - get the first matched variable, in database position order.
 *
 * Returns
 *		* MID_FAIL(-1):		failed to process transaction or sect, name, sVal are NULL,
 *							or no variable matches
 *		* others:			successfully and return the index number
 *
 * Note
 *		* If successful, variable data are filled in ptuple, including section, name and value.
 *
 * See Also
 *		mapi_ccfg_idx_search_any_by_val, mapi_ccfg_idx_search_first_by_val,
 *		mapi_ccfg_idx_search_last_by_val, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_search_by_val(int tid, char* sect, char* name, char* sVal, int iOrder )
{
	mapi_ccfg_tuple_t	ccfg;
	char				fmt[ MAPI_CCFG_IDX_NAME_FMT_MAXLEN ];
	char*				pPtr;
	int					iMatchType;

	if (sect == NULL || name == NULL || sVal == NULL)
		return MID_FAIL;

	strncpy( ccfg.section, sect, sizeof(ccfg.section)-1 );
	ccfg.section[ sizeof(ccfg.section)-1 ] = '\0';

	strncpy( (char*)ccfg.value, sVal, sizeof(ccfg.value)-1 );
	ccfg.value[ sizeof(ccfg.value)-1 ] = '\0';

	strcpy( fmt, _namePatternFmt );
	if ( (pPtr=strstr(fmt,"%u")) == NULL )
		return MID_FAIL;

	if (pPtr[2] == '\0') {
		iMatchType = UMNGCFG_MATCH_PREFIX;
		pPtr[0] = '\0';
		pPtr = fmt;
	} else {
		iMatchType = UMNGCFG_MATCH_POSTFIX;
		pPtr += 2; /* ignore those string before %u in XXX%dXXX%sXXX */
	}
	snprintf( ccfg.name, sizeof(ccfg.name), pPtr, name );
	if (iOrder != UMNGCFG_MATCH_SPC_FIRST && iOrder != UMNGCFG_MATCH_SPC_LAST)
		iOrder = 0;
	ccfg.type = UMNGCFG_MATCH_TYPE( UMNGCFG_MATCH_EXACT|iOrder, iMatchType|iOrder, UMNGCFG_MATCH_EXACT|iOrder );

	if (mapi_ccfg_search( tid, &ccfg ) == MID_FAIL)
		return MID_FAIL;

	strcpy( fmt, _namePatternFmt );
	if ( (pPtr=strstr(fmt,"%u")) == NULL )
		return MID_FAIL;

	if (iMatchType == UMNGCFG_MATCH_PREFIX) {
		for (pPtr = &ccfg.name[strlen(name)]; *pPtr != '\0'; pPtr++) {
			if ( *pPtr >= '0' && *pPtr <= '9' )
				break;
		}
		return atoi( pPtr );
	} else {
		for (pPtr = &ccfg.name[0]; *pPtr != '\0'; pPtr++) {
			if ( *pPtr >= '0' && *pPtr <= '9' )
				break;
		}
		return atoi( pPtr );
	}
}

/*******************************************************************************
 * Description
 *		Search an index-style variable data in common configuration database by its value.
 *		If multiple variables match, return the first variable in database position order.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		sVal:	variable value to search
 *
 * Returns
 *		* MID_FAIL(-1):		failed to process transaction or sect, name, sVal are NULL,
 *							or no variable matches
 *		* others:			successfully and return the index number
 *
 * Note
 *		* If successful, variable data are filled in ptuple, including section, name and value.
 *
 * See Also
 *		mapi_ccfg_idx_search_by_val, mapi_ccfg_idx_search_first_by_val,
 *		mapi_ccfg_idx_search_last_by_val, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_search_any_by_val(int tid, char* sect, char* name, char* sVal )
{
	return mapi_ccfg_idx_search_by_val( tid, sect, name, sVal, 0 );
}

/*******************************************************************************
 * Description
 *		Search an index-style variable data in common configuration database by its value.
 *		If multiple variables match, return the first variable in alphabetical name order.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		sVal:	variable value to search
 *
 * Returns
 *		* MID_FAIL(-1):		failed to process transaction or sect, name, sVal are NULL,
 *							or no variable matches
 *		* others:			successfully and return the index number
 *
 * Note
 *		* If successful, variable data are filled in ptuple, including section, name and value.
 *
 * See Also
 *		mapi_ccfg_idx_search_by_val, mapi_ccfg_idx_search_any_by_val,
 *		mapi_ccfg_idx_search_last_by_val, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_search_first_by_val(int tid, char* sect, char* name, char* sVal )
{
	return mapi_ccfg_idx_search_by_val( tid, sect, name, sVal, UMNGCFG_MATCH_SPC_FIRST );
}

/*******************************************************************************
 * Description
 *		Search an index-style variable data in common configuration database by its value.
 *		If multiple variables match, return the last variable in alphabetical name order.
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		idx:	index number, positive interger number
 *		sVal:	variable value to search
 *
 * Returns
 *		* MID_FAIL(-1):		failed to process transaction or sect, name, sVal are NULL,
 *							or no variable matches
 *		* others:			successfully and return the index number
 *
 * Note
 *		* If successful, variable data are filled in ptuple, including section, name and value.
 *
 * See Also
 *		mapi_ccfg_idx_search_by_val, mapi_ccfg_idx_search_any_by_val,
 *		mapi_ccfg_idx_search_first_by_val, mapi_start_transc
 *
 ******************************************************************************/
int mapi_ccfg_idx_search_last_by_val(int tid, char* sect, char* name, char* sVal )
{
	return mapi_ccfg_idx_search_by_val( tid, sect, name, sVal, UMNGCFG_MATCH_SPC_LAST );
}

/*******************************************************************************
 * Description
 *		Find out an unused index number by name in a section of common database .
 *
 * Parameters
 *		tid:	transaction ID returned by calling mapi_start_transc()
 *		sect:	section of the variable belonging to
 *		name:	name of the variable
 *		iFrom:	the beginning index number to search from. It MUST be zero or a positive number.
 *
 * Returns
 *		* MID_FAIL(-1):		failed to process transaction or sect, name are NULL,
 *							or iFrom is a negative number
 *		* others:			successfully and return the index number
 *
 * Note
 *		* This function is to search an unused index number but not the number next to the largest
 *			index number.
 *
 ******************************************************************************/
int mapi_ccfg_idx_find_unused( int tid, char* sect, char* name, int iFrom)
{
	mapi_ccfg_tuple_t	ccfg;
	char				fmt[ MAPI_CCFG_IDX_NAME_FMT_MAXLEN ];
	int					iOff;
	char*				pPtr;

	if (sect == NULL || name == NULL || iFrom < 0)
		return MID_FAIL;

	strncpy( ccfg.section, sect, sizeof(ccfg.section)-1 );
	ccfg.section[ sizeof(ccfg.section)-1 ] = '\0';

	strcpy( fmt, _namePatternFmt );
	if ( (pPtr=strstr(fmt,"%u")) == NULL )
		return MID_FAIL;

	iOff = (int)(pPtr-fmt);
	fmt[iOff+0] = '=';
	fmt[iOff+1] = '=';
	snprintf( ccfg.name, sizeof(ccfg.name), fmt, name );
	pPtr = strstr( ccfg.name, "==" );
	pPtr[0] = '%';
	pPtr[1] = 'u';

	ccfg.type = iFrom;

	return mapi_ccfg_find_unused( tid, &ccfg );
}

/*******************************************************************************
 * Description
 *		Switch to use the factory default configuration file or
 *      switch back to the normal configuration file
 *
 * Parameters
 *		tid:	    transaction ID returned by calling mapi_start_transc()
 *      switch_in:  use 1 to switch to use the factory default configuration file;
 *                  use 0 to switch back to the normal configuration file
 *
 * Returns
 *		* MID_FAIL(-1):		failed to process transaction
 *		* MID_SUCCESS(0):   successful
 *
 * Note
 *		* 
 *
 ******************************************************************************/
int   mapi_ccfg_switch_in_out_factory_cfg( int tid, int switch_in)
{
    int                 buf[CCFG_MSG_BUF_LEN];
    mid_msg_t           *msg;
    int                 slen, rlen;
    unsigned short      act_code;

    //set msg header
    msg = (mid_msg_t *)buf;
    if(switch_in)
        act_code=MID_MSG_ACT_CFG_OPENDFT;
    else
        act_code=MID_MSG_ACT_CFG_CLOSEDFT;
    mapi_set_trnx_msghd(msg, MID_MSG_MOD_COMMONCFG, act_code, sizeof(mid_msg_header_t));
        
    //send the request
    slen = mapi_tx_transc_req(tid, msg);
    if (slen != msg->header.mlen) {
        MID_TRACE("[%s] mapi_tx_transc_req() to send %d, sent %d\n", __FUNCTION__, msg->header.mlen, slen);
        
        return MID_FAIL;
    }
    
    //get response
    memset(buf, 0, sizeof(mid_msg_header_t));
    rlen=mapi_rx_transc_rsp(tid, msg, sizeof(buf));
    if(rlen<=0){
        MID_TRACE("[%s] mapi_rx_transc_rsp() return %d\n", __FUNCTION__, rlen);
        
        return MID_FAIL;
    }
    
    //parse content
    if(msg->header.act_rsp_code!=MID_CCFG_RSP_SUCCESS){
        MID_TRACE("[%s] fail, rsp code=%u\n", __FUNCTION__, msg->header.act_rsp_code);
        
        return MID_FAIL;
    }

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		import config from other file
 *
 * Parameters
 *		tid:	    transaction ID returned by calling mapi_start_transc()
 *		filename:   import file name
 *      type:  		replace 0, merge(new config has higher priority) 1
 *		imList:		a list to decide import parameters
 *
 * Returns
 *		* MID_FAIL(-1):		failed to process transaction
 *		* MID_SUCCESS(0):   successful
 *
 * Note
 *		*
 *
 ******************************************************************************/
int mapi_ccfg_import( int tid, char* filename, int iType, char *imList)
{
    mapi_ccfg_tuple_t ptuple;
    mid_msg_t           *msg;
    int                 slen, rlen;
    int                 buf[CCFG_MSG_BUF_LEN];
    if((filename==NULL)
    	||(iType < 0)
    	){
        MID_TRACE("[%s] NULL filename or wrong type\n", __FUNCTION__);

        return MID_FAIL;
    }
    //set msg header
    msg=(mid_msg_t *)buf;
    mapi_set_trnx_msghd(msg, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_IMPORT, sizeof(mid_msg_header_t)+sizeof(mapi_ccfg_tuple_t));
    //set msg data, may be a data structure for the handler's convenience to parse it
    ptuple.type = iType;
    strcpy((char*)ptuple.value, filename);
    strcpy((char*)ptuple.value2, imList);
    memcpy(&(msg->data[0]), &ptuple, sizeof(mapi_ccfg_tuple_t));

    //send the request
    slen=mapi_tx_transc_req(tid, msg);
    if(slen!=msg->header.mlen){
        MID_TRACE("[%s] mapi_tx_transc_req() to send %d, sent %d\n", __FUNCTION__, msg->header.mlen, slen);

        return MID_FAIL;
    }
    //get response
    memset(buf, 0, sizeof(mid_msg_header_t)+sizeof(mapi_ccfg_tuple_t));
    rlen=mapi_rx_transc_rsp(tid, msg, sizeof(buf));
    if(rlen<=0){
        MID_TRACE("[%s] mapi_rx_transc_rsp() return %d\n", __FUNCTION__, rlen);

        return MID_FAIL;
    }
    //parse content
    //memcpy((char *)ptuple, &(msg->data[0]), sizeof(mapi_ccfg_tuple_t));
    //MID_TRACE("[%s] %s@%s=%s\n", __FUNCTION__, ptuple->name, ptuple->section, ptuple->value);
    if((msg->header.act_rsp_code&MID_MSG_RSP_MASK)!=MID_MSG_RSP_SUCCESS){
        MID_TRACE("[%s] fail, rsp code=%u\n", __FUNCTION__, msg->header.act_rsp_code);

        return MID_FAIL;
    }
	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		export config to other file
 *
 * Parameters
 *		tid:	    transaction ID returned by calling mapi_start_transc()
 *		sFileName:   import file name
 *      type:  		raw, encode by list define
 *		exList:		a list to decide export parameters and encode target
 *
 * Returns
 *		* MID_FAIL(-1):		failed to process transaction
 *		* MID_SUCCESS(0):   successful
 *
 * Note
 *		*
 *
 ******************************************************************************/
int mapi_ccfg_export( int tid, char* sFileName, int iType, char *sExList)
{
	mapi_ccfg_tuple_t ptuple;
	int		slen, rlen;
	int                 buf[CCFG_MSG_BUF_LEN];
	mid_msg_t           *msg;

	if ( sFileName == NULL || (slen=strlen(sFileName)) >= MID_CCFG_MAX_FILENAME)
		return MID_FAIL;

    msg=(mid_msg_t *)buf;
    mapi_set_trnx_msghd(msg, MID_MSG_MOD_COMMONCFG, MID_MSG_ACT_CFG_EXPORT, sizeof(mid_msg_header_t)+sizeof(mapi_ccfg_tuple_t));
    ptuple.type = iType;
    strcpy((char*)ptuple.value, sFileName);
    strcpy((char*)ptuple.value2, sExList);
    memcpy(&(msg->data[0]), &ptuple, sizeof(mapi_ccfg_tuple_t));

    //send the request
    slen=mapi_tx_transc_req(tid, msg);
    if(slen!=msg->header.mlen){
        MID_TRACE("[%s] mapi_tx_transc_req() to send %d, sent %d\n", __FUNCTION__, msg->header.mlen, slen);

        return MID_FAIL;
    }

    //get response
    memset(buf, 0, sizeof(mid_msg_header_t)+sizeof(mapi_ccfg_tuple_t));
    rlen=mapi_rx_transc_rsp(tid, msg, sizeof(buf));
    if(rlen<=0){
        MID_TRACE("[%s] mapi_rx_transc_rsp() return %d\n", __FUNCTION__, rlen);

        return MID_FAIL;
    }
    //parse content
    //memcpy((char *)ptuple, &(msg->data[0]), sizeof(mapi_ccfg_tuple_t));
    //MID_TRACE("[%s] %s@%s=%s\n", __FUNCTION__, ptuple->name, ptuple->section, ptuple->value);
    if((msg->header.act_rsp_code&MID_MSG_RSP_MASK)!=MID_MSG_RSP_SUCCESS){
        MID_TRACE("[%s] fail, rsp code=%u\n", __FUNCTION__, msg->header.act_rsp_code);

        return MID_FAIL;
    }
	return MID_SUCCESS;
    return MID_SUCCESS;
}