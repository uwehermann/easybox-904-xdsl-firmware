
#ifndef _MID_TYPES_H_
#define _MID_TYPES_H_


#include "mid_hdl.h"
#include "mid_mapi_trnx.h"


#define		MID_BIT_LSHTV( val, cnt )		( (val) << (cnt) )	/*get bit-wise-left-shift of a value*/
#define		MID_BIT_RSHTV( val, cnt )		( (val) >> (cnt) )	/*get bit-wise-right-shift of a value*/
#define		MID_BIT_LSHT(  val, cnt )		( (val) <<= (cnt) )	/*set bit-wise-left-shift of a value*/
#define		MID_BIT_RSHT(  val, cnt )		( (val) >>= (cnt) )	/*set bit-wise-right-shift of a value*/

#define		MID_BIT_SET(   val, bit )		( (val) |=  MID_BIT_LSHTV(0x1,bit))			/*set a certain bit of a value*/
#define		MID_BIT_CLEAR( val, bit )		( (val) &= ~(MID_BIT_LSHTV(0x1,bit)))		/*clear a certain bit of a value*/
#define		MID_BIT_CHECK( val, bit )		(((val) &   MID_BIT_LSHTV(0x1,bit)) != 0x0 )/*check if a certain bit of a value is set*/


enum mid_rsp_e {
	MID_RSP_SUCCESS=0,
	MID_RSP_FAIL=-1,
};

enum mid_link_state_e {
	MID_LINK_UP=1,
	MID_LINK_DOWN=2,
};

/** A type for handling boolean issues. */
typedef enum {
   /** false */
   ARC_FALSE = 0,
   /** true */
   ARC_TRUE = 1
} BOOLEAN;

typedef	signed char		I8;
typedef	signed short	I16;
typedef	signed long		I32;
typedef	unsigned char	U8;
typedef	unsigned short	U16;
typedef	unsigned long	U32;
typedef	unsigned long long	U64;
typedef	unsigned int	Uint;


#endif // _MID_TYPES_H_
