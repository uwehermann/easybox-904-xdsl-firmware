/*
 * Arcadyan middle layer APIs for WAN interface
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libArcComApi.h>
#include <mid_fmwk.h>
#include "mid_mapi_lte.h"
#include "mid_ccfg.h"
#include "mid_mapi_ccfg.h"
#include "mid_mapi_mon.h"

/*
 * -- Helper functions --
 */

/*
 * -- Public functions --
 */


/*******************************************************************************
 * Description
 *		Get LTE status
 *
 * Parameters
 *		pstBuf:		pointer to the buffer of data structure stUpnpStatus.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_lte_status_get (int tid, stLteStatus *status)
{
    int         rst=MID_SUCCESS, new_transc=0;
	char IMSI[16] = "", PIN[5]="";
	char antenna[16];
	
	if (status == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( status, 0, sizeof(*status) );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_STATUS_GET, status, sizeof(*status) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

    mapi_ccfg_get_str2(tid, "lte", "IMSI", IMSI, sizeof(IMSI));

	if (access("/tmp/reset_in_progress", 0) != 0) /* DO NOT check IMSI if system is being reset to default */
	{
	if (IMSI[0]==0) 
	{   /*Check PIN first to avoid IMSI store again during restoring default setting*/
	    mapi_ccfg_get_str2(tid, "lte", "pin", PIN, sizeof(PIN));
		if (PIN[0]!= 0)
		{/*wait until runtime IMSI is available*/
			if (strcmp(status->SIM_IMSI, "") != 0) 
			{/* save current IMSI if no previous stored*/   
				mapi_ccfg_set_str(tid, "lte", "IMSI", status->SIM_IMSI);
				mapi_ccfg_commit(tid, MID_CCFG_COMMIT_FILE|MID_CCFG_COMMIT_FLASH, NULL );
			}
		}	
		else
		{  /*If no stored PIN but in PIN OK mode, it means this card is PIN free */
			if ((status->LTE_pin_mode == 3))
			{	/*wait until runtime IMSI is available*/
				if (strcmp(status->SIM_IMSI, "") != 0) 
				{/* save current IMSI if no previous stored*/   
					mapi_ccfg_set_str(tid, "lte", "IMSI", status->SIM_IMSI);
					mapi_ccfg_commit(tid, MID_CCFG_COMMIT_FILE|MID_CCFG_COMMIT_FLASH, NULL );
				}
			}			
		}	
	}
	else 
	{ /*wait until runtime IMSI is available*/
		if (strcmp(status->SIM_IMSI, "") != 0)
		{/*compare with the one stored*/
			if (strcmp(IMSI, status->SIM_IMSI) !=0)
			{ /*the card has been changed*/
				status->LTE_sim_card_changed = 1;
			}
			else 
				status->LTE_sim_card_changed = 0;
		}
		else
		{  /*If there's previous IMSI but now in PIN mode, it means card changes*/
			if ((status->LTE_pin_mode == 0)||(status->LTE_pin_mode == 1))
				status->LTE_sim_card_changed = 1;
		}	 
	} 
	}
	
	if ( new_transc == 1 )
		mapi_end_transc( tid );

	if(access("/proc/arcusb/status.lte", F_OK) != -1){
		osSystem_GetOutput("cat /proc/arcusb/status.lte |grep antenna_state|cut -d ' ' -f 2",antenna,sizeof(antenna));
		if(!strncmp(antenna,"internal",8))
			status->antenna_type=MID_ANTENNA_TYPE_INTERNAL;
		else if(!strncmp(antenna,"external",8))
			status->antenna_type=MID_ANTENNA_TYPE_EXTERNAL;
	}
	
    return rst;
}

/*******************************************************************************
 * Description
 *		Get LTE status From /proc/arcusb/status.lte
 *
 * Parameters
 *		pstBuf:		pointer to the buffer of data structure stUpnpStatus.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_status_lte_get (int tid, stStatusLte *status)
{
    int  rst=MID_SUCCESS, new_transc=0;
	
	if (status == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( status, 0, sizeof(*status) );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_STATUS_LTE_GET, status, sizeof(*status) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

    return rst;
	
	// need to add later....
}

/*******************************************************************************
 * Description
 *		Get UMTS status
 *
 * Parameters
 *		pstBuf:		pointer to the buffer of data structure stUpnpStatus.
 *
 * Returns
 *		* MID_SUCCESS(0):	successfully
 *		* MID_FAIL(-1):		failed to process transaction or pstBuf is NULL.
 *
 * Note
 *		* 
 *
 * See Also
 *
 ******************************************************************************/
int mapi_lte_umts_status_get (int tid, stUmtsStatus *status)
{
    int         rst=MID_SUCCESS, new_transc=0;
	
	if (status == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
				return MID_FAIL;
	}

	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( status, 0, sizeof(*status) );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_UMTS_STATUS_GET, status, sizeof(*status) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *              Set LTE PIN
 *
 * Parameters
 *              pin:   current pin of LTE
 *				newPinCode: new pin for LTE
 *
 * Returns
 *              * MID_SUCCESS(0):       successfully
 *              * MID_FAIL(-1):         failed to set pin.
 *
 * Note
 *              if newPinCode is NULL, it means that the pin is for SIM pin.
 *              if newPinCode has a value, it means that the pin is for PUK and the newPinCode 
 *				is for new SIM pin.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_lte_Give_LTE_SIM_PIN(int tid, char *pin, char *newPinCode)
{
    stSetSimPin sim_pins;
    int         rst=MID_SUCCESS, new_transc=0;
	stLteStatus status;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}

    memset((char *)&sim_pins, 0, sizeof(sim_pins));
    if(pin!=NULL)
        strncpy(sim_pins.sim_pin1, pin, 15);
    if(newPinCode!=NULL)
        strncpy(sim_pins.sim_pin2, newPinCode, 15);

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_SIM_PIN_GIVE, &sim_pins, sizeof(sim_pins) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if (rst == MID_SUCCESS)
	{
		do {
			usleep(500000);
			mapi_lte_status_get (tid, &status);
		} while (status.pinCodeVerify == MAPI_LTE_SIM_PINCODE_UNKNOWN);
		
		//if (status.LTE_pin_state)
		if (status.pinCodeVerify == MAPI_LTE_SIM_PINCODE_CORRECT)
		{
			rst = MID_SUCCESS;		
		}
		else
			rst = MID_FAIL; 
	}
	
	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}


/*******************************************************************************
 * Description
 *              Set UMTS PIN
 *
 * Parameters
 *              pin:   current pin of UMTS
 *				newPinCode: new pin for UMTS
 *
 * Returns
 *              * MID_SUCCESS(0):       successfully
 *              * MID_FAIL(-1):         failed to set pin.
 *
 * Note
 *              if newPinCode is NULL, it means that the pin is for SIM pin.
 *              if newPinCode has a value, it means that the pin is for PUK and the newPinCode 
 *				is for new SIM pin.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_lte_Give_UMTS_SIM_PIN(int tid, char *pin, char *newPinCode)
{
    stSetSimPin sim_pins;
    int         rst=MID_SUCCESS, new_transc=0;
    int			pin_enb;
	stUmtsStatus status;

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}
    
    memset((char *)&sim_pins, 0, sizeof(sim_pins));
    if(pin!=NULL)
        strncpy(sim_pins.sim_pin1, pin, 15);
    if(newPinCode!=NULL)
        strncpy(sim_pins.sim_pin2, newPinCode, 15);

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_UMTS_SIM_PIN_GIVE, &sim_pins, sizeof(sim_pins) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}
	
	if (rst == MID_SUCCESS)
	{
		do {
			usleep(500000);
			mapi_lte_umts_status_get (tid, &status);
		} while (status.pinCodeVerify == MAPI_LTE_SIM_PINCODE_UNKNOWN);
		
		//if (status.UMTS_pin_state == 0)
		if (status.pinCodeVerify == MAPI_LTE_SIM_PINCODE_CORRECT)
		{
			rst = MID_SUCCESS;

			//send the request to keep the PIN in USB driver
			pin_enb = 1;
			if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_UMTS_STORE_PIN_SET, &pin_enb, sizeof(pin_enb) ) != MID_SUCCESS) {
				MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
			}
		}
		else
			rst = MID_FAIL; 
	}
		
	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

/*******************************************************************************
 * Description
 *              Change UMTS PIN
 *
 * Parameters
 *              pin:   current pin of UMTS
 *				newPinCode: new pin for UMTS
 *
 * Returns
 *              * MID_SUCCESS(0):       successfully
 *              * MID_FAIL(-1):         failed to set pin.
 *
 * Note
 *              if newPinCode is NULL, it means that the pin is for SIM pin.
 *              if newPinCode has a value, it means that the pin is for PUK and the newPinCode 
 *				is for new SIM pin.
 *
 * See Also
 *
 ******************************************************************************/
int mapi_lte_Change_UMTS_SIM_PIN(int tid, char *pin, char *newPinCode)
{
    stSetSimPin sim_pins;
    int         rst=MID_SUCCESS, new_transc=0;
	stUmtsStatus status;

	if ((pin == NULL) || (newPinCode == NULL))
		return MID_FAIL;
	
	if ((pin[0] == '\0') || (newPinCode[0] == '\0'))
		return MID_FAIL;
		
	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}
    
    memset((char *)&sim_pins, 0, sizeof(sim_pins));
    if(pin!=NULL)
        strncpy(sim_pins.sim_pin1, pin, 15);
    if(newPinCode!=NULL)
        strncpy(sim_pins.sim_pin2, newPinCode, 15);

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_UMTS_SIM_PIN_CHANGE, &sim_pins, sizeof(sim_pins) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}
	
	if (rst == MID_SUCCESS)
	{
		do {
			usleep(500000);
			mapi_lte_umts_status_get (tid, &status);
		} while (status.pinCodeVerify == MAPI_LTE_SIM_PINCODE_UNKNOWN);

		//if (status.UMTS_pin_state == 0)
		if (status.pinCodeVerify == MAPI_LTE_SIM_PINCODE_CORRECT)
		{
			rst = MID_SUCCESS;
		}
		else
			rst = MID_FAIL; 
	}
		
	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

int mapi_lte_status_get_OnlineReady (int tid, int *onlineReady)
{
    int         rst=MID_SUCCESS, new_transc=0;
        
	if (onlineReady == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}
	
	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( onlineReady, 0, sizeof(*onlineReady) );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_ONLINE_READY_GET, onlineReady, sizeof(*onlineReady) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

int mapi_lte_status_get_VoiceSupport (int tid, int *UMTS_voice_support)
{
    int         rst=MID_SUCCESS, new_transc=0;
        
	if (UMTS_voice_support == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}
	
	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( UMTS_voice_support, 0, sizeof(*UMTS_voice_support) );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_UMTS_VOICE_SUPPORT_GET, UMTS_voice_support, sizeof(*UMTS_voice_support) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

int mapi_lte_get_RSRP(int tid, int *RSRP_val)
{
    int         rst=MID_SUCCESS, new_transc=0;
        
	if (RSRP_val == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}
	
	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( RSRP_val, 0, sizeof(*RSRP_val) );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_RSRP_GET, RSRP_val, sizeof(*RSRP_val) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

int mapi_lte_get_cell_info(int tid, struct umtsRunParameter_s *info)
{
    int         rst=MID_SUCCESS, new_transc=0;
        
	if (info == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}
	
	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( info, 0, sizeof(struct umtsRunParameter_s) );

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_CELLINFO_GET, info, sizeof(struct umtsRunParameter_s) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

int mapi_lte_get_umts_conn_stats(int tid, stConnStats *stats)
{
    int         rst=MID_SUCCESS, new_transc=0;
        
	if (stats == NULL) {
		MID_TRACE( "[%s] NULL buffer\n", __FUNCTION__ );
		return MID_FAIL;
	}

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}
	
	//set msg data, may be a data structure for the handler's convenience to parse it
	memset( stats, 0, sizeof(stConnStats) );
	stats->ifno = 3;		// 3=UMTS

	//send the request
	if (mapi_trx_transc( tid, MID_MSG_MOD_LTE, MID_LTE_STATS_GET, stats, sizeof(stConnStats) ) != MID_SUCCESS) {
		MID_TRACE( "[%s] mapi_trx_transc() failed\n", __FUNCTION__ );
		rst=MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );

    return rst;
}

//backup_mode@lte
int mapi_lte_set_backup_mode (int tid, int mode)
{
	int 	new_transc=0;
	char 	shcmd[128];

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}

	mapi_ccfg_set_long( tid, "lte", "backup_mode", mode );
	
	mapi_mon_set_backup_mode(tid, mode);

	if ( new_transc == 1 )
		mapi_end_transc( tid );

	sprintf (shcmd, "echo backup_mode = %d > /proc/arcusb/config", mode);
	if (osSystem (shcmd) != ARC_COM_OK)	return MID_FAIL;
		
	return MID_SUCCESS;
}

//apn_data@lte
int mapi_lte_set_LTE_data_APN (int tid, char *apn)
{
	int 	new_transc=0;
	char 	shcmd[128];

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}

	mapi_ccfg_set_str( tid, "lte", "apn_data", apn );

	if ( new_transc == 1 )
		mapi_end_transc( tid );

	sprintf (shcmd, "echo dataAPN = %s > /proc/arcusb/config", apn);
	if (osSystem (shcmd) != ARC_COM_OK)	return MID_FAIL;
}

//apn_voice@lte
int mapi_lte_set_LTE_voice_APN (int tid, char *apn)
{
	int 	new_transc=0;
	char 	shcmd[128];

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}

	mapi_ccfg_set_str( tid, "lte", "apn_voice", apn );

	if ( new_transc == 1 )
		mapi_end_transc( tid );
	
	sprintf (shcmd, "echo voiceAPN = %s > /proc/arcusb/config", apn);
	if (osSystem (shcmd) != ARC_COM_OK)	return MID_FAIL;
}

//pinenb@lte
int mapi_lte_set_LTE_store_PIN (int tid, unsigned char *option)
{
	char shcmd[128];
}

//apn_umts@lte
int mapi_lte_set_UMTS_APN (int tid, char *apn)
{
	int 	new_transc=0;
	char shcmd[128];

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}

	mapi_ccfg_set_str( tid, "lte", "apn_umts", apn );

	if ( new_transc == 1 )
		mapi_end_transc( tid );
	
	sprintf (shcmd, "echo umtsAPN = %s > /proc/arcusb/config", apn);
	if (osSystem (shcmd) != ARC_COM_OK)	return MID_FAIL;
}

//umts_idletime@lte
int mapi_lte_set_UMTS_idle_time (int tid, unsigned long idletime)
{
	int 	new_transc=0;
	char shcmd[128];

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}

	mapi_ccfg_set_long( tid, "lte", "idletime", idletime );
	
	mapi_mon_set_umts_idletime( tid, idletime);

	if ( new_transc == 1 )
		mapi_end_transc( tid );
}

//umts_phonenumber@lte
int mapi_lte_set_UMTS_phone_number (int tid, char *phonenumber)
{
	char shcmd[128];
}

//pinenb_umts@lte
int mapi_lte_set_UMTS_store_PIN (int tid, int store)
{
	int 	new_transc=0;
	char 	shcmd[128];

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}

	if (store) {
		mapi_ccfg_set_str( tid, "lte", "pinenb_umts", "1");
		sprintf (shcmd, "echo umtsPINenb = 1 > /proc/arcusb/config");
		if (osSystem (shcmd) != ARC_COM_OK)	return MID_FAIL;
	}
	else {
		mapi_ccfg_set_str( tid, "lte", "pinenb_umts", "0");
		sprintf (shcmd, "echo umtsPINenb = 0 > /proc/arcusb/config");
		if (osSystem (shcmd) != ARC_COM_OK)	return MID_FAIL;
	}

	if ( new_transc == 1 )
		mapi_end_transc( tid );
	
	return MID_SUCCESS;
}

//redir_confirm@lte
int mapi_lte_set_UMTS_redir_confirm (int tid, int conf)
{
	int 	new_transc=0;
	char 	shcmd[128];

	if (tid == 0){
		new_transc = 1;
		if ( (tid=mapi_start_transc()) == MID_FAIL )
			return MID_FAIL;
	}

	if (conf) {
		mapi_ccfg_set_str( tid, "lte", "redir_confirm", "1");
	}
	else {
		mapi_ccfg_set_str( tid, "lte", "redir_confirm", "0");
	}
	
	sprintf(shcmd, "/usr/sbin/httpd-brn -s");
	osSystem(shcmd);

	if ( new_transc == 1 )
		mapi_end_transc( tid );
	
	return MID_SUCCESS;
}
