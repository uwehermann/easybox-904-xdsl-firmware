/*
 * $Id: capi20.c,v 1.28 2006/08/08 13:23:29 keil Exp $
 *
 * $Log: capi20.c,v $
 * Revision 1.28  2006/08/08 13:23:29  keil
 * some endian fixes for BIGENDIAN systems
 *
 * Revision 1.27  2005/05/09 08:21:57  calle
 * - get_buffer() now returns 0, if no buffer is available.
 *
 * Revision 1.26  2005/03/04 11:00:31  calle
 * New functions: cleanup_buffers_for_ncci() and cleanup_buffers_for_plci()
 * triggered by DISCONNECT_B3_RESP and DISCONNECT_IND to fix buffer leak.
 *
 * Revision 1.25  2005/02/21 17:37:06  keil
 * libcapi20 version 3.0.0
 *  - add SENDING COMPLETE in ALERT_REQ
 *  - add Globalconfiguration to CONNECT_REQ/RESP and SELECT_B_PROTOCOL_REQ
 *
 * * NOTE: incompatible to 2.X.Y versions
 *
 * Revision 1.24  2004/12/15 14:27:54  calle
 * Bugfix: returncode of get_buffer() is now checked.
 *
 * Revision 1.23  2004/10/06 15:24:42  calle
 * - "SendingComplete"-Patch reverted => 2.0.8 was not binaer compartible
 * - Bugfix: capi20_register() with MaxB3Connection == 0 results in a
 *   core dump. Now at least one buffer is allocated.
 *
 * Revision 1.22  2004/06/14 11:23:48  calle
 * Erweiterungen fuer ALERT_REQ.
 *
 * Revision 1.21  2004/03/31 18:12:40  calle
 * - add receive buffer managment according to CAPI2.0 spec.
 * - send buffer is now on stack.
 * - new library version 2.0.7
 *
 * Revision 1.20  2004/01/16 15:27:11  calle
 * remove several warnings.
 *
 * Revision 1.19  2001/03/01 14:59:11  paul
 * Various patches to fix errors when using the newest glibc,
 * replaced use of insecure tempnam() function
 * and to remove warnings etc.
 *
 * Revision 1.18  2000/11/12 16:06:41  kai
 * fix backwards compatibility in capi20 library, small other changes
 *
 * Revision 1.17  2000/06/26 15:00:43  calle
 * - Will also compile with 2.0 Kernelheaders.
 *
 * Revision 1.16  2000/05/18 15:02:26  calle
 * Updated _cmsg handling added new functions need by "capiconn".
 *
 * Revision 1.15  2000/04/10 09:08:06  calle
 * capi20_wait_for_message will now return CapiReceiveQueueEmpty on
 * timeout and error.
 *
 * Revision 1.14  2000/04/07 16:06:09  calle
 * Bugfix: without devfs open where without NONBLOCK, ahhh.
 *
 * Revision 1.13  2000/04/03 14:27:15  calle
 * non CAPI2.0 standard functions now named capi20ext not capi20.
 * Extentionfunctions will work with actual driver version.
 *
 * Revision 1.12  2000/03/03 15:56:14  calle
 * - now uses cloning device /dev/capi20.
 * - middleware extentions prepared.
 *
 * Revision 1.11  1999/12/22 17:46:21  calle
 * - Last byte in serial number now always 0.
 * - Last byte of manufacturer now always 0.
 * - returncode in capi20_isinstalled corrected.
 *
 * Revision 1.10  1999/11/11 09:24:07  calle
 * add shared lib destructor, to close "capi_fd" on unload with dlclose ..
 *
 * Revision 1.9  1999/10/20 16:43:17  calle
 * - The CAPI20 library is now a shared library.
 * - Arguments of function capi20_put_message swapped, to match capi spec.
 * - All capi20 related subdirs converted to use automake.
 * - Removed dependency to CONFIG_KERNELDIR where not needed.
 *
 * Revision 1.8  1999/09/15 08:10:44  calle
 * Bugfix: error in 64Bit extention.
 *
 * Revision 1.7  1999/09/10 17:20:33  calle
 * Last changes for proposed standards (CAPI 2.0):
 * - AK1-148 "Linux Extention"
 * - AK1-155 "Support of 64-bit Applications"
 *
 * Revision 1.6  1999/09/06 17:40:07  calle
 * Changes for CAPI 2.0 Spec.
 *
 * Revision 1.5  1999/04/20 19:52:19  calle
 * Bugfix in capi20_get_profile: wrong size in memcpy from
 * Kai Germaschewski <kai@thphy.uni-duesseldorf.de>
 *
 * Revision 1.4  1998/11/18 17:05:44  paul
 * fixed a (harmless) warning
 *
 * Revision 1.3  1998/08/30 09:57:14  calle
 * I hope it is know readable for everybody.
 *
 * Revision 1.1  1998/08/25 16:33:16  calle
 * Added CAPI2.0 library. First Version.
 *
 */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#define _LINUX_LIST_H
#include <linux/capi.h>
#include "capi20.h"

#ifndef CAPI_GET_FLAGS
#define CAPI_GET_FLAGS		_IOR('C',0x23, unsigned)
#endif
#ifndef CAPI_SET_FLAGS
#define CAPI_SET_FLAGS		_IOR('C',0x24, unsigned)
#endif
#ifndef CAPI_CLR_FLAGS
#define CAPI_CLR_FLAGS		_IOR('C',0x25, unsigned)
#endif
#ifndef CAPI_NCCI_OPENCOUNT
#define CAPI_NCCI_OPENCOUNT	_IOR('C',0x26, unsigned)
#endif
#ifndef CAPI_NCCI_GETUNIT
#define CAPI_NCCI_GETUNIT	_IOR('C',0x27, unsigned)
#endif

#define SEND_BUFSIZ		(128+2048)

static char capidevname[] = "/dev/capi20";
static char capidevnamenew[] = "/dev/isdn/capi20";

static int                  capi_fd = -1;
static capi_ioctl_struct    ioctl_data;

unsigned capi20_isinstalled (void)
{
    if (capi_fd >= 0)
        return CapiNoError;

    /*----- open managment link -----*/
    if ((capi_fd = open(capidevname, O_RDWR, 0666)) < 0 && errno == ENOENT)
       capi_fd = open(capidevnamenew, O_RDWR, 0666);
    if (capi_fd < 0)
       return CapiRegNotInstalled;

    if (ioctl(capi_fd, CAPI_INSTALLED, 0) == 0)
	return CapiNoError;
    return CapiRegNotInstalled;
}

/*
 * managment of application ids
 */

#define MAX_APPL 1024

static int applidmap[MAX_APPL];

static inline int remember_applid(unsigned applid, int fd)
{
   if (applid >= MAX_APPL)
	return -1;
   applidmap[applid] = fd;
   return 0;
}

static inline unsigned alloc_applid(int fd)
{
   unsigned applid;
   for (applid=1; applid < MAX_APPL; applid++) {
       if (applidmap[applid] < 0) {
          applidmap[applid] = fd;
          return applid;
       }
   }
   return 0;
}

static inline void freeapplid(unsigned applid)
{
    if (applid < MAX_APPL)
       applidmap[applid] = -1;
}

static inline int validapplid(unsigned applid)
{
    return applid > 0 && applid < MAX_APPL && applidmap[applid] >= 0;
}

static inline int applid2fd(unsigned applid)
{
    if (applid < MAX_APPL)
	    return applidmap[applid];
    return -1;
}

/*
 * buffer management
 */

struct recvbuffer {
   struct recvbuffer *next;
   unsigned int       datahandle;
   unsigned int       used;
   unsigned int       ncci;
   unsigned char     *buf; /* 128 + MaxSizeB3 */
};

struct applinfo {
   unsigned  maxbufs;
   unsigned  nbufs;
   size_t    recvbuffersize;
   struct recvbuffer *buffers;
   struct recvbuffer *firstfree;
   struct recvbuffer *lastfree;
   unsigned char *bufferstart;
};

static struct applinfo *alloc_buffers(unsigned MaxB3Connection,
		                      unsigned MaxB3Blks,
		                      unsigned MaxSizeB3)
{
   struct applinfo *ap;
   unsigned nbufs = 2 + MaxB3Connection * (MaxB3Blks + 1);
   size_t recvbuffersize = 128 + MaxSizeB3;
   unsigned i;
   size_t size;

   if (recvbuffersize < 2048) recvbuffersize = 2048;

   size = sizeof(struct applinfo);
   size += sizeof(struct recvbuffer) * nbufs;
   size += recvbuffersize * nbufs;

   ap = (struct applinfo *)malloc(size);
   if (ap == 0) return 0;

   memset(ap, 0, size);
   ap->maxbufs = nbufs;
   ap->recvbuffersize = recvbuffersize;
   ap->buffers = (struct recvbuffer *)(ap+1);
   ap->firstfree = ap->buffers;
   ap->bufferstart = (unsigned char *)(ap->buffers+nbufs);
   for (i=0; i < ap->maxbufs; i++) {
      ap->buffers[i].next = &ap->buffers[i+1];
      ap->buffers[i].used = 0;
      ap->buffers[i].ncci = 0;
      ap->buffers[i].buf = ap->bufferstart+(recvbuffersize*i);
   }
   ap->lastfree = &ap->buffers[ap->maxbufs-1];
   ap->lastfree->next = 0;
   return ap;
}

static void free_buffers(struct applinfo *ap)
{
   free(ap);
}

static struct applinfo *applinfo[MAX_APPL];

static unsigned char *get_buffer(unsigned applid, size_t *sizep, unsigned *handle)
{
   struct applinfo *ap;
   struct recvbuffer *buf;

   assert(validapplid(applid));
   ap = applinfo[applid];
   if ((buf = ap->firstfree) == 0)
      return 0;
   ap->firstfree = buf->next;
   buf->next = 0;
   buf->used = 1;
   ap->nbufs++;
   *sizep = ap->recvbuffersize;
   *handle  = (buf->buf-ap->bufferstart)/ap->recvbuffersize;
   return buf->buf;
}

static void save_datahandle(unsigned char applid,
                            unsigned offset,
			    unsigned datahandle,
			    unsigned ncci)
{
   struct applinfo *ap;
   struct recvbuffer *buf;

   assert(validapplid(applid));
   ap = applinfo[applid];
   assert(offset < ap->maxbufs);
   buf = ap->buffers+offset;
   buf->datahandle = datahandle;
   buf->ncci = ncci;
}

static unsigned return_buffer(unsigned char applid, unsigned offset)
{
   struct applinfo *ap;
   struct recvbuffer *buf;

   assert(validapplid(applid));
   ap = applinfo[applid];
   assert(offset < ap->maxbufs);
   buf = ap->buffers+offset;
   assert(buf->used == 1);
   assert(buf->next == 0);
   if (ap->lastfree) {
      ap->lastfree->next = buf;
      ap->lastfree = buf;
   } else {
      ap->firstfree = ap->lastfree = buf;
   }
   buf->used = 0;
   buf->ncci = 0;
   assert(ap->nbufs-- > 0);
   return buf->datahandle;
}

static void cleanup_buffers_for_ncci(unsigned char applid, unsigned ncci)
{
   struct applinfo *ap;
   unsigned i;

   assert(validapplid(applid));
   ap = applinfo[applid];

   for (i=0; i < ap->maxbufs; i++) {
      if (ap->buffers[i].used) {
         assert(ap->buffers[i].ncci != 0);
	 if (ap->buffers[i].ncci == ncci)
            return_buffer(applid, i);
      }
   }
}

static void cleanup_buffers_for_plci(unsigned char applid, unsigned plci)
{
   struct applinfo *ap;
   unsigned i;

   assert(validapplid(applid));
   ap = applinfo[applid];

   for (i=0; i < ap->maxbufs; i++) {
      if (ap->buffers[i].used) {
         assert(ap->buffers[i].ncci != 0);
	 if (ap->buffers[i].ncci & 0xffff == plci) {
            return_buffer(applid, i);
	 }
      }
   }
}

/*
 * CAPI2.0 functions
 */

unsigned
capi20_register (unsigned MaxB3Connection,
		 unsigned MaxB3Blks,
		 unsigned MaxSizeB3,
		 unsigned *ApplID)
{
    int applid = 0;
    char buf[PATH_MAX];
    int i, fd = -1;

    *ApplID = 0;

    if (capi20_isinstalled() != CapiNoError)
       return CapiRegNotInstalled;

    if ((fd = open(capidevname, O_RDWR|O_NONBLOCK, 0666)) < 0 && errno == ENOENT)
         fd = open(capidevnamenew, O_RDWR|O_NONBLOCK, 0666);

    if (fd < 0)
	return CapiRegOSResourceErr;

    ioctl_data.rparams.level3cnt = MaxB3Connection;
    ioctl_data.rparams.datablkcnt = MaxB3Blks;
    ioctl_data.rparams.datablklen = MaxSizeB3;

    if ((applid = ioctl(fd, CAPI_REGISTER, &ioctl_data)) < 0) {
        if (errno == EIO) {
            if (ioctl(fd, CAPI_GET_ERRCODE, &ioctl_data) < 0) {
		close (fd);
                return CapiRegOSResourceErr;
	    }
	    close (fd);
            return (unsigned)ioctl_data.errcode;

        } else if (errno == EINVAL) { // old kernel driver
	    close (fd);
	    fd = -1;
	    for (i=0; fd < 0; i++) {
		/*----- open pseudo-clone device -----*/
		sprintf(buf, "/dev/capi20.%02d", i);
		if ((fd = open(buf, O_RDWR|O_NONBLOCK, 0666)) < 0) {
		    switch (errno) {
		    case EEXIST:
			break;
		    default:
			return CapiRegOSResourceErr;
		    }
		}
	    }
	    if (fd < 0)
		return CapiRegOSResourceErr;

	    ioctl_data.rparams.level3cnt = MaxB3Connection;
	    ioctl_data.rparams.datablkcnt = MaxB3Blks;
	    ioctl_data.rparams.datablklen = MaxSizeB3;

	    if ((applid = ioctl(fd, CAPI_REGISTER, &ioctl_data)) < 0) {
		if (errno == EIO) {
		    if (ioctl(fd, CAPI_GET_ERRCODE, &ioctl_data) < 0) {
			close(fd);
			return CapiRegOSResourceErr;
		    }
		    close(fd);
		    return (unsigned)ioctl_data.errcode;
		}
		close(fd);
		return CapiRegOSResourceErr;
	    }
	    applid = alloc_applid(fd);
	} // end old driver compatibility
    }
    if (remember_applid(applid, fd) < 0) {
       close(fd);
       return CapiRegOSResourceErr;
    }
    applinfo[applid] = alloc_buffers(MaxB3Connection, MaxB3Blks, MaxSizeB3);
    if (applinfo[applid] == 0) {
       close(fd);
       return CapiRegOSResourceErr;
    }
    *ApplID = applid;
    return CapiNoError;
}

unsigned
capi20_release (unsigned ApplID)
{
    if (capi20_isinstalled() != CapiNoError)
       return CapiRegNotInstalled;
    if (!validapplid(ApplID))
        return CapiIllAppNr;
    (void)close(applid2fd(ApplID));
    freeapplid(ApplID);
    free_buffers(applinfo[ApplID]);
    applinfo[ApplID] = 0;
    return CapiNoError;
}

unsigned
capi20_put_message (unsigned ApplID, unsigned char *Msg)
{
    unsigned char sndbuf[SEND_BUFSIZ];
    unsigned ret;
    int len = (Msg[0] | (Msg[1] << 8));
    int cmd = Msg[4];
    int subcmd = Msg[5];
    int rc;
    int fd;

    if (capi20_isinstalled() != CapiNoError)
       return CapiRegNotInstalled;

    if (!validapplid(ApplID))
        return CapiIllAppNr;

    fd = applid2fd(ApplID);

    memcpy(sndbuf, Msg, len);

    if (cmd == CAPI_DATA_B3) {
       if (subcmd == CAPI_REQ) {
          int datalen = (Msg[16] | (Msg[17] << 8));
          void *dataptr;
          if (sizeof(void *) != 4) {
	      if (len >= 30) { /* 64Bit CAPI-extention */
	          _cqword data64;
	          data64 = CAPIMSG_U64(Msg, 22);
	          if (data64 != 0)
	              dataptr = (void *)(unsigned long)data64;
	          else
	              dataptr = Msg + len; /* Assume data after message */
	      } else {
                  dataptr = Msg + len; /* Assume data after message */
	      }
          } else {
              _cdword data;
              data = CAPIMSG_U32(Msg, 12);
              if (data != 0)
		  dataptr = (void *)(unsigned long)data;
              else
                  dataptr = Msg + len; /* Assume data after message */
	  }
	  if (len + datalen > SEND_BUFSIZ)
             return CapiMsgOSResourceErr;
          memcpy(sndbuf+len, dataptr, datalen);
          len += datalen;
      } else if (subcmd == CAPI_RESP) {
          capimsg_setu16(sndbuf, 12,
			 return_buffer(ApplID, CAPIMSG_U16(sndbuf, 12)));
      }
   }

   if (cmd == CAPI_DISCONNECT_B3 && subcmd == CAPI_RESP)
      cleanup_buffers_for_ncci(ApplID, CAPIMSG_U32(sndbuf, 8));

   ret = CapiNoError;
   errno = 0;

    if ((rc = write(fd, sndbuf, len)) != len) {
        switch (errno) {
            case EFAULT:
            case EINVAL:
                ret = CapiIllCmdOrSubcmdOrMsgToSmall;
                break;
            case EBADF:
                ret = CapiIllAppNr;
                break;
            case EIO:
                if (ioctl(fd, CAPI_GET_ERRCODE, &ioctl_data) < 0)
                    ret = CapiMsgOSResourceErr;
                else ret = (unsigned)ioctl_data.errcode;
                break;
          default:
                ret = CapiMsgOSResourceErr;
                break;
       }
    }

    return ret;
}

unsigned
capi20_get_message (unsigned ApplID, unsigned char **Buf)
{
    unsigned char *rcvbuf;
    unsigned offset;
    unsigned ret;
    size_t bufsiz;
    int rc, fd;

    if (capi20_isinstalled() != CapiNoError)
       return CapiRegNotInstalled;

    if (!validapplid(ApplID))
        return CapiIllAppNr;

    fd = applid2fd(ApplID);

    if ((*Buf = rcvbuf = get_buffer(ApplID, &bufsiz, &offset)) == 0)
        return CapiMsgOSResourceErr;

    if ((rc = read(fd, rcvbuf, bufsiz)) > 0) {
	CAPIMSG_SETAPPID(rcvbuf, ApplID); // workaround for old driver
        if (   CAPIMSG_COMMAND(rcvbuf) == CAPI_DATA_B3
	    && CAPIMSG_SUBCOMMAND(rcvbuf) == CAPI_IND) {
           save_datahandle(ApplID, offset, CAPIMSG_U16(rcvbuf, 18),
					   CAPIMSG_U32(rcvbuf, 8));
           capimsg_setu16(rcvbuf, 18, offset); /* patch datahandle */
           if (sizeof(void *) == 4) {
	       u_int32_t data = (u_int32_t)rcvbuf + CAPIMSG_LEN(rcvbuf);
	       rcvbuf[12] = data & 0xff;
	       rcvbuf[13] = (data >> 8) & 0xff;
	       rcvbuf[14] = (data >> 16) & 0xff;
	       rcvbuf[15] = (data >> 24) & 0xff;
           } else {
	       u_int64_t data;
	       ulong radr = (ulong)rcvbuf;
	       if (CAPIMSG_LEN(rcvbuf) < 30) {
		  /*
		   * grr, 64bit arch, but no data64 included,
	           * seems to be old driver
		   */
	          memmove(rcvbuf+30, rcvbuf+CAPIMSG_LEN(rcvbuf),
		          CAPIMSG_DATALEN(rcvbuf));
	          rcvbuf[0] = 30;
	          rcvbuf[1] = 0;
	       }
	       data = radr + CAPIMSG_LEN(rcvbuf);
	       rcvbuf[12] = rcvbuf[13] = rcvbuf[14] = rcvbuf[15] = 0;
	       rcvbuf[22] = data & 0xff;
	       rcvbuf[23] = (data >> 8) & 0xff;
	       rcvbuf[24] = (data >> 16) & 0xff;
	       rcvbuf[25] = (data >> 24) & 0xff;
	       rcvbuf[26] = (data >> 32) & 0xff;
	       rcvbuf[27] = (data >> 40) & 0xff;
	       rcvbuf[28] = (data >> 48) & 0xff;
	       rcvbuf[29] = (data >> 56) & 0xff;
	   }
	   /* keep buffer */
           return CapiNoError;
	}
        return_buffer(ApplID, offset);
        if (   CAPIMSG_COMMAND(rcvbuf) == CAPI_DISCONNECT
	    && CAPIMSG_SUBCOMMAND(rcvbuf) == CAPI_IND)
           cleanup_buffers_for_plci(ApplID, CAPIMSG_U32(rcvbuf, 8));
        return CapiNoError;
    }

    return_buffer(ApplID, offset);

    if (rc == 0)
        return CapiReceiveQueueEmpty;

    switch (errno) {
        case EMSGSIZE:
            ret = CapiIllCmdOrSubcmdOrMsgToSmall;
            break;
        case EAGAIN:
            return CapiReceiveQueueEmpty;
        default:
            ret = CapiMsgOSResourceErr;
            break;
    }

    return ret;
}

unsigned char *
capi20_get_manufacturer(unsigned Ctrl, unsigned char *Buf)
{
    if (capi20_isinstalled() != CapiNoError)
       return 0;
    ioctl_data.contr = Ctrl;
    if (ioctl(capi_fd, CAPI_GET_MANUFACTURER, &ioctl_data) < 0)
       return 0;
    memcpy(Buf, ioctl_data.manufacturer, CAPI_MANUFACTURER_LEN);
    Buf[CAPI_MANUFACTURER_LEN-1] = 0;
    return Buf;
}

unsigned char *
capi20_get_version(unsigned Ctrl, unsigned char *Buf)
{
    if (capi20_isinstalled() != CapiNoError)
        return 0;
    ioctl_data.contr = Ctrl;
    if (ioctl(capi_fd, CAPI_GET_VERSION, &ioctl_data) < 0)
        return 0;
    memcpy(Buf, &ioctl_data.version, sizeof(capi_version));
    return Buf;
}

unsigned char *
capi20_get_serial_number(unsigned Ctrl, unsigned char *Buf)
{
    if (capi20_isinstalled() != CapiNoError)
        return 0;
    ioctl_data.contr = Ctrl;
    if (ioctl(capi_fd, CAPI_GET_SERIAL, &ioctl_data) < 0)
        return 0;
    memcpy(Buf, &ioctl_data.serial, CAPI_SERIAL_LEN);
    Buf[CAPI_SERIAL_LEN-1] = 0;
    return Buf;
}

unsigned
capi20_get_profile(unsigned Ctrl, unsigned char *Buf)
{
    if (capi20_isinstalled() != CapiNoError)
        return CapiMsgNotInstalled;

    ioctl_data.contr = Ctrl;
    if (ioctl(capi_fd, CAPI_GET_PROFILE, &ioctl_data) < 0) {
        if (errno != EIO)
            return CapiMsgOSResourceErr;
        if (ioctl(capi_fd, CAPI_GET_ERRCODE, &ioctl_data) < 0)
            return CapiMsgOSResourceErr;
        return (unsigned)ioctl_data.errcode;
    }
    if (Ctrl)
        memcpy(Buf, &ioctl_data.profile, sizeof(struct capi_profile));
    else
        memcpy(Buf, &ioctl_data.profile.ncontroller,
                       sizeof(ioctl_data.profile.ncontroller));
    return CapiNoError;
}
/*
 * functions added to the CAPI2.0 spec
 */

unsigned
capi20_waitformessage(unsigned ApplID, struct timeval *TimeOut)
{
  int fd;
  fd_set rfds;

  FD_ZERO(&rfds);

  if (capi20_isinstalled() != CapiNoError)
    return CapiRegNotInstalled;

  if(!validapplid(ApplID))
    return CapiIllAppNr;

  fd = applid2fd(ApplID);

  FD_SET(fd, &rfds);

  if (select(fd + 1, &rfds, NULL, NULL, TimeOut) < 1)
	return CapiReceiveQueueEmpty;

  return CapiNoError;
}

int
capi20_fileno(unsigned ApplID)
{
   return applid2fd(ApplID);
}

/*
 * Extensions for middleware
 */

int
capi20ext_get_flags(unsigned ApplID, unsigned *flagsptr)
{
   if (ioctl(applid2fd(ApplID), CAPI_GET_FLAGS, flagsptr) < 0)
      return CapiMsgOSResourceErr;
   return CapiNoError;
}

int
capi20ext_set_flags(unsigned ApplID, unsigned flags)
{
   if (ioctl(applid2fd(ApplID), CAPI_SET_FLAGS, &flags) < 0)
      return CapiMsgOSResourceErr;
   return CapiNoError;
}

int
capi20ext_clr_flags(unsigned ApplID, unsigned flags)
{
   if (ioctl(applid2fd(ApplID), CAPI_CLR_FLAGS, &flags) < 0)
      return CapiMsgOSResourceErr;
   return CapiNoError;
}

char *
capi20ext_get_tty_devname(unsigned applid, unsigned ncci, char *buf, size_t size)
{
	int unit;
        unit = ioctl(applid2fd(applid), CAPI_NCCI_GETUNIT, &ncci);
        if (unit < 0)
		return 0;
	snprintf(buf, size, "/dev/capi/%d", unit);
	return buf;
}

char *
capi20ext_get_raw_devname(unsigned applid, unsigned ncci, char *buf, size_t size)
{
	int unit;
        unit = ioctl(applid2fd(applid), CAPI_NCCI_GETUNIT, &ncci);
        if (unit < 0)
		return 0;
	snprintf(buf, size, "/dev/capi/r%d", unit);
	return buf;
}

int capi20ext_ncci_opencount(unsigned applid, unsigned ncci)
{
   return ioctl(applid2fd(applid), CAPI_NCCI_OPENCOUNT, &ncci);
}

static void initlib(void) __attribute__((constructor));
static void exitlib(void) __attribute__((destructor));

static void initlib(void)
{
   int i;
   for (i=0; i < MAX_APPL; i++)
	applidmap[i] = -1;
}

static void exitlib(void)
{
    if (capi_fd >= 0) {
       close(capi_fd);
       capi_fd = -1;
    }
}
