/*
 * Arcadyan middle layer header file of APIs for Network Monitor, including connection manager
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_MAPI_MON_H_
#define _MID_MAPI_MON_H_


#include "mid_mon.h"


#ifdef __cplusplus
extern "C" {
#endif


extern int mapi_mon_start_daemon( int tid );
extern int mapi_mon_restart_daemon( int tid );
extern int mapi_mon_lte_data_connreq( int tid );
extern int mapi_mon_lte_data_disconnreq( int tid );

extern int mapi_mon_lte_voice_connreq( int tid );
extern int mapi_mon_lte_voice_disconnreq( int tid );

extern int mapi_mon_umts_connreq( int tid );
extern int mapi_mon_umts_disconnreq( int tid );

extern int mapi_mon_set_upgrade_state( int tid, int state);
extern int mapi_mon_set_backup_mode( int tid, int mode);
extern int mapi_mon_set_umts_idletime( int tid, unsigned long idletime);

#ifdef __cplusplus
}
#endif


#endif // _MID_MAPI_MON_H_
