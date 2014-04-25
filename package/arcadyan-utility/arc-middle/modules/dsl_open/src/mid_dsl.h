/*
 * Arcadyan middle layer definition for DSL access
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _MID_DSL_H_
#define _MID_DSL_H_


#include <mid_types.h>




#define MID_DSL_STAT_GET			0x01
#define MID_DSL_COUNTER_GET			0x02
#define MID_DSL_RECONNECT			0x03


#define	MID_DSL_VER_LEN				64


enum mid_dsl_mode_e {
	MID_DSL_MODE_NA=0,
	MID_DSL_MODE_VDSL=1,
	MID_DSL_MODE_ADSL_A=2,
	MID_DSL_MODE_ADSL_B=3,
	MID_DSL_MODE_ADSL_G992_1_A=4,
	MID_DSL_MODE_ADSL_G992_2=5,
	MID_DSL_MODE_ADSL_G992_3_A=6,
	MID_DSL_MODE_ADSL_G992_5_A=7,
	MID_DSL_MODE_ADSL_T1_413=8,
	MID_DSL_MODE_ADSL_G992_3_L=9,
	MID_DSL_MODE_ADSL_G992_3_I=10,
	MID_DSL_MODE_ADSL_G992_5_I=11,
	MID_DSL_MODE_ADSL_G992_3_M=12,
	MID_DSL_MODE_ADSL_G992_5_M=13,
	MID_DSL_MODE_ADSL_G992_1_B=14,
	MID_DSL_MODE_ADSL_G992_3_B=15,
	MID_DSL_MODE_ADSL_G992_5_B=16,	
	MID_DSL_MODE_ADSL_G992_3_J=17,	
	MID_DSL_MODE_ADSL_G992_5_J=18,
	MID_DSL_MODE_OTHER=255													
};

enum mid_dsl_link_state_e {
	MID_DSL_LINK_NOTINIT=0,
	MID_DSL_LINK_IDLE=0x100,
	MID_DSL_LINK_SILENT=0x200,
	MID_DSL_LINK_HANDSHAKE=0x300,
	MID_DSL_LINK_FULLINIT=0x380,
	MID_DSL_LINK_SHORTINIT=0x3C0,
	MID_DSL_LINK_DISCOVERY=0x400,
	MID_DSL_LINK_TRAINING=0x500,
	MID_DSL_LINK_SHOWTIME_NOSYNC=0x800,
	MID_DSL_LINK_SHOWTIME_TCSYNC=0x801,
	MID_DSL_LINK_FASTRETRAIN=0x900,
	MID_DSL_LINK_L2=0xA00,
	MID_DSL_LINK_DIAG_ACTIVE=0xB00,
	MID_DSL_LINK_DIAG_COMPLETE=0xC00,
	MID_DSL_LINK_RESYNC=0xD00,
	MID_DSL_LINK_LINETEST=0x1000000,
	MID_DSL_LINK_LOOPTEST=0x1000001,
	MID_DSL_LINK_L3=0x2000000,
	MID_DSL_LINK_NA=0x3000000,
};

enum mid_dsl_vsdl_prof_e {
	MID_DSL_VDSL_PROF_NA = -1,
	MID_DSL_VDSL_PROF_8A = 0,
	MID_DSL_VDSL_PROF_8B = 1,
	MID_DSL_VDSL_PROF_8C = 2,
	MID_DSL_VDSL_PROF_8D = 3,
	MID_DSL_VDSL_PROF_12A = 4,
	MID_DSL_VDSL_PROF_12B = 5,
	MID_DSL_VDSL_PROF_17A = 6,
	MID_DSL_VDSL_PROF_30A = 7,
	MID_DSL_VDSL_PROF_17B = 8,
};

enum mid_dsl_vsdl_bandplan_e {
	MID_DSL_VDSL_BANDPLAN_NA = -1,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_A_M1_EU32 = 0,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_A_M9_EU64 = 1,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_A_M1_ADLU32 = 2,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_A_M9_ADLU64 = 3,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B01 = 4,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B02 = 5,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B03 = 6,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B04 = 7,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B05 = 8,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B06 = 9,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B07 = 10,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B08 = 11,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B09 = 12,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B10 = 13,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B11 = 14,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B11_17 = 15,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B12 = 16,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B12_17 = 17,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B13 = 18,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_B13_17 = 19,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_C = 20,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_C_8K = 21,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_997E30_M2X_NUS0 = 22,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_C_1M1 = 23,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_C_8K_1M1 = 24,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_998E17_M2X_A = 25,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_B_998E17_M2X_NUS0 = 26,
	MID_DSL_VDSL_BANDPLAN_ANFP_CAL0_LONG = 27,
	MID_DSL_VDSL_BANDPLAN_ANFP_CAL0_MEDIUM = 28,
	MID_DSL_VDSL_BANDPLAN_ANFP_CAL0_SHORT = 29,
	MID_DSL_VDSL_BANDPLAN_ANFP_CAL0_ESHORT = 30,
	MID_DSL_VDSL_BANDPLAN_KOREA_FTTCAB = 31,
	MID_DSL_VDSL_BANDPLAN_KOREA_FTTCAB_8K = 32,
	MID_DSL_VDSL_BANDPLAN_HANARO_FTTCAB_8K = 33,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_C_FTTEX_A = 34,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_C_FTTEX_M = 35,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_C_FTTCAB_A = 36,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_C_FTTCAB_A_8K = 37,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_C_FTTCAB_M = 38,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_C_FTTCAB_M_8K = 39,
	MID_DSL_VDSL_BANDPLAN_ITU_ANNEX_A_M10_EU128 = 40,
	MID_DSL_VDSL2_ITU_ANNEXA_M1_EU32 = 50,
	MID_DSL_VDSL2_ITU_ANNEXA_M2_EU36 = 51,
	MID_DSL_VDSL2_ITU_ANNEXA_M3_EU40 = 52,
	MID_DSL_VDSL2_ITU_ANNEXA_M4_EU44 = 53,
	MID_DSL_VDSL2_ITU_ANNEXA_M5_EU48 = 54,
	MID_DSL_VDSL2_ITU_ANNEXA_M6_EU52 = 55,
	MID_DSL_VDSL2_ITU_ANNEXA_M7_EU56 = 56,
	MID_DSL_VDSL2_ITU_ANNEXA_M8_EU60 = 57,
	MID_DSL_VDSL2_ITU_ANNEXA_M9_EU64 = 58,
	MID_DSL_VDSL2_ITU_ANNEXA_EU128 = 59,
	MID_DSL_VDSL2_ITU_ANNEXA_M1_ADLU32 = 70,
	MID_DSL_VDSL2_ITU_ANNEXA_M2_ADLU36 = 71,
	MID_DSL_VDSL2_ITU_ANNEXA_M3_ADLU40 = 72,
	MID_DSL_VDSL2_ITU_ANNEXA_M4_ADLU44 = 73,
	MID_DSL_VDSL2_ITU_ANNEXA_M5_ADLU48 = 74,
	MID_DSL_VDSL2_ITU_ANNEXA_M6_ADLU52 = 75,
	MID_DSL_VDSL2_ITU_ANNEXA_M7_ADLU56 = 76,
	MID_DSL_VDSL2_ITU_ANNEXA_M8_ADLU60 = 77,
	MID_DSL_VDSL2_ITU_ANNEXA_M9_ADLU64 = 78,
	MID_DSL_VDSL2_ITU_ANNEXA_ADLU128 = 79,
	MID_DSL_VDSL2_ITU_ANNEXB_B7_1_997_M1C_A_7 = 100,
	MID_DSL_VDSL2_ITU_ANNEXB_B7_2_997_M1X_M_8 = 101,
	MID_DSL_VDSL2_ITU_ANNEXB_B7_3_997_M1X_M = 102,
	MID_DSL_VDSL2_ITU_ANNEXB_B7_4_997_M2X_M_8 = 103,
	MID_DSL_VDSL2_ITU_ANNEXB_B7_5_997_M2X_A = 104,
	MID_DSL_VDSL2_ITU_ANNEXB_B7_6_997_M2X_M = 105,
	MID_DSL_VDSL2_ITU_ANNEXB_B7_7_HPE17_M1_NUS0 = 106,
	MID_DSL_VDSL2_ITU_ANNEXB_B7_8_HPE30_M1_NUS0 = 107,
	MID_DSL_VDSL2_ITU_ANNEXB_B7_9_997E17_M2X_NUS0 = 108,
	MID_DSL_VDSL2_ITU_ANNEXB_B7_10_997E30_M2X_NUS0 = 109,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_1_998_M1X_A = 120,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_2_998_M1X_B = 121,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_3_998_M1X_NUS0 = 122,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_4_998_M2X_A = 123,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_5_998_M2X_M = 124,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_6_998_M2X_B = 125,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_7_998_M2X_NUS0 = 126,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_8_998E17_M2X_NUS0 = 127,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_9_998E17_M2X_NUS0_M = 128,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_10_998ADE17_M2X_NUS0_M = 129,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_11_998ADE17_M2X_A = 130,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_12_998ADE17_M2X_B = 131,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_13_998E30_M2X_NUS0 = 132,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_14_998E30_M2X_NUS0_M = 133,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_15_998ADE30_M2X_NUS0_M = 134,
	MID_DSL_VDSL2_ITU_ANNEXB_B8_16_998ADE30_M2X_NUS0_A = 135,
	MID_DSL_VDSL2_ITU_ANNEXB_998ADE17_M2X_M = 140,
	MID_DSL_VDSL2_ITU_ANNEXB_998E17_M2X_A = 141,
	MID_DSL_VDSL2_ITU_ANNEXC_FTTCAB_A = 160,
	MID_DSL_VDSL2_ITU_ANNEXC_FTTCAB_M = 161,
	MID_DSL_VDSL2_ITU_ANNEXC_FTTEX_A = 162,
	MID_DSL_VDSL2_ITU_ANNEXC_FTTEX_M = 163,
	MID_DSL_VDSL2_ITU_ANNEXC_O_ADSL = 164,
	MID_DSL_VDSL2_ITU_ANNEXC_O_TCMISDN = 165,
	MID_DSL_VDSL_ANSI_FTTCAB_M1 = 180,
	MID_DSL_VDSL_ANSI_FTTCAB_M2 = 181,
	MID_DSL_VDSL_ANSI_FTTEX_M1 = 182,
	MID_DSL_VDSL_ANSI_FTTEX_M2 = 183,
	MID_DSL_VDSL_ETSI_FTTCAB_PCAB_M1 = 184,
	MID_DSL_VDSL_ETSI_FTTCAB_PCAB_M2 = 185,
	MID_DSL_VDSL_ETSI_FTTEX_P1_M1_O_ISDN = 186,
	MID_DSL_VDSL_ETSI_FTTEX_P1_M2_O_ISDN = 187,
	MID_DSL_VDSL_ETSI_FTTEX_P2_M1_O_POTS = 188,
	MID_DSL_VDSL_ETSI_FTTEX_P2_M2_O_POTS = 189,
	MID_DSL_VDSL1_ITU_ANNEXE_E2_PCABA_M1 = 190,
	MID_DSL_VDSL_HANARO_FTTCAB = 200,
	MID_DSL_VDSL_ANFP_CAL0 = 201,
};

enum mid_dsl_latency_path_e {
	MID_DSL_LATENCY_NA = -1,
	MID_DSL_LATENCY_FAST = 0,
	MID_DSL_LATENCY_INTERLEAVED = 1,
};

typedef struct mid_dsl_state_s {	/* MID_DSL_STAT_GET */
	char						version[MID_DSL_VER_LEN];	/* dsl firmware version; format drvVer-fwVer-meiVer */
	enum mid_dsl_mode_e			mode;						/* dsl training mode */
	enum mid_dsl_link_state_e 	linkState;					/* current link state */
	U32							usDataRate;					/* upstream actual data rate, in Kbps */
	U32							usPrevRate;					/* upstream previous data rate, in Kbps */
	U32							usAttnRate;					/* upstream attainable data rate, in Kbps */
	U32							usIntDelay;					/* upstream actual interleave delay, in 0.01 ms */
	I32							usSnr;						/* upstream SNR margin, in 0.1 dB */
	I32							usLineAtn;					/* upstream line attenuation, in 0.1 dB */
	I32							usSigAtn;					/* upstream signal attenuation, in 0.1 dB */
	I32							usPsd;						/* upstream actual power spectrum density, in 0.1 dB */
	I32							usAtp;						/* upstream actual aggregate transmit power, in 0.1 dB */
	U32							usInp;						/* upstream actual impulse noise protection, in 0.1 symbols */
	U32							dsDataRate;					/* downstream actual data rate, in Kbps */
	U32							dsPrevRate;					/* downstream previous data rate, in Kbps */
	U32							dsAttnRate;					/* downstream attainable data rate, in Kbps */
	U32							dsIntDelay;					/* downstream actual interleave delay, in 0.01 ms */
	U32							dsInp;						/* downstream actual impulse noise protection, in 0.1 symbols */
	I32							dsSnr;						/* downstream SNR margin, in 0.1 dB */
	I32							dsLineAtn;					/* downstream line attenuation, in 0.1 dB */
	I32							dsSigAtn;					/* downstream signal attenuation, in 0.1 dB */
	I32							dsPsd;						/* downstream actual power spectrum density, in 0.1 dB */
	I32							dsAtp;						/* downstream actual aggregate transmit power, in 0.1 dB */
	enum mid_dsl_vsdl_prof_e	vdslProf;					/* current applied VDSL profile */
	enum mid_dsl_vsdl_bandplan_e vdslBandPlan;				/* current applied VDSL band plan */
	enum mid_dsl_latency_path_e	usLpath;					/* upstream latency path */
	U32							usCwSize;					/* upstream actual codeword size, in bytes */
	U32							usCwRdn;					/* upstream actual redundnacy bytes in codeword */
	U32							usSymLen;					/* upstream actual length per symbol, in bits */
	U32							usIntlvDepth;				/* upstream actual interleaving depth, 1 means no interleaving */
	U32							usBlkLen;					/* upstream actual interleaving block length, in bytes */
	enum mid_dsl_latency_path_e	dsLpath;					/* downstream latency path */
	U32							dsCwSize;					/* downstream actual codeword size, in bytes */
	U32							dsCwRdn;					/* downstream actual redundnacy bytes in codeword */
	U32							dsSymLen;					/* downstream actual length per symbol, in bits */
	U32							dsIntlvDepth;				/* downstream actual interleaving depth, 1 means no interleaving */
	U32							dsBlkLen;					/* downstream actual interleaving block length, in bytes */
} mid_dsl_state_t;	/* MID_DSL_STAT_GET */

typedef struct mid_dsl_counter_s {	/* MID_DSL_COUNTER_GET, from linkup */
	U32							neFec;						/* Near-end xDSL FEC anomalies */
	U32							neCv;						/* Near-end xDSL CRC-8 anomalies */
	U32							neHec;						/* Near-end ATM HEC anomalies */
	U32							neRxUsrCell;				/* Near-end ATM Rx user cells */
	U32							neTxUsrCell;				/* Near-end ATM Tx user cells */
	U32							neCrcP;						/* Near-end PTM CRC-n anomalies */
	U32							neCrcpP;					/* Near-end PTM CRC-np anomalies */
	U32							neCvP;						/* Near-end PTM CV-n anomalies */
	U32							neCvpP;						/* Near-end PTM CV-np anomalies */
	U32							feFec;						/* Far-end xDSL FEC anomalies */
	U32							feCv;						/* Far-end xDSL CRC-8 anomalies */
	U32							feHec;						/* Far-end ATM HEC anomalies */
	U32							feRxUsrCell;				/* Far-end ATM Rx user cells */
	U32							feTxUsrCell;				/* Far-end ATM Tx user cells */
	U32							feCrcP;						/* Far-end PTM CRC-n anomalies */
	U32							feCrcpP;					/* Far-end PTM CRC-np anomalies */
	U32							feCvP;						/* Far-end PTM CV-n anomalies */
	U32							feCvpP;						/* Far-end PTM CV-np anomalies */
} mid_dsl_counter_t;	/* MID_DSL_COUNTER_GET */

#endif // _MID_DSL_H_
