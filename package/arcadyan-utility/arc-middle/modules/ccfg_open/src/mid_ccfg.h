/*
 * Arcadyan middle layer definition for common connfiguration access
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _mid_ccfg_h_
#define _mid_ccfg_h_


#include <mid_types.h>
#include <mid_glbcfg.h>

#define MID_CCFG_MAX_SECTION_NAME_LEN   64	/* maximum length of section name */
#define MID_CCFG_MAX_PARAM_NAME_LEN     64	/* maximum length of variable name */
#define MID_CCFG_MAX_VALUE_LEN          512	/* maximum length of variable value, in ASCII string format */

#define MID_CCFG_COMMIT_FILE	0x01	/* save configuration to file */
#define MID_CCFG_COMMIT_FLASH	0x02	/* save configuration to flash */
#define MID_CCFG_COMMIT_SYNC	0x04	/* synchronization service configuration and then save configuration to flash */

#define MID_CCFG_MAX_FILENAME   200		/* maximum length of backup and restore file name, including terminating '\0' */

enum mid_ccfg_act_t {
    MID_MSG_ACT_CFG_GET=1,              //configuration GET operation
    MID_MSG_ACT_CFG_SET=2,              //configuration SET operation
    MID_MSG_ACT_CFG_DEL=3,              //configuration DEL operation, single parameter
    MID_MSG_ACT_CFG_COMMIT=4,           //commit all the changes to the file
    MID_MSG_ACT_CFG_DEL_MULTI=5,        //configuration DEL operation, multiple parameters
    MID_MSG_ACT_CFG_SEARCH=6,           //configuration SEARCH operation
    MID_MSG_ACT_CFG_FIND_UNUSED=7,      //configuration FIND_UNUSED operation
    MID_MSG_ACT_CFG_BACKUP=8,           //make a backup file of /etc/config
    MID_MSG_ACT_CFG_RESTORE=9,          //restore /etc/config from a backup file
    MID_MSG_ACT_CFG_DUPLICATE=10,       //duplicate a section
    MID_MSG_ACT_CFG_OPENDFT=11,         //switch to the factory default config file
    MID_MSG_ACT_CFG_CLOSEDFT=12,        //switch back to the normal config file
    MID_MSG_ACT_CFG_IMPORT=13,          //import configuration file from a secondary file
    MID_MSG_ACT_CFG_EXPORT=14,			//export configuration file to a secondary file
    MID_MSG_ACT_CFG_BACKUP_PARTIAL=15,  //make a backup file of /etc/config/.glbcfg
    MID_MSG_ACT_CFG_RESTORE_PARTIAL=16, //restore /etc/config/.glbcfg from a backup file
    MID_MSG_ACT_CFG_COMMIT_LAZY=17,     //lazily commit when system is not busy
};

enum mid_ccfg_get_type_t {
    MID_CCFG_GET_THIS=0,
    MID_CCFG_GET_FIRST=1,
    MID_CCFG_GET_NEXT=2,
    MID_CCFG_GET_LAST=3,
    MID_CCFG_GET_SORT_FIRST=4,
    MID_CCFG_GET_SORT_NEXT=5,
    MID_CCFG_GET_SORT_LAST=6,
};

enum mid_ccfg_import_type_t {
    MID_CCFG_IMPORT_REPLACE=0,
    MID_CCFG_IMPORT_MERGE=1,
};
enum mid_ccfg_export_type_t {
    MID_CCFG_EXPORT_RAW=0,
    MID_CCFG_EXPORT_ENCODEBYLIST=1,
};

enum mid_ccfg_rsp_t {
    MID_CCFG_RSP_SUCCESS=0,
    MID_CCFG_RSP_FAIL=-1,
};

enum mid_ccfg_var_type_t {
    MID_CCFG_STR=0,
    MID_CCFG_INT8,
    MID_CCFG_UINT8,
    MID_CCFG_INT16,
    MID_CCFG_UINT16,
    MID_CCFG_INT32,
    MID_CCFG_UINT32,
    MID_CCFG_INET4,
    MID_CCFG_INET6
};

typedef struct mid_ccfg_tuple {
	char            section[MID_CCFG_MAX_SECTION_NAME_LEN];
	char            name[MID_CCFG_MAX_PARAM_NAME_LEN];
	int             type;
	unsigned char   value[MID_CCFG_MAX_VALUE_LEN];
	unsigned char   value2[MID_CCFG_MAX_VALUE_LEN];
} mid_ccfg_tuple_t;

//mid_ccfg_tuple_t is to be used in the messages between mapi and the hdl to carry the target section and variable the application wants to access
//the section fiels can be the name of a specific section to be access or a specific command prefixed with '!':
// ![index] :    the target is at the index-th section
// !.       :    keep in the current section
// !>       :    go to the next section
//
//the name field can be the specific variable name to be access or a specific command prefixed with '!':
// ![index] :    the target is at the index-th variable in the section
// !>       :    go to the next variable of the section
//
//the type field is to specify how to interprete the value[] field, defined in mid_ccfg_var_type_t

#endif // _mid_ccfg_h_
