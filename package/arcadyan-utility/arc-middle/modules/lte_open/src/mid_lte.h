/*
 * Arcadyan middle layer definition for LTE
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_LTE_H_
#define _MID_LTE_H_

#include <mid_types.h>



//tmp file used in the handler
#define MID_LTE_TMP_FILE                "/tmp/lte_hdl.tmp"
#define MID_LTE_UMTS_TMP_FILE          	"/tmp/lte_hdl_umts.tmp"

//REQ message types
enum mid_lte_act_t {
	MID_LTE_STATUS_GET,
	MID_LTE_ONLINE_READY_GET,
	MID_LTE_UMTS_VOICE_SUPPORT_GET,  
	MID_LTE_RSRP_GET,  
	
	MID_LTE_SIM_PIN_GIVE,
	MID_LTE_STATS_GET,
	MID_LTE_CELLINFO_GET,
	
	MID_LTE_UMTS_STATUS_GET,
	MID_LTE_UMTS_IDLE_TIME_SET,
	MID_LTE_UMTS_PHONE_NUM_SET,
	MID_LTE_UMTS_STORE_PIN_SET,
	MID_LTE_UMTS_SIM_PIN_GIVE,
	MID_LTE_UMTS_SIM_PIN_CHANGE,
	MID_STATUS_LTE_GET
};

#define	MAPI_LTE_DIAL_STATE_NA				0
#define	MAPI_LTE_DIAL_STATE_IN_PROGRESS		1
#define	MAPI_LTE_DIAL_STATE_SUCCESS			2
#define	MAPI_LTE_DIAL_STATE_FAIL			3


#define	MAPI_LTE_BOOL_NO	0
#define	MAPI_LTE_BOOL_YES	1


#define MAPI_LTE_SIM_PINCODE_EMPTY		0
#define MAPI_LTE_SIM_PINCODE_INCORRECT	1
#define MAPI_LTE_SIM_PINCODE_CORRECT	2
#define MAPI_LTE_SIM_PINCODE_UNKNOWN	0xff


// TODO: TEMP USED. IT MUST SYNC WITH run-time shared include directory BEGIN
enum {
	MDM_DISABLE_TYPE = -1, 		// -1, PIN function is not enable
	MDM_READY_TYPE = 0,
	MDM_PIN_TYPE,		   		// 1, "PIN"
	MDM_PUK_TYPE,		   		// 2, "PUK"
	// ... refine later
	MDM_PIN_ERROR_TYPE = 16,    // 16
	MDM_PIN_STAT_MAX,	   		// 17
};

#define MDM_SYSSUBMD_NOSERV	0
#define MDM_SYSSUBMD_GSM	1
#define MDM_SYSSUBMD_GPRS	2
#define MDM_SYSSUBMD_EDGE	3
#define MDM_SYSSUBMD_WCDMA	4
#define MDM_SYSSUBMD_HSDPA	5
#define MDM_SYSSUBMD_HSUPA	6
#define MDM_SYSSUBMD_HSPA	7
// TODO: TEMP USED. IT MUST SYNC WITH run-time shared include directory END

enum mid_lte_antenna_state{
	MID_ANTENNA_TYPE_UNKNOWN,
	MID_ANTENNA_TYPE_INTERNAL,
	MID_ANTENNA_TYPE_EXTERNAL
};

/* LTE status */

typedef struct stLteStatus_s {
	int		dialReady;		// ready to start dialing, 0-no, 1-yes
	int		dialStateD;		// data APN, 0-NA, 1-in progress, 2-success, 3-fail
	int		dialStateV;		// Voice APN, 0-NA, 1-in progress, 2-success, 3-fail
	int		dialStateU;		// UMTS, 0-NA, 1-in progress, 2-success, 3-fail
	int		technology;		// 0-NA, 2-2G, 3-3G, 4-4G
	int		sigStrength_percent;
	int		sigStrength_dBm;
	char	status[128];
	char	provider_long[44];
	char	provider_short[12];
	long	provider_code;
	char	SIM_IMSI[64];
	char	modemModel[64];
	char	modemIMEI[32];
	char	fwVersion[128];
	char	hwVersion[128];
	int		simCardInsert;	// 1: "Yes" or 0: "No"
	int		pinCodeVerify;	// 1: "Empty", 2: "Incorrect", 3: "Correct", or 0xff: "Unknown"
	int		onlineReady;	// 1: "Yes" or 0: "No"

//LTE 
//status:
	int 	LTE_sim_card_changed; // lteIsSIMCardChange()      [ 0 : not changed,    1 : changed]
	int 	LTE_plugged;          // lteGetValidCnt()          [ 0 : not plugged,   >1 : plugged]
	int 	LTE_pin_ready;        // lteGetPINState()          [ 0 : not ready,      1 : ready for PIN Check]
	int 	LTE_pin_state;        // lteGetPINState()          [ 0 : PIN check fail, 1 : PIN check successfully]
	int 	LTE_pin_mode;         // lteGetPINState()          [ 0 : PIN mode,       1 : PUK mode, 2 : Invalid or No SIM, 3 : Ready ]
	int 	LTE_pin_leftCounter;  // lteGetPINTryLeftCounter() [ <=3 : possible tries] 
	unsigned long LTE_modem_state;
	unsigned long LTE_PDPmodem_state;
	unsigned long LTE_vPDPmodem_state;
// config:
	char	LTE_sim_pin[16];      // value@lte       
	int		LTE_pin_enable;       // value@lte    [0 : disable,  1: enable]

//UMTS
//status:
	int 	UMTS_modem_state;     //  getModemState()        [ UMTS_STATE_PIN_OK, UMTS_STATE_ATTACH , etc]
	int 	UMTS_plugged;         //  getValidUmtsCnt()	     [ 0 : not plugged, >1 : plugged]
	int 	UMTS_detection_state; //  getUmtsDetectState()   [ 0: umts device idle,  1: umts detetct process ongoing]
	int 	UMTS_pin_state;       //  getPINState()          [ enum {
								  //                           MDM_DISABLE_TYPE = -1,   // -1, PIN function is not enable
								  //                           MDM_READY_TYPE = 0,
								  //                           MDM_PIN_TYPE,		    // 1, "PIN"
								  //                           MDM_PUK_TYPE,		    // 2, "PUK"
								  //                           MDM_PIN_ERROR_TYPE = 16, // 16
								  //                           MDM_PIN_STAT_MAX,	    // 17
								  //                           }; ]
	int		UMTS_pin_LeftCounter; //  getPINTryLeftCounter() [<=3  : possible tries]
	int 	UMTS_event_state;     //  getUMTSEventStatus()   [ #define MDM_SYSSUBMD_NOSERV	0
								  //                           #define MDM_SYSSUBMD_GSM		1
								  //                           #define MDM_SYSSUBMD_GPRS	2
								  //                           #define MDM_SYSSUBMD_EDGE	3
								  //                           #define MDM_SYSSUBMD_WCDMA	4
								  //                           #define MDM_SYSSUBMD_HSDPA	5
								  //                           #define MDM_SYSSUBMD_HSUPA	6
								  //                           #define MDM_SYSSUBMD_HSPA	7 ]	
	int 	UMTS_voice_support;   //  getUmtsVoiceSupport()  [ 0: disable , 1:enable ]
	// need confirm with adai IsHSPA_PPP_Connected()
	int 	UMTS_data_support;    //  getUmtsVoiceSupport()  [ 0: disable , 1:enable ]
	char 	UMTS_sim_id[32];
//config:
	int 	UMTS_pin_Enable;      //  getPINState() & value@umts [ 0: disable , 1:enable ]
	char 	UMTS_sim_pin[16];     //  value@umts []	      
	char 	UMTS_number[32];      //  value@umts []
	int		UMTS_idle_time;		  //  value@umts []
	int		UMTS_backup_mode;		  //  [ 0: none , 1: data , 2: voice , 3: data&voice , 4: onlyumts]
	int		UMTS_pin_MaxLen;
	char 	UMTS_pin_UIStr[32];   //  value@umts []
	int		UMTS_pin_Err;


//OTHERS:
	int 	antenna_type;

} stLteStatus;

typedef struct stStatusLte_s {
	int		dataAPNconnected;	  // 1: "Yes" or 0: "No"
	int		voiceAPNconnected;	  // 1: "Yes" or 0: "No"
	int 	antenna_type;
	// need to add later....
} stStatusLte;

typedef struct stUmtsStatus_s {
	char	deviceManufacturer[32];
	char	deviceVendorInfo[64];
	char	deviceModel[64];
	char	deviceFirmware[64];
	char	deviceIMEI[32];
	char	SIM_IMSI[64];
	
	char	provider_long[44];
	char	provider_short[12];
	long	provider_code;

	int		sigStrength_percent;
	int		sigStrength_dBm;
	int 	UMTS_modem_state;
	int		pinCodeVerify;
	char	voice_support;
	char	data_support;
	int		packetSwitchAttached;
	int		circuitSwitchAttached;
} stUmtsStatus;

typedef struct {
    char 	sim_pin1[16];
    char 	sim_pin2[16];
} stSetSimPin;

typedef struct {
	unsigned short ifno;
	unsigned short connect;
	unsigned long ds_time;				// connection time
	unsigned long tx_rate;				// current transmiting rate (bps.)
	unsigned long rx_rate;				// current receiving rate (bps.)
	unsigned long tx_flow;				// current transmiting traffic (bytes)
	unsigned long rx_flow;				// current receiving traffic (bytes)
	unsigned long tx_qos_rate;			// PDP transmiting rate after negotiate with network side (bps.)
	unsigned long rx_qos_rate;			// PDP receiving rate after negotiate with network side (bps.)
} stConnStats;

#define MAX_UMTS_CELLINFO_SZ					16

struct lteCellInfo_s {
#define UMTS_CELLINFO_FLAG_GSM					0x0001
#define UMTS_CELLINFO_FLAG_WCDMA				0x0002
#define UMTS_CELLINFO_FLAG_LTE					0x0003
#define UMTS_CELLINFO_FLAG_TYPE_MSK				0x000f
#define UMTS_CELLINFO_FLAG_GSM_BAND1900			0x1000
#define UMTS_CELLINFO_FLAG_GSM_CID_VALID		0x2000
#define UMTS_CELLINFO_FLAG_INTER				0x4000		// means inter frequence cell,  without this bit means intra frequence cell
#define UMTS_CELLINFO_FLAG_SERVING				0x8000		// serving cell
	unsigned short flag;
	unsigned short pci;										// LTE:physical cell identity, WCDMA:primary scrambling code, GSM:BSIC-ID
	unsigned long cgi;										// LTE:cell global identity, WCDMA/GSM:cell index
	int rssi;
	int rssi_db;											//  (dBm)
	int rsrp;												// LTE:rsrp(dBm), WCDMA:rscp(dBm)
	int rsrq;												// LTE:rsrq(dBm), WCDMA:CPICH_ECNO
	int xarfcn;												// for EARFCN, UARFCN or ARFCN
	int srxlev;												// signal receive level
};

struct umtsRunParameter_s {
	//int antennaState;
	int neighborCellNum;
	struct lteCellInfo_s cellInfo[MAX_UMTS_CELLINFO_SZ];
	//struct lteCellCommonInfo_s cellCommonInfo;
    int band;
    int bandwidth;
    int sinr_int;
    int sinr_deci;
};

// item name definitions
#define MAPI_LTE_LINE_BUF_SZ	128
#define MAPI_LTE_STATUS_OFFSET	2 /* shift two bytes ": " */
#define MIDLTE_IS_ITEM(itemname)	(strncmp (linebuf, itemname, strlen(itemname)) == 0)
#define MIDLTE_CONTENT(itemname)	{							\
		pos = strlen (itemname) + MAPI_LTE_STATUS_OFFSET;		\
		valueptr = &linebuf[pos];								\
		LFpos = strlen (valueptr) - 1;							\
		if (LFpos >= 0) valueptr[LFpos] = 0x00;					\
	}

#endif // _MID_LTE_H_
