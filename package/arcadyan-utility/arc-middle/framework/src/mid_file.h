/*
 * Arcadyan middle layer core hearder file
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _mid_file_h_
#define _mid_file_h_


#include <umngcfg.h>


#ifdef __cplusplus
extern "C" {
#endif

extern umngcfg_handle_t*	umngcfg_openFile( void );
extern void					umngcfg_closeFile( void );
extern umngcfg_handle_t*	umngcfg_fileHdl( void );

extern umngcfg_handle_t*	umngcfg_openFactoryDefault( void );
extern void					umngcfg_closeFactoryDefault( void );
extern umngcfg_handle_t*	umngcfg_fileHdlFactoryDefault( void );

#ifdef __cplusplus
}
#endif


#endif // _mid_file_h_
