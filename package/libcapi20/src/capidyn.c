/*
 * $Id: capidyn.c,v 1.4 2004/01/16 15:27:11 calle Exp $
 *
 * $Log: capidyn.c,v $
 * Revision 1.4  2004/01/16 15:27:11  calle
 * remove several warnings.
 *
 * Revision 1.3  2004/01/16 14:57:22  calle
 * gcc warning removed.
 *
 * Revision 1.2  2001/03/01 14:59:11  paul
 * Various patches to fix errors when using the newest glibc,
 * replaced use of insecure tempnam() function
 * and to remove warnings etc.
 *
 * Revision 1.1  2000/05/18 15:21:37  calle
 * static CAPI2.0 library that loads libcapi20.so dynamicly.
 *
 */

#include <sys/types.h>
#define _LINUX_LIST_H
#include <linux/capi.h>
#include <string.h>
#include "capi20.h"
#include <dlfcn.h>


#define LIBCAPI	"libcapi20.so"
static int loadlib(void);

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi20_isinstalled)(void);

unsigned capi20_isinstalled (void)
{
	if (loadlib() < 0)
		return CapiRegNotInstalled;
	return fptr_capi20_isinstalled();
}

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi20_register)(unsigned, unsigned, unsigned, unsigned *);

unsigned capi20_register (unsigned MaxB3Connection,
				 unsigned MaxB3Blks,
				 unsigned MaxSizeB3,
				 unsigned *ApplID)
{
	if (loadlib() < 0)
		return CapiRegNotInstalled;
	return fptr_capi20_register(MaxB3Connection,
					 MaxB3Blks, MaxSizeB3, ApplID);
}

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi20_release)(unsigned);

unsigned
capi20_release (unsigned ApplID)
{
	if (loadlib() < 0)
		return CapiRegNotInstalled;
	return fptr_capi20_release(ApplID);
}

/* ---------------------------------------------------------------------- */
static unsigned (*fptr_capi20_put_message)(unsigned, unsigned char *);

unsigned
capi20_put_message (unsigned ApplID, unsigned char *Msg)
{
	if (loadlib() < 0)
		return CapiRegNotInstalled;
	return fptr_capi20_put_message(ApplID, Msg);
}

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi20_get_message)(unsigned, unsigned char **);

unsigned
capi20_get_message (unsigned ApplID, unsigned char **Buf)
{
	if (loadlib() < 0)
		return CapiRegNotInstalled;
	return fptr_capi20_get_message(ApplID, Buf);
}

/* ---------------------------------------------------------------------- */
static unsigned char *
(*fptr_capi20_get_manufacturer)(unsigned, unsigned char *);

unsigned char *
capi20_get_manufacturer(unsigned Ctrl, unsigned char *Buf)
{
	if (loadlib() < 0)
		return "";
	return fptr_capi20_get_manufacturer(Ctrl, Buf);
}

/* ---------------------------------------------------------------------- */
static unsigned char *
(*fptr_capi20_get_version)(unsigned, unsigned char *);

unsigned char *
capi20_get_version(unsigned Ctrl, unsigned char *Buf)
{
	if (loadlib() < 0)
		return "";
	return fptr_capi20_get_version(Ctrl, Buf);
}

/* ---------------------------------------------------------------------- */
static unsigned char *
(*fptr_capi20_get_serial_number)(unsigned, unsigned char *);

unsigned char *
capi20_get_serial_number(unsigned Ctrl, unsigned char *Buf)
{
	if (loadlib() < 0)
		return "";
	return fptr_capi20_get_serial_number(Ctrl, Buf);
}

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi20_get_profile)(unsigned, unsigned char *);

unsigned
capi20_get_profile(unsigned Ctrl, unsigned char *Buf)
{
	if (loadlib() < 0)
		return CapiRegNotInstalled;
	return fptr_capi20_get_profile(Ctrl, Buf);
}

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi20_waitformessage)(unsigned, struct timeval *);

unsigned
capi20_waitformessage(unsigned ApplID, struct timeval *TimeOut)
{
	if (loadlib() < 0)
		return CapiRegNotInstalled;
	return fptr_capi20_waitformessage(ApplID, TimeOut);
}

/* ---------------------------------------------------------------------- */
static int
(*fptr_capi20_fileno)(unsigned);

int
capi20_fileno(unsigned ApplID)
{
	if (loadlib() < 0)
		return -1;
	return fptr_capi20_fileno(ApplID);
}

/* ---------------------------------------------------------------------- */
static int
(*fptr_capi20ext_get_flags)(unsigned, unsigned *);

int
capi20ext_get_flags(unsigned ApplID, unsigned *flagsptr)
{
	if (loadlib() < 0)
		return -1;
	return fptr_capi20ext_get_flags(ApplID, flagsptr);
}

/* ---------------------------------------------------------------------- */
static int
(*fptr_capi20ext_set_flags)(unsigned, unsigned);

int
capi20ext_set_flags(unsigned ApplID, unsigned flags)
{
	if (loadlib() < 0)
		return -1;
	return fptr_capi20ext_set_flags(ApplID, flags);
}

/* ---------------------------------------------------------------------- */
static int
(*fptr_capi20ext_clr_flags)(unsigned, unsigned);

int
capi20ext_clr_flags(unsigned ApplID, unsigned flags)
{
	if (loadlib() < 0)
		return -1;
	return fptr_capi20ext_clr_flags(ApplID, flags);
}
/* ---------------------------------------------------------------------- */
static char *
(*fptr_capi20ext_get_tty_devname)(unsigned, unsigned, char *, size_t);

char *
capi20ext_get_tty_devname(unsigned applid, unsigned ncci, char *buf, size_t size)
{
	if (loadlib() < 0)
		return 0;
	return fptr_capi20ext_get_tty_devname(applid, ncci, buf, size);
}

/* ---------------------------------------------------------------------- */
static char *
(*fptr_capi20ext_get_raw_devname)(unsigned, unsigned, char *, size_t);

char *
capi20ext_get_raw_devname(unsigned applid, unsigned ncci, char *buf, size_t size)
{
	if (loadlib() < 0)
		return 0;
	return fptr_capi20ext_get_raw_devname(applid, ncci, buf, size);
}

/* ---------------------------------------------------------------------- */
static int
(*fptr_capi20ext_ncci_opencount)(unsigned, unsigned);

int
capi20ext_ncci_opencount(unsigned applid, unsigned ncci)
{
	if (loadlib() < 0)
		return 0;
	return fptr_capi20ext_ncci_opencount(applid, ncci);
}

/* ---------------------------------------------------------------------- */
static char *
(*fptr_capi_info2str)(_cword);

char *
capi_info2str(_cword reason)
{
	if (loadlib() < 0)
		return 0;
	return fptr_capi_info2str(reason);
}

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi_cmsg2message)(_cmsg *, _cbyte *);

unsigned
capi_cmsg2message(_cmsg * cmsg, _cbyte * msg)
{
	if (loadlib() < 0)
		return 0;
	return fptr_capi_cmsg2message(cmsg, msg);
}

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi_message2cmsg)(_cmsg *, _cbyte *);

unsigned
capi_message2cmsg(_cmsg * cmsg, _cbyte * msg)
{
	if (loadlib() < 0)
		return 0;
	return fptr_capi_message2cmsg(cmsg, msg);
}

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi_cmsg_header)(_cmsg *, unsigned, _cbyte, _cbyte, _cword, _cdword);

unsigned
capi_cmsg_header (_cmsg *cmsg, unsigned _ApplId,
				 _cbyte _Command, _cbyte _Subcommand,
				 _cword _Messagenumber, _cdword _Controller)
{
	if (loadlib() < 0)
		return 0;
	return fptr_capi_cmsg_header(cmsg, _ApplId,
				 _Command, _Subcommand,
				 _Messagenumber, _Controller);
}

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi_put_cmsg)(_cmsg *);

unsigned
capi_put_cmsg (_cmsg *cmsg)
{
	if (loadlib() < 0)
		return CapiRegNotInstalled;
	return fptr_capi_put_cmsg(cmsg);
}

/* ---------------------------------------------------------------------- */
static unsigned
(*fptr_capi_get_cmsg)(_cmsg *, unsigned);

unsigned
capi_get_cmsg (_cmsg *cmsg, unsigned applid)
{
	if (loadlib() < 0)
		return CapiRegNotInstalled;
	return fptr_capi_get_cmsg(cmsg, applid);
}

/* ---------------------------------------------------------------------- */
static char *
(*fptr_capi_cmd2str)(_cbyte, _cbyte);

char *
capi_cmd2str(_cbyte cmd, _cbyte subcmd)
{
	if (loadlib() < 0)
		return "";
	return fptr_capi_cmd2str(cmd, subcmd);
}

/* ---------------------------------------------------------------------- */
static char *
(*fptr_capi_message2str)(_cbyte *);

char *
capi_message2str(_cbyte * msg)
{
	if (loadlib() < 0)
		return "";
	return fptr_capi_message2str(msg);
}

/* ---------------------------------------------------------------------- */
static char *
(*fptr_capi_cmsg2str)(_cmsg *);

char *
capi_cmsg2str(_cmsg * cmsg)
{
	if (loadlib() < 0)
		return "";
	return fptr_capi_cmsg2str(cmsg);
}

/* ---------------------------------------------------------------------- */

static void *handle;

#define	resolv_sym(x)	\
    if ((fptr_##x = dlsym(handle, #x)) == 0) { \
	write(2, "Can't resolv " #x, sizeof("Can't resolv " #x)-1); \
	dlclose(handle); \
	handle = 0; \
	return -1; \
    } else

static char emsg[] = "Couldn't load shared library ";

static int loadlib(void)
{
	const char *err;
	if (handle)
		return 0;
	handle = dlopen(LIBCAPI, RTLD_GLOBAL | RTLD_NOW);
	if (handle == 0) {
		err = dlerror();
		write(2, emsg, sizeof(emsg)-1);
		write(2, LIBCAPI, sizeof(LIBCAPI)-1);
		write(2, "\n", 1);
		if (err) {
			write(2, err, strlen(err));
			write(2, "\n", 1);
		}
		return -1;
	}
	resolv_sym(capi20_isinstalled);
	resolv_sym(capi20_register);
	resolv_sym(capi20_release);
	resolv_sym(capi20_put_message);
	resolv_sym(capi20_get_message);
	resolv_sym(capi20_get_manufacturer);
	resolv_sym(capi20_get_version);
	resolv_sym(capi20_get_serial_number);
	resolv_sym(capi20_get_profile);
	resolv_sym(capi20_waitformessage);
	resolv_sym(capi20_fileno);
	resolv_sym(capi20ext_get_flags);
	resolv_sym(capi20ext_set_flags);
	resolv_sym(capi20ext_clr_flags);
	resolv_sym(capi20ext_get_tty_devname);
	resolv_sym(capi20ext_get_raw_devname);
	resolv_sym(capi20ext_ncci_opencount);
	resolv_sym(capi_info2str);
	resolv_sym(capi_cmsg2message);
	resolv_sym(capi_message2cmsg);
	resolv_sym(capi_cmsg_header);
	resolv_sym(capi_put_cmsg);
	resolv_sym(capi_get_cmsg);
	resolv_sym(capi_cmd2str);
	resolv_sym(capi_message2str);
	resolv_sym(capi_cmsg2str);
	return 0;
}

#undef	resolv_sym
