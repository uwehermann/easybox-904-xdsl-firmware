/*
 * Arcadyan middle layer definition for Answering Machine
 *
 * Copyright 2012, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_HDL_ASM_H
#define _MID_HDL_ASM_H

#include "mid_ansmach.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Macro Definitions
 * */
#define MID_ASM_PATH                 "/tmp/ansmach"

#define MID_HDL_ASM_MAX_FILEPATH    128
#define MID_HDL_ASM_MAX_CALLEE_LEN  MAPI_ASM_INTERNAL_NUM_LEN
#define MID_HDL_ASM_MAX_CALLER_LEN  MAPI_ASM_CALLER_ID_LEN

#define MID_HDL_ASM_GRT_HDR_MAX     MAPI_ASM_MAX_GMSG_COUNT
#define MID_HDL_ASM_REC_HDR_MAX     MAPI_ASM_MAX_AMSG_COUNT

/*
 * Data Structure Definitions 
 * */

/* ASM Handler Operations */
typedef enum {
	/* 
     * Greeting Message Specific Operation Code 
     * */
	/* Used to save Greeting Message user has recorded to flash disk */
	MID_HDL_ASM_GRT_SAVE = 0,

	/* Used to load Greeting Message user had recorded from flash disk */
	MID_HDL_ASM_GRT_LOAD,

	/* Used to delete Greeting Message user had recorded in flash disk */
	MID_HDL_ASM_GRT_DEL,

	/* Used to obtain a specific Header Info of Greeting Message */
	MID_HDL_ASM_GRT_HDR_GET,

	/* Used to obtain all Header Info of Greeting Messages */
	MID_HDL_ASM_GRT_HDR_GET_ALL,

	/* Used to obtain all Header Info of Greeting Messages which is inused */
	MID_HDL_ASM_GRT_HDR_GET_INUSE,

	/* 
     * XXX_SHOW_XXX are Internal Used in Handler Layer 
     * */
	/* Used to display a specific Header Info of Greeting Message */
	MID_HDL_ASM_GRT_HDR_SHOW,

	/* Used to display display all Header Info of Greeting Messages */
	MID_HDL_ASM_GRT_HDR_SHOW_ALL,

	/* Used to display all Header Info of Greeting Messages which is inused */
	MID_HDL_ASM_GRT_HDR_SHOW_INUSE,

	/* 
     * Recording Message Specific Operation Code 
     * */
	/* Used to save Recording Message user has recorded to flash disk */
	MID_HDL_ASM_REC_SAVE,

	/* Used to load Recording Message user had recorded from flash disk */
	MID_HDL_ASM_REC_LOAD,

	/* Used to mark the Recording Message user had recorded as read in flash disk */
	MID_HDL_ASM_REC_MREAD,

	/* Used to mark the Recording Message user had recorded as not read in flash disk */
	MID_HDL_ASM_REC_MNREAD,
	
	/* Used to delete Recording Message user had recorded in flash disk */
	MID_HDL_ASM_REC_DEL,

	/* Used to delete all Recording Message user had recorded in flash disk */
	MID_HDL_ASM_REC_DEL_ALL,
	
	/* Used to obtain a specific Header Info of Recording Message */
	MID_HDL_ASM_REC_HDR_GET,

	/* Used to obtain all Header Info of Recording Messages */
	MID_HDL_ASM_REC_HDR_GET_ALL,

	/* Used to obtain all Header Info of Recording Messages which is inused */
	MID_HDL_ASM_REC_HDR_GET_INUSE,

	/* Used to obtain all Header Info of Recording Messages which is inused and stores to a file in /tmp */
	MID_HDL_ASM_REC_HDR_GET_INUSE2,

	/* 
     * XXX_SHOW_XXX are Internal Used in Handler Layer 
     * */
	/* Used to display a specific Header Info of Recording Message */
	MID_HDL_ASM_REC_HDR_SHOW,
	
	/* Used to display display all Header Info of Recording Messages */
	MID_HDL_ASM_REC_HDR_SHOW_ALL,
	
	/* Used to display all Header Info of Recording Messages which is inused */
	MID_HDL_ASM_REC_HDR_SHOW_INUSE,

	/* Used to display all Header Info of Recording Messages which is inused by reading header file in /tmp 
	 * The header file will be indicated in filepath string buffer in info union in mid_hdl_op_t structure 
	 * The caller of this handler should take the responsibility to remove the header file 
	 * The header is written in binary format of struture mid_hdl_rec_hdr_t */
	MID_HDL_ASM_REC_HDR_SHOW_INUSE2,

	/* 
     * Partition Layout Info 
     * */
	/* Used to obtain the Partition Summary */
	MID_HDL_ASM_LINFO_GET,

	/* Used to display the Partition Summary */
	MID_HDL_ASM_LINFO_SHOW,

	/* Used to Reset the Voice Partition. This operation will erase all data in partition!! */
	MID_HDL_ASM_RESET_FLASH,

	/* DO NOT TOUCH */
	MID_HDL_ASM_OP_MAX
} mid_hdl_asm_op_code_t;

/* Partition Info */
typedef struct {
	U32 status;
	U32 grtmsg_used_cnt;
	U32 recmsg_used_cnt;
	U32 grtmsg_used_size;
	U32 recmsg_used_size;
} mid_hdl_layout_info_t;

/* Greeting Mesage Headers */
typedef struct {
	U64 id;
	U32 size;
} mid_hdl_grt_hdr_udata_t;

typedef struct {
	U32 status;
	mid_hdl_grt_hdr_udata_t udata;
} mid_hdl_grt_hdr_t;

/* Recording Message Headers */
typedef struct {
	U64 time;									/* Identifier for matching recording messages, should be the UTC of the recording message created */
	U32 size;									/* Size of this recording message */
	U32 read;
	char callee_num[MID_HDL_ASM_MAX_CALLEE_LEN];
	char caller_num[MID_HDL_ASM_MAX_CALLER_LEN];
} mid_hdl_rec_hdr_udata_t;

typedef struct {
	U32 status;
	U32 offset;									/* Offset of this recording message in voice partition */
	mid_hdl_rec_hdr_udata_t udata;
} mid_hdl_rec_hdr_t;

/* Data Structure for MID_HDL_ASM_XX Operations  */
typedef struct {
	/*
     * For [GRT|REC]_HDR_GET, [GRT|REC]_DEL and GRT_SAVE and [GRT|REC]_LOAD Operations 
     * For GRT Messages, idx indicates 0 ~ MID_HDL_ASM_GRT_HDR_MAX. 0 means default
     * For REC Messages, idx indicates the member "time" in structure of mid_hdl_rec_hdr_udata_t
     * */
	U64 id;
#define num id
	/* Used to indicate the success or failure of Handler process 
	 * Possible value is MID_SUCCESS and MID_FAIL so far */
	U32 result;

	/* Leave it Empty for all operations. Used in Handler Only */
	U32 type;

	union {
		/* For SAVE and LOAD Operations. Also for MID_HDL_ASM_REC_HDR_GET_INUSE2 
         * MAPI Layer needs to indicate the filepath for above Operations */
		char filepath[MID_HDL_ASM_MAX_FILEPATH];

		/* For [GRT|REC]_HDR_GET_ALL] Operations 
		 * Used to indicate the numbers of elements of the array "data" provided by related callee
		 **/
		U32 len;
	} info;
	/*
	 * Memory allocated by commander
	 * For HDR_GET and SAVE, allocated with only one element
	 * For HDR_GET_ALL, allocated with the numbers of according message type
	 *                  MID_HDL_ASM_GRT_HDR_MAX for GRT Messages
	 *                  Dynamic numbers for REC Messages, which might be obtained from MID_HDL_ASM_LINFO_GET first
	 **/
	char data[0];
#undef num
} mid_hdl_op_t;

/* 
 * Global Function Definitions
 * */
extern int mid_ini_ansmach(void);
extern int mid_hdl_ansmach(mid_hdl_arg_t* arg);

#ifdef __cplusplus
}
#endif

#endif
