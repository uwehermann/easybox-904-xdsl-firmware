/*
 * Arcadyan middle layer header file of APIs of Answering Machine
 *
 * Copyright 2012, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_MAPI_ANSMACHINE_H_
#define _MID_MAPI_ANSMACHINE_H_


#include "mid_ansmach.h"


#ifdef __cplusplus
extern "C" {
#endif

/*********************/
/* Voice Mail        */
/*********************/
extern int mapi_ansmach_get_ansmach(mapi_ansmach_t* p_ansmach_cfg, int *p_num);
extern int mapi_ansmach_set_ansmach(mapi_ansmach_t* p_ansmach_cfg);
extern void mapi_ansmach_put_ansmach(mapi_ansmach_t* p_ansmach_cfg);
extern int mapi_ansmach_phone_recMsg_save(mapi_ansmach_phone_msg_hdr_t *hdr);	//note: "caller/callee/filepath" must be specified in the input data structure
extern int mapi_ansmach_phone_recMsg_load(mapi_ansmach_phone_msg_hdr_t *hdr);	//note: "id/filepath" must be specified in the input data structure
extern int mapi_ansmach_phone_recMsg_delete(mapi_ansmach_phone_msg_hdr_t *hdr);	//note: "id" must be specified in the input data structure
extern int mapi_ansmach_phone_recMsg_delete_all(mapi_ansmach_phone_msg_list_t *list);
extern int mapi_ansmach_phone_recMsg_list_get(mapi_ansmach_phone_msg_list_t *list);	//note: "callee" must be specified in the input data structure
extern int mapi_ansmach_phone_recMsg_list_get_all(mapi_ansmach_phone_msg_list_t *list);
extern void mapi_ansmach_phone_recMsg_list_put(mapi_ansmach_phone_msg_list_t *list);
extern int mapi_ansmach_phone_recMsg_list_show(mapi_ansmach_phone_msg_list_t *list);
extern int mapi_ansmach_phone_recMsg_mark_read(mapi_ansmach_phone_msg_hdr_t *hdr);	//note: "id" must be specified in the input data structure
extern int mapi_ansmach_phone_recMsg_mark_unread(mapi_ansmach_phone_msg_hdr_t *hdr);	//note: "id" must be specified in the input data structure
extern int mapi_ansmach_recMsg_load_wav(unsigned long long id, char *filepath);
extern int mapi_ansmach_recMsg_load_mp3(unsigned long long id, char *filepath);
extern int mapi_ansmach_recMsg_remain_size_get(int *R_byte);
extern int mapi_ansmach_recMsg_used_capacity_get(float *R_sec);
extern int mapi_ansmach_recMsg_show_hdrs(void);
extern int mapi_ansmach_recMsg_check_exist(unsigned long long id);
extern int mapi_ansmach_recMsg_delete_all(void);
extern int mapi_ansmach_enable_status_get(int *enable);

extern int mapi_ansmach_debug_status_get(int *enable);

extern int mapi_ansmach_account_info_get(mapi_ansmach_account_info_t *account);	//note: "userId" must be specified in the input data structure. (this function does not approach displayName)
extern int mapi_ansmach_account_info_get_all(mapi_ansmach_account_info_list_t *list);
extern void mapi_ansmach_account_info_list_put(mapi_ansmach_account_info_list_t *list);
extern int mapi_ansmach_account_info_show(void);

/*********************/
/* Greeting Messages */
/*********************/
extern int mapi_ansmach_get_grtmsg(mapi_grtmsg_t* p_grtmsg_cfg, int factory_dft);
extern int mapi_ansmach_set_grtmsg(mapi_grtmsg_t* p_grtmsg_cfg);
extern int mapi_ansmach_get_grtmsg_Idx_AftSec_Size(char *caller, char *callee, int *idx, int *aft_sec, int *size);
extern int mapi_ansmach_grtMsg_used_num_get(int *R_num);
extern int mapi_ansmach_phone_grtMsg_info_get(mapi_ansmach_phone_msg_hdr_t *hdr);	//note: "caller/callee" must be specified in the input data structure
extern int mapi_ansmach_phone_grtMsg_save(mapi_ansmach_phone_msg_hdr_t *hdr);	//note: "id/filepath" must be specified in the input data structure, id = (specified id) or (MAPI_ASM_GRTMSG_FIND_EMPTY_ID)
extern int mapi_ansmach_phone_grtMsg_load(mapi_ansmach_phone_msg_hdr_t *hdr);	//note: "id/filepath" must be specified in the input data structure
extern int mapi_ansmach_ui_grtMsg_delete(int id);	//test
extern int mapi_ansmach_grtMsg_load_wav(int id, char *filepath);
extern int mapi_ansmach_grtMsg_load_mp3(int id, char *filepath);
extern int mapi_ansmach_grtMsg_show_hdrs(void);

extern int mapi_ansmach_grtMsg_show_index_usage(void);	//for debug
extern int mapi_ansmach_grtMsg_get_empty_index(int *R_id);	//for debug


/*********************/
/* Email Forwarding  */
/*********************/
extern int mapi_ansmach_get_emailfw(mapi_email_fw_t *p_email_fw_cfg, int factory_dft);
extern int mapi_ansmach_set_emailfw(mapi_email_fw_t *p_email_fw_cfg);
extern int mapi_ansmach_sendmail_emailfw(mapi_email_fw_t *emailfw_cfg, char *conf, char *send_to, char *subject, char *context, char **attach, unsigned int attach_num);


/*********************/
/* Test Only         */
/*********************/
extern int mapi_ansmach_hdl_cli(int tid, U16 asm_op, void *data, U32 len);

/*********************/
/* Number Converting */
/*********************/
extern int mapi_ansmach_phonenum_convert(char *dstPhoneNum, char *srcPhoneNum);
extern int mapi_voip_get_call_cnt();
extern int mapi_voip_set_call_cnt(int cnt);

#ifdef __cplusplus
}
#endif


#endif // _MID_MAPI_ANSMACHINE_H_
