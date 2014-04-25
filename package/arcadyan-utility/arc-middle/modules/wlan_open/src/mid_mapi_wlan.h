/*
 * Arcadyan middle layer header file of APIs of misc system access
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_MAPI_WLAN_H_
#define _MID_MAPI_WLAN_H_


#include <mid_fmwk.h>
#include "mid_wlan.h"


#ifdef __cplusplus
extern "C" {
#endif



extern int mapi_wlan_sta_get( int tid, mid_wlan_sta_t* pstBuf );
extern int mapi_wlan_counter_get( int tid, mid_wlan_counter_t* pstBuf );
extern int mapi_wlan_wps( int tid, mid_wlan_wps_t* pstBuf );
extern int mapi_wlan_wsc_get_profile( int tid, mid_wlan_wsc_profile_t* pstBuf );
extern int mapi_wlan_wsc_get_status( int tid, mid_wlan_wsc_status_t* pstBuf );
extern int mapi_wlan_scan( int tid, mid_wlan_scan_t* pstBuf );
extern int mapi_wlan_wsc_gen_pin(int tid, mid_wlan_gen_pin_t* pstBuf );
extern int mapi_wlan_staus_get( int tid, mid_wlan_status_t* pstBuf );
extern wlan_switch_t mapi_wlan_get_switches(void);


#ifdef __cplusplus
}
#endif


#endif // _MID_MAPI_WLAN_H_
