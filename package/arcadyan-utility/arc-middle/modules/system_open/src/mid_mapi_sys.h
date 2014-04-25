/*
 * Arcadyan middle layer header file of APIs of misc system access
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_MAPI_SYS_H_
#define _MID_MAPI_SYS_H_


#include "mid_sys.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MAPI_SYS_FWUG_PATH "/mnt/misc/fw-upgrade"
#define MAPI_SYS_FWUG_EXTRA_BUF (10 * 1024 * 1024) /* 10 MB for extra fw upgrade buffer plus fw size for garantee successfuly upgrade */

typedef enum {
	MAPI_SYS_FWUG_STORE_MEM = 0,
	MAPI_SYS_FWUG_STORE_FLASH
} mapi_sys_fw_store_e;

extern float mapi_sys_cpu_usage(void);
extern int mapi_sys_fwug_store(int fw_size_bytes);
extern int mapi_sys_tmp_free(void);
extern int mapi_sys_mem_free(void);
extern float mapi_sys_mem_usage(void);
extern int mapi_sys_fw_upgrade( int tid, mid_sys_fw_upgrade_t* pstData );
extern int mapi_sys_fw_type( int tid, mid_sys_fw_upgrade_t* pstData );
extern int mapi_sys_timezone_list_get( int tid, mid_sys_timezone_list_t* pstBuf );
extern int mapi_sys_time_get( int tid, mid_sys_time_t* pstBuf );
extern int mapi_sys_time_set( int tid, mid_sys_time_t* pstData );
extern int mapi_sys_led_control( int tid, mid_sys_led_control_t* pstData );
extern int mapi_sys_led_status( int tid, mid_sys_led_control_t* pstData );
extern int mapi_sys_reboot( int tid, unsigned short delay );
extern int mapi_sys_ssh_status_get( int tid, mid_sys_ssh_t* pstBuf );
extern int mapi_sys_ssh_status_set( int tid, mid_sys_ssh_t data );
extern int mapi_sys_verify_MIC(int tid, char *PINCode);

#ifdef __cplusplus
}
#endif


#endif // _MID_MAPI_SYS_H_
