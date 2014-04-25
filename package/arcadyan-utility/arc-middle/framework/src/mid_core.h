/*
 * Arcadyan middle layer core hearder file
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _mid_core_h_
#define _mid_core_h_

#include <pthread.h>

#define MID_MODULE_INI_SUPPORT

#define MID_REG_MODULE_DIR  "/etc/config/arc-middle/register"   //the module-register dir
#define MID_REG_MODULE_LIB_NAME_MAXLEN  128
#define MID_REG_MODULE_FUN_NAME_MAXLEN  64
#define MID_REG_TABLE_SIZE              MID_MAX_MODULE_NUM
#define MID_MAX_CONCURRENT_DL_COUNT     32

typedef struct mid_reg {
    char            lib_name[MID_REG_MODULE_LIB_NAME_MAXLEN];
    char            hdl_name[MID_REG_MODULE_FUN_NAME_MAXLEN];
#ifdef MID_MODULE_INI_SUPPORT
    char            ini_name[MID_REG_MODULE_FUN_NAME_MAXLEN];
#endif
    void            *libp;
	void            *module_hdl;
#ifdef MID_MODULE_INI_SUPPORT
	void            *module_ini;
#endif
	pthread_mutex_t module_mutex;
	unsigned long   last_hit_t;
	int             ref_cnt;
} mid_reg_t;

typedef struct mid_thread{
    pthread_t   id;             //thread id
    int         transc_count;   //served transaction numbers
} mid_thread_t;

#endif // _mid_core_h_
