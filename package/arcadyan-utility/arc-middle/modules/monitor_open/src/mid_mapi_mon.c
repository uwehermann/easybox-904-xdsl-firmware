/*
 * Arcadyan middle layer APIs for Network Monitor, including connection manager
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libArcComApi.h>
#include <mid_fmwk.h>
#include "mid_mapi_mon.h"
#include "mid_mapi_ccfg.h"


/*
 * -- Helper functions --
 */

/*
 * -- Public functions --
 */


/*******************************************************************************
 * Description
 *		Start the monitor daemon
 *
 * Parameters
 *      tid:        the transaction handler
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_start_daemon( int tid )
{
    int         rst=MID_SUCCESS, new_transc=0;
    stMonConfig	config;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	memset((char*)&config, 0x0, sizeof(config));
	config.backup_mode = mapi_ccfg_get_long( tid, "lte", "backup_mode", 0);
	config.umts_idletime = mapi_ccfg_get_long( tid, "lte", "idletime", 5);

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_DAEMON_START, &config, sizeof(config) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;	    
}

/*******************************************************************************
 * Description
 *		Stop the monitor daemon
 *
 * Parameters
 *      tid:        the transaction handler
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_stop_daemon( int tid )
{
    int         rst=MID_SUCCESS, new_transc=0;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_DAEMON_STOP, NULL, 0 ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;	    
}

/*******************************************************************************
 * Description
 *		Restart the monitor daemon
 *
 * Parameters
 *      tid:        the transaction handler
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_restart_daemon( int tid )
{
    int         rst=MID_SUCCESS, new_transc=0;
    stMonConfig	config;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	memset((char*)&config, 0x0, sizeof(config));
	config.backup_mode = mapi_ccfg_get_long( tid, "lte", "backup_mode", 0);

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_DAEMON_RESTART, &config, sizeof(config) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;	    
}


/*******************************************************************************
 * Description
 *		Send LTE Data connection request to the daemon
 *
 * Parameters
 *      tid:        the transaction handler
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_lte_data_connreq( int tid )
{
	mid_mon_connreq_t connreq;
    int         rst=MID_SUCCESS, new_transc=0;
	
	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( &connreq, 0, sizeof(connreq) );
	connreq.wan_idx = MID_MON_WAN_LTE_DATA;
	connreq.req_type = MID_MON_CONNREQ_DIAL;

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_4G3G_CONN_REQUEST, &connreq, sizeof(connreq) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *		Send LTE Data dis-connection request to the daemon
 *
 * Parameters
 *      tid:        the transaction handler
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_lte_data_disconnreq( int tid )
{
	mid_mon_connreq_t connreq;
    int         rst=MID_SUCCESS, new_transc=0;
	
	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( &connreq, 0, sizeof(connreq) );
	connreq.wan_idx = MID_MON_WAN_LTE_DATA;
	connreq.req_type = MID_MON_CONNREQ_HANGUP;

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_4G3G_CONN_REQUEST, &connreq, sizeof(connreq) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *		Send LTE voice connection request to the daemon
 *
 * Parameters
 *      tid:        the transaction handler
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_lte_voice_connreq( int tid )
{
	mid_mon_connreq_t connreq;
    int         rst=MID_SUCCESS, new_transc=0;
	
	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( &connreq, 0, sizeof(connreq) );
	connreq.wan_idx = MID_MON_WAN_LTE_VOICE;
	connreq.req_type = MID_MON_CONNREQ_DIAL;

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_4G3G_CONN_REQUEST, &connreq, sizeof(connreq) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *		Send LTE Voice dis-connection request to the daemon
 *
 * Parameters
 *      tid:        the transaction handler
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_lte_voice_disconnreq( int tid )
{
	mid_mon_connreq_t connreq;
    int         rst=MID_SUCCESS, new_transc=0;
	
	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( &connreq, 0, sizeof(connreq) );
	connreq.wan_idx = MID_MON_WAN_LTE_VOICE;
	connreq.req_type = MID_MON_CONNREQ_HANGUP;

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_4G3G_CONN_REQUEST, &connreq, sizeof(connreq) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *		Send UMTS connection request to the daemon
 *
 * Parameters
 *      tid:        the transaction handler
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_umts_connreq( int tid )
{
    int         rst=MID_SUCCESS, new_transc=0;
	mid_mon_connreq_t connreq;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( &connreq, 0, sizeof(connreq) );
	connreq.wan_idx = MID_MON_WAN_UMTS;
	connreq.req_type = MID_MON_CONNREQ_DIAL;

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_4G3G_CONN_REQUEST, &connreq, sizeof(connreq) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *		Send UMTS dis-connection request to the daemon
 *
 * Parameters
 *      tid:        the transaction handler
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_umts_disconnreq( int tid )
{
    int         rst=MID_SUCCESS, new_transc=0;
	mid_mon_connreq_t connreq;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( &connreq, 0, sizeof(connreq) );
	connreq.wan_idx = MID_MON_WAN_UMTS;
	connreq.req_type = MID_MON_CONNREQ_HANGUP;

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_4G3G_CONN_REQUEST, &connreq, sizeof(connreq) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *		Get LTE status
 *
 * Parameters
 *      tid:        the transaction handler
 *      connreq:    pointer to mid_mon_connreq_t, containing request content
 *                  required fields: wan_idx and 
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_get_status( int tid, stConnectionStatus *status)
{
    int         rst=MID_SUCCESS, new_transc=0;

	if (status == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( status, 0, sizeof(stConnectionStatus) );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_LTE_STATUS_GET, status, sizeof(stConnectionStatus) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}
	
	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *		Set LTE module upgrade
 *
 * Parameters
 *      tid:        the transaction handler
 *      state:    	1: upgrade start, 0: upgrade stop
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_set_upgrade_state( int tid, int state)
{
    int         rst=MID_SUCCESS, new_transc=0;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_SET_UPGRADE_STATE, &state, sizeof(state) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}
	
	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *		Set UMTS upgrade mode
 *
 * Parameters
 *      tid:        the transaction handler
 *      mode:    	0: Data+Voice, 1: Data, 2: Voice, 3: No UMTS, 4: Only UMTS
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_set_backup_mode( int tid, int mode)
{
    int         rst=MID_SUCCESS, new_transc=0;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}
	
	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_SET_BACKUP_MODE, &mode, sizeof(mode) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}
	
	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *		Set UMTS idle timeout
 *
 * Parameters
 *      tid:        the transaction handler
 *      idletime:   timeout value in minute
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_set_umts_idletime( int tid, unsigned long idletime)
{
    int         rst=MID_SUCCESS, new_transc=0;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}
	
	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_SET_UMTS_IDLETIME, &idletime, sizeof(idletime) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}
	
	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *		Set if skip provision check
 *
 * Parameters
 *      tid:        the transaction handler
 *      mode:    	0: not skip, 1: skip the provision check
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_mon_set_skip_provision( int tid, int skip)
{
    int         rst=MID_SUCCESS, new_transc=0;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}
	
	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_MON, MID_MON_SET_SKIP_PROVISION, &skip, sizeof(skip) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}
	
	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}
