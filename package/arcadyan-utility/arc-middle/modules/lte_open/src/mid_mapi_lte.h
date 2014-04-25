/*
 * Arcadyan middle layer header file of APIs of LTE
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_MAPI_LTE_H_
#define _MID_MAPI_LTE_H_


#include "mid_lte.h"


#ifdef __cplusplus
extern "C" {
#endif


extern int mapi_lte_status_get( int tid, stLteStatus* pstBuf );
extern int mapi_status_lte_get( int tid, stStatusLte* pstBuf );
extern int mapi_lte_Give_LTE_SIM_PIN( int tid, char *pin, char *newPinCode );
extern int mapi_lte_Give_UMTS_SIM_PIN( int tid, char *pin, char *newPinCode );
extern int mapi_lte_Change_UMTS_SIM_PIN( int tid, char *pin, char *newPinCode );
extern int mapi_lte_set_backup_mode (int tid, int mode);
extern int mapi_lte_set_UMTS_store_PIN (int tid, int store);
extern int mapi_lte_set_UMTS_redir_confirm (int tid, int conf);
extern int mapi_lte_set_UMTS_idle_time (int tid, unsigned long idletime);

#ifdef __cplusplus
}
#endif


#endif // _MID_MAPI_LTE_H_
