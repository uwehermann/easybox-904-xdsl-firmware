/*
 * Arcadyan middle layer APIs for Answering Machine
 *
 * Copyright 2012, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <byteswap.h>
#include <libgen.h>
#include <mid_fmwk.h>
#include <mid_mapi_trnx.h>
#include <sys/timeb.h>	//ftime
#include "libArcComOs.h"
#include "mid_mapi_ccfg.h"
#include "mid_mapi_ccfgsal_voip.h"
#include "mid_mapi_ansmach.h"
#include "mid_hdl_ansmach.h"
#include "mid_mapi_ansmach_db.h"
#include "mid_mapi_ansmach_wav.h"
#include "mid_ansmach.h"
#include <arc_openwrt_config.h>


/*
 * Local Function Definitions
 * */
static int mapi_ansmach_recMsg_hdr_get_by_callee(char *callee, mid_hdl_rec_hdr_udata_t *hdrp, int len, int *R_num);
static int mapi_ansmach_recMsg_hdr_sort_by_time(mid_hdl_rec_hdr_udata_t *hdrp, int len);
static int mapi_ansmach_recMsg_hdr_sort_by_readStatus(mid_hdl_rec_hdr_udata_t *hdrp, int len, int *R_newNum);
static int show_these_recMsg_hdrs(mid_hdl_rec_hdr_udata_t *hdrp, int len);	//for debug
static int mapi_ansmach_grtMsg_empty_index_get(int *R_id);

static void show_wave_header(struct WAVE_HEADER *wave_header);
static void fill_header(struct WAVE_HEADER *wave_header, int file_size);
static int do_G7112wav(int file_size, FILE *fp_in, FILE *fp_out);

//core api for processing transaction
static int mapi_ansmach_recMsg_hdr_get_all(mid_hdl_rec_hdr_udata_t **R_hdrs, int *num);
static int mapi_ansmach_recMsg_save(mid_hdl_op_t *hdl_op, mid_hdl_rec_hdr_udata_t *hdl_hdr);
static int mapi_ansmach_recMsg_load(mid_hdl_op_t *hdl_op);
static int mapi_ansmach_recMsg_delete(mid_hdl_op_t *hdl_op);
static int mapi_ansmach_recMsg_mark_read(mid_hdl_op_t *hdl_op);
static int mapi_ansmach_recMsg_mark_unread(mid_hdl_op_t *hdl_op);
static int mapi_ansmach_grtMsg_hdr_get_all(mid_hdl_grt_hdr_udata_t **R_hdrs, int *num);
static int mapi_ansmach_grtMsg_save(mid_hdl_op_t *hdl_op, mid_hdl_grt_hdr_udata_t *hdl_hdr);
static int mapi_ansmach_grtMsg_load(mid_hdl_op_t *hdl_op);
static int mapi_ansmach_grtMsg_delete(mid_hdl_op_t *hdl_op);
static int mapi_ansmach_msg_info_get(mid_hdl_layout_info_t *info);

/*
 * Local Function Definitions
 * */

/*******************************************************************************
 * Description
 *		Get Answering Machine configuration
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *      p_ansmach_cfg:	pointer to the buffer of data structure mapi_ansmach_t
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or p_ansmach_cfg is NULL.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_ansmach_get_ansmach(mapi_ansmach_t *p_ansmach_cfg, int *p_num)
{
	mid_hdl_rec_hdr_udata_t *hdrp = NULL, *tmp;
	int total_hdr_num = 0;
	int i,ret, tid;
	int	new_transc=0;
	float used_capacity;

    if ( p_ansmach_cfg == NULL ) {
        MID_TRACE("[%s] NULL p_ansmach_cfg\n", __FUNCTION__);
        return MID_FAIL;
    }

	if ( (tid=mapi_start_transc()) == MID_FAIL )
		return MID_FAIL;
	new_transc = 1;

	p_ansmach_cfg->enable = mapi_ccfg_get_long( tid, MAPI_ASM_ANSMACH_SECTION_NAME, "enable", 0);

    if(new_transc) {
        if(mapi_end_transc(tid)==MID_FAIL) {
            return MID_FAIL;
        }
    }

	ret = mapi_ansmach_recMsg_used_capacity_get(&used_capacity);
	if(ret == MID_SUCCESS)
	{
		printf("used_capacity = %f (second)\n", used_capacity);
		p_ansmach_cfg->used_capacity= used_capacity;
	}

	ret = mapi_ansmach_recMsg_hdr_get_all(&hdrp, &total_hdr_num);
	tmp = hdrp;

	if(ret == MID_FAIL)
		return MID_FAIL;

	if((hdrp == NULL) && (total_hdr_num != 0)){
		printf("show_recMsg_hdrs_web() : No recMsg hdr exist\n");
		return MID_FAIL;
	}

	if (!(p_ansmach_cfg->msgs = (mapi_ansmach_msg_t *) malloc(sizeof(mapi_ansmach_msg_t) * total_hdr_num))) {
		ASM_MSG("Allocate Memory Failed\n");
		return MID_FAIL;
	}

	for(i=0; i<total_hdr_num; i++){
		struct tm tm;
		time_t time_sec;

		p_ansmach_cfg->msgs[i].id = hdrp->time;

		memset(&tm, 0, sizeof(struct tm));
		time_sec = (U32) (hdrp->time / 1000);	//sec = millisec / 1000
		localtime_r((const time_t *) &time_sec, &tm);
		p_ansmach_cfg->msgs[i].time.tm_sec = tm.tm_sec;
		p_ansmach_cfg->msgs[i].time.tm_min = tm.tm_min;
		p_ansmach_cfg->msgs[i].time.tm_hour = tm.tm_hour;
		p_ansmach_cfg->msgs[i].time.tm_mday = tm.tm_mday;
		p_ansmach_cfg->msgs[i].time.tm_mon = tm.tm_mon + 1;
		p_ansmach_cfg->msgs[i].time.tm_year = tm.tm_year + 1900;

		p_ansmach_cfg->msgs[i].size = hdrp->size;
		p_ansmach_cfg->msgs[i].read = hdrp->read;
		strcpy(p_ansmach_cfg->msgs[i].caller_num ,hdrp->caller_num);
		strcpy(p_ansmach_cfg->msgs[i].callee_num ,hdrp->callee_num);

		hdrp++;
	}

	*p_num = total_hdr_num;

	if(tmp != NULL)
		free(tmp);

	return MID_SUCCESS;
}


/*******************************************************************************
 * Description
 *		Set Answering_Machine configuration
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *      p_ansmach_cfg:	pointer to the buffer of data structure mapi_ansmach_t
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or p_ansmach_cfg is NULL.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_ansmach_set_ansmach(mapi_ansmach_t* p_ansmach_cfg)
{
	int	new_transc=0;
	int tid;

	if ( p_ansmach_cfg == NULL ) {
        MID_TRACE("[%s] NULL p_ansmach_cfg\n", __FUNCTION__);
        return MID_FAIL;
    }

	if ( (tid=mapi_start_transc()) == MID_FAIL )
		return MID_FAIL;
	new_transc = 1;

	mapi_ccfg_set_long( tid, MAPI_ASM_ANSMACH_SECTION_NAME, "enable", p_ansmach_cfg->enable);

    if(new_transc){
        if(mapi_end_transc(tid)==MID_FAIL){
            return MID_FAIL;
        }
    }

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Free the malloc-memory in the mapi_ansmach_t structure
 *
 * Parameters
 *
 * Returns
 *
 ******************************************************************************/
void mapi_ansmach_put_ansmach(mapi_ansmach_t* p_ansmach_cfg)
{
	if(p_ansmach_cfg->msgs)
		free(p_ansmach_cfg->msgs);
}

/*******************************************************************************
 * Description
 *		Get Greeting Messages configuration
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *      p_grtmsg_cfg:	pointer to the buffer of data structure mapi_grtmsg_t
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or p_grtmsg_cfg is NULL.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_ansmach_get_grtmsg(mapi_grtmsg_t* p_grtmsg_cfg, int factory_dft)
{
	mid_hdl_grt_hdr_udata_t *hdrp = NULL;
	mid_hdl_grt_hdr_udata_t *hdrp_ori = NULL;
	int total_hdr_num = 0;
	int ret, tid;
	int	new_transc=0,i,j;
	char tmp_para[64];

    if ( p_grtmsg_cfg == NULL ) {
        MID_TRACE("[%s] NULL p_grtmsg_cfg\n", __FUNCTION__);
        return MID_FAIL;
    }

	if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	new_transc = 1;

    if(factory_dft){
        mapi_ccfg_switch_in_out_factory_cfg(tid, 1);
    }

	ret = mapi_ansmach_grtMsg_hdr_get_all(&hdrp, &total_hdr_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	if(hdrp == NULL){
		printf("mapi_ansmach_grtMsg_show_hdrs() : No grtMsg hdr exist\n");
		return MID_FAIL;
	}

	hdrp_ori = hdrp;

	for(i=0; i<total_hdr_num; i++){
		p_grtmsg_cfg->grtmsg[hdrp->id].filesize=hdrp->size;
		p_grtmsg_cfg->grtmsg[hdrp->id].id=hdrp->id;
		hdrp++;
	}

	if(hdrp_ori != NULL)
		free(hdrp_ori);

	for (i=0;i<MAPI_ASM_MAX_GMSG_COUNT;i++) {
		sprintf(tmp_para,"enable%d",i);
		p_grtmsg_cfg->grtmsg[i].Enable = mapi_ccfg_get_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, 0);

		sprintf(tmp_para,"name%d",i);
		mapi_ccfg_get_str( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].Name, MAPI_ASM_NAME_LEN, NULL);

		sprintf(tmp_para,"after_sec%d",i);
		p_grtmsg_cfg->grtmsg[i].AfterSec = mapi_ccfg_get_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, 0);

		for (j=0;j<MAPI_ASM_ASI_TO_COUNT;j++) {
			sprintf(tmp_para,"grtmsg%d_asiTo%d",i, j);
			mapi_ccfg_get_str( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].AsiTo[j], MAPI_ASM_CODE_PHONE_NUM_LEN, NULL);
		}

		sprintf(tmp_para,"c_enable%d",i);
		p_grtmsg_cfg->grtmsg[i].CodeEnable = mapi_ccfg_get_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, 0);

		for (j=0;j<MAPI_ASM_COUNTRY_CODE_COUNT;j++) {
			sprintf(tmp_para,"grtmsg%d_fromC%d",i, j);
			mapi_ccfg_get_str( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].CountryCode[j], MAPI_ASM_CODE_LEN+2, NULL);
		}

		sprintf(tmp_para,"n_enable%d",i);
		p_grtmsg_cfg->grtmsg[i].NumEnable = mapi_ccfg_get_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, 0);

		for (j=0;j<MAPI_ASM_PHONE_NUMBER_COUNT;j++) {
			sprintf(tmp_para,"grtmsg%d_fromN%d",i, j);
			mapi_ccfg_get_str( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].PhoneNum[j], MAPI_ASM_PHONE_LEN+2, NULL);
		}

		sprintf(tmp_para,"grtmsg%d_asiToAll", i);
		p_grtmsg_cfg->grtmsg[i].AsiToAll = mapi_ccfg_get_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, 0);

		sprintf(tmp_para,"grtmsg%d_phoneNumToAll", i);
		p_grtmsg_cfg->grtmsg[i].PhoneNumToAll = mapi_ccfg_get_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, 0);
	}

    if(factory_dft) {
        mapi_ccfg_switch_in_out_factory_cfg(tid, 0);
    }

    if(new_transc) {
        if(mapi_end_transc(tid)==MID_FAIL) {
            return MID_FAIL;
        }
    }

	return MID_SUCCESS;
}


/*******************************************************************************
 * Description
 *		Set Greeting Messages configuration
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *      p_grtmsg_cfg:	pointer to the buffer of data structure mapi_grtmsg_t
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or p_grtmsg_cfg is NULL.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_ansmach_set_grtmsg(mapi_grtmsg_t* p_grtmsg_cfg)
{
	int	new_transc=0,i,j, tid;
	char tmp_para[64];

	if ( p_grtmsg_cfg == NULL ) {
        MID_TRACE("[%s] NULL p_grtmsg_cfg\n", __FUNCTION__);
        return MID_FAIL;
    }

	if ( (tid=mapi_start_transc()) == MID_FAIL )
		return MID_FAIL;
	new_transc = 1;

	for (i=0;i<MAPI_ASM_MAX_GMSG_COUNT;i++) {
		sprintf(tmp_para,"enable%d",i);
		mapi_ccfg_set_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].Enable);

		sprintf(tmp_para,"name%d",i);
		mapi_ccfg_set_str( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].Name);

		sprintf(tmp_para,"after_sec%d",i);
		mapi_ccfg_set_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].AfterSec);

		for (j=0;j<MAPI_ASM_ASI_TO_COUNT;j++) {
			sprintf(tmp_para,"grtmsg%d_asiTo%d",i, j);
			mapi_ccfg_set_str( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].AsiTo[j]);
		}

		sprintf(tmp_para,"c_enable%d",i);
		mapi_ccfg_set_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].CodeEnable);

		for (j=0;j<MAPI_ASM_COUNTRY_CODE_COUNT;j++) {
			sprintf(tmp_para,"grtmsg%d_fromC%d",i, j);
			mapi_ccfg_set_str( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].CountryCode[j]);
		}

		sprintf(tmp_para,"n_enable%d",i);
		mapi_ccfg_set_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].NumEnable);

		for (j=0;j<MAPI_ASM_PHONE_NUMBER_COUNT;j++) {
			sprintf(tmp_para,"grtmsg%d_fromN%d",i, j);
			mapi_ccfg_set_str( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].PhoneNum[j]);
		}

		sprintf(tmp_para,"grtmsg%d_asiToAll", i);
		mapi_ccfg_set_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].AsiToAll);

		sprintf(tmp_para,"grtmsg%d_phoneNumToAll", i);
		mapi_ccfg_set_long( tid, MAPI_ASM_GRTMSG_SECTION_NAME, tmp_para, p_grtmsg_cfg->grtmsg[i].PhoneNumToAll);
	}

    if(new_transc){
        if(mapi_end_transc(tid)==MID_FAIL){
            return MID_FAIL;
        }
    }

	return MID_SUCCESS;
}


/*******************************************************************************
 * Description
 *		Get E-mail forwarding configuration
 *
 * Parameters
 *     	p_grtmsg_cfg:	pointer to the buffer of data structure mapi_grtmsg_t
  *		factory_dft:		get default value or not
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or p_grtmsg_cfg is NULL.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_ansmach_get_emailfw(mapi_email_fw_t *p_email_fw_cfg, int factory_dft)
{
	int tid, new_transc=0;

	if ( p_email_fw_cfg == NULL ) {
		MID_TRACE("[%s] NULL p_grtmsg_cfg\n", __FUNCTION__);
		return MID_FAIL;
	}

	if ( (tid=mapi_start_transc()) == MID_FAIL )
		return MID_FAIL;
	new_transc = 1;

	if(factory_dft){
		mapi_ccfg_switch_in_out_factory_cfg(tid, 1);
	}

	p_email_fw_cfg->Enable = mapi_ccfg_get_long(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "enable", 0);
	mapi_ccfg_get_str(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "email_addr", p_email_fw_cfg->EmailAddress, MAPI_ASM_EMAIL_ADDRESS_LEN, "");
	mapi_ccfg_get_str(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "username", p_email_fw_cfg->Username, MAPI_ASM_EMAIL_ADDRESS_LEN, "");
	mapi_ccfg_get_str(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "password", p_email_fw_cfg->Password, MAPI_ASM_PASSWORD_LEN, "");
	mapi_ccfg_get_str(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "smtp_srv", p_email_fw_cfg->SMTP_server, MAPI_ASM_SMTP_SERVER_LEN, "");
	p_email_fw_cfg->TLS = mapi_ccfg_get_long(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "smtp_tls", 0);
	p_email_fw_cfg->port= mapi_ccfg_get_long(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "smtp_port", 0);

	if(factory_dft) {
		mapi_ccfg_switch_in_out_factory_cfg(tid, 0);
	}

	if(new_transc) {
		if(mapi_end_transc(tid)==MID_FAIL) {
			return MID_FAIL;
		}
	}

	return MID_SUCCESS;
}


/*******************************************************************************
 * Description
 *		Get E-mail forwarding configuration
 *
 * Parameters
 *     	p_grtmsg_cfg:	pointer to the buffer of data structure mapi_grtmsg_t
  *		factory_dft:		get default value or not
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or p_grtmsg_cfg is NULL.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_ansmach_set_emailfw(mapi_email_fw_t *p_email_fw_cfg)
{
	int	tid, new_transc=0;

	if ( p_email_fw_cfg == NULL ) {
		MID_TRACE("[%s] NULL p_grtmsg_cfg\n", __FUNCTION__);
		return MID_FAIL;
	}

	if ( (tid=mapi_start_transc()) == MID_FAIL )
		return MID_FAIL;
	new_transc = 1;

	mapi_ccfg_set_long(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "enable", p_email_fw_cfg->Enable);
	mapi_ccfg_set_str(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "email_addr", p_email_fw_cfg->EmailAddress);
	mapi_ccfg_set_str(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "username", p_email_fw_cfg->Username);
	mapi_ccfg_set_str(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "password", p_email_fw_cfg->Password);
	mapi_ccfg_set_str(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "smtp_srv", p_email_fw_cfg->SMTP_server);
	mapi_ccfg_set_long(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "smtp_tls", p_email_fw_cfg->TLS);
	mapi_ccfg_set_long(tid, MAPI_ASM_EMAILFW_SECTION_NAME, "smtp_port", p_email_fw_cfg->port);

	if(new_transc){
		if(mapi_end_transc(tid)==MID_FAIL){
			return MID_FAIL;
		}
	}

	return MID_SUCCESS;
}


/*******************************************************************************
 * Description
 *		Get contact name by phone number.
 *
 * Parameters
 *          num:  phone number
 * 		buf:   buffer of contact name
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed
 *
 ******************************************************************************/
int mapi_ansmach_get_pkname_by_number(char *num, char *buf)
{
	mapi_ccfg_voip_phone_book_t phonebook_profile;
	int j;
	int tid = MID_FAIL;

	if((tid=mapi_start_transc()) == MID_FAIL)
	{
		ASM_MSG("%s.%d> # mapi_start_transc() fail\n", __FUNCTION__, __LINE__);
		return MID_FAIL;
	}

	memset((char *)&phonebook_profile, 0, sizeof(phonebook_profile));
	if(mapi_ccfg_get_voip_phone_book(tid, &phonebook_profile) == MID_FAIL)
	{
		ASM_MSG("%s.%d> # mapi_ccfg_get_voip_phone_book() fail\n", __FUNCTION__, __LINE__);
		mapi_end_transc(tid);
		return MID_FAIL;
    }

	for(j=0; j<phonebook_profile.counter; j++)
	{
		if(strcmp(phonebook_profile.contact[j].phone_num, num) == 0)
		{
			strcpy(buf, phonebook_profile.contact[j].name);
			mapi_end_transc(tid);
			return MID_SUCCESS;
		}
	}

	mapi_end_transc(tid);
	return MID_FAIL;
}


/*******************************************************************************
 * Description
 *		Get phone number by recording ID.
 *
 * Parameters
 *          num:  recording ID
 * 		buf:   buffer of phone number
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed
 *
 ******************************************************************************/

int mapi_ansmach_get_msg_by_id(unsigned long long id, mapi_ansmach_msg_t *msg)
{
	mapi_ansmach_t ansmach_info;
	int i, totalnum = 0;

	if (mapi_ansmach_get_ansmach(&ansmach_info, &totalnum) == MID_SUCCESS)
	{
		for(i=0; i<totalnum; i++)
		{
			if(id == ansmach_info.msgs[i].id)
			{
				memcpy(msg, &ansmach_info.msgs[i], sizeof(ansmach_info.msgs[i]));
				mapi_ansmach_put_ansmach(&ansmach_info);
				return MID_SUCCESS;
			}
		}

		mapi_ansmach_put_ansmach(&ansmach_info);
	}
	else
		ASM_MSG("%s.%d> # mapi_ansmach_get_ansmach() fail\n", __FUNCTION__, __LINE__);

	return MID_FAIL;
}



/*******************************************************************************
 * Description
 *		Generate zip filename for vodafone's requirement.
 *
 * Parameters
 * 		keyword: fname_buf
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed
 *
 ******************************************************************************/
int mapi_ansmach_get_zip_filename(char *fname_buf)
{
	struct tm *tm;
	time_t curtime;

	time(&curtime);
	tm = localtime(&curtime);
	sprintf(fname_buf, "%s_%d%d%d_%02d%02d.zip", 
		ASM_MSG_EMAILFW_ZIP_PREFIX, 
		tm->tm_year + 1900, 
		tm->tm_mon + 1, 
		tm->tm_mday, 
		tm->tm_hour, 
		tm->tm_min);

	return MID_SUCCESS;
}


/*******************************************************************************
 * Description
 *		Generate keyword with identify feature.
 *		CalleeNum_CallerNum_YYYYMMDD_HHMM
 *
 * Parameters
 * 		id: the unique id for mtd entry
 * 		keyword: buf
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed
 *
 ******************************************************************************/
int mapi_ansmach_get_emailfw_keyword(unsigned long long id, char *keyword)
{
	mapi_ansmach_t ansmach_info;
	unsigned char displayName[MAPI_CCFG_VOIP_PHONE_BOOK_NAME_LEN];
	int i, totalnum = 0;

	memset(keyword, 0, sizeof(keyword));
	if (mapi_ansmach_get_ansmach(&ansmach_info, &totalnum) == MID_SUCCESS)
	{
		for(i=0; i<totalnum; i++)
		{
			if(id == ansmach_info.msgs[i].id)
			{
				strcpy(displayName, ansmach_info.msgs[i].caller_num);

				mapi_ansmach_get_pkname_by_number(ansmach_info.msgs[i].caller_num, displayName);

				sprintf(keyword, "%d%d%d_%02d%02d_%s",
					ansmach_info.msgs[i].time.tm_year,
					ansmach_info.msgs[i].time.tm_mon,
					ansmach_info.msgs[i].time.tm_mday,
					ansmach_info.msgs[i].time.tm_hour,
					ansmach_info.msgs[i].time.tm_min,
					displayName);

				mapi_ansmach_put_ansmach(&ansmach_info);
				return MID_SUCCESS;
			}
		}

		mapi_ansmach_put_ansmach(&ansmach_info);
	}
	else
		ASM_MSG("%s.%d> # mapi_ansmach_get_ansmach() fail\n", __FUNCTION__, __LINE__);

	return MID_FAIL;
}


/*******************************************************************************
 * Description
 *		Prepare for sending mail.
 *			1. Firm E-mail forwarding configuration.
 *			2. Load wav from mtd and gzip them.
 *			3. Sync umng to generate smtp config for email_fw.
 *			4. Call API to send mail with attached.
 *
 * Parameters
 * 		id: A list of attached ID.
 * 		num: Number of ID.
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed
 *
 ******************************************************************************/
int
mapi_ansmach_emailfw_prepare(unsigned long long **id, int num)
{
	int i, tid = MID_FAIL, retValue = MID_FAIL;
	char fpath[128], cmd[128];
	char *flist[1];
	char zip_name[128] = {0};
	char buf[256] = {0};
	char subject[128] = {0};
	char content[512] = {0};
	mapi_email_fw_t emailfw_cfg;

	memset(&emailfw_cfg, 0, sizeof(emailfw_cfg));
	if(mapi_ansmach_get_emailfw(&emailfw_cfg, 0) == MID_SUCCESS)
	{
		if(!emailfw_cfg.Enable || strlen(emailfw_cfg.EmailAddress)<=0 || strlen(emailfw_cfg.SMTP_server)<=0)
		{
			ASM_MSG("%s.%d> # E-mail forwarding disable or have no info.\n", __FUNCTION__, __LINE__);
			return MID_FAIL;
		}
	}
	else
	{
		ASM_MSG("%s.%d> # mapi_ansmach_get_emailfw() fail.\n", __FUNCTION__, __LINE__);
		return MID_FAIL;
	}
	
	//Follow Vodafone's naming rules.
	sprintf(cmd, "mkdir %s", ASM_MSG_EMAILFW_PREFIX);
	ASM_EXEC(cmd);
	mapi_ansmach_get_zip_filename(zip_name);

	for(i=0; i<num; i++)
	{
		//load wav from mtd.
		memset(fpath, 0x0, sizeof(fpath));
		if(mapi_ansmach_get_emailfw_keyword(*id[i], buf) == MID_FAIL)
		{
			ASM_MSG("%s.%d> #    mapi_ansmach_get_emailfw_keyword() fail\n", __FUNCTION__, __LINE__);
			return MID_FAIL;
		}
		sprintf(fpath, "%s%s.wav", ASM_MSG_EMAILFW_PREFIX, buf);
		mapi_ansmach_recMsg_load_wav(*id[i], fpath);
	}

	sprintf(cmd, "zip -1 -j /tmp/%s %s*.wav", zip_name, ASM_MSG_EMAILFW_PREFIX);	//-0: "no compress", -1: "compress faster", -9: "compress better" (default is -6), -j: junk (don't record) directory names
	ASM_EXEC(cmd);
	sprintf(cmd, "rm %s*.wav", ASM_MSG_EMAILFW_PREFIX);
	ASM_EXEC(cmd);
	sprintf(fpath, "/tmp/%s", zip_name);
	flist[0] = malloc(strlen(fpath));
	strcpy(flist[0], fpath);

	//generate email subject and content
	if(num > 1)
	{
		strcpy(subject, ASM_MSG_EMAILFW_SUBJECT);
		sprintf(content, "%s\n%s", ASM_MSG_EMAILFW_CONTENT, zip_name);
	}
	else
	{
		mapi_ansmach_msg_t msg;
		char displayName[MAPI_CCFG_VOIP_PHONE_BOOK_NAME_LEN+1] = {0};

		strcpy(subject, ASM_MSG_EMAILFW_SUBJECT_C);
		if(mapi_ansmach_get_msg_by_id(*id[0], &msg) == MID_SUCCESS)
		{
			strcpy(displayName, msg.caller_num);
			mapi_ansmach_get_pkname_by_number(msg.caller_num, displayName);
			strcat(subject, displayName);
		}
		else
		{
			ASM_MSG("Cannot find matched number???\n");
			strcat(subject, "Unbekannt");//append a space for identify
		}

		sprintf(content, ASM_MSG_EMAILFW_CONTENT_C_FMT,
			displayName,
			msg.time.tm_mday,
			msg.time.tm_mon,
			msg.time.tm_year,
			msg.time.tm_hour,
			msg.time.tm_min);
	}

	if((tid=mapi_start_transc()) == MID_FAIL)
	{
		ASM_MSG("%s.%d> # mapi_start_transc() fail\n", __FUNCTION__, __LINE__);
		return MID_FAIL;
	}

	mapi_ccfg_commit(tid, MID_CCFG_COMMIT_SYNC, MAPI_ASM_EMAILFW_SECTION_NAME);
	retValue = mapi_ansmach_sendmail_emailfw(
		&emailfw_cfg,
		NULL,
		emailfw_cfg.EmailAddress,
		subject,
		content,
		flist,
		1);

	free(flist[0]);
	sprintf(cmd, "rm /tmp/%s", zip_name);
	ASM_EXEC(cmd);

	mapi_end_transc(tid);

	return retValue;
}


/*******************************************************************************
 * Description
 *		Send e-Mail with Voice Mails
 *
 * Parameters
 * 		conf: path to the ssmtp configuration
 * 		send_to: email address to be sent
 * 		subject: used as email subject
 * 		attach: array for storing the attachment path
 * 		attach_num: numbers of attachments
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed
 *
 ******************************************************************************/
int
mapi_ansmach_sendmail_emailfw(mapi_email_fw_t *emailfw_cfg, char *conf, char *send_to, char *subject, char *context, char **attach, unsigned int attach_num)
{
	char mailresult[8];
	char cmdbuf[512];
	char message[MAPI_ASM_MAX_FILEPATH];
	int ret = MID_FAIL;
	snprintf(message, sizeof(message), "/tmp/ansmach.XXXXXX");

	close(mkstemp(message));

	ASM_EXEC("echo \"subject: %s\" > %s", subject, message);
	ASM_EXEC("echo \"From: %s\" >> %s", send_to, message);
	ASM_EXEC("echo \"to: %s\" >> %s", send_to, message);
	if (attach && attach_num > 0) {
		ASM_EXEC("echo \"Content-Type: multipart/mixed; boundary=bcaec53f2f737e3d8604d6adea88\" >> %s", message);
	}
	else {
		ASM_EXEC("echo \"Content-Type: text/plain; charset=ISO-8859-1\" >> %s", message);
		ASM_EXEC("echo \"Content-Transfer-Encoding: quoted-printable\" >> %s", message);
	}
	ASM_EXEC("echo \"\" >> %s", message);
	ASM_EXEC("echo -e \"%s\" >> %s", context, message);

	int i;
	for (i = 0; i < attach_num; i++) {
		ASM_EXEC("echo \"\" >> %s", message);
		ASM_EXEC("echo \"--bcaec53f2f737e3d8604d6adea88\" >> %s", message);
		ASM_EXEC("echo \"Content-Type: application/zip\" >> %s", message);
		ASM_EXEC("echo \"Content-Disposition: attachment; filename=\\\"%s\\\"\" >> %s", basename(attach[i]), message);
		ASM_EXEC("echo \"Content-Transfer-Encoding: base64\" >> %s", message);
		ASM_EXEC("echo \"\" >> %s", message);
		ASM_EXEC("uuencode -m %s %s >> %s", attach[i], basename(attach[i]), message);
		ASM_EXEC("sed -i '/begin-base64 644.*/d' %s", message);
		ASM_EXEC("sed -i '/====/d' %s", message);
		ASM_EXEC("echo \"--bcaec53f2f737e3d8604d6adea88--\" >> %s", message);
	}

	if(conf != NULL){
#if defined(CONFIG_PACKAGE_msmtp)
		ASM_MSG("Configure file of msmtp is not ready yet.\n");
		goto out;
#elif defined(CONFIG_PACKAGE_ssmtp)
		sprintf(cmdbuf,"ssmtp -C %s %s < %s;echo $?", conf, send_to, message);
		osSystem_GetOutput(cmdbuf,mailresult,sizeof(mailresult));
		if(strncmp(mailresult,"0",1) != 0){
			ASM_MSG("\"Fail\"\n");
			goto out;
		}		
#else
#error "Need to define the mail client first\n"
#endif
	}
	else if(emailfw_cfg != NULL){
#if defined(CONFIG_PACKAGE_msmtp)
		snprintf(cmdbuf, sizeof(cmdbuf), 
				 "msmtp --host=%s --user=%s --passwordeval=\"echo %s\" --port=%d -f %s --tls=on --tls-starttls=on --tls-certcheck=off --auth=on %s < %s;echo $?",
				 emailfw_cfg->SMTP_server, emailfw_cfg->Username, emailfw_cfg->Password,
				 emailfw_cfg->port, emailfw_cfg->EmailAddress, emailfw_cfg->EmailAddress, message);
		osSystem_GetOutput(cmdbuf,mailresult,sizeof(mailresult));
		if(strncmp(mailresult,"0",1) != 0){
			ASM_MSG("\"Fail\"\n");
			goto out;
		}
#elif defined(CONFIG_PACKAGE_ssmtp)
		ASM_MSG("Command Line of ssmtp is not ready yet.\n");
		goto out;
#else
#error "Need to define the mail client first\n"
#endif		
	}
	else{
		ASM_MSG("Need to define a configuration file or provide the configuration\n");
		goto out;
	}

	ret = MID_SUCCESS;

out:
	unlink(message);
	return ret;
}

int analyze_country_code(const char *caller, int grt_num)
{
	mapi_grtmsg_t grtmsg_cfg;
	int len=0, code_flag=0, j;
	memset(&grtmsg_cfg, 0, sizeof(grtmsg_cfg));
	mapi_ansmach_get_grtmsg(&grtmsg_cfg, 0);



	if(caller[0] == '+')
	{  
		for (j=0;j<MAPI_ASM_COUNTRY_CODE_COUNT;j++)
		{
			len = strlen(grtmsg_cfg.grtmsg[grt_num].CountryCode[j]);
			if( (len != 0) && (strncmp( caller+1, grtmsg_cfg.grtmsg[grt_num].CountryCode[j], len) == 0) )
				code_flag ++;
		}
	}
	else if(caller[0] == '0' && caller[1] == '0')
	{
		for (j = 0; j < MAPI_ASM_COUNTRY_CODE_COUNT; j++)
		{
			len = strlen(grtmsg_cfg.grtmsg[grt_num].CountryCode[j]);
			if ((len != 0) && (strncmp(caller + 2, grtmsg_cfg.grtmsg[grt_num].CountryCode[j], len) == 0))
			{
				code_flag++;
			}
		}
	}
	return code_flag;
}

int mapi_ansmach_get_grtmsg_Idx_AftSec_Size(char *caller, char *callee, int *idx, int *aft_sec, int *size)
{
	mapi_grtmsg_t grtmsg_cfg;
	mapi_ansmach_account_info_list_t list;
	mapi_ansmach_account_info_t *account;

	int i, j, k, s;
	int callee_flag;
	int caller_flag;
	int caller_code_flag;
	int caller_num_flag;

	*idx = -1;
	*aft_sec = 0;
	*size =0;

	memset(&grtmsg_cfg, 0, sizeof(grtmsg_cfg));
	mapi_ansmach_get_grtmsg(&grtmsg_cfg, 0);

	for (s=0;s<MAPI_ASM_MAX_GMSG_COUNT;s++)			
	{
		switch(s)
		{	
			case 0:
				i=1;
				break;
			case 1:
				i=2;
				break;				
			case 2:
				i=0;
				break;				
		}
		
		callee_flag=0;
		caller_flag=0;
		caller_code_flag=0;
		caller_num_flag=0;

		if(grtmsg_cfg.grtmsg[i].Enable == 1)
		{	//check callee number
			printf("grtmsg_cfg.grtmsg[%d].AsiToAll=%d\n", i, grtmsg_cfg.grtmsg[i].AsiToAll);

			if(grtmsg_cfg.grtmsg[i].AsiToAll == 1)
			{
				if(mapi_ansmach_account_info_get_all(&list) == MID_FAIL)
				{
					printf("Can not found callee from DB\n");
					*idx = -1;
				}

				account = list.account;

				for(k=0; k<(list.len); k++)
				{
					printf("account->userId=%s\n", account->userId);
					if( strcmp(account->userId, callee) == 0)
					{
						callee_flag++;
						printf("FOUND¡@callee, account->userId=%s, callee_flag=%d\n",
							   account->userId, callee_flag);
					}
					account++;
				}

				mapi_ansmach_account_info_list_put(&list);
			}
			else
			{
				for (j=0;j<MAPI_ASM_ASI_TO_COUNT;j++)
				{
					if( strcmp(grtmsg_cfg.grtmsg[i].AsiTo[j], callee) == 0)
					{
						callee_flag++;
						printf("FOUND¡@callee, grtmsg_cfg.grtmsg[%d].AsiTo[%d]=%s, callee_flag=%d\n",
							   i, j ,grtmsg_cfg.grtmsg[i].AsiTo[j], callee_flag);
					}
				}
			}

			//check caller number
			if((grtmsg_cfg.grtmsg[i].CodeEnable == 0) && (grtmsg_cfg.grtmsg[i].NumEnable == 0))
			{	// don't check
				caller_flag++;
				printf("C=0, N=0, caller_flag=%d\n", caller_flag);
			}
			else if((grtmsg_cfg.grtmsg[i].CodeEnable == 1) && (grtmsg_cfg.grtmsg[i].NumEnable == 0))
			{	//check country_code only match
				if( analyze_country_code(caller, i) != 0)
				{
					caller_flag++;
					printf("C=1, N=0, FOUND,¡@caller_flag=%d\n", caller_flag);
				}
			}
			else if((grtmsg_cfg.grtmsg[i].CodeEnable == 0) && (grtmsg_cfg.grtmsg[i].NumEnable == 1))
			{	//check phone_number only match
				if(grtmsg_cfg.grtmsg[i].PhoneNumToAll == 1)
				{

					int ret_idx;
					mapi_ccfg_voip_phone_book_t voip_phone_book={0};

					if(mapi_ccfg_get_voip_phone_book(0, &voip_phone_book)==MID_FAIL)
					{
						printf("mapi_ccfg_get_voip_phone_book fail\n");
					}
					else
					{
						//if(mapi_ccfg_get_Record_by_PhoneNum(&voip_phone_book, action_iteam, &ret_idx)==MID_FAIL)
						if(mapi_ccfg_get_Record_by_PhoneNum(&voip_phone_book, caller, &ret_idx)==MID_FAIL)
						{
							printf("mapi_ccfg_get_Record_by_PhoneNum fail\n");
						}
						else
						{
							if(ret_idx!=0)
							{
								printf("R(%d) name(%s) phone_name(%s) phone_num(%s)\n", ret_idx, voip_phone_book.contact[ret_idx-1].name, voip_phone_book.contact[ret_idx-1].phone_name, voip_phone_book.contact[ret_idx-1].phone_num);
								caller_flag++;
								printf("C=0, N=1, FOUND¡@caller_flag=%d\n",caller_flag);
							}
						}
					}
				}
				else
				{
					for (j=0;j<MAPI_ASM_PHONE_NUMBER_COUNT;j++)
					{
						if( strcmp(grtmsg_cfg.grtmsg[i].PhoneNum[j], caller) == 0)
						{
							caller_flag++;
							printf("C=0, N=1, FOUND¡@grtmsg_cfg.grtmsg[%d].PhoneNum[%d]=%s, caller_flag=%d\n",
								   i, j ,grtmsg_cfg.grtmsg[i].PhoneNum[j], caller_flag);
						}
					}
				}
			}
			else if((grtmsg_cfg.grtmsg[i].CodeEnable == 1) && (grtmsg_cfg.grtmsg[i].NumEnable == 1))
			{	//check country_code & phone_number match
					if( analyze_country_code(caller, i) != 0)
					{
						caller_code_flag++;
						printf("C=1, N=1, FOUND, caller_code_flag=%d\n", caller_code_flag);
					}

					if(grtmsg_cfg.grtmsg[i].PhoneNumToAll == 1)
					{

						int ret_idx;
						mapi_ccfg_voip_phone_book_t voip_phone_book={0};

						if(mapi_ccfg_get_voip_phone_book(0, &voip_phone_book)==MID_FAIL)
						{
							printf("mapi_ccfg_get_voip_phone_book fail\n");
						}
						else
						{
							//if(mapi_ccfg_get_Record_by_PhoneNum(&voip_phone_book, action_iteam, &ret_idx)==MID_FAIL)
							if(mapi_ccfg_get_Record_by_PhoneNum(&voip_phone_book, caller, &ret_idx)==MID_FAIL)
							{
								printf("mapi_ccfg_get_Record_by_PhoneNum fail\n");
							}
							else
							{
								if(ret_idx!=0)
								{
									printf("R(%d) name(%s) phone_name(%s) phone_num(%s)\n", ret_idx, voip_phone_book.contact[ret_idx-1].name, voip_phone_book.contact[ret_idx-1].phone_name, voip_phone_book.contact[ret_idx-1].phone_num);
									caller_num_flag++;
									printf("C=1, N=1, FOUND¡@caller_num_flag=%d\n", caller_num_flag);
								}
							}
						}
					}
					else
					{
						for (j=0;j<MAPI_ASM_PHONE_NUMBER_COUNT;j++)
						{
							if( strcmp(grtmsg_cfg.grtmsg[i].PhoneNum[j], caller) == 0)
							{
								caller_num_flag++;
								printf("C=1, N=1, FOUND¡@grtmsg_cfg.grtmsg[%d].PhoneNum[%d]=%s, caller_num_flag=%d\n",
									   i, j ,grtmsg_cfg.grtmsg[i].PhoneNum[j], caller_num_flag);
							}
						}
					}

					if((caller_code_flag !=0) && (caller_num_flag !=0))
					{
						caller_flag++;
						printf("C=1, N=1, caller_flag=%d\n",caller_flag);
					}
			}

			if(( callee_flag !=0 ) && (caller_flag !=0 ))
			{
				if (grtmsg_cfg.grtmsg[i].filesize !=0)
					*idx = i;
				else
					*idx = -1;

				*aft_sec = grtmsg_cfg.grtmsg[i].AfterSec;
				*size = grtmsg_cfg.grtmsg[i].filesize;

				printf("*idx=%d, *aft_sec=%d, *size=%d\n",*idx, *aft_sec, *size );
				return 0;
			}
		}
	}
	return 0;
}

#if 0
/*******************************************************************************
 * Description
 *		Get Email Forwarding configuration
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *      p_email_fw_cfg:	pointer to the buffer of data structure mapi_email_fw_t
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or p_email_fw_cfg is NULL.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_ansmach_get_emailfw(int tid, mapi_email_fw_t* p_email_fw_cfg, int factory_dft)
{
	int	new_transc=0;

    if ( p_email_fw_cfg == NULL ) {
        MID_TRACE("[%s] NULL p_email_fw_cfg\n", __FUNCTION__);
        return MID_FAIL;
    }

    if ( tid == 0 ) {
        if ( (tid=mapi_start_transc()) == MID_FAIL )
            return MID_FAIL;
        new_transc = 1;
    }

    if(factory_dft){
        mapi_ccfg_switch_in_out_factory_cfg(tid, 1);
    }

	p_email_fw_cfg->Enable = mapi_ccfg_get_long( tid, MAPI_ASM_EMAILFW_SECTION_NAME, "enable", 0);
	mapi_ccfg_get_str( tid, MAPI_ASM_EMAILFW_SECTION_NAME, "email_address", p_email_fw_cfg->EmailAddress, MAPI_ASM_EMAIL_ADDRESS_LEN, NULL);
	mapi_ccfg_get_str( tid, MAPI_ASM_EMAILFW_SECTION_NAME, "password", p_email_fw_cfg->Password, MAPI_ASM_PASSWORD_LEN, NULL);
	mapi_ccfg_get_str( tid, MAPI_ASM_EMAILFW_SECTION_NAME, "smtp_server", p_email_fw_cfg->SMTP_server, MAPI_ASM_SMTP_SERVER_LEN, NULL);

    if(factory_dft) {
        mapi_ccfg_switch_in_out_factory_cfg(tid, 0);
    }

    if(new_transc) {
        if(mapi_end_transc(tid)==MID_FAIL) {
            return MID_FAIL;
        }
    }

	return MID_SUCCESS;
}


/*******************************************************************************
 * Description
 *		Set Email Forwarding configuration
 *
 * Parameters
 *		tid:			transaction ID returned by calling mapi_start_transc()
 *      p_email_fw_cfg:	pointer to the buffer of data structure mapi_email_fw_t
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or p_email_fw_cfg is NULL.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_ansmach_set_emailfw(int tid, mapi_email_fw_t* p_email_fw_cfg)
{
	int	new_transc=0,i,j;
	char tmp_para[64];

	if ( p_email_fw_cfg == NULL ) {
        MID_TRACE("[%s] NULL p_email_fw_cfg\n", __FUNCTION__);
        return MID_FAIL;
    }

    if ( tid == 0 ) {
        if ( (tid=mapi_start_transc()) == MID_FAIL )
            return MID_FAIL;
        new_transc = 1;
    }

	mapi_ccfg_set_long( tid, MAPI_ASM_EMAILFW_SECTION_NAME, "enable", p_email_fw_cfg->Enable);
	mapi_ccfg_set_str( tid, MAPI_ASM_EMAILFW_SECTION_NAME, "email_address", p_email_fw_cfg->EmailAddress);
	mapi_ccfg_set_str( tid, MAPI_ASM_EMAILFW_SECTION_NAME, "password", p_email_fw_cfg->Password);
	mapi_ccfg_set_str( tid, MAPI_ASM_EMAILFW_SECTION_NAME, "smtp_server", p_email_fw_cfg->SMTP_server);

    if(new_transc){
        if(mapi_end_transc(tid)==MID_FAIL){
            return MID_FAIL;
        }
    }

	return MID_SUCCESS;
}
#endif

int mapi_ansmach_hdl_cli(int tid, U16 asm_op, void *data, U32 len)
{
	char handle[len];

	ASM_DBG("Enter\n");

	memcpy(handle, data, len);
	if (mapi_trx_transc( tid, MID_MSG_MOD_ASM, asm_op, handle, len) != MID_SUCCESS) {
		ASM_MSG( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		return MID_FAIL;
	}

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Process transaction for Recording Message save
 *
 * Parameters
 *		hdl_op:			handler operation structure for transaction
 *      hdl_hdr:		header of Recording Message being saved
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdl_op/hdl_hdr is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_recMsg_save(mid_hdl_op_t *hdl_op, mid_hdl_rec_hdr_udata_t *hdl_hdr)
{
	void *op = NULL;
	int tid;

	if((hdl_op == NULL) || (hdl_hdr == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	if(!(op = malloc(sizeof(mid_hdl_op_t) + sizeof(mid_hdl_rec_hdr_udata_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	/* fill op */
	memcpy((mid_hdl_op_t *) op, hdl_op, sizeof(mid_hdl_op_t));
	memcpy(((mid_hdl_op_t *) op)->data, hdl_hdr, sizeof(mid_hdl_rec_hdr_udata_t));

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_REC_SAVE, op, sizeof(mid_hdl_op_t) + sizeof(mid_hdl_rec_hdr_udata_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);
	
	if (op != NULL)
		free(op);

	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Save Recording Message for phone
 *
 * Parameters
 * 		hdr->time_millisec:	UTC time in millisecond
 * 							you can get it from:
 * 							{
 * 								#include <sys/timeb.h>
 * 								struct timeb tp;
 * 								ftime(&tp);
 * 								time_millisec = (tp.time * 1000) + tp.millitm;
 * 							}
 *		hdr->caller:		caller of the Recording Message
 *      hdr->callee:		callee of the Recording Message
 *      hdr->filepath:		full path of the Recording Message G.711 file
 *
 * Returns
 *		MID_SUCCESS(0): 	successfully
 *		MID_FAIL(-1):   	failed to process transaction or hdr (hdr->parameters) is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_recMsg_save(mapi_ansmach_phone_msg_hdr_t *hdr)
{
	mid_hdl_rec_hdr_udata_t hdl_hdr;
	mid_hdl_op_t hdl_op;
	int file_size = 0;
	struct stat st;
	unsigned long long *id_list[1];
	pid_t pid;

	if((hdr == NULL) || (hdr->caller == NULL) || (hdr->callee == NULL) || (hdr->filepath == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	if(stat(hdr->filepath, &st) == 0){
		file_size  = st.st_size;
		//printf("%s: %d (bytes)\n", filepath, file_size);
	}

	/* fill hdl_hdr */
	strcpy(hdl_hdr.caller_num, hdr->caller);
	strcpy(hdl_hdr.callee_num, hdr->callee);

#if 0
	{
		// remove area code if need
		int i;
		int ret;
		mapi_ansmach_account_info_list_t list;
		mapi_ansmach_account_info_t *account;

		ret = mapi_ansmach_account_info_get_all(&list);
		if(ret == MID_SUCCESS && list.len > 0)
		{
			account = list.account;

			for(i=0; i<(list.len); i++)
			{
				if (strcmp(account->userId, hdr->callee) == 0)
				{
					if (account->userId_area[0] != 0 && account->userId_local[0] != 0)
					{
						strcpy(hdl_hdr.callee_num, account->userId_local); // remove area code for callee

						if (strncmp(account->userId_area, hdr->caller, strlen(account->userId_area)) == 0)
						{
							strcpy(hdl_hdr.caller_num, hdr->caller + strlen(account->userId_area)); // remove area code for caller
						}

						break;
					}
				}
				account++;
			}

			mapi_ansmach_account_info_list_put(&list);
		}
	}

	printf("\n#[%s] %d, caller: %s\n", __FUNCTION__, __LINE__, hdl_hdr.caller_num); 
	printf("\n#[%s] %d, callee: %s\n", __FUNCTION__, __LINE__, hdl_hdr.callee_num); 
#endif

	hdl_hdr.time = hdr->time_millisec;
	hdl_hdr.read = 0;
	hdl_hdr.size = file_size;

	/* fill hdl_op */
	strcpy(hdl_op.info.filepath, hdr->filepath);

	/* do transaction */
	if(mapi_ansmach_recMsg_save(&hdl_op, &hdl_hdr) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_recMsg_save() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* after saving to flash, delete tmp file */
	ASM_EXEC("rm %s", hdr->filepath);

	/* fork child to sendmail */
	pid = fork();
	if(pid >= 0)
	{
		if(pid==0)
		{
			id_list[0] = malloc(sizeof(unsigned long long));
			*id_list[0] = hdl_hdr.time;
			mapi_ansmach_emailfw_prepare(id_list, 1);
			free(id_list[0]);
			exit(0);
		}
		else
		{
			ASM_EXEC("renice 5 %d", pid);
		}
	}
	else
		ASM_MSG("%s.%d> # fork() error.\n", __FUNCTION__, __LINE__);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Process transaction for Recording Message load
 *
 * Parameters
 *		hdl_op:			handler operation structure for transaction
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdl_op is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_recMsg_load(mid_hdl_op_t *hdl_op)
{
	void *op = NULL;
	int tid;

	if(hdl_op == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	if(!(op = malloc(sizeof(mid_hdl_op_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	/* fill op */
	memcpy((mid_hdl_op_t *) op, hdl_op, sizeof(mid_hdl_op_t));

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_REC_LOAD, op, sizeof(mid_hdl_op_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);

	/* update filepath */
	strcpy(hdl_op->info.filepath, ((mid_hdl_op_t *) op)->info.filepath);

	if (op != NULL)
		free(op);

	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Load Recording Message for phone
 *
 * Parameters
 *		hdr->id:		id of the Recording Message
 *      hdr->filepath:	full path of the Recording Message G.711 file
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdr is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_recMsg_load(mapi_ansmach_phone_msg_hdr_t *hdr)
{
	mid_hdl_op_t hdl_op;

	if(hdr == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* fill hdl_op */
	hdl_op.id = hdr->id;
	strcpy(hdl_op.info.filepath, hdr->filepath);

	/* do transaction */
	if(mapi_ansmach_recMsg_load(&hdl_op) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_recMsg_load() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Load Recording Message (G.711 format) and convert it to Wav format
 *
 * Parameters
 *		id:				id of the Recording Message
 *      filepath:		full path of the Recording Message Wav file
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or G7112wav
 *
 ******************************************************************************/
int mapi_ansmach_recMsg_load_wav(unsigned long long id, char *filepath)
{
	mid_hdl_op_t hdl_op;
	char wavfile[MAPI_ASM_MAX_FILEPATH] = "";
	struct timeb tb_start, tb_end;
	unsigned long long duration_millisec;
	int ret;

	if(filepath == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ftime(&tb_start);

	ASM_EXEC("mkdir -p /tmp/www");

	/* load G.711 file (the updated filepath will be hdl_op.info.filepath) */
	hdl_op.id = id;
	//strcpy(hdl_op.info.filepath, filepath);
	sprintf(hdl_op.info.filepath, "/tmp/www/rec_%lld.711", id);	//P.S. Wav file in /tmp/www can be accessed by http request
	if(mapi_ansmach_recMsg_load(&hdl_op) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_recMsg_load() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	sprintf(wavfile, "/tmp/www/rec_%lld.wav", id);
	ret = G7112wav(hdl_op.info.filepath, wavfile);
	if(ret == MID_FAIL){
		ASM_MSG("%s(%d): G7112wav() failed...\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* after converting to Wav, delete 711 file */
	ASM_EXEC("rm %s", hdl_op.info.filepath);

	/* Convert a-law Wav to pcm Wav */
	ASM_EXEC("ffmpeg -i %s -acodec pcm_s16le -y %s", wavfile, filepath);

	/* after converting to pcm Wav, delete a-law Wav */
	ASM_EXEC("rm %s", wavfile);

	ftime(&tb_end);
	duration_millisec = (tb_end.time * 1000 + tb_end.millitm) - (tb_start.time * 1000 + tb_start.millitm);
	ASM_DBG("%s(%d): [Time] Convert : %f sec \n", __func__, __LINE__, (double) duration_millisec / 1000);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Load Recording Message (G.711 format) and convert it to Mp3 format
 *
 * Parameters
 *		id:				id of the Recording Message
 *      filepath:		full path of the Recording Message Mp3 file
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or G7112wav
 *
 ******************************************************************************/
int mapi_ansmach_recMsg_load_mp3(unsigned long long id, char *filepath)
{
	mid_hdl_op_t hdl_op;
	char wavfile[MAPI_ASM_MAX_FILEPATH] = "";
	char pcmwav[MAPI_ASM_MAX_FILEPATH] = "";
	struct timeb tb_start, tb_end;
	unsigned long long duration_millisec;
	int ret;

	if(filepath == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ftime(&tb_start);

	ASM_EXEC("mkdir -p /tmp/www");

	/* load G.711 file (the updated filepath will be hdl_op.info.filepath) */
	hdl_op.id = id;
	//strcpy(hdl_op.info.filepath, filepath);
	sprintf(hdl_op.info.filepath, "/tmp/www/rec_%lld_.711", id);	//P.S. Wav file in /tmp/www can be accessed by http request
	if(mapi_ansmach_recMsg_load(&hdl_op) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_recMsg_load() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	sprintf(wavfile, "/tmp/www/rec_%lld_.wav", id);
	ret = G7112wav(hdl_op.info.filepath, wavfile);
	if(ret == MID_FAIL){
		ASM_MSG("%s(%d): G7112wav() failed...\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* after converting to Wav, delete 711 file */
	ASM_EXEC("rm %s", hdl_op.info.filepath);

#if 0
	sprintf(pcmwav, "/tmp/www/rec_%lld_pcm.wav", id);
	/* Convert a-law Wav to pcm Wav */
	ASM_EXEC("ffmpeg -i %s -acodec pcm_s16le %s", wavfile, pcmwav);

	/* after converting to pcm Wav, delete a-law Wav */
	ASM_EXEC("rm %s", wavfile);

	/* Convert pcm Wav to Mp3 */
	ASM_EXEC("lame -V2 -f %s %s", pcmwav, filepath);

	/* after converting to Mp3, delete pcm Wav */
	ASM_EXEC("rm %s", pcmwav);
#else
	/* Convert a-law Wav to Mp3 */
	ASM_EXEC("lame -V2 -f %s %s", wavfile, filepath);

	/* after converting to Mp3, delete a-law Wav */
	ASM_EXEC("rm %s", wavfile);
#endif

	ftime(&tb_end);
	duration_millisec = (tb_end.time * 1000 + tb_end.millitm) - (tb_start.time * 1000 + tb_start.millitm);
	ASM_DBG("%s(%d): [Time] Convert : %f sec \n", __func__, __LINE__, (double) duration_millisec / 1000);

	return MID_SUCCESS;
}

static void show_wave_header(struct WAVE_HEADER *wave_header)
{
/*
	printf("sizeof(struct RIFF_CHUNK) = %d\n", sizeof(struct RIFF_CHUNK));
	printf("sizeof(struct FORMAT_CHUNK) = %d\n", sizeof(struct FORMAT_CHUNK));
	printf("sizeof(struct FACT_CHUNK) = %d\n", sizeof(struct FACT_CHUNK));
	printf("sizeof(struct DATA_CHUNK) = %d\n", sizeof(struct DATA_CHUNK));
	printf("sizeof(struct WAVE_HEADER) = %d\n", sizeof(struct WAVE_HEADER));
*/

	printf("\tWAVE_HEADER\n");
	printf("---------- RIFF_CHUNK ----------\n");
	printf("\tckID = %s\n", wave_header->riff_chunk.ckID);
		printf("\t\tckID[0] = %c\n", wave_header->riff_chunk.ckID[0]);
		printf("\t\tckID[1] = %c\n", wave_header->riff_chunk.ckID[1]);
		printf("\t\tckID[2] = %c\n", wave_header->riff_chunk.ckID[2]);
		printf("\t\tckID[3] = %c\n", wave_header->riff_chunk.ckID[3]);
	printf("\tckSize = %u\n", (unsigned int) wave_header->riff_chunk.ckSize);
	printf("\tWAVEID = %s\n", wave_header->riff_chunk.WAVEID);
		printf("\t\tWAVEID[0] = %c\n", wave_header->riff_chunk.WAVEID[0]);
		printf("\t\tWAVEID[1] = %c\n", wave_header->riff_chunk.WAVEID[1]);
		printf("\t\tWAVEID[2] = %c\n", wave_header->riff_chunk.WAVEID[2]);
		printf("\t\tWAVEID[3] = %c\n", wave_header->riff_chunk.WAVEID[3]);

	printf("---------- FORMAT_CHUNK ----------\n");
	printf("\tckID = %s\n", wave_header->fmt_chunk.ckID);
		printf("\t\tckID[0] = %c\n", wave_header->fmt_chunk.ckID[0]);
		printf("\t\tckID[1] = %c\n", wave_header->fmt_chunk.ckID[1]);
		printf("\t\tckID[2] = %c\n", wave_header->fmt_chunk.ckID[2]);
		printf("\t\tckID[3] = %c\n", wave_header->fmt_chunk.ckID[3]);
	printf("\tckSize = %u\n", (unsigned int) wave_header->fmt_chunk.ckSize);
	printf("\twFormatTag = %d\n", wave_header->fmt_chunk.wFormatTag);
	printf("\tnChannels = %d\n", wave_header->fmt_chunk.nChannels);
	printf("\tnSamplesPerSec = %u\n", (unsigned int) wave_header->fmt_chunk.nSamplesPerSec);
	printf("\tnAvgBytesPerSec = %u\n", (unsigned int) wave_header->fmt_chunk.nAvgBytesPerSec);
	printf("\tnBlockAlign = %d\n", wave_header->fmt_chunk.nBlockAlign);
	printf("\twBitsPerSample = %d\n", wave_header->fmt_chunk.wBitsPerSample);
	printf("\tcbSize = %d\n", wave_header->fmt_chunk.cbSize);

	printf("---------- FACT_CHUNK ----------\n");
	printf("\tckID = %s\n", wave_header->fact_chunk.ckID);
		printf("\t\tckID[0] = %c\n", wave_header->fact_chunk.ckID[0]);
		printf("\t\tckID[1] = %c\n", wave_header->fact_chunk.ckID[1]);
		printf("\t\tckID[2] = %c\n", wave_header->fact_chunk.ckID[2]);
		printf("\t\tckID[3] = %c\n", wave_header->fact_chunk.ckID[3]);
	printf("\tckSize = %u\n", (unsigned int) wave_header->fact_chunk.ckSize);
	printf("\tdwSampleLength = %u\n", (unsigned int) wave_header->fact_chunk.dwSampleLength);

	printf("---------- DATA_CHUNK ----------\n");
	printf("\tckID = %s\n", wave_header->data_chunk.ckID);
		printf("\t\tckID[0] = %c\n", wave_header->data_chunk.ckID[0]);
		printf("\t\tckID[1] = %c\n", wave_header->data_chunk.ckID[1]);
		printf("\t\tckID[2] = %c\n", wave_header->data_chunk.ckID[2]);
		printf("\t\tckID[3] = %c\n", wave_header->data_chunk.ckID[3]);
	printf("\tckSize = %u\n", (unsigned int) wave_header->data_chunk.ckSize);
}

static void fill_header(struct WAVE_HEADER *wave_header, int file_size)
{
	int systemIsBigEndian = 0;
	EndianTest endian;

	//riff_chunk
	strcpy((char *) wave_header->riff_chunk.ckID, "RIFF");
	wave_header->riff_chunk.ckSize = 50 + file_size;
	strcpy((char *) wave_header->riff_chunk.WAVEID, "WAVE");

	//fmt_chunk
	strcpy((char *) wave_header->fmt_chunk.ckID, "fmt ");
	wave_header->fmt_chunk.ckSize = 18;
	wave_header->fmt_chunk.wFormatTag = 6;
	wave_header->fmt_chunk.nChannels = 1;
	wave_header->fmt_chunk.nSamplesPerSec = 8000;
	wave_header->fmt_chunk.nAvgBytesPerSec = 8000;
	wave_header->fmt_chunk.nBlockAlign = 1;
	wave_header->fmt_chunk.wBitsPerSample = 8;
	wave_header->fmt_chunk.cbSize = 0;

	//fact_chunk
	strcpy((char *) wave_header->fact_chunk.ckID, "fact");
	wave_header->fact_chunk.ckSize = 4;
	wave_header->fact_chunk.dwSampleLength = file_size;

	//data_chunk
	strcpy((char *) wave_header->data_chunk.ckID, "data");
	wave_header->data_chunk.ckSize = file_size;

	//check Endian
	endian.i = 1;	//0x00000001
	if(endian.c[0] == 0x01){
		//printf("LITTLE_ENDIAN is defined\n");	//host PC (x86) here!
		systemIsBigEndian = 0;
	}
	else if(endian.c[0] == 0x00){
		//printf("BIG_ENDIAN is defined\n");	//904LTE (mips) here
		systemIsBigEndian = 1;
	}
	else
		ASM_MSG("%s(%d): [ansmach] One of BIG_ENDIAN or LITTLE_ENDIAN must be #define'd.\n", __func__, __LINE__);


	if(systemIsBigEndian){
		wave_header->riff_chunk.ckSize = bswap_32(wave_header->riff_chunk.ckSize);

		wave_header->fmt_chunk.ckSize = bswap_32(wave_header->fmt_chunk.ckSize);
		wave_header->fmt_chunk.wFormatTag = bswap_16(wave_header->fmt_chunk.wFormatTag);
		wave_header->fmt_chunk.nChannels = bswap_16(wave_header->fmt_chunk.nChannels);
		wave_header->fmt_chunk.nSamplesPerSec = bswap_32(wave_header->fmt_chunk.nSamplesPerSec);
		wave_header->fmt_chunk.nAvgBytesPerSec = bswap_32(wave_header->fmt_chunk.nAvgBytesPerSec);
		wave_header->fmt_chunk.nBlockAlign = bswap_16(wave_header->fmt_chunk.nBlockAlign);
		wave_header->fmt_chunk.wBitsPerSample = bswap_16(wave_header->fmt_chunk.wBitsPerSample);
		wave_header->fmt_chunk.cbSize = bswap_16(wave_header->fmt_chunk.cbSize);

		wave_header->fact_chunk.ckSize = bswap_32(wave_header->fact_chunk.ckSize);
		wave_header->fact_chunk.dwSampleLength = bswap_32(wave_header->fact_chunk.dwSampleLength);

		wave_header->data_chunk.ckSize = bswap_32(wave_header->data_chunk.ckSize);
	}

	//show_wave_header(wave_header);
}

static int do_G7112wav(int file_size, FILE *fp_in, FILE *fp_out)
{
	struct WAVE_HEADER wave_header;
	int data_size = file_size;
	char *data;

	if((fp_in == NULL) || (fp_out == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	memset(&wave_header, 0, sizeof(struct WAVE_HEADER));
	fill_header(&wave_header, file_size);

	//write WAVE header to the beginning
	//printf("sizeof(wave_header) = %d\n", sizeof(wave_header));
	fwrite(&wave_header, sizeof(wave_header), 1, fp_out);

	//write original G.711 data
	data = malloc(sizeof(char) * data_size);
	if(data == NULL){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		return MID_FAIL;
	}
	fread(data, sizeof(char), data_size, fp_in);
	fwrite(data, sizeof(char), data_size, fp_out);

	free(data);
	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Convert G.711 file to Wav file
 *
 * Parameters
 *		g711_file:		full path of G.711 file
 *      wav_file:		full path of Wav file
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or g711_file/wav_file is NULL
 *
 ******************************************************************************/
int G7112wav(char *g711_file, char *wav_file)
{
	FILE *g711_in, *wav_out;
	int 	file_size = 0;
	//char	*ch;
	struct stat	st;
	int		duration_sec = 0;
	char	sCmd[128];
	char	szData[2];
	int		ret;

	if((g711_file == NULL) || (wav_file == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	//printf("@G7112wav() : g711_file = %s\n", g711_file);

	//printf("sizeof(struct FORMAT_CHUNK) = %d\n", sizeof(struct FORMAT_CHUNK));	//26->OK
/*
	strcpy(wav_file, g711_file);
	ch = strstr(wav_file, ".");
	*ch = '\0';
	strcat(wav_file, ".wav");
	//printf("@G7112wav() : real wav_location = %s\n", wav_file);
*/
	//sprintf(sCmd, "[ -f %s ] && echo \"1\" || echo \"0\"", wav_file);
	sprintf(sCmd, "ls %s ; echo $?", wav_file);
	osSystem_GetOutput(sCmd, szData, sizeof(szData));
	if(!atoi(szData)){
		ASM_DBG("%s(%d): %s already exist\nNo need to convert, just return...\n", __func__, __LINE__, wav_file);
		return MID_SUCCESS;
	}
	else{
		ASM_DBG("%s(%d): %s does not exist\n", __func__, __LINE__, wav_file);
	}

	g711_in = fopen(g711_file, "rb");
	if(g711_in == NULL){
		ASM_MSG("%s(%d): Can't open input file %s\n", __func__, __LINE__, g711_file);
		return MID_FAIL;
	}

	wav_out = fopen(wav_file, "wb");
	if(wav_out == NULL){
		ASM_MSG("%s(%d): Can't open output file %s\n", __func__, __LINE__, wav_file);
		fclose(g711_in);
		return MID_FAIL;
	}

	if(stat(g711_file, &st) == 0){
		file_size  = st.st_size;
		//printf("%s: %d (bytes)\n", g711_file, file_size);
	}

	duration_sec = file_size / 8000;
	ASM_DBG("%s(%d): Duration : %d secs\n", __func__, __LINE__, duration_sec);

	ret = do_G7112wav(file_size, g711_in, wav_out);
	if(ret == MID_FAIL)
		ASM_MSG("%s(%d): Failed: convert %s to %s\n", __func__, __LINE__, g711_file, wav_file);
	else
		ASM_DBG("%s(%d): Success: convert %s to %s\n", __func__, __LINE__, g711_file, wav_file);

	fclose(g711_in);
	fclose(wav_out);
	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Display All Headers of Recording Messages
 *
 * Parameters
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction
 *
 ******************************************************************************/
int mapi_ansmach_recMsg_show_hdrs(void)
{
	mid_hdl_rec_hdr_udata_t *hdrp = NULL;
	mid_hdl_rec_hdr_udata_t *hdrp_ori = NULL;
	int total_hdr_num = 0;
	int i;
	int ret;

	ret = mapi_ansmach_recMsg_hdr_get_all(&hdrp, &total_hdr_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	if(hdrp == NULL){
		ASM_MSG("%s(%d): No recMsg hdr exist\n", __func__, __LINE__);
		return MID_FAIL;
	}

	hdrp_ori = hdrp;

	ASM_MSG("---------------------------------------------------------------------\n");
	ASM_MSG("id\t\ttime\t\t\tcaller_num\tcallee_num\tread\tsize\t\t(total_hdr_num=%d)\n", total_hdr_num);
	ASM_MSG("---------------------------------------------------------------------\n");
	for(i=0; i<total_hdr_num; i++){
		struct tm tm;
		time_t time_sec;
		mapi_ansmach_time_t time;

		memset(&tm, 0, sizeof(struct tm));
		time_sec = (U32) (hdrp->time / 1000);	//sec = millisec / 1000
		localtime_r((const time_t *) &time_sec, &tm);
		time.tm_sec = tm.tm_sec;
		time.tm_min = tm.tm_min;
		time.tm_hour = tm.tm_hour;
		time.tm_mday = tm.tm_mday;
		time.tm_mon = tm.tm_mon + 1;
		time.tm_year = tm.tm_year + 1900;

		ASM_MSG("%lld\t{%d/%d/%d/%d:%d:%d}\t%s\t%s\t\t%u\t%u\n",
			(unsigned long long) hdrp->time,
			time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec,
			hdrp->caller_num,
			hdrp->callee_num,
			(unsigned int) hdrp->read,
			(unsigned int) hdrp->size);

		hdrp++;
	}
	ASM_MSG("---------------------------------------------------------------------\n");

	if(hdrp_ori != NULL)
		free(hdrp_ori);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Display specified Headers of Recording Messages (For Debug Use)
 *
 * Parameters
 *		hdrp: 			headers of specified Recording Messages
 *		len: 			valid length of specified Recording Messages
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdrp is NULL
 *
 ******************************************************************************/
static int show_these_recMsg_hdrs(mid_hdl_rec_hdr_udata_t *hdrp, int len)
{
	int total_hdr_num = len;
	int i;

	if(hdrp == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ASM_MSG("---------------------------------------------------------------------\n");
	ASM_MSG("id\t\ttime\t\t\tcaller_num\tcallee_num\tread\tsize\t\t(total_hdr_num=%d)\n", total_hdr_num);
	ASM_MSG("---------------------------------------------------------------------\n");
	for(i=0; i<total_hdr_num; i++){
		struct tm tm;
		time_t time_sec;
		mapi_ansmach_time_t time;

		memset(&tm, 0, sizeof(struct tm));
		time_sec = (U32) (hdrp->time / 1000);	//sec = millisec / 1000
		localtime_r((const time_t *) &time_sec, &tm);
		time.tm_sec = tm.tm_sec;
		time.tm_min = tm.tm_min;
		time.tm_hour = tm.tm_hour;
		time.tm_mday = tm.tm_mday;
		time.tm_mon = tm.tm_mon + 1;
		time.tm_year = tm.tm_year + 1900;

		ASM_MSG("%lld\t{%d/%d/%d/%d:%d:%d}\t%s\t%s\t\t%u\t%u\n",
			(unsigned long long) hdrp->time,
			time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec,
			hdrp->caller_num,
			hdrp->callee_num,
			(unsigned int) hdrp->read,
			(unsigned int) hdrp->size);

		hdrp++;
	}
	ASM_MSG("---------------------------------------------------------------------\n");

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get All Recording Messages (type: mid_hdl_rec_hdr_udata_t) belong to the same callee
 *
 * Parameters
 *		callee:			callee of the Recording Messages
 *		hdrp:			headers of all Recording Messages
 *		len: 			valid length of all Recording Messages
 *		list->hdr_num:	numbers of all headers
 *
 * Returns
 *		hdrp: 			headers of all Recording Messages with requested callee sorted on top
 *		R_num: 			number of Recording Messages belong to the same callee
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or callee/hdrp/R_num is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_recMsg_hdr_get_by_callee(char *callee, mid_hdl_rec_hdr_udata_t *hdrp, int len, int *R_num)
{
	mid_hdl_rec_hdr_udata_t *hdrs_callee, *hdrs_else;
	mid_hdl_rec_hdr_udata_t *call_p, *else_p;
	mid_hdl_rec_hdr_udata_t *hdrp_ori = hdrp;
	int num_callee = 0, num_else = 0;
	int i;

	if((callee == NULL) || (hdrp == NULL)|| (len <= 0) || (R_num == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	hdrs_callee = malloc(sizeof(mid_hdl_rec_hdr_udata_t) * len);
	if(hdrs_callee == NULL){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		return MID_FAIL;
	}
	call_p = hdrs_callee;

	hdrs_else = malloc(sizeof(mid_hdl_rec_hdr_udata_t) * len);
	if(hdrs_else == NULL){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		free(hdrs_callee);
		return MID_FAIL;
	}
	else_p = hdrs_else;

	for(i=0; i<len; i++){
		if(strcmp(hdrp->callee_num, callee) == 0){
			memcpy(call_p, hdrp, sizeof(mid_hdl_rec_hdr_udata_t));
			num_callee++;
			call_p++;
		}
		else{
			memcpy(else_p, hdrp, sizeof(mid_hdl_rec_hdr_udata_t));
			num_else++;
			else_p++;
		}

		hdrp++;
	}

	//check
	//mapi_ansmach_recMsg_show_hdrs();
	//mapi_ansmach_recMsg_show_hdrs();

	//check
	ASM_DBG("%s(%d): len=%d  (num_callee=%d,  num_else=%d)\n", __func__, __LINE__, len, num_callee, num_else);

	//replace original array
	memcpy(hdrp_ori, hdrs_callee, sizeof(mid_hdl_rec_hdr_udata_t)*num_callee);

	//memcpy((mid_hdl_rec_hdr_udata_t *)(hdrp_ori+num_callee), hdrs_else, sizeof(mid_hdl_rec_hdr_udata_t)*num_else);
	//memcpy(((char *)hdrp_ori) + sizeof(mid_hdl_rec_hdr_udata_t)*num_callee, hdrs_else, sizeof(mid_hdl_rec_hdr_udata_t)*num_else);
	memcpy(&hdrp_ori[num_callee], hdrs_else, sizeof(mid_hdl_rec_hdr_udata_t)*num_else);

	*R_num = num_callee;

	free(hdrs_callee);
	free(hdrs_else);

	return MID_SUCCESS;
}

int time_compare_desc(const void *x, const void *y)	//descending by time
{
	mid_hdl_rec_hdr_udata_t *a = (mid_hdl_rec_hdr_udata_t *) x;
	mid_hdl_rec_hdr_udata_t *b = (mid_hdl_rec_hdr_udata_t *) y;

	return ( b->time - a->time );
}

/*******************************************************************************
 * Description
 *		Sort All Recording Messages by time (descending order)
 *
 * Parameters
 *		hdrp:			headers of all Recording Messages
 *		len: 			valid length of all Recording Messages
 *
 * Returns
 *		hdrp: 			headers of all Recording Messages in time descending order
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdrp is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_recMsg_hdr_sort_by_time(mid_hdl_rec_hdr_udata_t *hdrp, int len)
{
	if(hdrp == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	if(len <= 0){
		ASM_DBG("%s(%d): len=0, no need to sort\n", __func__, __LINE__);
		return MID_SUCCESS;
	}

	qsort(hdrp, len, sizeof(mid_hdl_rec_hdr_udata_t), time_compare_desc);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Sort All Recording Messages by Read Status (order: unread->read)
 *
 * Parameters
 *		hdrp:			headers of all Recording Messages
 *		len: 			valid length of all Recording Messages
 *
 * Returns
 *		hdrp: 			headers of all Recording Messages in read status order
 *		R_newNum: 		number of New (Unread) Recording Messages
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdrp is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_recMsg_hdr_sort_by_readStatus(mid_hdl_rec_hdr_udata_t *hdrp, int len, int *R_newNum)
{
	mid_hdl_rec_hdr_udata_t *hdrs_new, *hdrs_old;
	mid_hdl_rec_hdr_udata_t *new_p, *old_p;
	mid_hdl_rec_hdr_udata_t *hdrp_ori = hdrp;
	int num_new = 0, num_old = 0;
	int i;

	if((hdrp == NULL)|| (len <= 0) || (R_newNum == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	hdrs_new = malloc(sizeof(mid_hdl_rec_hdr_udata_t) * len);
	if(hdrs_new == NULL){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		return MID_FAIL;
	}
	new_p = hdrs_new;

	hdrs_old = malloc(sizeof(mid_hdl_rec_hdr_udata_t) * len);
	if(hdrs_old == NULL){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		free(hdrs_new);
		return MID_FAIL;
	}
	old_p = hdrs_old;

	for(i=0; i<len; i++){
		if(hdrp->read == 1){	//Readed -> old
			memcpy(old_p, hdrp, sizeof(mid_hdl_rec_hdr_udata_t));
			num_old++;
			old_p++;
		}
		else{	//UnReaded -> new
			memcpy(new_p, hdrp, sizeof(mid_hdl_rec_hdr_udata_t));
			num_new++;
			new_p++;
		}

		hdrp++;
	}

	//check
	ASM_DBG("%s(%d): len=%d  (num_new=%d,  num_old=%d)\n", __func__, __LINE__, len, num_new, num_old);

	//replace original array
	memcpy(hdrp_ori, hdrs_new, sizeof(mid_hdl_rec_hdr_udata_t)*num_new);
	memcpy(&hdrp_ori[num_new], hdrs_old, sizeof(mid_hdl_rec_hdr_udata_t)*num_old);

	*R_newNum = num_new;

	free(hdrs_new);
	free(hdrs_old);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get All Recording Message headers
 *
 * Parameters
 *
 * Returns
 *		R_hdrs: 		headers of all Recording Messages
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or num is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_recMsg_hdr_get_all(mid_hdl_rec_hdr_udata_t **R_hdrs, int *num)
{
	mid_hdl_rec_hdr_udata_t *hdr_r=NULL, one_hdr;
	mid_hdl_op_t hdl_op;
	U32 hdr_num, udata_hdr_size;
	int tid;
	int i, fd=-1;
	char filepath[MAPI_ASM_MAX_FILEPATH];

	if(num == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_REC_HDR_GET_INUSE2, &hdl_op, sizeof(mid_hdl_op_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);	//solution for RID #1202
		goto err;
	}

	mapi_end_transc(tid);

	sprintf(filepath, "%s", hdl_op.info.filepath);
	hdr_num = (U32) hdl_op.id;
	if (hdr_num == 0) {
		*num = 0;
		unlink(filepath);	//ansmach hdl still creates the temp file even there is no rec msg, we must unlink the file before return.
		return MID_SUCCESS;
	}

	udata_hdr_size = sizeof(mid_hdl_rec_hdr_udata_t) * hdr_num;
	if (!(hdr_r = malloc(udata_hdr_size))) {
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}
	memset(hdr_r, 0, udata_hdr_size);	//for read fail case below

	if ((fd = open(filepath, O_RDONLY)) == -1) {
		ASM_MSG("%s(%d): Open file %s Failed\n", __func__, __LINE__, filepath);
		goto err;
	}

#if 0
	for (i = 0; i < hdr_num; i++) {
		if (read(fd, (void *) &one_hdr, sizeof(mid_hdl_rec_hdr_udata_t)) < sizeof(mid_hdl_rec_hdr_udata_t)) {
			ASM_MSG("%s(%d): Read file %s Failed in %dth times\n", __func__, __LINE__, filepath, i);
			goto close_file;	//read fail case
		}

		memcpy(&hdr_r[i], &one_hdr, sizeof(mid_hdl_rec_hdr_udata_t));
	}
#else
	i = read(fd, (void *) hdr_r, udata_hdr_size);
	if (i != udata_hdr_size) {
		ASM_MSG("%s(%d): Read file %s only %d of %d bytes  \n", __func__, __LINE__, filepath, i, (int) udata_hdr_size);
		goto close_file;	//read fail case
	}
#endif

close_file:
	if (fd != -1) close(fd);
	unlink(filepath);

	*R_hdrs = hdr_r;
	*num = hdr_num;
	return MID_SUCCESS;

err:
	if (hdr_r != NULL)
		free(hdr_r);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Get All Recording Messages belong to the same callee for phone
 *
 * Parameters
 *		list->callee:	callee of the Recording Messages
 *
 * Returns
 *		list: 			list infomation (numbers) & headers (list->hdr)
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or list is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_recMsg_list_get(mapi_ansmach_phone_msg_list_t *list)
{
	mid_hdl_rec_hdr_udata_t *hdl_hdr = NULL;
	int total_hdr_num = 0;
	int callee_num = 0;
	int new_num = 0;
	int ret;
	char *callee;
	int i;
	mapi_ansmach_phone_msg_hdr_t *phone_hdr = NULL, *phone_hdr_ori = NULL;
	mid_hdl_rec_hdr_udata_t *hdl_hdr_ori = NULL;

	if(list == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ret = mapi_ansmach_recMsg_hdr_get_all(&hdl_hdr, &total_hdr_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	if(total_hdr_num == 0){
		ASM_DBG("%s(%d): No msgs existed.\n", __func__, __LINE__);
		goto notfound;
	}

	callee = list->callee;

	ret = mapi_ansmach_recMsg_hdr_get_by_callee(callee, hdl_hdr, total_hdr_num, &callee_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	if(callee_num == 0){
		ASM_DBG("%s(%d): msgs for callee \"%s\" are not found.\n", __func__, __LINE__, callee);
		goto notfound;
	}

	hdl_hdr_ori = hdl_hdr;

	//show_these_recMsg_hdrs(hdl_hdr, total_hdr_num);	//debug

	ret = mapi_ansmach_recMsg_hdr_sort_by_readStatus(hdl_hdr, callee_num, &new_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	//list->hdr_num = total_hdr_num;
	list->hdr_num = callee_num;
	list->new_num = new_num;
	list->old_num = callee_num - new_num;
	ASM_DBG("%s(%d): list->hdr_num=%u (new_num=%u, old_num=%u)\n", __func__, __LINE__, (unsigned int) list->hdr_num, (unsigned int) list->new_num, (unsigned int) list->old_num);

	//show_these_recMsg_hdrs(hdl_hdr, total_hdr_num);	//debug

	ret = mapi_ansmach_recMsg_hdr_sort_by_time(&(hdl_hdr[0]), new_num);
	if(ret == MID_FAIL)
		return MID_FAIL;
	ret = mapi_ansmach_recMsg_hdr_sort_by_time(&(hdl_hdr[new_num]), callee_num-new_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	//show_these_recMsg_hdrs(hdl_hdr, total_hdr_num);	//debug

	/* process mapi_ansmach_phone_msg_hdr_t for phone */
	//phone_hdr = (mapi_ansmach_phone_msg_hdr_t *) malloc(sizeof(mapi_ansmach_phone_msg_hdr_t) * total_hdr_num);
	phone_hdr = (mapi_ansmach_phone_msg_hdr_t *) malloc(sizeof(mapi_ansmach_phone_msg_hdr_t) * callee_num);
	//memset(phone_hdr, 0, sizeof(mapi_ansmach_phone_msg_hdr_t) * total_hdr_num);
	if(phone_hdr == NULL){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		return MID_FAIL;
	}
	phone_hdr_ori = phone_hdr;

#if 1
	//for(i=0; i<total_hdr_num; i++){
	for(i=0; i<callee_num; i++){
		struct tm tm;
		time_t time_sec;

		memset(&tm, 0, sizeof(struct tm));
		//localtime_r((const time_t *) &hdl_hdr[i].time, &tm);
		time_sec = (U32) (hdl_hdr[i].time / 1000);	//sec = millisec / 1000
		//printf("time_sec = %ld\n", time_sec);
		localtime_r((const time_t *) &time_sec, &tm);
		phone_hdr[i].time.tm_sec = tm.tm_sec;
		phone_hdr[i].time.tm_min = tm.tm_min;
		phone_hdr[i].time.tm_hour = tm.tm_hour;
		phone_hdr[i].time.tm_mday = tm.tm_mday;
		phone_hdr[i].time.tm_mon = tm.tm_mon + 1;
		phone_hdr[i].time.tm_year = tm.tm_year + 1900;

		phone_hdr[i].id = hdl_hdr[i].time;
		strcpy(phone_hdr[i].caller, hdl_hdr[i].caller_num);
		strcpy(phone_hdr[i].callee, hdl_hdr[i].callee_num);
		phone_hdr[i].read = hdl_hdr[i].read;
		phone_hdr[i].filesize = hdl_hdr[i].size;
		phone_hdr[i].enable = 1;
	}

	list->hdr = phone_hdr;
#else
	//for(i=0; i<total_hdr_num; i++){
	for(i=0; i<callee_num; i++){
		struct tm tm;
		time_t time_sec;

		memset(&tm, 0, sizeof(struct tm));
		//localtime_r((const time_t *) &hdl_hdr->time, &tm);
		time_sec = (U32) (hdl_hdr[i].time / 1000);	//sec = millisec / 1000
		//printf("time_sec = %ld\n", time_sec);
		localtime_r((const time_t *) &time_sec, &tm);
		phone_hdr->time.tm_sec = tm.tm_sec;
		phone_hdr->time.tm_min = tm.tm_min;
		phone_hdr->time.tm_hour = tm.tm_hour;
		phone_hdr->time.tm_mday = tm.tm_mday;
		phone_hdr->time.tm_mon = tm.tm_mon + 1;
		phone_hdr->time.tm_year = tm.tm_year + 1900;

		phone_hdr->id = hdl_hdr->time;
		strcpy(phone_hdr->caller, hdl_hdr->caller_num);
		strcpy(phone_hdr->callee, hdl_hdr->callee_num);
		phone_hdr->read = hdl_hdr->read;
		phone_hdr->filesize = hdl_hdr->size;
		phone_hdr->enable = 1;

		phone_hdr++;
		hdl_hdr++;
	}

	list->hdr = phone_hdr_ori;
#endif

	if(hdl_hdr_ori)
		free(hdl_hdr_ori);

	return MID_SUCCESS;

notfound:

	list->hdr_num = 0;
	list->new_num = 0;
	list->old_num = 0;
	list->hdr = NULL;

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Free the malloc-memory in the mapi_ansmach_phone_msg_list_t structure
 *
 * Parameters
 *
 * Returns
 *
 ******************************************************************************/
void mapi_ansmach_phone_recMsg_list_put(mapi_ansmach_phone_msg_list_t *list)
{
	if(list->hdr)
		free(list->hdr);
}

/*******************************************************************************
 * Description
 *		Get All Recording Messages on Answering Machine
 *
 * Parameters
 *
 * Returns
 *		list: 			list infomation (numbers) & headers (list->hdr)
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or list is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_recMsg_list_get_all(mapi_ansmach_phone_msg_list_t *list)
{
	mid_hdl_rec_hdr_udata_t *hdl_hdr = NULL;
	int total_hdr_num = 0;
	int new_num = 0;
	int ret;
	int i;
	mapi_ansmach_phone_msg_hdr_t *phone_hdr = NULL;

	if(list == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/*
	 * To prevent that list structure (which may not be initialized by upper layer) still has
	 * unpredictable value after mapi return MID_FAIL (e.g. unknown list->hdr will cause upper layer
	 * segmentation fault while executing mapi_ansmach_phone_recMsg_list_put()), We must initialize
	 * list structure at first.
	 */
	memset(list, 0, sizeof(mapi_ansmach_phone_msg_list_t));

	ret = mapi_ansmach_recMsg_hdr_get_all(&hdl_hdr, &total_hdr_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	if(total_hdr_num == 0){
		ASM_DBG("%s(%d): No msgs existed.\n", __func__, __LINE__);
		goto notfound;
	}

	ret = mapi_ansmach_recMsg_hdr_sort_by_readStatus(hdl_hdr, total_hdr_num, &new_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	list->hdr_num = total_hdr_num;
	list->new_num = new_num;
	list->old_num = total_hdr_num - new_num;
	ASM_DBG("%s(%d): list->hdr_num=%u (new_num=%u, old_num=%u)\n", __func__, __LINE__, (unsigned int) list->hdr_num, (unsigned int) list->new_num, (unsigned int) list->old_num);

	ret = mapi_ansmach_recMsg_hdr_sort_by_time(&(hdl_hdr[0]), list->new_num);
	if(ret == MID_FAIL)
		return MID_FAIL;
	ret = mapi_ansmach_recMsg_hdr_sort_by_time(&(hdl_hdr[new_num]), list->old_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	/* process mapi_ansmach_phone_msg_hdr_t for phone */
	phone_hdr = (mapi_ansmach_phone_msg_hdr_t *) malloc(sizeof(mapi_ansmach_phone_msg_hdr_t) * total_hdr_num);
	//memset(phone_hdr, 0, sizeof(mapi_ansmach_phone_msg_hdr_t) * total_hdr_num);
	if(phone_hdr == NULL){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	for(i=0; i<total_hdr_num; i++){
		struct tm tm;
		time_t time_sec;

		memset(&tm, 0, sizeof(struct tm));
		time_sec = (U32) (hdl_hdr[i].time / 1000);	//sec = millisec / 1000
		//printf("time_sec = %ld\n", time_sec);
		localtime_r((const time_t *) &time_sec, &tm);
		phone_hdr[i].time.tm_sec = tm.tm_sec;
		phone_hdr[i].time.tm_min = tm.tm_min;
		phone_hdr[i].time.tm_hour = tm.tm_hour;
		phone_hdr[i].time.tm_mday = tm.tm_mday;
		phone_hdr[i].time.tm_mon = tm.tm_mon + 1;
		phone_hdr[i].time.tm_year = tm.tm_year + 1900;

		phone_hdr[i].id = hdl_hdr[i].time;
		strcpy(phone_hdr[i].caller, hdl_hdr[i].caller_num);
		strcpy(phone_hdr[i].callee, hdl_hdr[i].callee_num);
		phone_hdr[i].read = hdl_hdr[i].read;
		phone_hdr[i].filesize = hdl_hdr[i].size;
		phone_hdr[i].enable = 1;
	}

	list->hdr = phone_hdr;

	if(hdl_hdr)
		free(hdl_hdr);

	return MID_SUCCESS;

notfound:

	list->hdr_num = 0;
	list->new_num = 0;
	list->old_num = 0;
	list->hdr = NULL;

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Display All Recording Messages on the list for phone
 *
 * Parameters
 *		list->hdr:		headers of all Recording Messages belong to the same callee
 *		list->hdr_num:	numbers of all headers
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or list is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_recMsg_list_show(mapi_ansmach_phone_msg_list_t *list)
{
	mapi_ansmach_phone_msg_hdr_t *hdrp;
	int i;

	if((list == NULL) || (list->hdr == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	if(list->hdr_num <= 0){
		ASM_MSG("%s(%d): hdr_num=%u (<= 0)\n", __func__, __LINE__, (unsigned int) list->hdr_num);
		return MID_SUCCESS;
	}

	hdrp = list->hdr;

	ASM_MSG("---------------------------------------------------------------------\n");
	ASM_MSG("id\t\ttime\t\t\tcaller_num\tcallee_num\tread\tsize\t\t(list->hdr_num=%u)\n", (unsigned int) list->hdr_num);
	ASM_MSG("---------------------------------------------------------------------\n");
	for(i=0; i<(list->hdr_num); i++){
		ASM_MSG("%lld\t{%d/%d/%d/%d:%d:%d}\t%s\t%s\t\t%u\t%u\n",
			(unsigned long long) hdrp->id,
			hdrp->time.tm_year, hdrp->time.tm_mon, hdrp->time.tm_mday, hdrp->time.tm_hour, hdrp->time.tm_min, hdrp->time.tm_sec,
			hdrp->caller,
			hdrp->callee,
			(unsigned int) hdrp->read,
			(unsigned int) hdrp->filesize);

		hdrp++;
	}
	ASM_MSG("---------------------------------------------------------------------\n");

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Process transaction for Recording Message delete
 *
 * Parameters
 *		hdl_op:			handler operation structure for transaction
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdl_op is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_recMsg_delete(mid_hdl_op_t *hdl_op)
{
	void *op = NULL;
	int tid;

	if(hdl_op == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	if(!(op = malloc(sizeof(mid_hdl_op_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	/* fill op */
	memcpy((mid_hdl_op_t *) op, hdl_op, sizeof(mid_hdl_op_t));

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_REC_DEL, op, sizeof(mid_hdl_op_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);
	
	if (op != NULL)
		free(op);

	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Delete Recording Message for phone
 *
 * Parameters
 *		hdr->id:		id of the Recording Message
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdr is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_recMsg_delete(mapi_ansmach_phone_msg_hdr_t *hdr)
{
	mid_hdl_op_t hdl_op;

	if(hdr == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* fill hdl_op */
	hdl_op.id = hdr->id;

	/* do transaction */
	if(mapi_ansmach_recMsg_delete(&hdl_op) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_recMsg_delete() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Delete All Recording Messages belong to the same callee for phone
 *
 * Parameters
 *		list->hdr:		headers of all Recording Messages belong to the same callee
 *		list->hdr_num:	numbers of all headers
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or list is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_recMsg_delete_all(mapi_ansmach_phone_msg_list_t *list)
{
	mapi_ansmach_phone_msg_hdr_t *hdrp;
	int i;

	if((list == NULL) || (list->hdr == NULL) || (list->hdr_num == 0)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	hdrp = list->hdr;
	for(i=0; i<(list->hdr_num); i++){
		mapi_ansmach_phone_recMsg_delete(&hdrp[i]);
	}

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Process transaction for Recording Message mark read
 *
 * Parameters
 *		hdl_op:			handler operation structure for transaction
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdl_op is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_recMsg_mark_read(mid_hdl_op_t *hdl_op)
{
	void *op = NULL;
	int tid;

	if(hdl_op == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	if(!(op = malloc(sizeof(mid_hdl_op_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	/* fill op */
	memcpy((mid_hdl_op_t *) op, hdl_op, sizeof(mid_hdl_op_t));

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_REC_MREAD, op, sizeof(mid_hdl_op_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);
	
	if (op != NULL)
		free(op);

	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Mark Recording Message as readed for phone
 *
 * Parameters
 *		hdr->id:		id of the Recording Message
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdr is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_recMsg_mark_read(mapi_ansmach_phone_msg_hdr_t *hdr)
{
	mid_hdl_op_t hdl_op;

	if(hdr == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* fill hdl_op */
	hdl_op.id = hdr->id;

	/* do transaction */
	if(mapi_ansmach_recMsg_mark_read(&hdl_op) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_recMsg_mark_read() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Process transaction for Recording Message mark unread
 *
 * Parameters
 *		hdl_op:			handler operation structure for transaction
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdl_op is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_recMsg_mark_unread(mid_hdl_op_t *hdl_op)
{
	void *op = NULL;
	int tid;

	if(hdl_op == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	if(!(op = malloc(sizeof(mid_hdl_op_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	/* fill op */
	memcpy((mid_hdl_op_t *) op, hdl_op, sizeof(mid_hdl_op_t));

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_REC_MNREAD, op, sizeof(mid_hdl_op_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);
	
	if (op != NULL)
		free(op);

	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Mark Recording Message as unreaded for phone
 *
 * Parameters
 *		hdr->id:		id of the Recording Message
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdr is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_recMsg_mark_unread(mapi_ansmach_phone_msg_hdr_t *hdr)
{
	mid_hdl_op_t hdl_op;

	if(hdr == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* fill hdl_op */
	hdl_op.id = hdr->id;

	/* do transaction */
	if(mapi_ansmach_recMsg_mark_unread(&hdl_op) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_recMsg_mark_unread() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get partition information of Greeting/Recording Message area
 *
 * Parameters
 *
 * Returns
 *      info->grtmsg_used_cnt:	used count of Greeting Message
 *      info->grtmsg_used_size:	used size (byte) of Greeting Message area
 *      info->recmsg_used_cnt:	used count of Recording Message
 *      info->recmsg_used_size:	used size (byte) of Recording Message area
 *		MID_SUCCESS(0): 		successfully
 *		MID_FAIL(-1):   		failed to process transaction or info is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_msg_info_get(mid_hdl_layout_info_t *info)
{
	void *op = NULL;
	mid_hdl_layout_info_t *info_data;
	int tid;

	if(info == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	if(!(op = malloc(sizeof(mid_hdl_op_t) + sizeof(mid_hdl_layout_info_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	info_data = (mid_hdl_layout_info_t *) (((mid_hdl_op_t *) op)->data);

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_LINFO_GET, op, sizeof(mid_hdl_op_t) + sizeof(mid_hdl_layout_info_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);

	/* get info */
	memcpy(info, info_data, sizeof(mid_hdl_layout_info_t));
	
	if (op != NULL)
		free(op);

	ASM_DBG("Voice Partition Layout Info:\n");
	ASM_DBG("  Status: %lu\n", info->status);
	ASM_DBG("  GRT Used Count: %d\n", (int) info->grtmsg_used_cnt);
	ASM_DBG("  GRT Used Bytes: %lu\n", info->grtmsg_used_size);
	ASM_DBG("  REC Used Count: %d\n", (int) info->recmsg_used_cnt);
	ASM_DBG("  REC Used Bytes: %lu\n", info->recmsg_used_size);

	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Get remain size (byte) of Recording Message area
 *
 * Parameters
 *
 * Returns
 *      R_byte:			remain size (byte) of Recording Message area
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or R_byte is NULL
 *
 ******************************************************************************/
int mapi_ansmach_recMsg_remain_size_get(int *R_byte)
{
	mid_hdl_layout_info_t info_data;
	int ret;
	U32 remain_size;

	if(R_byte == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ret = mapi_ansmach_msg_info_get(&info_data);
	if(ret != MID_SUCCESS)
		return MID_FAIL;

	/* get info */
	remain_size = G711_SEC_TO_BYTE(MAPI_ASM_MAX_AMSG_DURATION_ALL) - info_data.recmsg_used_size;	//byte
	ASM_DBG("%s(%d): remain_size=%u byte (total_size=%u,  recmsg_used_size=%u)\n", __func__, __LINE__, (unsigned int) remain_size, G711_SEC_TO_BYTE(MAPI_ASM_MAX_AMSG_DURATION_ALL), (unsigned int) info_data.recmsg_used_size);

	*R_byte = remain_size;

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get used capacity (second) of Recording Message area
 *
 * Parameters
 *
 * Returns
 *      R_sec:		used capacity (second) of Recording Message area
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or R_sec is NULL
 *
 ******************************************************************************/
int mapi_ansmach_recMsg_used_capacity_get(float *R_sec)
{
	mid_hdl_layout_info_t info_data;
	int ret;

	if(R_sec == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ret = mapi_ansmach_msg_info_get(&info_data);
	if(ret != MID_SUCCESS)
		return MID_FAIL;

	/* get info */
	*R_sec = G711_BYTE_TO_SEC((float)info_data.recmsg_used_size);
	ASM_DBG("%s(%d): used_second=%f second (used_size=%u)\n", __func__, __LINE__, *R_sec, (unsigned int) info_data.recmsg_used_size);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get All Greeting Message headers
 *
 * Parameters
 *
 * Returns
 *		R_hdrs: 		headers of all Greeting Messages
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or num is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_grtMsg_hdr_get_all(mid_hdl_grt_hdr_udata_t **R_hdrs, int *num)
{
	mid_hdl_grt_hdr_udata_t *hdr, *hdr_r;
	mid_hdl_op_t *hdl_op;
	void *op = NULL;
	int tid;

	if(num == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	if (!(op = malloc(sizeof(mid_hdl_op_t) + sizeof(mid_hdl_grt_hdr_udata_t) * MID_HDL_ASM_GRT_HDR_MAX))) {
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	hdl_op = (mid_hdl_op_t *) op;
	hdr = (mid_hdl_grt_hdr_udata_t *) (hdl_op->data);

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_GRT_HDR_GET_INUSE, op, sizeof(mid_hdl_op_t) + sizeof(mid_hdl_grt_hdr_udata_t) * MID_HDL_ASM_GRT_HDR_MAX) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);

	if (!(hdr_r = malloc(sizeof(mid_hdl_grt_hdr_udata_t) * MID_HDL_ASM_GRT_HDR_MAX))) {
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}
	memcpy(hdr_r, hdr, sizeof(mid_hdl_grt_hdr_udata_t) * MID_HDL_ASM_GRT_HDR_MAX);

	*R_hdrs = hdr_r;
	*num = hdl_op->info.len;

	if(op != NULL)
		free(op);

	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Display All Headers of Greeting Messages
 *
 * Parameters
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction
 *
 ******************************************************************************/
int mapi_ansmach_grtMsg_show_hdrs(void)
{
	mid_hdl_grt_hdr_udata_t *hdrp = NULL;
	mid_hdl_grt_hdr_udata_t *hdrp_ori = NULL;
	int total_hdr_num = 0;
	int i;
	int ret;

	ret = mapi_ansmach_grtMsg_hdr_get_all(&hdrp, &total_hdr_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	if(hdrp == NULL){
		ASM_MSG("%s(%d): No grtMsg hdr exist\n", __func__, __LINE__);
		return MID_FAIL;
	}

	hdrp_ori = hdrp;

	ASM_MSG("---------------------------------------------------------------------\n");
	ASM_MSG("index\tsize						total_hdr_num=%d\n", total_hdr_num);
	ASM_MSG("---------------------------------------------------------------------\n");
	for(i=0; i<total_hdr_num; i++){
		ASM_MSG("%lld\t%u\n", (unsigned long long) hdrp->id, (unsigned int) hdrp->size);
		hdrp++;
	}
	ASM_MSG("---------------------------------------------------------------------\n");

	if(hdrp_ori != NULL)
		free(hdrp_ori);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get number of used Greeting Messages
 *
 * Parameters
 *
 * Returns
 *      R_num:			number of used Greeting Messages
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or R_num is NULL
 *
 ******************************************************************************/
int mapi_ansmach_grtMsg_used_num_get(int *R_num)
{
	mid_hdl_layout_info_t info_data;
	int ret;

	if(R_num == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ret = mapi_ansmach_msg_info_get(&info_data);
	if(ret != MID_SUCCESS)
		return MID_FAIL;

	/* get info */
	*R_num = info_data.grtmsg_used_cnt;
	ASM_DBG("%s(%d): used_num=%d\n", __func__, __LINE__, *R_num);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get information of Greeting Message for phone
 *
 * Parameters
 *		hdr->caller:	caller of the Greeting Message
 *      hdr->callee:	callee of the Greeting Message
 *
 * Returns
 *      hdr->id:		index of the Greeting Message
 *      hdr->afterSec:	time (second) for the Greeting Message to answer incoming call
 *      hdr->filesize:	file size of the Greeting Message
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdr (hdr->parameters) is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_grtMsg_info_get(mapi_ansmach_phone_msg_hdr_t *hdr)
{
	int id = -1;
	int afterSec = 0;
	int filesize = 0;

	if((hdr == NULL) || (hdr->caller == NULL) || (hdr->callee == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	mapi_ansmach_get_grtmsg_Idx_AftSec_Size(hdr->caller, hdr->callee, &id, &afterSec, &filesize);
	if(id == -1){	//not found
		ASM_MSG("%s(%d): index of grtmsg not found\n", __func__, __LINE__);
		return MID_FAIL;
	}

	if((afterSec < 0) || (filesize < 0)){
		ASM_MSG("%s(%d): Invalid return (afterSec=%d, filesize=%d)\n", __func__, __LINE__, afterSec, filesize);
		return MID_FAIL;
	}

	/* update grtmsg info */
	hdr->id = (U64) id;
	hdr->afterSec = (U32) afterSec;
	hdr->filesize = (U32) filesize;

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get empty index of Greeting Message
 * 		If there is no empty index, it will return -1 for none
 *
 * Parameters
 *
 * Returns
 *      R_id:			empty index of Greeting Message, or -1 for none
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or R_id is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_grtMsg_empty_index_get(int *R_id)
{
	mid_hdl_grt_hdr_udata_t *hdrp = NULL;
	mid_hdl_grt_hdr_udata_t *hdrp_ori = NULL;
	int total_hdr_num = 0;
	int i;
	int ret;
	int grt_used_array[MID_HDL_ASM_GRT_HDR_MAX] = {0};
	int empty_id = -1;

	if(R_id == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ret = mapi_ansmach_grtMsg_hdr_get_all(&hdrp, &total_hdr_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	if(hdrp == NULL){
		ASM_MSG("%s(%d): No grtMsg hdr exist\n", __func__, __LINE__);
		return MID_FAIL;
	}

	hdrp_ori = hdrp;

	for(i=0; i<total_hdr_num; i++){
		grt_used_array[hdrp->id] = 1;
		hdrp++;
	}

	for(i=0; i<MID_HDL_ASM_GRT_HDR_MAX; i++){
		if(grt_used_array[i] == 0){
			empty_id = i;
			break;	//find first empty index
		}
	}

	*R_id = empty_id;

	if(hdrp_ori != NULL)
		free(hdrp_ori);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get empty index of Greeting Message
 * 		If there is no empty index, it will return -1 for none
 *
 * Parameters
 *
 * Returns
 *      R_id:			empty index of Greeting Message, or -1 for none
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or R_id is NULL
 *
 ******************************************************************************/
int mapi_ansmach_grtMsg_get_empty_index(int *R_id)
{
	int id;
	int ret;

	if(R_id == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ret = mapi_ansmach_grtMsg_empty_index_get(&id);
	if(ret == MID_FAIL)
		return MID_FAIL;

	*R_id = id;

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Display index usage of Greeting Message (nouse:0, used:1)
 *
 * Parameters
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction
 *
 ******************************************************************************/
int mapi_ansmach_grtMsg_show_index_usage(void)
{
	mid_hdl_grt_hdr_udata_t *hdrp = NULL;
	mid_hdl_grt_hdr_udata_t *hdrp_ori = NULL;
	int total_hdr_num = 0;
	int i;
	int ret;
	int grt_used_array[MID_HDL_ASM_GRT_HDR_MAX] = {0};

	ret = mapi_ansmach_grtMsg_hdr_get_all(&hdrp, &total_hdr_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	if(hdrp == NULL){
		ASM_MSG("%s(%d): No grtMsg hdr exist\n", __func__, __LINE__);
		return MID_FAIL;
	}

	hdrp_ori = hdrp;

	for(i=0; i<total_hdr_num; i++){
		grt_used_array[hdrp->id] = 1;
		hdrp++;
	}

	ASM_MSG("Usage of Greeting Message index (nouse:0, used:1) : ");
	for(i=0; i<MID_HDL_ASM_GRT_HDR_MAX; i++)
		ASM_MSG("[%d]", grt_used_array[i]);
	ASM_MSG("\n");

	if(hdrp_ori != NULL)
		free(hdrp_ori);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Process transaction for Greeting Message save
 *
 * Parameters
 *		hdl_op:			handler operation structure for transaction
 *      hdl_hdr:		header of Greeting Message being saved
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdl_op/hdl_hdr is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_grtMsg_save(mid_hdl_op_t *hdl_op, mid_hdl_grt_hdr_udata_t *hdl_hdr)
{
	void *op = NULL;
	int tid;

	if((hdl_op == NULL) || (hdl_hdr == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	if(!(op = malloc(sizeof(mid_hdl_op_t) + sizeof(mid_hdl_grt_hdr_udata_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	/* fill op */
	memcpy((mid_hdl_op_t *) op, hdl_op, sizeof(mid_hdl_op_t));
	memcpy(((mid_hdl_op_t *) op)->data, hdl_hdr, sizeof(mid_hdl_grt_hdr_udata_t));

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_GRT_SAVE, op, sizeof(mid_hdl_op_t) + sizeof(mid_hdl_grt_hdr_udata_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);

	if (op != NULL)
		free(op);
		
	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Save Greeting Message for phone
 *
 * Parameters
 * 		hdr->id:		must filled if you want to save grtmsg to specified id,
 * 						otherwise fill MAPI_ASM_GRTMSG_FIND_EMPTY_ID instead.
 *      hdr->filepath:	full path of the Greeting Message G.711 file
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdr (hdr->filepath) is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_grtMsg_save(mapi_ansmach_phone_msg_hdr_t *hdr)
{
	mid_hdl_grt_hdr_udata_t hdl_hdr;
	mid_hdl_op_t hdl_op;
	int file_size = 0;
	struct stat st;
	int ret;
	int empty_id;
	mapi_ansmach_phone_msg_hdr_t hdr_tmp;
	char *ch;
	char md5[2][MAPI_ASM_MAX_FILEPATH+40];
	FILE *fp;
	char cmd[128];
	char line[200];
	int i;

	if((hdr == NULL) || (hdr->filepath == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	if (access(hdr->filepath, F_OK) != 0) {
		ASM_MSG("Greeting Message does not exist!\n");
		return MID_FAIL;
	}

	if(stat(hdr->filepath, &st) == 0){
		file_size  = st.st_size;
		//printf("%s: %d (bytes)\n", filepath, file_size);
	}

	/* fill hdl_hdr */
	hdl_hdr.size = file_size;

	if(hdr->id == MAPI_ASM_GRTMSG_FIND_EMPTY_ID){
		ret = mapi_ansmach_grtMsg_empty_index_get(&empty_id);
		if(ret == MID_FAIL)
			return MID_FAIL;

		if(empty_id == -1){	//not found empty id
			ASM_MSG("%s(%d): No empty index for Greeting Message\n", __func__, __LINE__);
			return MID_FAIL;
		}

		hdl_hdr.id = (U64) empty_id;
	}
	else{
		hdl_hdr.id = hdr->id;
	}

	/* Extra check for default greeting message (if the saving file is the same file on flash, skip save operation) */
	if(hdl_hdr.id == 0){	//default greeting message id
		hdr_tmp.id = 0;
		sprintf(hdr_tmp.filepath, "%s", "/tmp/default_grtmsg_check.711");

		ret = mapi_ansmach_phone_grtMsg_load(&hdr_tmp);
		if(ret == MID_SUCCESS){
			sprintf(cmd, "md5sum %s %s", hdr_tmp.filepath, hdr->filepath);
			//printf("cmd = %s\n", cmd);

			fp = popen(cmd, "r");
			if(fp != NULL){
				i = 0;
				while(fgets(line, 200, fp) != NULL){
					//printf("%s", line);
					sprintf(md5[i], "%s", line);
					ch = strstr(md5[i], " ");
					*ch = '\0';

					i++;
				}
				pclose(fp);

				ASM_MSG("%s(%d): md5_ori=%s (%s)\n", __func__, __LINE__, md5[0], hdr_tmp.filepath);
				ASM_MSG("%s(%d): md5_new=%s (%s)\n", __func__, __LINE__, md5[1], hdr->filepath);

				if(strcmp(md5[0], md5[1]) == 0){
					ASM_MSG("%s(%d): md5 of grtmsg is the same... skip save operation\n", __func__, __LINE__);
					goto skip;
				}

				ASM_MSG("%s(%d): md5 of grtmsg is not the same... continue save operation\n", __func__, __LINE__);
			}
			else
				ASM_MSG("%s(%d): popen() for md5 check failed ... continue to save grtmsg\n", __func__, __LINE__);
		}
		else
			ASM_MSG("%s(%d): default grtmsg load failed ... continue to save grtmsg\n", __func__, __LINE__);
	}

	/* fill hdl_op */
	strcpy(hdl_op.info.filepath, hdr->filepath);
	hdl_op.id = hdl_hdr.id;

	/* do transaction */
	if(mapi_ansmach_grtMsg_save(&hdl_op, &hdl_hdr) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_grtMsg_save() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

skip:
	/* after saving to flash, delete tmp file */
	ASM_EXEC("rm %s", hdr->filepath);

	if(hdl_hdr.id == 0)
		ASM_EXEC("rm %s", hdr_tmp.filepath);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Process transaction for Greeting Message load
 *
 * Parameters
 *		hdl_op:			handler operation structure for transaction
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdl_op is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_grtMsg_load(mid_hdl_op_t *hdl_op)
{
	void *op = NULL;
	int tid;

	if(hdl_op == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	if(!(op = malloc(sizeof(mid_hdl_op_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	/* fill op */
	memcpy((mid_hdl_op_t *) op, hdl_op, sizeof(mid_hdl_op_t));

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_GRT_LOAD, op, sizeof(mid_hdl_op_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);

	/* update filepath */
	strcpy(hdl_op->info.filepath, ((mid_hdl_op_t *) op)->info.filepath);

	if (op != NULL)
		free(op);
		
	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Load Greeting Message for phone
 *
 * Parameters
 *		hdr->id:		index of the Greeting Message
 *      hdr->filepath:	full path of the Greeting Message G.711 file
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdr is NULL
 *
 ******************************************************************************/
int mapi_ansmach_phone_grtMsg_load(mapi_ansmach_phone_msg_hdr_t *hdr)
{
	mid_hdl_op_t hdl_op;

	if(hdr == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* fill hdl_op */
	hdl_op.id = hdr->id;
	strcpy(hdl_op.info.filepath, hdr->filepath);

	/* do transaction */
	if(mapi_ansmach_grtMsg_load(&hdl_op) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_grtMsg_load() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Load Greeting Message (G.711 format) and convert it to Wav format
 *
 * Parameters
 *		id:				index of the Greeting Message
 *      filepath:		full path of the Greeting Message Wav file
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or G7112wav
 *
 ******************************************************************************/
int mapi_ansmach_grtMsg_load_wav(int id, char *filepath)
{
	mid_hdl_op_t hdl_op;
	char wavfile[MAPI_ASM_MAX_FILEPATH] = "";
	struct timeb tb_start, tb_end;
	unsigned long long duration_millisec;
	int ret;

	if(filepath == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ftime(&tb_start);

	ASM_EXEC("mkdir -p /tmp/www");

	/* load G.711 file (the updated filepath will be hdl_op.info.filepath) */
	hdl_op.id = (U64) id;
	//strcpy(hdl_op.info.filepath, filepath);
	sprintf(hdl_op.info.filepath, "/tmp/www/grt_%d.711", id);	//P.S. Wav file in /tmp/www can be accessed by http request
	if(mapi_ansmach_grtMsg_load(&hdl_op) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_grtMsg_load() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	sprintf(wavfile, "/tmp/www/grt_%d.wav", id);
	ret = G7112wav(hdl_op.info.filepath, wavfile);
	if(ret < 0){
		ASM_MSG("%s(%d): G7112wav() failed...\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* after converting to Wav, delete 711 file */
	ASM_EXEC("rm %s", hdl_op.info.filepath);

	/* Convert a-law Wav to pcm Wav */
	ASM_EXEC("ffmpeg -i %s -acodec pcm_s16le -y %s", wavfile, filepath);

	/* after converting to pcm Wav, delete a-law Wav */
	ASM_EXEC("rm %s", wavfile);

	ftime(&tb_end);
	duration_millisec = (tb_end.time * 1000 + tb_end.millitm) - (tb_start.time * 1000 + tb_start.millitm);
	ASM_DBG("%s(%d): [Time] Convert : %f sec \n", __func__, __LINE__, (double) duration_millisec / 1000);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Load Greeting Message (G.711 format) and convert it to Mp3 format
 *
 * Parameters
 *		id:				id of the Greeting Message
 *      filepath:		full path of the Greeting Message Mp3 file
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or G7112wav
 *
 ******************************************************************************/
int mapi_ansmach_grtMsg_load_mp3(int id, char *filepath)
{
	mid_hdl_op_t hdl_op;
	char wavfile[MAPI_ASM_MAX_FILEPATH] = "";
	char pcmwav[MAPI_ASM_MAX_FILEPATH] = "";
	struct timeb tb_start, tb_end;
	unsigned long long duration_millisec;
	int ret;

	if(filepath == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ftime(&tb_start);

	ASM_EXEC("mkdir -p /tmp/www");

	/* load G.711 file (the updated filepath will be hdl_op.info.filepath) */
	hdl_op.id = (U64) id;
	//strcpy(hdl_op.info.filepath, filepath);
	sprintf(hdl_op.info.filepath, "/tmp/www/grt_%d_.711", id);	//P.S. Wav file in /tmp/www can be accessed by http request
	if(mapi_ansmach_grtMsg_load(&hdl_op) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_recMsg_load() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	sprintf(wavfile, "/tmp/www/grt_%d_.wav", id);
	ret = G7112wav(hdl_op.info.filepath, wavfile);
	if(ret == MID_FAIL){
		ASM_MSG("%s(%d): G7112wav() failed...\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* after converting to Wav, delete 711 file */
	ASM_EXEC("rm %s", hdl_op.info.filepath);

#if 0
	sprintf(pcmwav, "/tmp/www/grt_%d_pcm.wav", id);
	/* Convert a-law Wav to pcm Wav */
	ASM_EXEC("ffmpeg -i %s -acodec pcm_s16le %s", wavfile, pcmwav);

	/* after converting to pcm Wav, delete a-law Wav */
	ASM_EXEC("rm %s", wavfile);

	/* Convert pcm Wav to Mp3 */
	ASM_EXEC("lame -V2 -f %s %s", pcmwav, filepath);

	/* after converting to Mp3, delete pcm Wav */
	ASM_EXEC("rm %s", pcmwav);
#else
	/* Convert a-law Wav to Mp3 */
	ASM_EXEC("lame -V2 -f %s %s", wavfile, filepath);

	/* after converting to Mp3, delete a-law Wav */
	ASM_EXEC("rm %s", wavfile);
#endif

	ftime(&tb_end);
	duration_millisec = (tb_end.time * 1000 + tb_end.millitm) - (tb_start.time * 1000 + tb_start.millitm);
	ASM_DBG("%s(%d): [Time] Convert : %f sec \n", __func__, __LINE__, (double) duration_millisec / 1000);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Process transaction for Greeting Message delete
 *
 * Parameters
 *		hdl_op:			handler operation structure for transaction
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or hdl_op is NULL
 *
 ******************************************************************************/
static int mapi_ansmach_grtMsg_delete(mid_hdl_op_t *hdl_op)
{
	void *op = NULL;
	int tid;

	if(hdl_op == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		goto err;
	}

	if(!(op = malloc(sizeof(mid_hdl_op_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	/* fill op */
	memcpy((mid_hdl_op_t *) op, hdl_op, sizeof(mid_hdl_op_t));

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_GRT_DEL, op, sizeof(mid_hdl_op_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);
	
	if (op != NULL)
		free(op);

	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 *		Delete Greeting Message
 *
 * Parameters
 *		id:				index of the Greeting Message
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction
 *
 ******************************************************************************/
int mapi_ansmach_ui_grtMsg_delete(int id)	//test
{
	mid_hdl_op_t hdl_op;

	if((id < 0) || (id > (MID_HDL_ASM_GRT_HDR_MAX-1))){
		ASM_MSG("%s(%d): Invalid input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	/* fill hdl_op */
	hdl_op.id = (U64) id;

	/* do transaction */
	if(mapi_ansmach_grtMsg_delete(&hdl_op) != MID_SUCCESS){
		ASM_MSG("%s(%d): call mapi_ansmach_grtMsg_delete() failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get specified account information (this function does not approach displayName)
 *
 * Parameters
 * 		account->userId:		phone number of this account (callee)
 *
 * Returns
 * 		account->totalMsgNum:	total message number of this account
 * 		account->unreadMsgNum:	total unreaded message number of this account
 *		MID_SUCCESS(0): 		successfully
 *		MID_FAIL(-1):   		failed to process transaction or account/account->userId is NULL
 *
 ******************************************************************************/
int mapi_ansmach_account_info_get(mapi_ansmach_account_info_t *account)
{
	mid_hdl_rec_hdr_udata_t *hdl_hdr = NULL;
	mid_hdl_rec_hdr_udata_t *hdrp;
	int total_hdr_num = 0;
	int ret;
	int i;

	if((account == NULL) || (account->userId_local == NULL)){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	ret = mapi_ansmach_recMsg_hdr_get_all(&hdl_hdr, &total_hdr_num);
	if(ret == MID_FAIL)
		return MID_FAIL;

	if(total_hdr_num == 0){
		ASM_DBG("%s(%d): No msgs existed\n", __func__, __LINE__);
		goto notfound;
	}

	hdrp = hdl_hdr;

	account->totalMsgNum = 0;
	account->unreadMsgNum = 0;

	for(i=0; i<total_hdr_num; i++){
		if(strcmp(hdrp->callee_num, account->userId_local) == 0){
			account->totalMsgNum++;
			if(hdrp->read == 0)
				account->unreadMsgNum++;
		}
		hdrp++;
	}

	if(hdl_hdr)
		free(hdl_hdr);

	if(account->totalMsgNum == 0)
		ASM_DBG("%s(%d): No msgs existed of Account %s\n", __func__, __LINE__, account->userId_local);

	return MID_SUCCESS;

notfound:

	account->totalMsgNum = 0;
	account->unreadMsgNum = 0;
	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Get all account information (only return Enabled account)
 *
 * Parameters
 *
 * Returns
 *		list: 			list length (list->len) & accounts info (list->account)
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction or list is NULL
 *
 ******************************************************************************/
int mapi_ansmach_account_info_get_all(mapi_ansmach_account_info_list_t *list)
{
	mapi_ansmach_account_info_t account[MAPI_CCFG_VOIP_MAX_SIP_ACCOUNT_NUM+MAPI_CCFG_VOIP_ISDN_MAX_NUMBER];	//MAPI_CCFG_VOIP_MAX_SIP_ACCOUNT_NUM = 10
	mapi_ccfg_voip_sip_line_t voip_sip_account;
	char sect_name[]="isdn", para_name[24], num[40];
	int i;
	int tid;
	int ret;

	if(list == NULL){
		ASM_MSG("%s(%d): NULL input\n", __func__, __LINE__);
		return MID_FAIL;
	}

	list->len = 0;

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		return MID_FAIL;
	}

	//Reset before using, avoid unexpect charater showing. Bryce ++
	memset(account, 0x0, sizeof(account));

	for(i = 0; i < MAPI_CCFG_VOIP_MAX_SIP_ACCOUNT_NUM; i++){
        memset((char *)&voip_sip_account, 0, sizeof(mapi_ccfg_voip_sip_line_t));

        voip_sip_account.line_idx = i;

        if(mapi_ccfg_get_voip_sip(tid, &voip_sip_account, 0) == MID_FAIL){
            ASM_MSG("%s(%d): mapi_ccfg_get_voip_sip fail\n", __func__, __LINE__);
            mapi_end_transc(tid);
            return MID_FAIL;
        }

        //nbytes += so_printf(r, "voip_sip_enable[%d] = %d;\n", i, voip_sip_account.enable);
        //printf("[%d] voip_sip_account.enable = %d\n", i, voip_sip_account.enable);
        if(voip_sip_account.enable){
			if(strcmp(voip_sip_account.user_id, "") == 0){
				//continue;
				strcpy(account[list->len].userId, "");
				strcpy(account[list->len].displayName, "");
				strcpy(account[list->len].userId_area, "");
				strcpy(account[list->len].userId_local, "");
			}
			else{
				strcpy(account[list->len].userId, voip_sip_account.user_id);
				strcpy(account[list->len].displayName, voip_sip_account.display_name);
				strcpy(account[list->len].userId_area, voip_sip_account.userid_area);
				strcpy(account[list->len].userId_local, voip_sip_account.userid_local);

				ret = mapi_ansmach_account_info_get(&account[list->len]);
				//if(ret != MID_FAIL)
				//	printf("Account: userId=%s, totalMsgNum=%u, unreadMsgNum=%u\n", account[list->len].userId, (unsigned int) account[list->len].totalMsgNum, (unsigned int) account[list->len].unreadMsgNum);
			}

			list->len++;
		}
    }

    for(i = 0; i < MAPI_CCFG_VOIP_ISDN_MAX_NUMBER; i++){
        sprintf(para_name, "msn_number_%d", i);
        mapi_ccfg_get_str(tid, sect_name, para_name, num, MAPI_CCFG_VOIP_MSN_MAX_LEN, NULL);
        if(num[0]) {
			strcpy(account[list->len].userId, num);
			strcpy(account[list->len].userId_area, "");
			strcpy(account[list->len].userId_local, num);
			ret = mapi_ansmach_account_info_get(&account[list->len]);
			list->len++;
		}
	}

    mapi_end_transc(tid);

    if (!(list->account = malloc(sizeof(mapi_ansmach_account_info_t) * list->len))) {
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		return MID_FAIL;
	}

	memcpy(list->account, account, sizeof(mapi_ansmach_account_info_t) * list->len);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Free the malloc-memory in the mapi_ansmach_account_info_list_t structure
 *
 * Parameters
 *
 * Returns
 *
 ******************************************************************************/
void mapi_ansmach_account_info_list_put(mapi_ansmach_account_info_list_t *list)
{
	if(list->account)
		free(list->account);
}

/*******************************************************************************
 * Description
 *		Display all account information (only return Enabled account)
 *
 * Parameters
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction
 *
 ******************************************************************************/
int mapi_ansmach_account_info_show(void)
{
	mapi_ansmach_account_info_list_t list;
	mapi_ansmach_account_info_t *account;
	int i;
	int ret;

	ret = mapi_ansmach_account_info_get_all(&list);
	if(ret == MID_FAIL)
		return MID_FAIL;

	if(list.len == 0){
		ASM_MSG("%s(%d): No account exist\n", __func__, __LINE__);
		return MID_FAIL;
	}

	account = list.account;

	ASM_MSG("---------------------------------------------------------------------\n");
	ASM_MSG("userId(callee)\tdisplayName\ttotalMsgNum\tunreadMsgNum			list.len=%u\n", (unsigned int) list.len);
	ASM_MSG("---------------------------------------------------------------------\n");
	for(i=0; i<(list.len); i++){
		ASM_MSG("%s\t%s\t\t%u\t%u\n", account->userId, account->displayName, (unsigned int) account->totalMsgNum, (unsigned int) account->unreadMsgNum);
		account++;
	}
	ASM_MSG("---------------------------------------------------------------------\n");

	mapi_ansmach_account_info_list_put(&list);

	return MID_SUCCESS;
}

/*******************************************************************************
 * Description
 *		Check if the specific Recording Message is existed
 *
 * Parameters
 *		id:				index of the Recording Message
 *
 * Returns
 *		MID_SUCCESS(0): the specific Recording Message is existed
 *		MID_FAIL(-1):   failed to process transaction or the specific Recording Message is not existed
 *
 ******************************************************************************/
int mapi_ansmach_recMsg_check_exist(unsigned long long id)
{
	mid_hdl_op_t *hdl_op;
	void *op = NULL;
	int tid;

	if(!(op = malloc(sizeof(mid_hdl_op_t) + sizeof(mid_hdl_rec_hdr_udata_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	/* fill op */
	hdl_op = (mid_hdl_op_t *) op;
	hdl_op->id = id;
#if 1
	hdl_op->info.len = 0;	//if the recMsg is existed, this value will be set to "1" by hdl after transc()
#endif

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_REC_HDR_GET, op, sizeof(mid_hdl_op_t) + sizeof(mid_hdl_rec_hdr_udata_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);

#if 1
	if(hdl_op->info.len == 0)
		goto err;
#endif

	if (op != NULL)
		free(op);
	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 * 		Delete All Recording Messages on Answering Machine
 *
 * Parameters
 *
 * Returns
 *		MID_SUCCESS(0): successfully
 *		MID_FAIL(-1):   failed to process transaction
 *
 ******************************************************************************/
int mapi_ansmach_recMsg_delete_all(void)
{
	void *op = NULL;
	int tid;

	if(!(op = malloc(sizeof(mid_hdl_op_t)))){
		ASM_MSG("%s(%d): Malloc Failed\n", __func__, __LINE__);
		goto err;
	}

	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		goto err;
	}

	if(mapi_trx_transc(tid, MID_MSG_MOD_ASM, MID_HDL_ASM_REC_DEL_ALL, op, sizeof(mid_hdl_op_t)) != MID_SUCCESS){
		ASM_MSG("%s(%d): mapi_trx_transc() failed\n", __func__, __LINE__);
		mapi_end_transc(tid);
		goto err;
	}

	mapi_end_transc(tid);

	if (op != NULL)
		free(op);
		
	return MID_SUCCESS;

err:
	if (op != NULL)
		free(op);
	return MID_FAIL;
}

/*******************************************************************************
 * Description
 * 		Get enable status of Answering Machine
 *
 * Parameters
 *
 * Returns
 *		enable:			enable status of Answering Machine
 *		MID_SUCCESS(0):	successfully
 *		MID_FAIL(-1):	failed to process transaction
 *
 ******************************************************************************/
int mapi_ansmach_enable_status_get(int *enable)
{
	int tid;

	*enable = 0;
	
	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		return MID_FAIL;
	}

	*enable = mapi_ccfg_get_long( tid, MAPI_ASM_ANSMACH_SECTION_NAME, "enable", 0);

	mapi_end_transc(tid);
	
	return MID_SUCCESS;
}

int mapi_ansmach_debug_status_get(int *enable)
{
	int tid;

	*enable = 0;
	
	/* start transaction */
	if((tid = mapi_start_transc()) == MID_FAIL){
		ASM_MSG("%s(%d): Failed to start a transaction to arc middle\n", __func__, __LINE__);
		return MID_FAIL;
	}

	*enable = mapi_ccfg_get_long( tid, MAPI_ASM_ANSMACH_SECTION_NAME, "debug", 0);

	mapi_end_transc(tid);
	
	return MID_SUCCESS;
}


/*******************************************************************************
Description
	Convert phone number to International format
	For those prefixed with +xxx, convert it to 00xxx
	
Parameters
	char *dstPhoneNum: Pointer to the buffer to store the converted phone number
	char *srcPhoneNum: Pointer to the buffer to indicate the original phone number

Returns
	MID_SUCCESS(0): Success
	MID_FAIL(-1):   Fail

******************************************************************************/
int mapi_ansmach_phonenum_convert(char *dstPhoneNum, char *srcPhoneNum)
{
	if(dstPhoneNum==NULL||srcPhoneNum==NULL)
		return MID_FAIL;
	
#ifdef CONFIG_ARC_CUSTOMER_VODAFONE
	if(strncmp(srcPhoneNum, "+49", 3)==0){
		sprintf(dstPhoneNum, "0%s", &srcPhoneNum[3]);
		return MID_SUCCESS;
	}
	else if(strncmp(srcPhoneNum, "0049", 4)==0){
		sprintf(dstPhoneNum, "0%s", &srcPhoneNum[4]);
		return MID_SUCCESS;
	}
#endif
	if(strncmp(srcPhoneNum, "+", 1)==0){
		sprintf(dstPhoneNum, "00%s", &srcPhoneNum[1]);
		return MID_SUCCESS;
	}
	else{
		sprintf(dstPhoneNum, "%s", srcPhoneNum);
		return MID_SUCCESS;
	}

}


int mapi_voip_get_call_cnt()
{
	int tid = 0, new_transc = 0;
	int cnt = 0;

	if (tid == 0)
	{
		new_transc = 1;

		if ((tid = mapi_start_transc()) == MID_FAIL)
		{
			return MID_FAIL;
		}
	}

	cnt = (int)(mapi_ccfg_get_long(tid, "voip", "call_cnt", 0));


	if (new_transc)
	{
		if (mapi_end_transc(tid) == MID_FAIL)
		{
			return MID_FAIL;
		}
	}

	return cnt;
}


int mapi_voip_set_call_cnt(int cnt)
{
	int tid = 0, new_transc = 0;
	int ret = 0;

	if (tid == 0)
	{
		new_transc = 1;

		if ((tid = mapi_start_transc()) == MID_FAIL)
		{
			return MID_FAIL;
		}
	}

	ret = (int)(mapi_ccfg_set_long(tid, "voip", "call_cnt", cnt)); 

	if (new_transc)
	{
		if (mapi_end_transc(tid) == MID_FAIL)
		{
			return MID_FAIL;
		}
	}

	return ret;
}

