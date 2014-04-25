/*
 * Arcadyan middle layer APIs for DSL
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#include <stdio.h>
#include <string.h>

#include <libArcComApi.h>



#include "mid_mapi_wlan.h"
#include "mid_mapi_ccfg.h"

#define	CNT_OFFSET( memberName )	UTIL_OFFSETOF( mapi_ccfg_wlan_counter_t, memberName )


typedef struct {
	unsigned short	iOffset;
	char			sFieldKey[28];
	unsigned char	bU64;
	unsigned char	reserved;
} _stWlanCnt;


/*
 * -- Helper functions --
 */

/*
 * -- Public functions --
 */


/*******************************************************************************
 * Description
 *		Get a WLAN interface statistics
 *
 * Parameters
 *		tid:		transaction ID returned by calling mapi_start_transc()
 *					use 0 if no tid is provided (no transaction was opened before)
 *		pstBuf:		pointer to the buffer of data structure mapi_ccfg_wlan_status_t.
 *						pstBuf->port_idx is required.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL
 *							or pstBuf->band or pstBuf->ap_idx is incorrect.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_wlan_counter_get( int tid, mid_wlan_counter_t* pstBuf )
{
    int     new_transc = 0;

	if ( pstBuf == NULL )
	{
		MID_TRACE("[%s] NULL pstBuf\n", __FUNCTION__);
		return MID_FAIL;
	}

    if ( tid == 0 )
	{
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
		new_transc = 1;
	}

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_WLAN, MID_WLAN_COUNTER_GET, pstBuf, sizeof(*pstBuf) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
	    if ( new_transc )
		    mapi_end_transc(tid);
		return MID_FAIL;
	}

	if ( new_transc )
	    mapi_end_transc(tid);

	return MID_SUCCESS;

}




/*******************************************************************************
 * Description
 *		Get a WLAN interface connected stations list
 *
 * Parameters
 *		tid:		transaction ID returned by calling mapi_start_transc()
 *					use 0 if no tid is provided (no transaction was opened before)
 *		pstBuf:		pointer to the buffer of data structure mapi_ccfg_wlan_sta_t.
 *						pstBuf->port_idx is required.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL
 *							or pstBuf->band or pstBuf->apidx is incorrect.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_wlan_sta_get( int tid, mid_wlan_sta_t* pstBuf )
{
    int     new_transc = 0;

	if ( pstBuf == NULL )
	{
		MID_TRACE("[%s] NULL pstBuf\n", __FUNCTION__);
		return MID_FAIL;
	}

    if ( tid == 0 )
	{
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
		new_transc = 1;
	}

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_WLAN, MID_WLAN_STA_GET, pstBuf, sizeof(*pstBuf) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
	    if ( new_transc )
		    mapi_end_transc(tid);
		return MID_FAIL;
	}

    if ( new_transc )
	    mapi_end_transc(tid);

	return MID_SUCCESS;
}


int mapi_wlan_wps( int tid, mid_wlan_wps_t* pstBuf )
{
    int     new_transc = 0;

	if ( pstBuf == NULL )
	{
		MID_TRACE("[%s] NULL pstBuf\n", __FUNCTION__);
		return MID_FAIL;
	}

    if ( tid == 0 )
	{
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
		new_transc = 1;
	}

#ifdef MAPI_CCFG_WLAN_MFR_GUI_SUPPORT
	if(pstBuf->action == MID_WLAN_PIN)
	{
		if(mapi_ccfg_set_long(tid, "wireless", "WPS_STATUS", WPS_STATUS_GUI_PIN_START) != MID_SUCCESS)
			printf("%s.%d> Set WPS_STATUS_GUI_PIN_START fail.\n", __FUNCTION__, __LINE__);
	}
	else if(pstBuf->action == MID_WLAN_PBC)
	{
		if(mapi_ccfg_set_long(tid, "wireless", "WPS_STATUS", WPS_STATUS_GUI_PBC_START) != MID_SUCCESS)
			printf("%s.%d> Set WPS_STATUS_GUI_PBC_START fail.\n", __FUNCTION__, __LINE__);
	}
#endif

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_WLAN, MID_WLAN_WPS, pstBuf, sizeof(*pstBuf) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
	    if ( new_transc )
		    mapi_end_transc(tid);
		return MID_FAIL;
	}

    if ( new_transc )
	    mapi_end_transc(tid);

	return MID_SUCCESS;

}


int mapi_wlan_wsc_get_profile( int tid, mid_wlan_wsc_profile_t* pstBuf )
{
    int     new_transc = 0;

	if ( pstBuf == NULL )
	{
		MID_TRACE("[%s] NULL pstBuf\n", __FUNCTION__);
		return MID_FAIL;
	}

    if ( tid == 0 )
	{
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
		new_transc = 1;
	}

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_WLAN, MID_WLAN_WSC_GET_PROFILE, pstBuf, sizeof(*pstBuf) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
	    if ( new_transc )
		    mapi_end_transc(tid);
		return MID_FAIL;
	}

    if ( new_transc )
	    mapi_end_transc(tid);

	return MID_SUCCESS;

}


int mapi_wlan_wsc_get_status( int tid, mid_wlan_wsc_status_t* pstBuf )
{
    int     new_transc = 0;

	if ( pstBuf == NULL )
	{
		MID_TRACE("[%s] NULL pstBuf\n", __FUNCTION__);
		return MID_FAIL;
	}

    if ( tid == 0 )
	{
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
		new_transc = 1;
	}

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_WLAN, MID_WLAN_WSC_GET_STATUS, pstBuf, sizeof(*pstBuf) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
	    if ( new_transc )
		    mapi_end_transc(tid);
		return MID_FAIL;
	}

    if ( new_transc )
	    mapi_end_transc(tid);

	return MID_SUCCESS;

}


int mapi_wlan_scan( int tid, mid_wlan_scan_t* pstBuf )
{
    int     new_transc = 0;

	if ( pstBuf == NULL )
	{
		MID_TRACE("[%s] NULL pstBuf\n", __FUNCTION__);
		return MID_FAIL;
	}

    if ( tid == 0 )
	{
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
		new_transc = 1;
	}
	//printf("sizeof(*pstBuf)=%d\n",sizeof(*pstBuf));
	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_WLAN, MID_WLAN_SCAN, pstBuf, sizeof(*pstBuf) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
	    if ( new_transc )
		    mapi_end_transc(tid);
		return MID_FAIL;
	}

    if ( new_transc )
	    mapi_end_transc(tid);

	return MID_SUCCESS;

}




int mapi_wlan_wsc_gen_pin(int tid, mid_wlan_gen_pin_t* pstBuf )
{
    int     new_transc = 0,rst = MID_FAIL;
	unsigned char band, ap_idx,band_start,band_end,ap_start,ap_end;
	mapi_ccfg_wlan_wps_t wlan_wps_cfg;

	if ( pstBuf == NULL )
	{
		MID_TRACE("[%s] NULL pstBuf\n", __FUNCTION__);
		return MID_FAIL;
	}

    if ( tid == 0 )
	{
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
		new_transc = 1;
	}
	//printf("sizeof(*pstBuf)=%d\n",sizeof(*pstBuf));
	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_WLAN, MID_WLAN_WSC_GEN_PIN, pstBuf, sizeof(*pstBuf) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
	    if ( new_transc )
		    mapi_end_transc(tid);
		return MID_FAIL;
	}
	switch (pstBuf->sync_mode){
		case MID_WLAN_SYNC_SELF:
			band_start=pstBuf->band;
			band_end=pstBuf->band;			
			ap_start=pstBuf->ap_idx;
			ap_end=pstBuf->ap_idx;			
			break;
		case MID_WLAN_SYNC_ALL_BAND_IDX:
			band_start=pstBuf->band;
			band_end=pstBuf->band;
			ap_start=0;
			ap_end=MAPI_CCFG_WLAN_MAX_SSID_NUM;			
			break;
		case MID_WLAN_SYNC_ALL_AP_IDX:
			band_start=0;
			band_end=MAPI_CCFG_WLAN_MAX_BAND_NUM;
			ap_start=pstBuf->ap_idx;
			ap_end=pstBuf->ap_idx;			
			break;			
		case MID_WLAN_SYNC_ALL:
		default:	
			band_start=0;
			band_end=MAPI_CCFG_WLAN_MAX_BAND_NUM;
			ap_start=0;
			ap_end=MAPI_CCFG_WLAN_MAX_SSID_NUM;
			break;			
	}
	

	for (band=band_start ; band<band_end ; band++)
	 for (ap_idx=ap_start ; ap_idx<ap_end ; ap_idx++)
	  {
	   wlan_wps_cfg.band=band;
	   wlan_wps_cfg.ap_idx=ap_idx;
	   rst = mapi_ccfg_get_wlan_wps(tid, &wlan_wps_cfg, 0);
	   if (rst == MID_FAIL) goto fail;
	   memcpy(wlan_wps_cfg.pincode,pstBuf->pin,MAPI_CCFG_WLAN_WPS_PIN_MAX_LEN);
	   rst = mapi_ccfg_set_wlan_wps(tid, &wlan_wps_cfg);  
	   if (rst == MID_FAIL) goto fail;
	  }

fail:	

    if ( new_transc )
	    mapi_end_transc(tid);

	return rst;

}


int mapi_wlan_staus_get( int tid, mid_wlan_status_t* pstBuf )
{
    int     new_transc = 0;

	if ( pstBuf == NULL )
	{
		MID_TRACE("[%s] NULL pstBuf\n", __FUNCTION__);
		return MID_FAIL;
	}

    if ( tid == 0 )
	{
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
		new_transc = 1;
	}

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_WLAN, MID_WLAN_STATUS_GET, pstBuf, sizeof(*pstBuf) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
	    if ( new_transc )
		    mapi_end_transc(tid);
		return MID_FAIL;
	}

    if ( new_transc )
	    mapi_end_transc(tid);

	return MID_SUCCESS;
}

wlan_switch_t mapi_wlan_get_switches(void)
{
	wlan_switch_t result = 0;
	int tempresult,tid;
	
	tid = mapi_start_transc();
	
	//WLAN_ENABLE
	tempresult=mapi_ccfg_get_long(tid, "wireless", "2.4G_WIRELESS_ENABLE", MID_FAIL);
	if(tempresult == 1){
		result |= WLAN_ENABLE;
	}
	
	//WLAN_ENABLE_HW_ONOFF_BUTTON
	tempresult=mapi_ccfg_get_long(tid, "wireless", "BUTTON_ACT", MID_FAIL);
	if(tempresult == 1){
		result |= WLAN_ENABLE_HW_ONOFF_BUTTON;
	}
	
	//WLAN_DISPLAY_NETWORK_KEY
	tempresult=mapi_ccfg_get_long(tid, "lcd", "wifiPwdDisplay", MID_FAIL);
	if(tempresult == 1){
		result |= WLAN_DISPLAY_NETWORK_KEY;
	}

	//WLAN_NOTIFY_NEW_DEVICE
	tempresult=mapi_ccfg_get_long(tid, "wireless", "enable_notify_connected", MID_FAIL);
	if(tempresult == 1){
		result |= WLAN_NOTIFY_NEW_DEVICE;
	}

	//WLAN_ENABLE_GUEST_WIFI
	tempresult=mapi_ccfg_get_long(tid, "wireless_profile_1", "enable", MID_FAIL);
	if(tempresult == 1){
		result |= WLAN_ENABLE_GUEST_WIFI;
	}

	//WLAN_SCHEDULE
	tempresult=mapi_ccfg_get_long(tid, "wireless_timer", "2.4G_enable", MID_FAIL);
	if(tempresult == 1){
		result |= WLAN_SCHEDULE;
	}

	//WLAN_ENABLE_WPS
	tempresult=mapi_ccfg_get_long(tid, "wps", "enable_0_0", MID_FAIL);
	if(tempresult == 1){
		result |= WLAN_ENABLE_WPS;
	}

	//WLAN_ENABLE_HW_PARING_BUTTON
	tempresult=mapi_ccfg_get_long(tid, "wireless", "WPS_PBC_HW_BTN_ENABLE", MID_FAIL);
	if(tempresult == 1){
		result |= WLAN_ENABLE_HW_PARING_BUTTON;
	}
	
	//WLAN_ENABLE_WPS_PIN
	tempresult=mapi_ccfg_get_long(tid, "wireless", "WPS_PIN_ENR_ENABLE", MID_FAIL);
	if(tempresult == 1){
		result |= WLAN_ENABLE_WPS_PIN;
	}
	
	mapi_end_transc(tid);
	
	return result;
}


