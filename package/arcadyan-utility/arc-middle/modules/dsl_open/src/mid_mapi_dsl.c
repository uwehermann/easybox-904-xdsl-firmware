/*
 * Arcadyan middle layer APIs for DSL
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#include <stdio.h>
#include <string.h>
#include "mid_mapi_dsl.h"

/*
 * -- Helper functions --
 */

/*
 * -- Public functions --
 */

/*******************************************************************************
 * Description
 *		get the DSL line status
 *
 * Parameters
 *		pstBuf:	pointer to buffer of DSL line status to be retrieved.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed
 *
 * Note
 *		* this function get firnware version, link status and line rate
 *
 ******************************************************************************/
int mapi_dsl_state_get( int tid, mid_dsl_state_t* pstBuf )
{
	if (pstBuf == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( pstBuf, 0, sizeof(*pstBuf) );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_DSL, MID_DSL_STAT_GET, pstBuf, sizeof(*pstBuf) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		get the DSL PM counter from the lastest linkup
 *
 * Parameters
 *		pstBuf:	pointer to buffer of DSL PM counter to be retrieved.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed
 *
 * Note
 *		* this function get PM counter, including xDSL line, ATM and PTM
 *
 ******************************************************************************/
int mapi_dsl_counter_get( int tid, mid_dsl_counter_t* pstBuf )
{
	if (pstBuf == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( pstBuf, 0, sizeof(*pstBuf) );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_DSL, MID_DSL_COUNTER_GET, pstBuf, sizeof(*pstBuf) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

    return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		force the DSL line reconnect
 *
 * Parameters
 *		none
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed
 *
 * Note
 *		* this function force dsl reconnect/retrain.
 *
 ******************************************************************************/
int mapi_dsl_reconnect( int tid)
{
	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_DSL, MID_DSL_RECONNECT, NULL, 0 ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

    return MID_SUCCESS;
}

