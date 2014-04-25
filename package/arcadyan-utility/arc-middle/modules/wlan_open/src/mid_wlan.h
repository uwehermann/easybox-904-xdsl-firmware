/*
 * Arcadyan middle layer definition for misc system access
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_WLAN_H_
#define _MID_WLAN_H_

#include <mid_types.h>
#include <mid_mapi_ccfg.h>
#include <mid_mapi_ccfgsal_wlan.h>


#define GET_WLAN_IFNAME_SCRIPT 		"wlan_ifname.sh"  // This script should provide by driver package, 
													  // it reports interface name to middle layer

#define WLAN_WPS_SCRIPT 		"wps_action.sh"  	  // This script should provide by driver package, 
													  // it do WPS PBC/PIN 


#define MAPI_WLAN_STATUS_DISABLE	0	// disabled
#define MAPI_WLAN_STATUS_ENABLE		1	// enabled 




#define MID_WLAN_STA_GET			0x01
#define MID_WLAN_COUNTER_GET		0x02
#define MID_WLAN_WPS				0x03
#define MID_WLAN_WSC_GET_PROFILE	0x04
#define MID_WLAN_WSC_GET_STATUS     0x05
#define MID_WLAN_SCAN     			0x06
#define MID_WLAN_WSC_GEN_PIN		0x07
#define MID_WLAN_STATUS_GET			0x08



#define MID_WLAN_STA_MAX_CONNECTION	32


#define MID_WLAN_PBC			1
#define MID_WLAN_PIN			2
#define MID_WLAN_CANCEL			3


#define MID_WLAN_WPS_PIN_MAX_LEN 9

#define MID_WLAN_WPS_STATUS_NOT_USED 0
#define MID_WLAN_WPS_STATUS_IDLE 1
#define MID_WLAN_WPS_STATUS_INPROGRESS 2
#define MID_WLAN_WPS_STATUS_FAIL 3
#define MID_WLAN_WPS_STATUS_SUCESS 4

#define MID_MAX_SCAN_RESULT 50	// NOTE: becarful exceed maxmum mid buffer size

// Sync Mode
enum {
	MID_WLAN_SYNC_SELF,			// Only Sync specific band and specific ap_idx
	MID_WLAN_SYNC_ALL_BAND_IDX,  // Sync config to specific band, ie Bnd0 ApIpx1, Bnd0 ApIpx2, Bnd0 ApIpx3...
	MID_WLAN_SYNC_ALL_AP_IDX,     // Sync config to specific ap, ie Bnd0 ApIpx1, Bnd1 ApIpx1
	MID_WLAN_SYNC_ALL			// Sync config to all bands all ssids
};




/* WLAN Ethernet interface */

typedef struct {
    unsigned char   band;                  //MAPI_CCFG_WLAN_BAND_24GHZ / MAPI_CCFG_WLAN_BAND_5GHZ
    unsigned char   ap_idx;   
	unsigned char	sta_num;
	unsigned char	sta_mac[MID_WLAN_STA_MAX_CONNECTION][6];
	unsigned int	signal_strength[MID_WLAN_STA_MAX_CONNECTION]; // 0~100 Percent

} mid_wlan_sta_t;

typedef struct {
    unsigned char   band;                  //MAPI_CCFG_WLAN_BAND_24GHZ / MAPI_CCFG_WLAN_BAND_5GHZ
    unsigned char   ap_idx;   
	unsigned long   rx_packets;             /* total packets received       */
	unsigned long   tx_packets;             /* total packets transmitted    */
	unsigned long   rx_bytes;               /* total bytes received         */
	unsigned long   tx_bytes;               /* total bytes transmitted      */	
} mid_wlan_counter_t;

typedef struct {
    unsigned char   action;                  //MAPI_CCFG_WLAN_BAND_24GHZ / MAPI_CCFG_WLAN_BAND_5GHZ
    unsigned char   pin[MID_WLAN_WPS_PIN_MAX_LEN];
} mid_wlan_wps_t;


typedef struct {
    unsigned char band;                  //MAPI_CCFG_WLAN_BAND_24GHZ / MAPI_CCFG_WLAN_BAND_5GHZ
    unsigned char ap_idx; 
    unsigned char   sync_mode;
    unsigned char   pin[MID_WLAN_WPS_PIN_MAX_LEN];
} mid_wlan_gen_pin_t;


typedef struct {
    unsigned char band;                  //MAPI_CCFG_WLAN_BAND_24GHZ / MAPI_CCFG_WLAN_BAND_5GHZ
    unsigned char ap_idx;  
	unsigned char ssid[MAPI_CCFG_WLAN_SSID_MAX_LEN];
	unsigned char configured;
	unsigned char sec_mode;
	unsigned char encryp_type;
	unsigned char default_key;
	unsigned char key[MAPI_CCFG_WLAN_WPA_HEX_KEY_LEN];	
} mid_wlan_wsc_profile_t;

typedef struct {
    unsigned char band;                  //MAPI_CCFG_WLAN_BAND_24GHZ / MAPI_CCFG_WLAN_BAND_5GHZ
    unsigned char ap_idx;  
	int status;
} mid_wlan_wsc_status_t;

typedef struct {
unsigned char channel;
#ifdef MAPI_CCFG_WLAN_MFR_GUI_SUPPORT
unsigned char security;
#endif
unsigned char rssi;
unsigned char bssid[6];
unsigned char ssid[MAPI_CCFG_WLAN_SSID_MAX_LEN];
unsigned char   mode_mask;
}scan_info_t;

typedef struct {
    unsigned char band;                  //MAPI_CCFG_WLAN_BAND_24GHZ / MAPI_CCFG_WLAN_BAND_5GHZ
    unsigned char ap_num;
    scan_info_t scan_info[MID_MAX_SCAN_RESULT];
} mid_wlan_scan_t;


typedef struct {
    unsigned char   onoff_main;               					   // Current RF Staus MAPI_CCFG_WLAN_DISABLE / MAPI_CCFG_WLAN_ENABLE
    unsigned char   onoff_per_band[MAPI_CCFG_WLAN_MAX_BAND_NUM];   // Current RF Staus per band, not implement yet
    unsigned char   channel[MAPI_CCFG_WLAN_MAX_BAND_NUM];   	   // Current Channel per band, not implement yet
} mid_wlan_status_t;

typedef enum {
	WLAN_ENABLE = 1 << 0,
	WLAN_ENABLE_HW_ONOFF_BUTTON = 1 << 1,
	WLAN_DISPLAY_NETWORK_KEY = 1 << 2,
	WLAN_NOTIFY_NEW_DEVICE = 1 << 3,
	WLAN_ENABLE_GUEST_WIFI = 1 << 4,
	WLAN_SCHEDULE = 1 << 5,
	WLAN_ENABLE_WPS = 1 << 6,
	WLAN_ENABLE_HW_PARING_BUTTON = 1 << 7,
	WLAN_ENABLE_WPS_PIN = 1 << 8
} wlan_switch_t;


#endif // _MID_WLAN_H_
