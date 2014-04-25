/*
 * Arcadyan middle layer definition for Network Monitor, including connection manager
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_MON_H_
#define _MID_MON_H_

#include <mid_types.h>



//tmp file used in the handler
//#define MID_MON_TMP_FILE                "/tmp/mon_hdl.tmp"

//REQ message types
enum mid_mon_act_t {
    MID_MON_DAEMON_START,                       //to start the monitor daemon
    MID_MON_DAEMON_STOP,                        //to stop the monitor daemon
    MID_MON_DAEMON_RESTART,                     //to re-start the monitor daemon
	MID_MON_4G3G_CONN_REQUEST,                  //to dial/hang-up/cancel

	//since the monitor should periodically check LTE status,
	//maybe provide APIs for other AP to query LTE/UMTS here to reduce system overhead?
	MID_MON_LTE_STATUS_GET,                     //to get the current LTE physical statue
	MID_MON_UMTS_STATUS_GET,                    //to get the current LTE physical statue
	MID_MON_SET_UPGRADE_STATE,
	MID_MON_SET_BACKUP_MODE,
	MID_MON_SET_SKIP_PROVISION,
	MID_MON_SET_UMTS_IDLETIME
};

#define MID_MON_CONNREQ_READY           0       //ready to accept connreq
#define MID_MON_CONNREQ_QUEUED          1       //there's a queued connreq

#define MID_MON_CONNREQ_NONE            0
#define MID_MON_CONNREQ_DIAL            1
#define MID_MON_CONNREQ_HANGUP          2

#define	MID_MON_WAN_LTE_DATA			0
#define MID_MON_WAN_LTE_VOICE			1
#define MID_MON_WAN_UMTS				2

typedef struct mid_mon_connreq{
    int         wan_idx;                        //0-based
    int         req_type;                       //MID_MON_CONNREQ_NONE/MID_MON_CONNREQ_DIAL/MID_MON_CONNREQ_HANGUP
    int         req_queued;                     //MID_MON_CONNREQ_READY/MID_MON_CONNREQ_QUEUED
} mid_mon_connreq_t;

typedef struct stConnectionStatus_s {
	int		state[3];		// 0: LTE data, 1: LTE voice, 2: UMTS

	unsigned long 	lte_state;
	unsigned long 	lte_dpdp;
	unsigned long	lte_vpdp;
	unsigned long	umts_state;
	unsigned long	umts_pdp;
	unsigned long 	umts_lasthittime;
	unsigned long 	currentTime;
	unsigned char	lte_sim_insert;

} stConnectionStatus;

typedef struct stMonConfig_s {
    int				backup_mode;
    int				skip_provision;
    unsigned long	umts_idletime;
} stMonConfig;

#endif // _MID_MON_H_
