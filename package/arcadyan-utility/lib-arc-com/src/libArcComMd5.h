
/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
   rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

//#include <sys/types.h>

/* MD5 context. */

//typedef	DWORD		UINT4;
//typedef	WORD		UINT2;
//typedef	BYTE*		POINTER;

typedef struct {
  unsigned int		state[4];	/* state (ABCD) */
  unsigned short	count[2];	/* number of bits, modulo 2^64 (lsb first) */
  unsigned char		buffer[64];	/* input buffer */
} st_MD5_CTX;


void md5Init( st_MD5_CTX* );
void md5Update( st_MD5_CTX *, unsigned char*, unsigned int );
void md5Final( unsigned char [16], st_MD5_CTX* );

