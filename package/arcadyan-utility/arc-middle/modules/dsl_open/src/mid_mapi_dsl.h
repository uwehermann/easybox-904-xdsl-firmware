/*
 * Arcadyan middle layer header file of APIs of DSL
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_MAPI_DSL_H_
#define _MID_MAPI_DSL_H_


#include "mid_dsl.h"


#ifdef __cplusplus
extern "C" {
#endif


extern int mapi_dsl_state_get( int tid, mid_dsl_state_t* pstBuf );
extern int mapi_dsl_counter_get( int tid, mid_dsl_counter_t* pstBuf );
extern int mapi_dsl_reconnect( int tid);



#ifdef __cplusplus
}
#endif


#endif // _MID_MAPI_DSL_H_
