#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netdb.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <limits.h>
/* Try to pull in PATH_MAX */
#include <limits.h>
#include <sys/param.h>
#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#include <syslog.h>
#include <sys/syscall.h>
#include <sys/utsname.h>

#include <paths.h>
#include <sys/un.h>
#include <sys/uio.h>

#include <netinet/in.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "umng_syslogd.h"

#define DEBUG 1
// from supertask
#define _SYSLOG 1
#define _DO_FIRMWARE_RELOAD 0
#undef _SYS_LOG_DEBUG
#define dprintf printf
// end of from supertask

// global data
#define NOINLINE      __attribute__((__noinline__))
#define ALWAYS_INLINE __attribute__ ((always_inline)) inline
#define barrier() __asm__ __volatile__("":::"memory")
#define USE_FEATURE_IPC_SYSLOG(...) __VA_ARGS__

#define bb_msg_memory_exhausted "memory exhausted"

#define	MAXSYMLINKS	20
#define ULLONG_MAX	(~0ULL)

typedef signed char smallint;

smallint wrote_pidfile;

/* Allows us to have smaller initializer. Ugly. */
#define GLOBALS \
USE_FEATURE_IPC_SYSLOG( \
	int shmid; /* ipc shared memory id */   \
	int s_semid; /* ipc semaphore id */     \
	int shm_size;                           \
	struct sembuf SMwup[1];                 \
	struct sembuf SMwdn[3];                 \
)

struct init_globals {
	GLOBALS
};

struct globals {
	GLOBALS

	struct shbuf_ds *shbuf;

	/* We recv into recvbuf... */
	char recvbuf[MAX_READ];
	/* ...then copy to parsebuf, escaping control chars */
	/* (can grow x2 max) */
	char parsebuf[MAX_READ*2];
	/* ...then sprintf into printbuf, adding timestamp (15 chars),
	 * host (64), fac.prio (20) to the message */
	/* (growth by: 15 + 64 + 20 + delims = ~110) */
	char printbuf[MAX_READ*2 + 128];
};

static const struct init_globals init_data = {
	.shmid = -1,
	.s_semid = -1,
	.shm_size = (sizeof(struct shbuf_ds) + MAX_SYSTEM_LOG_POOL_SIZE * sizeof(SystemLogPool_s)), // default shm size
	.SMwup = { {1, -1, IPC_NOWAIT} },
	.SMwdn = { {0, 0}, {1, 0}, {1, +1} },
};

struct globals *const ptr_to_globals;

#define SET_PTR_TO_GLOBALS(x) do { \
	(*(struct globals**)&ptr_to_globals) = (x); \
	barrier(); \
} while (0)

#define G (*ptr_to_globals)
#define INIT_G() do { \
	SET_PTR_TO_GLOBALS(memcpy(xzalloc(sizeof(struct globals)), &init_data, sizeof(init_data))); \
} while (0)

SystemLogPool_s *sysLogPool = NULL;
SystemLogPool_Info sysLogPoolInfo;
// end of global data

static void ipcsyslog_cleanup(void);
static void timestamp_and_log_internal(const char *msg);

void bb_perror_msg_and_die(const char *s, ...)
{
#if 1 // bitonic
    char buf[MAX_READ];
	va_list p;

	memset(buf, 0, MAX_READ);
	va_start(p, s);
	vsprintf(buf, s, p);
	va_end(p);
	ipcsyslog_cleanup();
printf("%s\n", buf);
	exit(EXIT_FAILURE);
#else
	va_list p;

	va_start(p, s);
	/* Guard against "<error message>: Success" */
	bb_verror_msg(s, p, errno ? strerror(errno) : NULL);
	va_end(p);
	xfunc_die();
#endif
}

// Convert unsigned integer to ascii, writing into supplied buffer.
// A truncated result contains the first few digits of the result ala strncpy.
// Returns a pointer past last generated digit, does _not_ store NUL.
void BUG_sizeof_unsigned_not_4(void);
char *utoa_to_buf(unsigned n, char *buf, unsigned buflen)
{
	unsigned i, out, res;
	if (sizeof(unsigned) != 4)
		BUG_sizeof_unsigned_not_4();
	if (buflen) {
		out = 0;
		for (i = 1000000000; i; i /= 10) {
			res = n / i;
			if (res || out || i == 1) {
				if (!--buflen) break;
				out++;
				n -= res*i;
				*buf++ = '0' + res;
			}
		}
	}
	return buf;
}

// signal function
void sig_unblock(int sig)
{
	sigset_t ss;
	sigemptyset(&ss);
	sigaddset(&ss, sig);
	sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void bb_signals(int sigs, void (*f)(int))
{
	int sig_no = 0;
	int bit = 1;

	while (sigs) {
		if (sigs & bit) {
			sigs &= ~bit;
			signal(sig_no, f);
		}
		sig_no++;
		bit <<= 1;
	}
}
// end of signal function

/* Assuming the sig is fatal */
void kill_myself_with_sig(int sig)
{
	signal(sig, SIG_DFL);
	sig_unblock(sig);
	raise(sig);
	_exit(EXIT_FAILURE); /* Should not reach it */
}

// memory function
// Die if we can't allocate size bytes of memory.
void *xmalloc(size_t size)
{
	void *ptr = malloc(size);
	if (ptr == NULL && size != 0)
		bb_perror_msg_and_die(bb_msg_memory_exhausted);
	return ptr;
}

void *xzalloc(size_t size)
{
	void *ptr = xmalloc(size);
	memset(ptr, 0, size);
	return ptr;
}

// Die if we can't resize previously allocated memory.  (This returns a pointer
// to the new memory, which may or may not be the same as the old memory.
// It'll copy the contents to a new chunk and free the old one if necessary.)
void *xrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (ptr == NULL && size != 0)
		bb_perror_msg_and_die(bb_msg_memory_exhausted);
	return ptr;
}
// end of memory function

// string function
/* Like strncpy but make sure the resulting string is always 0 terminated. */
char *safe_strncpy(char *dst, const char *src, size_t size)
{
	if (!size) return dst;
	dst[--size] = '\0';
	return strncpy(dst, src, size);
}

// Die if we can't copy a string to freshly allocated memory.
char * xstrdup(const char *s)
{
	char *t;

	if (s == NULL)
		return NULL;

	t = strdup(s);

	if (t == NULL)
		bb_perror_msg_and_die(bb_msg_memory_exhausted);

	return t;
}

// Die if we can't allocate n+1 bytes (space for the null terminator) and copy
// the (possibly truncated to length n) string into it.
char *xstrndup(const char *s, int n)
{
	int m;
	char *t;

	if (DEBUG && s == NULL)
		bb_perror_msg_and_die("xstrndup bug");

	/* We can just xmalloc(n+1) and strncpy into it, */
	/* but think about xstrndup("abc", 10000) wastage! */
	m = n;
	t = (char*) s;
	while (m) {
		if (!*t) break;
		m--;
		t++;
	}
	n -= m;
	t = xmalloc(n + 1);
	t[n] = '\0';

	return memcpy(t, s, n);
}

/* Find out if the last character of a string matches the one given.
 * Don't underrun the buffer if the string length is 0.
 */
char* last_char_is(const char *s, int c)
{
	if (s && *s) {
		size_t sz = strlen(s) - 1;
		s += sz;
		if ( (unsigned char)*s == c)
			return (char*)s;
	}
	return NULL;
}

/*
 * "/"        -> "/"
 * "abc"      -> "abc"
 * "abc/def"  -> "def"
 * "abc/def/" -> ""
 */
char *bb_get_last_path_component_nostrip(const char *path)
{
	char *slash = strrchr(path, '/');

	if (!slash || (slash == path && !slash[1]))
		return (char*)path;

	return slash + 1;
}

/*
 * "/"        -> "/"
 * "abc"      -> "abc"
 * "abc/def"  -> "def"
 * "abc/def/" -> "def" !!
 */
char *bb_get_last_path_component_strip(char *path)
{
	char *slash = last_char_is(path, '/');

	if (slash)
		while (*slash == '/' && slash != path)
			*slash-- = '\0';

	return bb_get_last_path_component_nostrip(path);
}
// end of string function

// socket read/write function
ssize_t safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

ssize_t safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	do {
		n = write(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

/*
 * Write all of the supplied buffer out to a file.
 * This does multiple writes as necessary.
 * Returns the amount written, or -1 on an error.
 */
ssize_t full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = safe_write(fd, buf, len);

		if (cc < 0) {
			if (total) {
				/* we already wrote some! */
				/* user can do another write to know the error code */
				return total;
			}
printf("[full_write] safe_write() fail\n");
			return cc;	/* write() returns -1 on failure. */
		}

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}
// end of socket read/write function

// socket function
// Die with an error message if we can't open a new socket.
int xsocket(int domain, int type, int protocol)
{
	int r = socket(domain, type, protocol);

	if (r < 0) {
		/* Hijack vaguely related config option */
#if ENABLE_VERBOSE_RESOLUTION_ERRORS
		const char *s = "INET";
		if (domain == AF_PACKET) s = "PACKET";
		if (domain == AF_NETLINK) s = "NETLINK";
USE_FEATURE_IPV6(if (domain == AF_INET6) s = "INET6";)
		bb_perror_msg_and_die("socket(AF_%s)", s);
#else
		bb_perror_msg_and_die("socket");
#endif
	}

	return r;
}

// Die with an error message if we can't bind a socket to an address.
void xbind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen)
{
	if (bind(sockfd, my_addr, addrlen))
	{
		bb_perror_msg_and_die("bind");
	}
}

/*
 * NOTE: This function returns a malloced char* that you will have to free
 * yourself.
 */
char *xmalloc_readlink(const char *path)
{
	enum { GROWBY = 80 }; /* how large we will grow strings by */

	char *buf = NULL;
	int bufsize = 0, readsize = 0;

	do {
		bufsize += GROWBY;
		buf = xrealloc(buf, bufsize);
		readsize = readlink(path, buf, bufsize);
		if (readsize == -1) {
			free(buf);
			return NULL;
		}
	} while (bufsize < readsize + 1);

	buf[readsize] = '\0';

	return buf;
}

/*
 * This routine is not the same as realpath(), which
 * canonicalizes the given path completely. This routine only
 * follows trailing symlinks until a real file is reached and
 * returns its name. If the path ends in a dangling link or if
 * the target doesn't exist, the path is returned in any case.
 * Intermediate symlinks in the path are not expanded -- only
 * those at the tail.
 * A malloced char* is returned, which must be freed by the caller.
 */
char *xmalloc_follow_symlinks(const char *path)
{
	char *buf;
	char *lpc;
	char *linkpath;
	int bufsize;
	int looping = MAXSYMLINKS + 1;

	buf = xstrdup(path);
	goto jump_in;

	while (1) {
		linkpath = xmalloc_readlink(buf);
		if (!linkpath) {
			/* not a symlink, or doesn't exist */
			if (errno == EINVAL || errno == ENOENT)
				return buf;
			goto free_buf_ret_null;
		}

		if (!--looping) {
			free(linkpath);
 free_buf_ret_null:
			free(buf);
			return NULL;
		}

		if (*linkpath != '/') {
			bufsize += strlen(linkpath);
			buf = xrealloc(buf, bufsize);
			lpc = bb_get_last_path_component_strip(buf);
			strcpy(lpc, linkpath);
			free(linkpath);
		} else {
			free(buf);
			buf = linkpath;
 jump_in:
			bufsize = strlen(buf) + 1;
		}
	}
}

/* Don't inline: prevent struct sockaddr_un to take up space on stack
 * permanently */
static NOINLINE int create_socket(void)
{
	struct sockaddr_un sunx;
	int sock_fd;
	char *dev_log_name;

	memset(&sunx, 0, sizeof(sunx));
	sunx.sun_family = AF_UNIX;

	/* Unlink old /dev/log or object it points to. */
	/* (if it exists, bind will fail) */
	strcpy(sunx.sun_path, _ARCADYAN_PATH_LOG);

	dev_log_name = xmalloc_follow_symlinks(_ARCADYAN_PATH_LOG);

	if (dev_log_name) {
		safe_strncpy(sunx.sun_path, dev_log_name, sizeof(sunx.sun_path));
		free(dev_log_name);
	}

	unlink(sunx.sun_path);

	sock_fd = xsocket(AF_UNIX, SOCK_DGRAM, 0);

	xbind(sock_fd, (struct sockaddr *) &sunx, sizeof(sunx));

	chmod(_ARCADYAN_PATH_LOG, 0666);
	return sock_fd;
}
// end of socket function

/* our shared key (syslogd.c and logread.c must be in sync) */
static void ipcsyslog_cleanup(void)
{
	printf("[ipcsyslog_cleanup]\n");
	if (G.shmid != -1) {
		shmdt(G.shbuf);
	}
	if (G.shmid != -1) {
		shmctl(G.shmid, IPC_RMID, NULL);
	}
	if (G.s_semid != -1) {
		semctl(G.s_semid, 0, IPC_RMID, 0);
	}
	free(&G);
}

static void ipcsyslog_init(void)
{
	printf("[ipcsyslog_init]\n");
	if (DEBUG)
		printf("shmget(%x, %d,...)\n", (int)KEY_ID, G.shm_size);

	G.shmid = shmget(KEY_ID, G.shm_size, IPC_CREAT | 0644);
	if (G.shmid == -1) {
		bb_perror_msg_and_die("[ipcsyslog_init] shmget");
	}

	G.shbuf = shmat(G.shmid, NULL, 0);
	if (G.shbuf == (void*) -1L) { /* shmat has bizarre error return */
		bb_perror_msg_and_die("[ipcsyslog_init] shmat");
	}
	sysLogPool = ((void *)G.shbuf) + sizeof(struct shbuf_ds);
printf("[ipcsyslog_init] G.shbuf=0x%x, sysLogPool=0x%x\n",G.shbuf, sysLogPool);

	memset(G.shbuf, 0, sizeof(struct shbuf_ds));
	G.shbuf->log_pool_size = MAX_SYSTEM_LOG_POOL_SIZE;
	G.shbuf->head = 0;
	G.shbuf->tail = 0;

	// we'll trust the OS to set initial semval to 0 (let's hope)
	G.s_semid = semget(KEY_ID, 2, IPC_CREAT | IPC_EXCL | 1023);
	if (G.s_semid == -1) {
		if (errno == EEXIST) {
			G.s_semid = semget(KEY_ID, 2, 0);
			if (G.s_semid != -1)
				goto ipcsyslog_init_ok;
		}
		bb_perror_msg_and_die("[ipcsyslog_init] semget");
	}

ipcsyslog_init_ok:
	if(initSystemLogPool() == false)
	{
		ipcsyslog_cleanup();
		bb_perror_msg_and_die("[ipcsyslog_init] initSystemLogPool() fail");
	}
printf("[ipcsyslog_init] G.shmid=%d, G.s_semid=%d\n",G.shmid, G.s_semid);
}

/* len parameter is used only for "is there a timestamp?" check.
 * NB: some callers cheat and supply len==0 when they know
 * that there is no timestamp, short-circuiting the test. */
static void timestamp_and_log(unsigned char log_type, unsigned char log_level, unsigned char msg_type, char *msg, int len)
{
	//syslogd_SetSystemLog(log_type, msg);
	syslogd_SetSystemLogAndMsgType(log_type, log_level, msg_type, msg);
}

static void timestamp_and_log_internal(const char *msg)
{
	timestamp_and_log(LOG_TYPE_OTHER, LOG_LEVEL_DEFAULT, LOG_MESSAGE_TYPE_DEFAULT, (char*)msg, strlen(msg));
}

static void quit_signal(int sig)
{
	timestamp_and_log_internal("syslogd exiting");
	ipcsyslog_cleanup();
	puts("syslogd exiting");
	kill_myself_with_sig(sig);
}

void Log_Lock()
{
	if (semop(G.s_semid, G.SMwdn, 3) == -1) {
		bb_perror_msg_and_die("SMwdn");
	}
}

void Log_UnLock()
{
	if (semop(G.s_semid, G.SMwup, 1) == -1) {
		bb_perror_msg_and_die("SMwup");
	}
}

#if (_SYSLOG == 2)
#include "syslogct.h"
unsigned long logTypeMappingTbl[96] = {
	// send log type 1
	0x01		,	// other type
	0x02		,	// big pong service
	0x04		,	// DDNS-TZO service
	0x08		,	// DDNS-DYNDNS service
	0x10		,	// DDNS-DHIS service
	0x20		,	// DDNS-TZO for SMC service
	0x40		,	// http daemon
	0x80		,	// IGD 1.0 service, original version
	0x100		,	// IGD 1.0 service, accton version
	0x200		,	// LPD service
	0x400		,	// DHCP daemon service
	0x800		,	// DHCP client service
	0x1000		,	// DNS proxy service
	0x2000		,	// mail proxy service
	0x4000		,	// ping service
	0x8000		,	// POP3 service
	0x10000		,	// RIP service
	0x20000		,	// SMTP service
	0x40000		,	// SNTP service
	0x80000		,	// Software upgrade/TFTP service
	0x100000	,	// syslog client service
	0x200000	,	// xmodem service
	0x400000	,	// netbios service
	0x800000	,	// snmp service
	0x1000000	,	// telnet/rs232 daemon
	0x2000000	,	// UPnP service, original version
	0x4000000	,	// UPnP service, accton version
	0x8000000	,	// cgi module
	0x10000000	,	// 8947
	0x20000000	,	// 8947 for Edimax
	0x40000000	,	// 8957
	0x80000000	,	// ADM5106

	// send log type 2
	0x01		,	// ADM5106 for Edimax 2WAN
	0x02		,	// ADM5120
	0x04		,	// Conexent 82100
	0x08		,	// 2510
	0x10		,	// usb driver core
	0x20		,	// usb driver for MAC
	0x40		,	// usb driver for storage
	0x80		,	// usb driver for host
	0x100		,	// usb driver for device
	0x200		,	// hardware module
	0x400		,	// socket module
	0x800		,	// TCP/UDP layer
	0x1000		,	// FAT file system
	0x2000		,	// TSD file system
	0x4000		,	// NAT module
	0x8000		,	// kernel module
	0x10000		,	// crypto library
	0x20000		,	// big number operation library
	0x40000		,	// new standard library
	0x80000		,	// library for dial-up
	0x100000	,	// library for unzip
	0x200000	,	// library for UI
	0x400000	,	// library for some utility
	0x800000	,	// ame150 module for st20166
	0x1000000	,	// ame150 annex A module for st20156
	0x2000000	,	// ame150 annex B module for st20156
	0x4000000	,	// ATM module
	0x8000000	,	// bridge module
	0x10000000	,	// wireless 11g module
	0x20000000	,	// wireless 11b atmel module
	0x40000000	,	// wireless 11b atmel module for edimax
	0x80000000	,	// wireless 11b intersil module for conexent CPU

	// send log type 3
	0x01		,	// wireless 11b intersil module
	0x02		,	// ppp module
	0x04		,	// pppoe module
	0x08		,	// 802.1x module
	0x10		,	// WPA module
	0x20		,	// firewall module
	0x40		,	// firewall SPI module
	0x80		,	// firewall DoS module
	0x100		,	// firewall URl blocking module
	0x200		,	// firewall MAC filter module
	0x400		,	// ike module
	0x800		,	// ipsec module
	0x1000		,	// ipsec with ike module
	0x2000		,	// pptp module
	0x4000		,	// L2TP module
	0x8000		,	//
	0x10000		,	//
	0x20000		,	//
	0x40000		,	//
	0x80000		,	//
	0x100000	,	//
	0x200000	,	//
	0x400000	,	//
	0x800000	,	//
	0x1000000	,	//
	0x2000000	,	//
	0x4000000	,	//
	0x8000000	,	//
	0x10000000	,	//
	0x20000000	,	//
	0x40000000	,	//
	0x80000000	,	//
};
#endif // (_SYSLOG == 2)

int initSystemLogPool()
{
	int i;
#ifdef _SYS_LOG_DEBUG
	dprintf("[initSystemLogPool] sysLogPool=0x%x\n", sysLogPool);
#endif // _SYS_LOG_DEBUG

	if(sysLogPool==NULL) return false;

#if _DO_FIRMWARE_RELOAD
    if(GetReloadFlag(RELOAD_SYSLOG) == 0)
#endif // _DO_FIRMWARE_RELOAD
    {	// initialize system log pool
#ifdef _SYS_LOG_DEBUG
	    dprintf("GetReloadFlag(RELOAD_SYSLOG)=0\n");
#endif // _SYS_LOG_DEBUG
    	memset((char*)sysLogPool, 0, MAX_SYSTEM_LOG_POOL_SIZE * sizeof(SystemLogPool_s));
    	for(i = 0; i < (MAX_SYSTEM_LOG_POOL_SIZE - 1); i++)
    	{
    		sysLogPool[i].index = i; // 0-based index
			sysLogPool[i].next = i+2; // 1-based index
		}
   		sysLogPool[MAX_SYSTEM_LOG_POOL_SIZE - 1].index = MAX_SYSTEM_LOG_POOL_SIZE - 1;

    	memset(&sysLogPoolInfo, 0, sizeof(SystemLogPool_Info));
    	sysLogPoolInfo.count = MAX_SYSTEM_LOG_POOL_SIZE;
		sysLogPoolInfo.free = (char*)&sysLogPool[0];
    }
#if _DO_FIRMWARE_RELOAD
	SetReloadFlag(RELOAD_SYSLOG, 1);
#endif // _DO_FIRMWARE_RELOAD

#ifdef _SYS_LOG_DEBUG
	dprintf("initSystemLogPool()> ok\n");
#endif // _SYS_LOG_DEBUG
	return true;
}

char* GetSystemLogBuffer()
{
	char* buffer = NULL;

	Log_Lock();
#ifdef _SYS_LOG_DEBUG
		dprintf("GetSystemLogBuffer()> Begin---\n");
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.count=%d\n", sysLogPoolInfo.count);
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.free=%lp\n", sysLogPoolInfo.free);
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.head=%lp\n", sysLogPoolInfo.head);
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.tail=%lp\n", sysLogPoolInfo.tail);
#endif // _SYS_LOG_DEBUG
	if(sysLogPoolInfo.count > 0)
	{	// get new buffer
		buffer = sysLogPoolInfo.free;

		sysLogPoolInfo.free = SYSLOG_BUFFER_NEXT(buffer);
		sysLogPoolInfo.count--;
		if(sysLogPoolInfo.head == NULL && sysLogPoolInfo.tail == NULL)
		{
			SYSLOG_BUFFER_PREV_INDEX(buffer) = 0;
			SYSLOG_BUFFER_NEXT_INDEX(buffer) = 0;
			sysLogPoolInfo.head = buffer;
			sysLogPoolInfo.tail = buffer;

			G.shbuf->head = SYSLOG_BUFFER_INDEX(sysLogPoolInfo.head) + 1;
			G.shbuf->tail = SYSLOG_BUFFER_INDEX(sysLogPoolInfo.tail) + 1;
		}
		else
		{
			SYSLOG_BUFFER_PREV_INDEX(buffer) = 0;
			SYSLOG_BUFFER_NEXT_INDEX(buffer) = SYSLOG_BUFFER_INDEX(sysLogPoolInfo.head) + 1; // 1-based index
			SYSLOG_BUFFER_PREV_INDEX(sysLogPoolInfo.head) = SYSLOG_BUFFER_INDEX(buffer) + 1; // 1-based index
			sysLogPoolInfo.head = buffer;

			G.shbuf->head = SYSLOG_BUFFER_INDEX(sysLogPoolInfo.head) + 1; // 1-based index
		}
		Log_UnLock();
	}
	else
	{	// release the oldest buffer first, then get new buffer
		Log_UnLock();
		if(RelSystemLogBuffer(sysLogPoolInfo.tail) == false)
		{
#ifdef _SYS_LOG_DEBUG
			dprintf("GetSystemLogBuffer()> RelSystemLogBuffer() fail\n");
#endif // _SYS_LOG_DEBUG
			return NULL;
		}
		buffer = GetSystemLogBuffer();
	}
#ifdef _SYS_LOG_DEBUG
	dprintf("GetSystemLogBuffer()> end---\n");
	dprintf("GetSystemLogBuffer()> sysLogPoolInfo.count=%d\n", sysLogPoolInfo.count);
	dprintf("GetSystemLogBuffer()> sysLogPoolInfo.free=%lp\n", sysLogPoolInfo.free);
	dprintf("GetSystemLogBuffer()> sysLogPoolInfo.head=%lp\n", sysLogPoolInfo.head);
	dprintf("GetSystemLogBuffer()> sysLogPoolInfo.tail=%lp\n", sysLogPoolInfo.tail);
#endif // _SYS_LOG_DEBUG
	return buffer;
}

char RelSystemLogBuffer(char* buffer)
{
	if(buffer == NULL)
	{
#ifdef _SYS_LOG_DEBUG
		dprintf("RelSystemLogBuffer()> fail\n");
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.count=%d\n", sysLogPoolInfo.count);
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.free=%lp\n", sysLogPoolInfo.free);
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.head=%lp\n", sysLogPoolInfo.head);
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.tail=%lp\n", sysLogPoolInfo.tail);
#endif // _SYS_LOG_DEBUG
		return false;
	}

	Log_Lock();
	if(sysLogPoolInfo.head == NULL && sysLogPoolInfo.tail == NULL)
	{
		Log_UnLock();
#ifdef _SYS_LOG_DEBUG
		dprintf("RelSystemLogBuffer()> fail\n");
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.count=%d\n", sysLogPoolInfo.count);
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.free=%lp\n", sysLogPoolInfo.free);
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.head=%lp\n", sysLogPoolInfo.head);
		dprintf("GetSystemLogBuffer()> sysLogPoolInfo.tail=%lp\n", sysLogPoolInfo.tail);
#endif // _SYS_LOG_DEBUG
		return false;
	}

	if(sysLogPoolInfo.head == sysLogPoolInfo.tail)
	{
		if(buffer != sysLogPoolInfo.head)
		{
			Log_UnLock();
			return false;
		}
		memset(buffer, 0, sizeof(SystemLogPool_s));
		sysLogPoolInfo.head = NULL;
		sysLogPoolInfo.tail = NULL;

		G.shbuf->head = 0;
		G.shbuf->tail = 0;
	}
	else if(buffer == sysLogPoolInfo.head)
	{
		SYSLOG_BUFFER_PREV_INDEX(SYSLOG_BUFFER_NEXT(buffer)) = SYSLOG_BUFFER_PREV_INDEX(buffer);
		sysLogPoolInfo.head = SYSLOG_BUFFER_NEXT(buffer);
		memset(buffer, 0, sizeof(SystemLogPool_s));

		G.shbuf->head = SYSLOG_BUFFER_INDEX(sysLogPoolInfo.head) + 1; // 1-based index
	}
	else if(buffer == sysLogPoolInfo.tail)
	{
		SYSLOG_BUFFER_NEXT_INDEX(SYSLOG_BUFFER_PREV(buffer)) = 0;
		sysLogPoolInfo.tail = SYSLOG_BUFFER_PREV(buffer);

		G.shbuf->tail = SYSLOG_BUFFER_INDEX(sysLogPoolInfo.tail) + 1; // 1-based index
	}
	else
	{
		SYSLOG_BUFFER_PREV_INDEX(SYSLOG_BUFFER_NEXT(buffer)) = SYSLOG_BUFFER_PREV_INDEX(buffer);
		SYSLOG_BUFFER_NEXT_INDEX(SYSLOG_BUFFER_PREV(buffer)) = SYSLOG_BUFFER_NEXT_INDEX(buffer);
	}

	if(sysLogPoolInfo.free==NULL)
		SYSLOG_BUFFER_NEXT_INDEX(buffer) = 0;
	else
		SYSLOG_BUFFER_NEXT_INDEX(buffer) = SYSLOG_BUFFER_INDEX(sysLogPoolInfo.free) + 1;
	sysLogPoolInfo.free = buffer;
	sysLogPoolInfo.count++;
	Log_UnLock();
	return true;
}

#if _DO_FIRMWARE_RELOAD
// add by yclin 91/02/27
// For firmware reload
// backup to memory
int syslog_reload_backup(char* out)
{
	unsigned long offset = 0, i;
	unsigned short nLogCount = 0;
	SystemLogPool_s log;
	char* buffer;

	nLogCount = MAX_SYSTEM_LOG_POOL_SIZE - sysLogPoolInfo.count;
	memcpy(out+offset, &nLogCount, sizeof(unsigned short));
	offset+=sizeof(unsigned short);
	if(nLogCount == 0) return offset;
	buffer = sysLogPoolInfo.tail;
	while(buffer != NULL)
	{
		memcpy(&log, buffer, sizeof(SystemLogPool_s));
		log.next = NULL;
		log.prev = NULL;
		memcpy(out+offset, &log, sizeof(SystemLogPool_s));
		offset+=sizeof(SystemLogPool_s);
		buffer = SYSLOG_BUFFER_PREV(buffer);
	}

	return offset;
}

// restore from memory
int syslog_reload_restore(char* in)
{
	unsigned long offset = 0, i;
	unsigned short nLogCount = 0;
	SystemLogPool_s log;
	char* buffer;

	memcpy(&nLogCount, in+offset, sizeof(unsigned short));
	offset+=sizeof(unsigned short);
	if(nLogCount == 0)
	{
		SetReloadFlag(RELOAD_SYSLOG, 0);
		return offset;
	}
   	memset((char*)sysLogPool, 0, MAX_SYSTEM_LOG_POOL_SIZE * sizeof(SystemLogPool_s));
   	memset(&sysLogPoolInfo, 0, sizeof(SystemLogPool_Info));
   	for(i = 0; i < nLogCount; i++)
   	{
   		memcpy((char*)&sysLogPool[i], (char*)(in+offset), sizeof(SystemLogPool_s));
   		offset+=sizeof(SystemLogPool_s);

   		if(i == 0)
			sysLogPool[i].next = 0;
		else
			sysLogPool[i].next = i-1+1; // 1-based index
		if(i == (nLogCount - 1))
			sysLogPool[i].prev = 0;
		else
			sysLogPool[i].prev = i+1+1; // 1-based index
   	}

   	for(i = nLogCount; i < (MAX_SYSTEM_LOG_POOL_SIZE - 1); i++)
		sysLogPool[i].next = i+2; // 1-based index
	sysLogPool[MAX_SYSTEM_LOG_POOL_SIZE - 1].index = MAX_SYSTEM_LOG_POOL_SIZE - 1;

   	sysLogPoolInfo.count = MAX_SYSTEM_LOG_POOL_SIZE - nLogCount;
   	if(nLogCount == MAX_SYSTEM_LOG_POOL_SIZE)
		sysLogPoolInfo.free = NULL;
	else
		sysLogPoolInfo.free = (char*)&sysLogPool[nLogCount];
	sysLogPoolInfo.head = (char*)&sysLogPool[nLogCount - 1];
	sysLogPoolInfo.tail = (char*)&sysLogPool[0];
#ifdef _SYS_LOG_DEBUG
	dprintf("syslog_reload_restore()>\n");
	dprintf("GetSystemLogBuffer()> sysLogPoolInfo.count=%d\n", sysLogPoolInfo.count);
	dprintf("GetSystemLogBuffer()> sysLogPoolInfo.free=%lp\n", sysLogPoolInfo.free);
	dprintf("GetSystemLogBuffer()> sysLogPoolInfo.head=%lp\n", sysLogPoolInfo.head);
	dprintf("GetSystemLogBuffer()> sysLogPoolInfo.tail=%lp\n", sysLogPoolInfo.tail);
#endif // _SYS_LOG_DEBUG
	return offset;
}
// end of add by yclin 91/02/27
#endif // _DO_FIRMWARE_RELOAD

char syslogd_SetSystemLogAndMsgType(unsigned char log_type, unsigned char log_level, unsigned char msg_type, char *message)
{
	return syslogd_SetSystemLog2(log_type, log_level, msg_type, DEFAULT_SYS_LOG_LANG, message);
}

char syslogd_SetSystemLog(unsigned char log_type, unsigned char log_level, unsigned char log_facility, char* message) {
	return syslogd_SetSystemLog2(log_type, log_level, log_facility, DEFAULT_SYS_LOG_LANG, message);
}

char syslogd_SetSystemLog2(unsigned char log_type, unsigned char log_level, unsigned char log_facility, unsigned char lang_type, char *message)
{
	char* buffer;
	char temp[MAX_MESSAGE_CHAR_SIZE*2];
    time_t curtime;
    struct tm *loctime;
#ifdef _ATTACK_LED_ALERT
	extern int attack_led_alert;
#endif

#ifdef _ATTACK_LED_ALERT
	if ((log_type==LOG_TYPE_CBAC)&&(attack_led_alert==0)) attack_led_alert=1;
#endif

	buffer = GetSystemLogBuffer();
	Log_Lock();
	if(buffer == NULL)
	{
#ifdef _SYS_LOG_DEBUG
		dprintf("syslogd_SetSystemLog2()> GetSystemLogBuffer() return fail\n");
#endif // _SYS_LOG_DEBUG
		Log_UnLock();
		return 0;
	}
#ifdef _SYS_LOG_DEBUG
	dprintf("syslogd_SetSystemLog2()> buffer index=%d\n",SYSLOG_BUFFER_INDEX(buffer));
#endif // _SYS_LOG_DEBUG

	SYSLOG_BUFFER_LANG_TYPE(buffer) = lang_type;
	SYSLOG_BUFFER_LOG_TYPE(buffer) = log_type;
	SYSLOG_BUFFER_LOG_LEVEL(buffer) = log_level;
	SYSLOG_BUFFER_MSG_TYPE(buffer) = log_facility;

    curtime = time (NULL);
    loctime = localtime (&curtime);
    SYSLOG_BUFFER_TIME(buffer) = curtime;

	if(strlen(message)>=MAX_MESSAGE_CHAR_SIZE)
	{
		strncpy(temp, message,MAX_MESSAGE_CHAR_SIZE-1);
		temp[MAX_MESSAGE_CHAR_SIZE-1] = '\0';
		strcpy(SYSLOG_BUFFER_MESSAGE(buffer), temp);
		dprintf("[syslogd_SetSystemLog2] truncate too long msg\n");
	}
	else
		strcpy(SYSLOG_BUFFER_MESSAGE(buffer), message);

	// send to log server
#if (_SYSLOG == 2)
	if ( gSetting.syslog_ds.syslog_enable) {
		unsigned long *sendTypeTmp;

		if (log_type < 32)
			sendTypeTmp = &gSetting.syslog_ds.syslog_send_type1;
		else if ((log_type > 31) && (log_type < 64))
			sendTypeTmp = &gSetting.syslog_ds.syslog_send_type2;
		else
			sendTypeTmp = &gSetting.syslog_ds.syslog_send_type3;

		if ((*sendTypeTmp & logTypeMappingTbl[log_type]) != 0) {
			void syslog(int priority, char *title, char *fmt, ...);
			//syslog((16<<3)|6 , time, message);
			syslog(LOG_LOCAL0|gSetting.syslog_ds.severity, gSysVersion.HardwareModel, message);
			#ifdef _SYS_LOG_DEBUG
			dprintf(" syslog called .......\n");
			#endif
		}
	}
#endif // _SYSLOG

	Log_UnLock();
	return 1;
}

// messgae size < MAX_MESSAGE_CHAR_SIZE
char syslogd_sprintf_SetSystemLog(unsigned char log_type, unsigned char log_level, unsigned char log_facility, char *fmt, ...) {
    char pf_buf[MAX_MESSAGE_CHAR_SIZE*2]; // message buffer
    va_list args;

	memset(pf_buf, 0, MAX_MESSAGE_CHAR_SIZE*2);
	va_start(args,fmt);
	vsprintf(pf_buf, fmt, args);
	pf_buf[MAX_MESSAGE_CHAR_SIZE-1] = 0;     /* give buffer overflow clue */

	return syslogd_SetSystemLog(log_type, log_level, log_facility, pf_buf);
}

// messgae size < MAX_MESSAGE_CHAR_SIZE
char syslogd_sprintf_SetSystemLogMultiLaguage(unsigned char log_type, unsigned char msg_type, ...) {
    char pf_buf[MAX_MESSAGE_CHAR_SIZE*2]; // message buffer
    char ret;
    va_list args;

	memset(pf_buf, 0, MAX_MESSAGE_CHAR_SIZE*2);
	va_start(args, msg_type);

	// set every language
#ifdef _ENGLISH_SYS_LOG
	fmt = getLogStrFmt(SYS_LOG_LANG_EN,	msg_type);
	if (fmt != NULL) {
		visprintf(pf_buf, fmt, args);
		pf_buf[MAX_MESSAGE_CHAR_SIZE-1] = 0;	// give buffer overflow clue

		ret = syslogd_SetSystemLog2(log_type, LOG_LEVEL_DEFAULT, msg_type, SYS_LOG_LANG_EN, pf_buf);
	}
	else ret = 0;
#endif // _ENGLISH_SYS_LOG
#ifdef _GERMAN_SYS_LOG
	fmt = getLogStrFmt(SYS_LOG_LANG_DE,	msg_type);
	if (fmt != NULL) {
		visprintf(pf_buf, fmt, args);
		pf_buf[MAX_MESSAGE_CHAR_SIZE-1] = 0;	// give buffer overflow clue

		ret = syslogd_SetSystemLog2(log_type, LOG_LEVEL_DEFAULT, msg_type, SYS_LOG_LANG_DE, pf_buf);
	}
	else ret = 0;
#endif // _GERMAN_SYS_LOG

	return ret;
}

void write_pidfile(const char *path)
{
	int pid_fd;
	char *end;
	char buf[sizeof(int)*3 + 2];
	struct stat sb;

	if (!path)
		return;
	/* we will overwrite stale pidfile */
	pid_fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (pid_fd < 0)
		return;

	/* path can be "/dev/null"! Test for such cases */
	wrote_pidfile = (fstat(pid_fd, &sb) == 0) && S_ISREG(sb.st_mode);

	if (wrote_pidfile) {
		/* few bytes larger, but doesn't use stdio */
		end = utoa_to_buf(getpid(), buf, sizeof(buf));
		*end = '\n';
		full_write(pid_fd, buf, end - buf + 1);
	}
	close(pid_fd);
}

static void do_syslogd(void)
{
	int sock_fd = -1;
	#define recvbuf (G.recvbuf)

	/* Set up signal handlers */
	bb_signals(0
		+ (1 << SIGINT)
		+ (1 << SIGTERM)
		+ (1 << SIGQUIT)
		, quit_signal);
	signal(SIGHUP, SIG_IGN);
	/* signal(SIGCHLD, SIG_IGN); - why? */

	sock_fd = create_socket();

	ipcsyslog_init();

	timestamp_and_log_internal("umng_syslogd started");

	for (;;) {
		ssize_t sz;
		unsigned char log_type = 0;
		unsigned char log_level = 0;
		unsigned char log_facility = 0;

 read_again:
		sz = safe_read(sock_fd, recvbuf, MAX_READ - 1);
		if (sz < 0)
		{
			bb_perror_msg_and_die("read from /dev/arcadyan_log");
		}

		/* Drop trailing '\n' and NULs (typically there is one NUL) */
		while (1) {
			if (sz == 0)
				goto read_again;
			/* man 3 syslog says: "A trailing newline is added when needed".
			 * However, neither glibc nor uclibc do this:
			 * syslog(prio, "test")   sends "test\0" to /dev/log,
			 * syslog(prio, "test\n") sends "test\n\0".
			 * IOW: newline is passed verbatim!
			 * I take it to mean that it's syslogd's job
			 * to make those look identical in the log files. */
			if (recvbuf[sz-1] != '\0' && recvbuf[sz-1] != '\n')
				break;
			sz--;
		}

		recvbuf[sz] = '\0'; /* ensure it *is* NUL terminated */
		log_type = recvbuf[0];
		log_level = recvbuf[1];
		log_facility = recvbuf[2];
//printf("[do_syslogd] log_type=0x%x, log_level=0x%x, log_facility=0x%x\n", log_type, log_level, log_facility);
		if( ((unsigned char)log_type) == ((unsigned char)LOG_TYPE_CLEAR_SYSLOG) )
		{
			G.shbuf->head = 0;
			G.shbuf->tail = 0;

			if(initSystemLogPool() == false)
			{
				ipcsyslog_cleanup();
				bb_perror_msg_and_die("[do_syslogd] initSystemLogPool() fail");
			}
		}
		else
			timestamp_and_log(log_type, log_level, log_facility, recvbuf + 3, sz - 3);
	} /* for (;;) */
#undef recvbuf
}

int main(int argc, char **argv)
{
	INIT_G();

	write_pidfile("/var/run/arcadyan_syslogd.pid");
	do_syslogd();
	/* return EXIT_SUCCESS; */
}

#undef G
#undef GLOBALS
#undef INIT_G