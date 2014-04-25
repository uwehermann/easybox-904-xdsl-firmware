
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "libArcComApi.h"

/*******************************************************************************
 * Description
 *		Copy characters of one string to another. pls refer to ANSI strncpy()
 *
 * Note
 *		* if length of sSrc is less than ulCnt, then all the part in sDat from
 *		  length of sSrc to ulCnt will be filled with '\0'
 *
 ******************************************************************************/
char* utilStrNCpy(char* sDst, char* sSrc, size_t ulCnt)
{
	size_t	ulLen;

	if (strncpy(sDst,sSrc,ulCnt) == 0)
		return 0;

	ulLen = strlen(sSrc);
	if ((ulLen+1) < ulCnt)
		osBZero(sDst+ulLen+1,ulCnt-ulLen-1);

	return sDst;
}

/*******************************************************************************
 * Description
 *		Copy characters of one string to another. pls refer to ANSI strncpy()
 *
 * Note
 *		* if length of sSrc is less than ulCnt, then all the part in sDat from
 *		  length of sSrc to ulCnt will be filled with '\0'
 *		* if length of sSrc is equal to ulCnt, then sDst[ulCnt-1] will be '\0'
 *
 ******************************************************************************/
char* utilStrMCpy(char* sDst, char* sSrc, size_t ulCnt)
{
	size_t	ulLen;

	if (utilStrNCpy(sDst,sSrc,ulCnt) == 0)
		return 0;

	ulLen = strlen(sSrc);
	if (ulLen >= ulCnt)
		sDst[ulCnt-1] = '\0';

	return sDst;
}

/*******************************************************************************
 * Description
 *		Check an ascii string if it is a short integer, and return the value
 *
 * Parameters
 *		sStr:	pointer to the target string
 *		iLB:	the lower-bound of short integer
 *		iUB:	the lower-bound of short integer
 *		piNum:	pointer to the buffer for return value. It can be NULL.
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	sStr is NULL, or result value is not between iLB and iUB, or iUB is less than iLB or format of sStr is error
 *
 * Note
 *		* hex-decimal integer is acceptable
 *		* format : [Whitespaces][+/-][0x]digits
 *		* if piNum is NULL, the function won't copy any value but only check
 *		* check range of signed short integer : -32768..32767
 *
 ******************************************************************************/
long utilStrChkShort(char* sStr, short iLB, short iUB, short* piNum)
{
	long	lNum;
	char*	pPtr;
	char*	pTail;

	if (sStr==ARC_COM_NULL || iLB>iUB)
		return ARC_COM_ERROR;

	pPtr = sStr;
	while (*pPtr!='\0' && *pPtr==' ')
	{
		pPtr++;
	}
	if (*pPtr == '\0')
		return ARC_COM_ERROR;

	if (*pPtr=='0' && (*(pPtr+1)=='x'||*(pPtr+1)=='X'))
			lNum = strtol(pPtr,&pTail,16);
	  else	lNum = strtol(pPtr,&pTail,10);

	if (*pTail != '\0')
		return ARC_COM_ERROR;

	if (lNum<(long)iLB || lNum>(long)iUB)
		return ARC_COM_ERROR;

	if (piNum != NULL)
		*piNum = (short)lNum;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Trim those space characters in front of an ASCII string (remove left spaces)
 *
 * Parameters
 *		sStr:		pointer to the target string
 *		sTrimChars:	characters to be trimmed.
 *
 * Returns
 *		* ARC_COM_NULL(0):	sStr or sTrimChars are NULL
 *		* others:			pointer to the result string
 *
 ******************************************************************************/
char* utilStrLTrimChar(char* sStr, char* sTrimChars)
{
	char*	sPtr;

	if ( sStr == ARC_COM_NULL || sTrimChars == ARC_COM_NULL )
		return ARC_COM_NULL;

	sPtr = sStr;

	while ( *sPtr!='\0' && strchr(sTrimChars, *sPtr) != ARC_COM_NULL )
	{
		sPtr++;
	}

	if (sPtr != sStr)
		strcpy( sStr, sPtr );

	return sStr;
}

/*******************************************************************************
 * Description
 *		Trim those space characters behind of an ASCII string (remove right spaces)
 *
 * Parameters
 *		sStr:		pointer to the target string
 *		sTrimChars:	characters to be trimmed.
 *
 * Returns
 *		* ARC_COM_NULL(0):	sStr or sTrimChars are NULL
 *		* others:			pointer to the result string
 *
 ******************************************************************************/
char* utilStrRTrimChar(char* sStr, char* sTrimChars)
{
	char*	sPtr;

	if ( sStr == ARC_COM_NULL || sTrimChars == ARC_COM_NULL )
		return ARC_COM_NULL;

	sPtr = sStr + strlen(sStr);

	while ( sPtr >= sStr && strchr(sTrimChars, *sPtr) != ARC_COM_NULL )
	{
		sPtr--;
	}

	*(sPtr+1) = '\0';

	return sStr;
}

/*******************************************************************************
 * Description
 *		Trim those space characters in front and behind of an ASCII string
 *		(remove left and right spaces)
 *
 * Parameters
 *		sStr:		pointer to the target string
 *		sTrimChars:	characters to be trimmed.
 *
 * Returns
 *		* ARC_COM_NULL(0):	sStr or sTrimChars are NULL
 *		* others:			pointer to the result string
 *
 ******************************************************************************/
char* utilStrTrimChar(char* sStr, char* sTrimChars)
{
	char*	sPtr;

	if ( sStr == ARC_COM_NULL || sTrimChars == ARC_COM_NULL )
		return ARC_COM_NULL;

	sPtr = utilStrLTrimChar(sStr, sTrimChars);
	sPtr = utilStrRTrimChar(sPtr, sTrimChars);

	return sPtr;
}

/*******************************************************************************
 * Description
 *		Trim those space characters in front of an ASCII string (remove left spaces)
 *
 * Parameters
 *		sStr:	pointer to the target string
 *
 * Returns
 *		* ARC_COM_NULL(0):	sStr or sTrimChars are NULL
 *		* others:			pointer to the result string
 *
 ******************************************************************************/
char* utilStrLTrim(char* sStr)
{
	return utilStrLTrimChar( sStr, " \t\r\n");
}

/*******************************************************************************
 * Description
 *		Trim those space characters behind of an ASCII string (remove right spaces)
 *
 * Parameters
 *		sStr:	pointer to the target string
 *
 * Returns
 *		* ARC_COM_NULL(0):	sStr or sTrimChars are NULL
 *		* others:			pointer to the result string
 *
 ******************************************************************************/
char* utilStrRTrim(char* sStr)
{
	return utilStrRTrimChar( sStr, " \t\r\n");
}

/*******************************************************************************
 * Description
 *		Trim those space characters in front and behind of an ASCII string
 *		(remove left and right spaces)
 *
 * Parameters
 *		sStr:	pointer to the target string
 *
 * Returns
 *		* ARC_COM_NULL(0):	sStr or sTrimChars are NULL
 *		* others:			pointer to the result string
 *
 ******************************************************************************/
char* utilStrTrim(char* sStr)
{
	return utilStrTrimChar( sStr, " \t\r\n");
}

/*******************************************************************************
 * Description
 *		Replace specified characters with specific character in a string
 *
 * Parameters
 *		sStr:	pointer to the target string
 *		sChars:	characters to be replaced
 *		cNew:	new character
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	sStr or sChars are NULL
 *		* others:				count of characters replaced
 *
 ******************************************************************************/
long utilCharReplace( char* sStr, char* sChars, char cNew )
{
	char*	pPtr;
	long	iCnt;

	if ( sStr == ARC_COM_NULL || sChars == ARC_COM_NULL )
		return -1;

	for ( pPtr=sStr, iCnt=0 ; *pPtr != '\0' ; pPtr++ )
	{
		if ( strchr( sChars, *pPtr ) == ARC_COM_NULL )
			continue;
		*pPtr = cNew;
		iCnt++;
	}

	return iCnt;
}

/*******************************************************************************
 * Description
 *		Convert an array of bytes to an ascii string
 *
 * Parameters
 *		byAry:		array of bytes
 *		lAryCnt:	element count of array byAry, >= 0
 *		sBuf:		buffer to filled with string
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byAry or sStr are NULL
 *
 * Note
 *		* size of sBuf MUST be at least 2*lAryCnt+1
 *		* format of string is aabb....xx where a/b/x is hex-decimal
 *		  and between 00 and ff. sBuf will be zero ending '\0'.
 *
 ******************************************************************************/
long utilAry2Str( unsigned char* byAry, unsigned long lAryCnt, char* sBuf )
{
	unsigned long	lCnt;
	unsigned char	iHex;
	char*			pPtr;

	if ( byAry == ARC_COM_NULL || sBuf == ARC_COM_NULL )
		return ARC_COM_ERROR;

	for ( lCnt=0, pPtr=sBuf ; lCnt < lAryCnt ; lCnt++ ) {
		//
		iHex = ( byAry[lCnt] & 0xf0) >> 4;
		if ( iHex <= 9 )
			*pPtr++ = iHex + '0';
		else
			*pPtr++ = iHex -0 + 'a';
		//
		iHex = byAry[lCnt] & 0x0f;
		if ( iHex <= 9 )
			*pPtr++ = iHex + '0';
		else
			*pPtr++ = iHex -0 + 'a';
	}

	*pPtr = '\0';

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Convert an ascii string to an array of bytes
 *
 * Parameters
 *		sBuf:		ascii string to be converted
 *		byAry:		array of bytes
 *		lAryCnt:	element count of array byAry, >= 0
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	byAry or sStr are NULL
 *		* others:				count of bytes converted
 *
 * Note
 *		* convertion will be stopped if meets any character not in {0~9,a~f}
 *		  or reaches lAryCnt.
 *		* format of string is aabb....xx where a/b/x is hex-decimal
 *		  and between 0 and ff.
 *
 ******************************************************************************/
long utilStr2Ary(char* sStr, unsigned char* byAry, unsigned long lAryCnt )
{
	unsigned long	lCnt;
	char*			pPtr;
	int				cChar;
	unsigned char	cByte;

	if ( byAry == ARC_COM_NULL || sStr == ARC_COM_NULL )
		return ARC_COM_ERROR;

	for ( lCnt=0, pPtr=sStr ; lCnt < lAryCnt ; lCnt++ ) {
		//
		cChar = tolower(*pPtr++);
		if (cChar>='0' && cChar<='9')
			cByte = (cChar-'0') << 4;
		else if (cChar>='a' && cChar<='f')
			cByte = (cChar-'a'+10) << 4;
		else
			break;
		//
		cChar = tolower(*pPtr++);
		if (cChar>='0' && cChar<='9')
			cByte += (cChar-'0');
		else if (cChar>='a' && cChar<='f')
			cByte += (cChar-'a'+10);
		else
			break;
		//
		byAry[lCnt] = cByte;
	}

	return lCnt;
}

/*******************************************************************************
 * Description
 *		Clear MAC address
 *
 * Parameters
 *		byMac:	destination MAC address
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byMac is null
 *
 * Note
 *		* MAC address is composed of 6 bytes unsigned character
 *
 ******************************************************************************/
long utilMacZero(unsigned char* byMac)
{
	if (byMac == ARC_COM_NULL)
		return ARC_COM_ERROR;

	*byMac++ = 0;
	*byMac++ = 0;
	*byMac++ = 0;
	*byMac++ = 0;
	*byMac++ = 0;
	*byMac   = 0;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Check if MAC address is a zero address
 *
 * Parameters
 *		byMac:	destination MAC address
 *
 * Returns
 *		* ARC_COM_TRUE(1):		a zero address
 *		* ARC_COM_FALSE(0):		not a zero address
 *		* ARC_COM_ERROR(-1):	byMac is null
 *
 * Note
 *		* MAC address is composed of 6 bytes unsigned character
 *
 ******************************************************************************/
long utilMacIsZero(unsigned char* byMac)
{
	if (byMac == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if (byMac[0]==0 && byMac[1]==0
	 && byMac[2]==0 && byMac[3]==0
	 && byMac[4]==0 && byMac[5]==0)
		return ARC_COM_TRUE;
	else
		return ARC_COM_FALSE;
}

/*******************************************************************************
 * DescrMaction
 *		Convert MAC address to an ascii string
 *
 * Parameters
 *		byMac:	destination MAC address
 *		sBuf:	buffer to filled with MAC string
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byMac or sBuf are null
 *
 * Note
 *		* MAC address is composed of 6 bytes unsigned character
 *		* format of MAC string is "a-b-c-d-e-f"
 *			where a/b/c/d/e/f is hex-decimal and between 0 and ff
 *
 ******************************************************************************/
long utilMac2MacStr(unsigned char* byMac, char* sBuf)
{
	if (byMac==ARC_COM_NULL || sBuf==ARC_COM_NULL)
		return ARC_COM_ERROR;

	sprintf(sBuf,"%02x:%02x:%02x:%02x:%02x:%02x"
				,byMac[0],byMac[1],byMac[2],byMac[3],byMac[4],byMac[5]);

	return ARC_COM_OK;
}

/*******************************************************************************
 * DescrMaction
 *		Convert MAC address to an ascii string
 *
 * Parameters
 *		byMac:	destination MAC address
 *		sStr:	Mac string
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byMac or sBuf are null or format of sBuf is invalid
 *
 * Note
 *		* Mac address is composed of 6 bytes unsigned character
 *		* format of MAC string is "a-b-c-d-e-f" or "a:b:c:d:e:f"
 *			or "a b c d e f" or "a.b.c.d.e.f" or "a,b,c,d,e,f" or combined
 *			where a/b/c/d/e/f is hex-decimal and between 0 and ff
 *
 ******************************************************************************/
long utilMacStr2Mac(char* sStr, unsigned char* byMac)
{
	char	sMacStr[20];
	char*	pPtr;
	char*	pLast;
	int		iCnt;
	char	sSubMac[6];
	short	iSubMac;

	if (byMac==ARC_COM_NULL || sStr==ARC_COM_NULL)
		return ARC_COM_ERROR;

	utilStrMCpy(sMacStr, sStr, sizeof(sMacStr));

	utilMacZero(byMac);

	sSubMac[0] = '0'; sSubMac[1] = 'x'; sSubMac[5] = 'x';
	for (iCnt=0, pPtr=osStrTok_r(sMacStr,":-., ",&pLast);
		 iCnt<6 && pPtr!=0;
		 iCnt++, pPtr=osStrTok_r(pLast,":-., ",&pLast))
	{
		sSubMac[2] = pPtr[0];
		sSubMac[3] = pPtr[1];
		sSubMac[4] = pPtr[2];
		if (utilStrChkShort(sSubMac,0,255,&iSubMac) != 0)
			return -1;
		byMac[iCnt] = (unsigned char)iSubMac;
	}

	if (iCnt<6 || pLast!=ARC_COM_NULL)
		return ARC_COM_ERROR;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Copy MAC address
 *
 * Parameters
 *		pDst:	destination MAC address
 *		pSrc:	source MAC address
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byDst or bySrc are null
 *
 * Note
 *		* MAC address is composed of 6 bytes unsigned character
 *
 ******************************************************************************/
long utilMacCpy(unsigned char* byDst, unsigned char* bySrc)
{
	if (byDst==ARC_COM_NULL || bySrc==ARC_COM_NULL)
		return ARC_COM_ERROR;

	*byDst++ = *bySrc++;
	*byDst++ = *bySrc++;
	*byDst++ = *bySrc++;
	*byDst++ = *bySrc++;
	*byDst++ = *bySrc++;
	*byDst   = *bySrc;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Compare two MAC addresses
 *
 * Parameters
 *		byMac1:	the first MAC address
 *		byMac2:	the second MAC address
 *
 * Returns
 *		* 0:	byMac1 is euqal to byMac2 or both are null
 *		* 1:	byMac1 is greater than byMac1 or byMac2 is null
 *		* -1:	byMac1 is less than byMac2 or byMac1 is null
 *
 * Note
 *		* MAC address is composed of 6 bytes unsigned character
 *
 ******************************************************************************/
long utilMacCmp(unsigned char* byMac1, unsigned char* byMac2)
{
	unsigned char	iCnt;

	if (byMac1==ARC_COM_NULL && byMac2==ARC_COM_NULL)
		return 0;

	if (byMac1==ARC_COM_NULL && byMac2!=ARC_COM_NULL)
		return -1;

	if (byMac1!=ARC_COM_NULL && byMac2==ARC_COM_NULL)
		return 1;

	for (iCnt=0; iCnt<6; iCnt++) {
		if (byMac1[iCnt] > byMac2[iCnt])
			return 1;
		if (byMac1[iCnt] < byMac2[iCnt])
			return -1;
	}

	return 0;
}

/*******************************************************************************
 * Description
 *		Clear IP address
 *
 * Parameters
 *		byIp:	destination IP address
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byIp is null
 *
 * Note
 *		* IP address is composed of 4 bytes unsigned character
 *
 ******************************************************************************/
long utilIpZero(unsigned char* byIp)
{
	if (byIp == ARC_COM_NULL)
		return ARC_COM_ERROR;

	*byIp++ = 0;
	*byIp++ = 0;
	*byIp++ = 0;
	*byIp   = 0;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Check if IP address is a zero address
 *
 * Parameters
 *		byIp:	destination IP address
 *
 * Returns
 *		* ARC_COM_TRUE(1):		a zero address
 *		* ARC_COM_FALSE(0):		not a zero address
 *		* ARC_COM_ERROR(-1):	byIp is null
 *
 * Note
 *		* IP address is composed of 4 bytes unsigned character
 *
 ******************************************************************************/
long utilIpIsZero(unsigned char* byIp)
{
	if (byIp == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if (byIp[0]==0 && byIp[1]==0
	 && byIp[2]==0 && byIp[3]==0)
		return ARC_COM_TRUE;
	else
		return ARC_COM_FALSE;
}

/*******************************************************************************
 * Description
 *		Convert IP address to an ascii string
 *
 * Parameters
 *		byIp:	destination IP address
 *		sBuf:	buffer to filled with IP string
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byIp or sBuf are null
 *
 * Note
 *		* IP address is composed of 4 bytes unsigned character
 *		* format of IP string is a.b.c.d where a/b/c/d is decimal and between 0 and 255
 *
 ******************************************************************************/
long utilIp2IpStr(unsigned char* byIp, char* sBuf)
{
	if (byIp==ARC_COM_NULL || sBuf==ARC_COM_NULL)
		return ARC_COM_ERROR;

	sprintf(sBuf,"%u.%u.%u.%u"
				,byIp[0],byIp[1],byIp[2],byIp[3]);

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Convert IP address to an ascii string
 *
 * Parameters
 *		byIp:	destination IP address
 *		sStr:	IP string
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byIp or sBuf are null, or format of sBuf is invalid
 *
 * Note
 *		* IP address is composed of 4 bytes unsigned character
 *		* format of IP string is "a.b.c.d" or "a b c d" or "a,b,c,d"
 *			where a/b/c/d is decimal and between 0 and 255
 *
 ******************************************************************************/
long utilIpStr2Ip(char* sStr, unsigned char* byIp)
{
	char	sIPStr[20];
	char*	pPtr;
	char*	pLast;
	int		iCnt;
	short	iSubIp;

	if (byIp==ARC_COM_NULL || sStr==ARC_COM_NULL)
		return ARC_COM_ERROR;

	utilStrMCpy(sIPStr,sStr,sizeof(sIPStr));

	utilIpZero(byIp);

	for (iCnt=0, pPtr=osStrTok_r(sIPStr,"., ",&pLast);
		 iCnt<4 && pPtr!=0;
		 iCnt++, pPtr=osStrTok_r(pLast,"., ",&pLast))
	{
		if (utilStrChkShort(pPtr,0,255,&iSubIp) != 0)
			return ARC_COM_ERROR;
		byIp[iCnt] = (unsigned char)iSubIp;
	}

	if (iCnt<4 || pLast!=ARC_COM_NULL)
		return ARC_COM_ERROR;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Convert IP address to a U32 value
 *
 * Parameters
 *		byIp:	destination IP address
 *		pu4Val:	buffer to filled with unsigned long value
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byIp or pu4Val are null
 *
 * Note
 *		* IP address is composed of 4 bytes unsigned character
 *
 ******************************************************************************/
long utilIp2Val(unsigned char* byIp, unsigned long* pu4Val)
{
	if (byIp==ARC_COM_NULL || pu4Val==ARC_COM_NULL)
		return ARC_COM_ERROR;

	*pu4Val =  ( ((unsigned long)byIp[0]) << 24)
			 + ( ((unsigned long)byIp[1]) << 16)
			 + ( ((unsigned long)byIp[2]) <<  8)
			 + ( ((unsigned long)byIp[3]) <<  0);

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Convert a unsigned long value to IP address
 *
 * Parameters
 *		byIp:	destination IP address
 *		u4Val:	unsigned long IP value
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byIp is null
 *
 * Note
 *		* IP address is composed of 4 bytes unsigned character
 *
 ******************************************************************************/
long utilVal2Ip(unsigned long u4Val, unsigned char* byIp)
{
	if (byIp == ARC_COM_NULL)
		return ARC_COM_ERROR;

	byIp[0] = (unsigned char)(u4Val >> 24);
	byIp[1] = (unsigned char)(u4Val >> 16) & 0xFF;
	byIp[2] = (unsigned char)(u4Val >>  8) & 0xFF;
	byIp[3] = (unsigned char)(u4Val >>  0) & 0xFF;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Compare two IP addresses
 *
 * Parameters
 *		byIp1:	the first IP address
 *		byIp2:	the second IP address
 *
 * Returns
 *		* 0:	byIp1 is euqal to byIp2 or both are null
 *		* 1:	byIp1 is greater than byIp1 or byIp2 is null
 *		* -1:	byIp1 is less than byIp2 or byIp1 is null
 *
 * Note
 *		* IP address is composed of 4 bytes unsigned character
 *
 ******************************************************************************/
long utilIpCmp(unsigned char* byIp1, unsigned char* byIp2)
{
	unsigned char	iCnt;

	if (byIp1==ARC_COM_NULL && byIp2==ARC_COM_NULL)
		return 0;

	if (byIp1==ARC_COM_NULL && byIp2!=ARC_COM_NULL)
		return -1;

	if (byIp1!=ARC_COM_NULL && byIp2==ARC_COM_NULL)
		return 1;

	for (iCnt=0; iCnt<4; iCnt++) {
		if (byIp1[iCnt] > byIp2[iCnt])
			return 1;
		if (byIp1[iCnt] < byIp2[iCnt])
			return -1;
	}

	return 0;
}

/*******************************************************************************
 * Description
 *		Copy IP address
 *
 * Parameters
 *		byDst:	destination IP address
 *		bySrc:	source IP address
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byDst or bySrc are null
 *
 * Note
 *		* IP address is composed of 4 bytes unsigned character
 *
 ******************************************************************************/
long utilIpCpy(unsigned char* byDst, unsigned char* bySrc)
{
	if (byDst==ARC_COM_NULL || bySrc==ARC_COM_NULL)
		return ARC_COM_ERROR;

	*byDst++ = *bySrc++;
	*byDst++ = *bySrc++;
	*byDst++ = *bySrc++;
	*byDst   = *bySrc;

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Get the subnetwork address by an IP address and its subnetwork mask
 *
 * Parameters
 *		byIp:		the IP address
 *		bySubmask:	the subnet mask
 *		pSubAddr:	pointer to buffer for returning subnetwork address
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byIp or bySubmask or pSubAddr are NULL
 *
 * Note
 *		* IP address and subnet mask are composed of 4 bytes unsigned character
 *
 ******************************************************************************/
long utilIpSubnetAddr( unsigned char* byIp, unsigned char* bySubmask, unsigned char* pSubAddr )
{
	if (byIp==ARC_COM_NULL || bySubmask==ARC_COM_NULL || pSubAddr==ARC_COM_NULL)
		return ARC_COM_ERROR;

	pSubAddr[0] = byIp[0] & bySubmask[0];
	pSubAddr[1] = byIp[1] & bySubmask[1];
	pSubAddr[2] = byIp[2] & bySubmask[2];
	pSubAddr[3] = byIp[3] & bySubmask[3];

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Get the subnetwork broadcast address by an IP address and its subnetwork mask
 *
 * Parameters
 *		byIp:		the IP address
 *		bySubmask:	the subnet mask
 *		pSubBcAddr:	pointer to buffer for returning subnetwork broadcast address
 *
 * Returns
 *		* ARC_COM_OK(0):		successfully
 *		* ARC_COM_ERROR(-1):	byIp or bySubmask or pSubBcAddr are NULL
 *
 * Note
 *		* IP address and subnet mask are composed of 4 bytes unsigned character
 *
 ******************************************************************************/
long utilIpSubnetBcAddr( unsigned char* byIp, unsigned char* bySubmask, unsigned char* pSubBcAddr)
{
	unsigned char	bySubAddr[4];
	long			lRet;

	if (pSubBcAddr == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if ( (lRet=utilIpSubnetAddr( byIp, bySubmask, bySubAddr )) != ARC_COM_OK)
		return lRet;

	pSubBcAddr[0] = bySubAddr[0] | ~bySubmask[0];
	pSubBcAddr[1] = bySubAddr[1] | ~bySubmask[1];
	pSubBcAddr[2] = bySubAddr[2] | ~bySubmask[2];
	pSubBcAddr[3] = bySubAddr[3] | ~bySubmask[3];

	return ARC_COM_OK;
}

/*******************************************************************************
 * Description
 *		Check if two subnetworks have the same subnetwork address
 *
 * Parameters
 *		byIp1:		the first IP address
 *		bySubmask1:	the first subnet mask
 *		byIp2:		the second IP address
 *		bySubmask2:	the second subnet mask
 *
 * Returns
 *		* ARC_COM_FALSE(0):		not the same subnet address
 *		* ARC_COM_TRUE(1):		be the same subnet address
 *		* ARC_COM_ERROR(-1):	byIp1 or bySubmask1 or byIp2 or bySubmask2 are NULL
 *
 * Note
 *		* IP address and subnet mask are composed of 4 bytes unsigned character
 *
 ******************************************************************************/
long utilIpIsSameSubnetAddr( unsigned char* byIp1, unsigned char* bySubmask1, unsigned char* byIp2, unsigned char* bySubmask2 )
{
	unsigned char	bySubAddr1[4];
	unsigned char	bySubAddr2[4];
	unsigned char	bySubBc1[4];
	unsigned char	bySubBc2[4];

	if (byIp1==ARC_COM_NULL || bySubmask1==ARC_COM_NULL || byIp2==ARC_COM_NULL || bySubmask2==ARC_COM_NULL)
		return ARC_COM_ERROR;

	if (utilIpCmp( bySubmask1, bySubmask2 ) != 0)
		return ARC_COM_FALSE;

	utilIpSubnetAddr( byIp1, bySubmask1, bySubAddr1 );
	utilIpSubnetAddr( byIp2, bySubmask2, bySubAddr2 );
	utilIpSubnetBcAddr( byIp1, bySubmask1, bySubBc1 );
	utilIpSubnetBcAddr( byIp2, bySubmask2, bySubBc2 );

	if ( utilIpCmp( bySubAddr1, bySubAddr2) == 0
	 &&  utilIpCmp( bySubBc1, bySubBc2) == 0 )
	{
		return ARC_COM_TRUE;
	}

	return ARC_COM_FALSE;
}

/*******************************************************************************
 * Description
 *		Check if two IP addresses are in the same subnet
 *
 * Parameters
 *		byIp1:		the first IP address
 *		byIp2:		the second IP address
 *		bySubmask:	the subnet mask
 *
 * Returns
 *		* ARC_COM_FALSE(0):		not the same subnet
 *		* ARC_COM_TRUE(1):		be the same subnet
 *		* ARC_COM_ERROR(-1):	byIp1 or byIp2 or bySubmask are null
 *
 * Note
 *		* IP address and subnet mask are composed of 4 bytes unsigned character
 *
 ******************************************************************************/
long utilIpInSameSubnet( unsigned char* byIp1, unsigned char* byIp2, unsigned char* bySubmask )
{
	return utilIpIsSameSubnetAddr( byIp1, bySubmask, byIp2, bySubmask );
}

/*******************************************************************************
 * Description
 *		Parse argument string to an array of arguments
 *
 * Parameters
 *		sArgStr:		string of arguments
 *		sDelimiters:	the delimiters of argument string. It can be NULL (default ' ').
 *		iArgc:			count of pArgv
 *		pArgv:			pointer to an array to be filled with pointers of parsed arguments. It can be NULL.
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* others:				number of all arguments
 *
 * Note
 *		* sArgStr will be divided into multiple parts of arguments
 *
 ******************************************************************************/
int utilParseArgs (char* sArgStr, char* sDelimiters, int iArgc, char** pArgv)
{
	int		iCnt;
	char*	pPtr;
	char*	pLast;

	if (sArgStr == ARC_COM_NULL)
		return ARC_COM_ERROR;

	if (pArgv == ARC_COM_NULL)
		iArgc = 0;

	if (iArgc > 0)
		osBZero( pArgv, sizeof(pArgv[0])*iArgc );

	for (iCnt=0, pPtr=pLast=sArgStr; pPtr ;iCnt++)
	{
		if (sDelimiters)
				pPtr = osStrTok_r( pLast, sDelimiters, &pLast );
		  else	pPtr = osStrTok_r( pLast, " ", &pLast );
		if (iCnt < iArgc)
			pArgv[iCnt] = pPtr;
	}

	return iCnt - 1;
}

/*******************************************************************************
 * Description
 *		get lock/unlock by file name
 *		open file when filename is not NULL, and close file when unlock
 *
 * Parameters
 *		filename:		string of arguments
 *		fd:				the file id
 *		lock_type:		lock or unlock
 *
 * Returns
 *		* ARC_COM_ERROR(-1):	failed
 *		* others:				fd
 *
 * Note
 *		* sArgStr will be divided into multiple parts of arguments
 *
 ******************************************************************************/
int utilGetlockByName(char* filename, int *fd, int type)
{
	int ret = ARC_COM_OK;
	int trial;
	char tmpBuf[255];

	if(type == UTIL_FILE_LCK){
//		printf("--------->[%s]%d start\n", __FUNCTION__, __LINE__);
		osSystem_GetOutput("lock /var/lock/iptables.lock", tmpBuf, sizeof(tmpBuf));

	}else{
//		printf("--------->[%s]%d end\n", __FUNCTION__, __LINE__);
		osSystem_GetOutput("lock -u /var/lock/iptables.lock", tmpBuf, sizeof(tmpBuf));
	}

    return ret;
}
