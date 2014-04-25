#ifndef _ARC_COM_UTIL_H_
#define _ARC_COM_UTIL_H_


#include <stddef.h>

#define UTIL_FILE_MAX_OPEN_INTERVAL	100	/* 100 msec */
#define UTIL_FILE_MAX_OPEN_TRIAL		100	/* try up to 100 times */

#define UTIL_FILE_LCK  0
#define UTIL_FILE_UNLCK  1

#define		UTIL_ARRAYCNT(ary)			( sizeof(ary) / sizeof( (ary)[0] ) )	/*get the element count of an array*/
#define		UTIL_ALIGN4(num) 			( ((num)+3) / 4 * 4 )					/*get a number aligned to 4*/
#define		UTIL_ALIGN4REST(num) 		( UTIL_ALIGN4(num) - (num) )			/*get the offset of a number aligned to 4*/
#define		UTIL_MIN(val1,val2)			( (val1) < (val2) ? (val1) : (val2) )	/*get the minimum value of two numbers*/
#define		UTIL_MAX(val1,val2)			( (val1) > (val2) ? (val1) : (val2) )	/*get the maximum value of two numbers*/
#define		UTIL_INRANGE(val,min,max)	( (val) >= (min) && (val) <= (max) )	/*check if a number is in the range of two values*/
#define		UTIL_OFFSETOF(dataType,memberName)	((size_t) &((dataType*)0)->memberName)	/*get the offset of member data in a data structure*/

#define		UTIL_BIT_LSHTV(val,cnt)		( (val) << (cnt) )		/*get bit-wise-left-shift of a value*/
#define		UTIL_BIT_RSHTV(val,cnt)		( (val) >> (cnt) )		/*get bit-wise-right-shift of a value*/
#define		UTIL_BIT_LSHT(val,cnt)		( (val) <<= (cnt) )		/*set bit-wise-left-shift of a value*/
#define		UTIL_BIT_RSHT(val,cnt)		( (val) >>= (cnt) )		/*set bit-wise-right-shift of a value*/

#define		UTIL_BIT_SET(val,bit)		( (val) |=  UTIL_BIT_LSHTV(0x1,bit))	/*set a certain bit of a value*/
#define		UTIL_BIT_CLEAR(val,bit)		( (val) &= ~(UTIL_BIT_LSHTV(0x1,bit)))	/*clear a certain bit of a value*/
#define		UTIL_BIT_CHECK(val,bit)		(((val) &   UTIL_BIT_LSHTV(0x1,bit)) != 0x0 )/*check if a certain bit of a value is set*/


#ifdef __cplusplus
extern "C" {
#endif


extern char*	utilStrNCpy ( char* sDst, char* sSrc, size_t ulCnt );
extern char*	utilStrMCpy ( char* sDst, char* sSrc, size_t ulCnt );
extern long		utilStrChkShort ( char* sStr, short iLB, short iUB, short* piNum );
extern char*	utilStrLTrimChar (char* sStr, char* sTrimChars);
extern char*	utilStrRTrimChar (char* sStr, char* sTrimChars);
extern char*	utilStrTrimChar (char* sStr, char* sTrimChars);
extern char*	utilStrLTrim (char* sStr);
extern char*	utilStrRTrim (char* sStr);
extern char*	utilStrTrim (char* sStr);
extern long		utilCharReplace (char* sStr, char* sChars, char cNew);

extern long		utilAry2Str ( unsigned char* byAry, unsigned long lAryCnt, char* sBuf );
extern long		utilStr2Ary ( char* sStr, unsigned char* byAry, unsigned long lAryCnt );

extern long		utilMacZero ( unsigned char* byMac );
extern long		utilMacIsZero ( unsigned char* byMac );
extern long		utilMac2MacStr ( unsigned char* byMac, char* sBuf );
extern long		utilMacStr2Mac ( char* sStr, unsigned char* byMac );
extern long		utilMacCmp ( unsigned char* byMac1, unsigned char* byMac2 );
extern long		utilMacCpy ( unsigned char* byDst, unsigned char* bySrc );

extern long		utilIpZero ( unsigned char* byIp );
extern long		utilIpIsZero ( unsigned char* byIp );
extern long		utilIp2IpStr ( unsigned char* byIp, char* sBuf );
extern long		utilIpStr2Ip ( char* sStr, unsigned char* byIp );
extern long		utilIp2Val ( unsigned char* byIp, unsigned long* pu4Val );
extern long		utilVal2Ip ( unsigned long u4Val, unsigned char* byIp );
extern long		utilIpCmp ( unsigned char* byIp1, unsigned char* byIp2 );
extern long		utilIpCpy ( unsigned char* byIp1, unsigned char* byIp2 );

extern long		utilIpSubnetAddr( unsigned char* byIp, unsigned char* bySubmask, unsigned char* pSubAddr);
extern long		utilIpSubnetBcAddr( unsigned char* byIp, unsigned char* bySubmask, unsigned char* pSubBcAddr);
extern long		utilIpIsSameSubnetAddr( unsigned char* byIp1, unsigned char* bySubmask1, unsigned char* byIp2, unsigned char* bySubmask2);
extern long		utilIpInSameSubnet (  unsigned char* byIp1, unsigned char* byIp2, unsigned char* bySubmask );

extern int		utilParseArgs (char* sArgStr, char* sDelimiters, int iArgc, char** pArgv);
extern int 		utilGetlockByName(char* filename, int *fd, int type);
extern long		utilSendMailByProfileID(int id, char* toAccount, char* subject, char* context);
extern long		utilSendMail(char* toAccount, char* subject, char* context);

#ifdef __cplusplus
}
#endif


#endif /* _ARC_COM_UTIL_H_ */
