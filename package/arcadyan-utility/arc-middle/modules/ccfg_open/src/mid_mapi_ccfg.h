/*
 * Arcadyan middle layer header file of APIs of base connfiguration
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _mid_mapi_ccfg_h_
#define _mid_mapi_ccfg_h_

#include <umngcfg.h>
#include "mid_ccfg.h"

#define MAPI_CCFG_COMMIT_FILE   MID_CCFG_COMMIT_FILE
#define MAPI_CCFG_COMMIT_FLASH  MID_CCFG_COMMIT_FLASH
#define MAPI_CCFG_COMMIT_SYNC   MID_CCFG_COMMIT_SYNC

typedef mid_ccfg_tuple_t    mapi_ccfg_tuple_t;
//the design uses mapi_ccfg_tuple_t for application to specify the configuration variable he wants to access
//here's what it looks like (defined in mid_cfg.h)
//typedef struct mid_ccfg_tuple {
//	char            section[MID_CCFG_MAX_SECTION_NAME_LEN];
//	char            name[MID_CCFG_MAX_PARAM_NAME_LEN];
//	int             type;
//	unsigned char   value[MID_CCFG_MAX_VALUE_LEN];
//} mid_ccfg_tuple_t;
//
//the section fiels can be the name of a specific section to be access or a specific command prefixed with '!':
// ![index] :    the target is at the index-th section
// !.       :    keep in the current section
// !>       :    go to the next section
//
//the name field can be the specific variable name to be access or a specific command prefixed with '!':
// ![index] :    the target is at the index-th variable in the section
// !.       :    keep in the current variable
// !>       :    go to the next variable of the section
//the type field is to specify how to interprete the value[] field, defined in mid_ccfg_var_type_t


#ifdef __cplusplus
extern "C" {
#endif


extern int mapi_ccfg_get( int tid, mapi_ccfg_tuple_t* ptuple);
extern int mapi_ccfg_set( int tid, mapi_ccfg_tuple_t* ptuple);
extern int mapi_ccfg_del( int tid, mapi_ccfg_tuple_t* ptuple);
extern int mapi_ccfg_commit( int tid, char commit_mask, char* sSect);
extern int mapi_ccfg_commit_send( int tid, char commit_mask, char* sSect);
extern int mapi_ccfg_commit_recv( int tid);
extern int mapi_ccfg_commit_lazy( int tid, char commit_mask, char* sSect);
extern int mapi_ccfg_commit_lazy_send( int tid, char commit_mask, char* sSect);
extern int mapi_ccfg_del_multi( int tid, mapi_ccfg_tuple_t* ptuple);
extern int mapi_ccfg_search( int tid, mapi_ccfg_tuple_t* ptuple);
extern int mapi_ccfg_find_unused( int tid, mapi_ccfg_tuple_t* ptuple);
extern int mapi_ccfg_backup( int tid, char* sFileName);
extern int mapi_ccfg_backup_partial( int tid, char* sFileName);
extern int mapi_ccfg_restore( int tid, char* sFileName);
extern int mapi_ccfg_restore_partial( int tid, char* sFileName);
extern int mapi_ccfg_dup( int tid, char* sDstSect, char* sSrcSect);

extern char* mapi_ccfg_get_str( int tid, char* sect, char* name, char* buf, int bufSz, char* sDft );
extern long  mapi_ccfg_get_long( int tid, char* sect, char* name, long lDft );
extern int   mapi_ccfg_set_str( int tid, char* sect, char* name, char* sVal );
extern int   mapi_ccfg_set_long( int tid, char* sect, char* name, long iVal );
extern int   mapi_ccfg_unset( int tid, char* sect, char* name );
extern int   mapi_ccfg_unset_multi( int tid, char* sect, char* name );
extern int   mapi_ccfg_get_dft( int tid, mapi_ccfg_tuple_t* ptuple, char* sDft );
extern int   mapi_ccfg_get_str2( int tid, char* sect, char* name, char* buf, int bufSz );
extern int   mapi_ccfg_get_str_dft( int tid, char* sect, char* name, char* buf, int bufSz, char* sDft );
extern int   mapi_ccfg_get_cmp_str( int tid, char* sect, char* name, char* buf, int bufSz, char* sStr );
extern int   mapi_ccfg_cmp_str( int tid, char* sect, char* name, char* sStr );
extern int   mapi_ccfg_cmp_str_dft( int tid, char* sect, char* name, char* sStr, char* sDft );
extern int   mapi_ccfg_cmp_long( int tid, char* sect, char* name, long lVal );
extern int   mapi_ccfg_cmp_long_dft( int tid, char* sect, char* name, long lVal, long lDft );
extern int   mapi_ccfg_update_str( int tid, char* sect, char* name, char* sStr );
extern int   mapi_ccfg_update_long( int tid, char* sect, char* name, long lVal );

extern int   mapi_ccfg_idx_set_pattern( char* namePat );
extern char* mapi_ccfg_idx_get_str( int tid, char* sect, char* name, unsigned int idx, char* buf, int bufSz, char* sDft );
extern long  mapi_ccfg_idx_get_long( int tid, char* sect, char* name, unsigned int idx, long lDft );
extern int   mapi_ccfg_idx_set_str( int tid, char* sect, char* name, unsigned int idx, char* sVal );
extern int   mapi_ccfg_idx_set_long( int tid, char* sect, char* name, unsigned int idx, long iVal );
extern int   mapi_ccfg_idx_unset( int tid, char* sect, char* name, unsigned int idx );
extern int   mapi_ccfg_idx_get_cmp_str( int tid, char* sect, char* name, unsigned int idx, char* buf, int bufSz, char* sStr );
extern int   mapi_ccfg_idx_cmp_str( int tid, char* sect, char* name, unsigned int idx, char* sStr );
extern int   mapi_ccfg_idx_cmp_str_dft( int tid, char* sect, char* name, unsigned int idx, char* sStr, char* sDft );
extern int   mapi_ccfg_idx_cmp_long( int tid, char* sect, char* name, unsigned int idx, long lVal );
extern int   mapi_ccfg_idx_cmp_long_dft( int tid, char* sect, char* name, unsigned int idx, long lVal, long lDft );
extern int   mapi_ccfg_idx_update_str( int tid, char* sect, char* name, unsigned int idx, char* sStr );
extern int   mapi_ccfg_idx_update_str( int tid, char* sect, char* name, unsigned int idx, char* sStr );
extern int   mapi_ccfg_idx_update_long( int tid, char* sect, char* name, unsigned int idx, long lVal );
extern int   mapi_ccfg_idx_unset_all( int tid, char* sect, unsigned int idx );
extern int   mapi_ccfg_idx_search_by_val( int tid, char* sect, char* name, char* sVal, int iOrder);
extern int   mapi_ccfg_idx_search_any_by_val( int tid, char* sect, char* name, char* sVal);
extern int   mapi_ccfg_idx_search_first_by_val( int tid, char* sect, char* name, char* sVal);
extern int   mapi_ccfg_idx_search_last_by_val( int tid, char* sect, char* name, char* sVal);
extern int   mapi_ccfg_idx_find_unused( int tid, char* sect, char* name, int iFrom);

extern int   mapi_ccfg_switch_in_out_factory_cfg( int tid, int switch_in);

extern int	 mapi_ccfg_import( int tid, char* filename, int iType, char *imList);


#ifdef __cplusplus
}
#endif


#endif // _mid_mapi_ccfg_h_
