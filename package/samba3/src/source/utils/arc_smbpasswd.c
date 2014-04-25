/*
 * merge of avm_smbpasswd.c and owrt_smbpasswd.c
 *
 * Copyright (C) John Crispin <blogic@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675
 * Mass Ave, Cambridge, MA 02139, USA.  */

#include "includes.h"
#include <endian.h>


void E_md4hash(const char *passwd, uchar p16[16])
{
	int len;
	uchar wpwd[129];
	int i, j;

	/* Password must be converted to NT unicode - null terminated. */
	len = strlen(passwd);
	for (i=0, j=0; passwd[i]!='\0'; i++) {
		wpwd[j++] = passwd[i];
		wpwd[j++] = '\0';
	}
	wpwd[j++] = '\0';
	wpwd[j++] = '\0';
	/* Calculate length in bytes */
	len = i * 2;

	mdfour(p16, wpwd, len);
}

/**
 * Creates the DES forward-only Hash of the users password in DOS ASCII charset
 * @param passwd password in 'unix' charset.
 * @param p16 return password hashed with DES, caller allocated 16 byte buffer
 * @return False if password was > 14 characters, and therefore may be incorrect, otherwise True
 * @note p16 is filled in regardless
 */
 
BOOL E_deshash(const char *passwd, uchar p16[16])
{
	BOOL ret = True;
	char dospwd[256+2];
	int i;
	int len;
	
	/* Password must be converted to DOS charset - null terminated, uppercase. */
//	push_ascii(dospwd, passwd, sizeof(dospwd), STR_UPPER|STR_TERMINATE);
	len = strlen(passwd);
	for (i = 0; i < len; i++) {
		char c = passwd[i];
		if (islower(c)) c = toupper(c);
		dospwd[i] = c;
	}
	dospwd[i] = 0;
       
	/* Only the fisrt 14 chars are considered, password need not be null terminated. */
	E_P16((const unsigned char *)dospwd, p16);

	if (strlen(dospwd) > 14) {
		ret = False;
	}

	memset(dospwd, 0, sizeof(dospwd));
	// ZERO_STRUCT(dospwd);	

	return ret;
}

static void my_pdb_sethexpwd(char *p, const unsigned char *pwd)
{
	if (pwd != NULL) {
		int i;
		for (i = 0; i < 16; i++)
			slprintf(&p[i*2], 3, "%02X", pwd[i]);
	} else {
		strncpy(p, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", 32);
	}
}

static void crypt_password (const char *user_name,
			   const char *new_passwd, char *new_lanman_p16, char *new_nt_p16)
{
	/* Calculate the MD4 hash (NT compatible) of the password */
	E_md4hash(new_passwd, new_nt_p16);

	if (!E_deshash(new_passwd, new_lanman_p16)) {
		/* E_deshash returns false for 'long' passwords (> 14
		   DOS chars).  This allows us to match Win2k, which
		   does not store a LM hash for these passwords (which
		   would reduce the effective password length to 14 */

		memset(new_lanman_p16, 0, LM_HASH_LEN);
	}
}

/* returns -1 if user is not present in /etc/passwd*/
int find_uid_for_user(char *user)
{
	char t[256];
	FILE *fp = fopen("/etc/passwd", "r");
	int ret = -1;

	if(!fp)
	{
		printf("failed to open /etc/passwd");
		goto out;
	}

	while(!feof(fp))
	{
		if(fgets(t, 255, fp))
		{
			char *p1, *p2;
			p1 = strchr(t, ':');
			if(p1 && (p1 - t == strlen(user)) && (strncmp(t, user, strlen(user))) == 0)
			{
				p1 = strchr(t, ':');
				if(!p1)
					goto out;
				p2 = strchr(++p1, ':');
				if(!p2)
					goto out;
				p1 = strchr(++p2, ':');
				if(!p1)
					goto out;
				*p1 = '\0';
				ret = atoi(p2);
				goto out;
			}
		}
	}
	printf("No valid user found in /etc/passwd\n");

out:
	if(fp)
		fclose(fp);
	return ret;
}

void insert_user_in_smbpasswd(char *user, char *line)
{
	char t[256];
	FILE *fp = fopen("/etc/samba/smbpasswd", "r+");

	if(!fp)
	{
		printf("failed to open /etc/samba/smbpasswd");
		goto out;
	}

	while(!feof(fp))
	{
		if(fgets(t, 255, fp))
		{
			char *p;
			p = strchr(t, ':');
			if(p && (p - t == strlen(user)) && (strncmp(t, user, strlen(user))) == 0)
			{
				fseek(fp, -strlen(line), SEEK_CUR);
				break;
			}
		}
	}

	fprintf(fp, line);

out:
	if(fp)
		fclose(fp);
}

void delete_user_from_smbpasswd(char *user)
{
	char t[256];
	FILE *fp = fopen("/etc/samba/smbpasswd", "r+");

	if(!fp)
	{
		printf("failed to open /etc/samba/smbpasswd");
		goto out;
	}

	while(!feof(fp))
	{
		if(fgets(t, 255, fp))
		{
			char *p;
			p = strchr(t, ':');
			if(p && (p - t == strlen(user)) && (strncmp(t, user, strlen(user))) == 0)
			{
				fpos_t r_pos, w_pos;
				char t2[256];
				fgetpos(fp, &r_pos);
				w_pos = r_pos;
				w_pos.__pos -= strlen(t);
				while(fgets(t2, 256, fp))
				{
					fsetpos(fp, &w_pos);
					fputs(t2, fp);
					r_pos.__pos += strlen(t2);
					w_pos.__pos += strlen(t2);
					fsetpos(fp, &r_pos);
				}
				ftruncate(fileno(fp), w_pos.__pos);
				break;
			}
		}
	}

out:
	if(fp)
		fclose(fp);
}

int read_line( FILE* fp, uchar* sBuf, int iMax )
{
	char*	pPtr;
	char	sTmp[ 512 ];
	int		iPos;

	if (fp == 0 || sBuf == 0)
		return -1;

	if (fgets( sTmp, sizeof(sTmp), fp ) == 0)
		return -1;

	iPos = 0;
	pPtr = strtok(sTmp,"\r\n\t ");
	while (pPtr != 0 && iPos < iMax)
	{
		sBuf[iPos++] = (uchar)strtoul(pPtr,NULL,16);
		pPtr=strtok(0,"\r\n ");
	}
	if (iPos < iMax)
		sBuf[iPos] = '\0';

	return iPos;
}

void print_line( uchar* sBuf, int iSize)
{
	int		iCnt;

	if (sBuf == 0)
		return;

	for (iCnt=0; iCnt<iSize; iCnt++)
	{
		printf( "%02X ", sBuf[iCnt]);
	}
	printf( "\n" );
}

void test_HmacMd5( char* sFile )
{
	HMACMD5Context	ctx;
	int				uidLen;
	uchar			uid[64];
	int				pwdLen;
	uchar			pwd[64];
	int				domainLen;
	uchar			domain[64];
	int				svrChalLen;
	uchar			svrChal[8];
	int				clnChalLen;
	uchar			clnChal[256];
	uchar			resp_buf[16];
	FILE*			fp;

	if (sFile == 0)
		return;

	if (strcmp(sFile,"help") == 0) {
		printf("file format:\n");
		printf("\t0. total five lines in hex-string format\n");
		printf("\t1. 1st line: username - do nothing in program\n");
		printf("\t2. 2nd line: password - will do unicode conversion in program\n");
		printf("\t3. 3rd line: domain   - do nothing in program\n");
		printf("\t4. 4th line: server challenge - do nothing in program\n");
		printf("\t5. 5th line: client challenge - do nothing in program\n");
		printf("example:\n");
		printf("\tusername: TEST\n");
		printf("\tpassword: SecREt01\n");
		printf("\tdomain:   DOMAIN\n");
		printf("\tserver challenge: 01 23 45 67 89 ab cd ef\n");
		printf("\tclient challenge: xx xx xx ...\n");
		printf("example file content:\n");
		printf("\t55 00 53 00 45 00 52 00\n");
		printf("\t53 65 63 52 45 74 30 31\n");
		printf("\t44 00 4f 00 4d 00 41 00 49 00 4e 00\n");
		printf("\t01 23 45 67 89 ab cd ef\n");
		printf("\t01 01 00 00 00 00 00 ...\n");
		return;
	}

	fp = fopen(sFile, "r");
	if (!fp)
		return;

	uidLen = read_line( fp, uid, sizeof(uid) );
	pwdLen = read_line( fp, pwd, sizeof(pwd) );
	domainLen = read_line( fp, domain, sizeof(domain) );
	svrChalLen = read_line( fp, svrChal, sizeof(svrChal) );
	clnChalLen = read_line( fp, clnChal, sizeof(clnChal) );

	fclose( fp );

	if (clnChalLen <= 0)
		return;

	E_md4hash( pwd, resp_buf );
	printf("MD4 is:\n");
	print_line( resp_buf, sizeof(resp_buf) );

	hmac_md5_init_limK_to_64(resp_buf, sizeof(resp_buf), &ctx);
	hmac_md5_update(uid, uidLen, &ctx);
	hmac_md5_update(domain, domainLen, &ctx);
	hmac_md5_final(resp_buf, &ctx);
	printf("HMAC_MD5_1 is:\n");
	print_line( resp_buf, sizeof(resp_buf) );

	hmac_md5_init_limK_to_64(resp_buf, sizeof(resp_buf), &ctx);
	hmac_md5_update(svrChal, svrChalLen, &ctx);
	hmac_md5_update(clnChal, clnChalLen, &ctx);
	hmac_md5_final(resp_buf, &ctx);
	printf("HMAC_MD5_2 is:\n");
	print_line( resp_buf, sizeof(resp_buf) );
}

int main(int argc, char **argv)
{
	unsigned uid;
	uchar new_lanman_p16[LM_HASH_LEN];
	uchar new_nt_p16[NT_HASH_LEN];
	int g;
	int smbpasswd_present;
	char smbpasswd_line[256];
	char *s;

	if(argc != 3)
	{
		printf( "usage for openwrt_smbpasswd - \n"
				"\t%s USERNAME PASSWD\n"
				"\t%s -del USERNAME\n"
				"\t%s -test TESTFILE\n"
				"\t%s -test help\n"
			, argv[0], argv[0], argv[0]);
		//printf("usage for openwrt_smbpasswd - \n\t%s USERNAME PASSWD\n\t%s -del USERNAME\n", argv[0], argv[0]);
		exit(1);
	}

	if(strcmp(argv[1], "-del") == 0)
	{
		printf("deleting user %s\n", argv[2]);
		delete_user_from_smbpasswd(argv[2]);
		return 0;
	}

	if(strcmp(argv[1], "-test") == 0)
	{
		test_HmacMd5( argv[2] );
		return 0;
	}

	uid = find_uid_for_user(argv[1]);
	if(uid == -1)
		exit(2);

	crypt_password(argv[1], argv[2], new_lanman_p16, new_nt_p16);

	s = smbpasswd_line;
	s += snprintf(s, sizeof(smbpasswd_line) - (s - smbpasswd_line), "%s:%u:", argv[1], uid);
	for(g = 0; g < LM_HASH_LEN; g++)
		s += snprintf(s, sizeof(smbpasswd_line) - (s - smbpasswd_line), "%02X", new_lanman_p16[g]);
	s += snprintf(s, sizeof(smbpasswd_line) - (s - smbpasswd_line), ":");
	for(g = 0; g < NT_HASH_LEN; g++)
		s += snprintf(s, sizeof(smbpasswd_line) - (s - smbpasswd_line), "%02X", new_nt_p16[g]);
	snprintf(s, sizeof(smbpasswd_line) - (s - smbpasswd_line), ":[U          ]:LCT-00000001:\n");

	insert_user_in_smbpasswd(argv[1], smbpasswd_line);

	return 0;
}
