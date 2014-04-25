/*
 * Arcadyan middle layer common definitions for mapi, mhdl, and core
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _mid_fmwk_h_
#define _mid_fmwk_h_

//#define MID_DEBUG

#ifdef MID_DEBUG
 #define MID_DBG(msg) \
	do { \
		fprintf( stderr, "%s(%i) in %s(): %s\n", \
		   __FILE__, __LINE__, __FUNCTION__, msg ? msg : "?"); \
		fflush( stderr ); \
	} while (0)

 #define MID_TRACE(...) \
	do { \
		fprintf( stderr, __VA_ARGS__); \
		fflush( stderr ); \
	} while (0)
#else
 #define MID_DBG(msg)
 #define MID_TRACE(...)
#endif //#ifdef MID_DEBUG

#define MID_SUCCESS         0
#define MID_FAIL            -1
#define MID_UNSUPPORT       -2
#define MID_TIMEOUT         -3

#define MID_MSG_MAX_LEN     4096    //todo: Message size is currently limited to reduce programming complexity.
                                    //NOTICE: This means the size of the data structure that a module used to
                                    //        carry requests and responses must not be greater than
                                    //        MID_MSG_MAX_LEN-sizeof(mid_msg_header_t)
                                    //        It's possible to remove the constraint in the future

#define MID_MAX_MODULE_NUM  128

enum mid_msg_mod_t {
    MID_MSG_MOD_CORE=0,                         //reserved for middle-core use
    MID_MSG_MOD_COMMONCFG=1,                    //for common configuration
    //put your module/service mid definition here
    //reserved for temporarily use or test
    MID_MSG_MOD_RESERVE1=MID_MAX_MODULE_NUM-3,
    MID_MSG_MOD_RESERVE2,
    MID_MSG_MOD_RESERVE3
};
//todo: Above is for fixed modules/services.
//      For dynamically installed module/service (I'm not sure if we'll need to provide something like
//      "plugin" in the future), a mechanism to allow to register and allocate mid is needed

/* should be defined in modules, like in mid_ccfg.h
enum mid_msg_act_t {
    MID_MSG_ACT_MOD_REG=0,		 //reserved for registration/re-registration
    MID_MSG_ACT_CFG_GET=1,       //configuration GET operation
    MID_MSG_ACT_CFG_SET=2,       //configuration SET operation
    MID_MSG_ACT_CFG_DEL=3,       //configuration DEL operation, single parameter
    MID_MSG_ACT_CFG_COMMIT=4,	 //commit all the changes to the file
    MID_MSG_ACT_CFG_DEL_MULTI=5, //configuration DEL operation, multiple parameters
    MID_MSG_ACT_CFG_SEARCH=6,    //configuration SEARCH operation
    MID_MSG_ACT_CFG_FIND_UNUSED=7,//configuration FIND_UNUSED operation
    MID_MSG_ACT_CFG_BACKUP=8,	 //make a backup file of configuration file
    MID_MSG_ACT_CFG_RESTORE=9,	 //restore configuration file from a backup file
    MID_MSG_ACT_CFG_DUPLICATE=10,//duplicate a section
    MID_MSG_ACT_STAT_GET=20,     //status/statistics GET operation
    MID_MSG_ACT_STAT_SET=21,     //status/statistics SET operation
    MID_MSG_ACT_STAT_SUB=22,     //subscribe an event report
    MID_MSG_ACT_STAT_RPT=23      //report an event
};
*/

#define MID_MSG_RSP_MASK        0xfff
#define MID_MSG_RSP_TRUNCATED   0x8000

enum mid_msg_rsp_t {
    MID_MSG_RSP_SUCCESS=0,
    MID_MSG_RSP_FAIL_UNREGISTER_MOD=1,
    MID_MSG_RSP_FAIL_LOAD_MOD=2,
    MID_MSG_RSP_FAIL_UNSPECIFIED=MID_MSG_RSP_MASK
};

//NOTICE: it's better to make sure the size of mid_msg_header_t is times of 4 bytes
typedef struct mid_msg_header {
	int             mid;            //module id
	int             pid;            //request process id
	unsigned short  rid;            //request id
	unsigned short  act_rsp_code;
    int             mlen;           //msg length, including this header	
} mid_msg_header_t;

#pragma pack(push, 4)
typedef struct mid_msg {
	mid_msg_header_t	header;
	char				data[0];    //module specific
} mid_msg_t;
#pragma pack(pop)

#define MID_MSG_MOD(pMsg)	((pMsg)->header.mid)
#define MID_MSG_PID(pMsg)	((pMsg)->header.pid)
#define MID_MSG_RID(pMsg)	((pMsg)->header.rid)
#define MID_MSG_RSP(pMsg)	((pMsg)->header.act_rsp_code)
#define MID_MSG_LEN(pMsg)	((pMsg)->header.mlen)
#define MID_MSG_DAT(pMsg)	((pMsg)->data)
#define MID_MSG_REQ(pMsg)	MID_MSG_RSP(pMsg)

#define MID_MSG_DATA_MAX_LEN	(MID_MSG_MAX_LEN-sizeof(mid_msg_header_t))

#endif // _mid_fmwk_h_
