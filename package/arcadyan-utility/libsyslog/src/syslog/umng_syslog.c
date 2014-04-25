#define __FORCE_GLIBC
#include <features.h>
#include <sys/file.h>
#include <sys/signal.h>
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
#include <pthread.h>
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

#include "umng_syslog.h"

#ifndef ATTRIBUTE_NORETURN
#define ATTRIBUTE_NORETURN __attribute__ ((__noreturn__))
#endif /* ATTRIBUTE_NORETURN */

typedef signed char smallint;

#if 0 // bitonic
static const struct sembuf init_sem[3] = {
	{0, -1, IPC_NOWAIT | SEM_UNDO},
	{1, 0}, {0, +1, SEM_UNDO}
};
#endif

struct globals {
	struct sembuf SMrup[1]; // {0, -1, IPC_NOWAIT | SEM_UNDO},
	struct sembuf SMrdn[2]; // {1, 0}, {0, +1, SEM_UNDO}
	void *shbuf;
};

static struct globals init_data = {
	.SMrup = { {0, -1, IPC_NOWAIT | SEM_UNDO} },
	.SMrdn = { {1, 0}, {0, +1, SEM_UNDO} },
	.shbuf = NULL,
};

#define G (*(struct globals*)&init_data)
#define SMrup (G.SMrup)
#define SMrdn (G.SMrdn)
#define shbuf (G.shbuf)
#if 0 // bitonic
#define INIT_G() do { \
	memcpy(SMrup, init_sem, sizeof(init_sem)); \
} while (0)
#endif

char *syslog_shbuf = NULL;
SystemLogPool_s *sysLogPool = NULL;
int log_semid = -1; /* ipc semaphore id */
int log_shmid = -1; /* ipc shared memory id */
smallint wrote_pidfile;

void bb_perror_msg_and_die(const char *s, ...)
{
#if 1 // bitonic
    char buf[MAX_READ];
	va_list p;

	memset(buf, 0, MAX_READ);
	va_start(p, s);
	vsprintf(buf, s, p);
	va_end(p);
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

static void error_exit(const char *str) ATTRIBUTE_NORETURN;
static void error_exit(const char *str)
{
	CloseSystemLog();

	bb_perror_msg_and_die(str);
}

/*
 * sem_up - up()'s a semaphore.
 */
static void sem_up(int semid)
{
	if (semop(semid, SMrup, 1) == -1)
		error_exit("semop[SMrup]");
}

void Log_Lock()
{
	if(log_semid != -1)
	{
		if (semop(log_semid, SMrdn, 2) == -1)
			error_exit("semop[SMrdn]");
	}
}

void Log_UnLock()
{
	/* release the lock on the log chain */
	if(log_semid != -1)
		sem_up(log_semid);
}

void libsyslog_init()
{
	log_shmid = shmget(KEY_ID, 0, 0);
	if (log_shmid == -1)
		bb_perror_msg_and_die("[libsyslog_init] can't find syslogd buffer");

	/* Attach shared memory to our char* */
	shbuf = shmat(log_shmid, NULL, SHM_RDONLY);
	if (shbuf == NULL)
		bb_perror_msg_and_die("[libsyslog_init] can't access syslogd buffer");

	log_semid = semget(KEY_ID, 0, 0);
	if (log_semid == -1)
		error_exit("[libsyslog_init] can't get access to semaphores for syslogd buffer");

	syslog_shbuf = (void *)shbuf;
	sysLogPool = (void *)shbuf + sizeof(struct shbuf_ds);
//printf("[libsyslog_init] syslog_shbuf=0x%x, sysLogPool=0x%x\n",syslog_shbuf,sysLogPool);
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

// from linux syslog
pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;

#define libc_hidden_def(x)
#define __UCLIBC_MUTEX_LOCK(x) pthread_mutex_lock( &(x) );
#define __UCLIBC_MUTEX_UNLOCK(x) pthread_mutex_unlock( &(x) );
#define attribute_unused __attribute__ ((unused))

static int	LogFile = -1;		/* fd for log */
static int	connected = 0;		/* have done connect */
static int	LogStat = 0;		/* status bits, set by openlog() */
static const char *LogTag = "arcadyan syslog";	/* string to tag the entry with */
static int	LogFacility = LOG_USER;	/* default facility code */
static int	LogMask = 0xff;		/* mask of priorities to be logged */
static struct sockaddr SyslogAddr;	/* AF_UNIX address of local logger */

static void
closelog_intern(int to_default)
{
	__UCLIBC_MUTEX_LOCK(mylock);
	if (LogFile != -1) {
	    (void) close(LogFile);
	}
	LogFile = -1;
	connected = 0;
	if (to_default)
	{
		LogStat = 0;
		LogTag = "arcadyan syslog";
		LogFacility = LOG_USER;
		LogMask = 0xff;
	}
	__UCLIBC_MUTEX_UNLOCK(mylock);
}


static void
sigpipe_handler (attribute_unused int sig)
{
  closelog_intern (0);
}

/*
 * OPENLOG -- open system log
 */
void
arcadyan_openlog( const char *ident, int logstat, int logfac )
{
    int logType = SOCK_DGRAM;

    __UCLIBC_MUTEX_LOCK(mylock);

    if (ident != NULL)
		LogTag = ident;
    LogStat = logstat;
    if (logfac != 0 && (logfac &~ LOG_FACMASK) == 0)
		LogFacility = logfac;
    if (LogFile == -1) {
		SyslogAddr.sa_family = AF_UNIX;
		(void)strncpy(SyslogAddr.sa_data, _ARCADYAN_PATH_LOG,
		      	sizeof(SyslogAddr.sa_data));
retry:
		if (LogStat & LOG_NDELAY) {
	    	if ((LogFile = socket(AF_UNIX, logType, 0)) == -1) {
				goto DONE;
	    	}
	    	/*			fcntl(LogFile, F_SETFD, 1); */
		}
    }

    if (LogFile != -1 && !connected)
    {
		if (connect(LogFile, &SyslogAddr, sizeof(SyslogAddr) -
		    sizeof(SyslogAddr.sa_data) + strlen(SyslogAddr.sa_data)) != -1)
		{
	    	connected = 1;
		} else if (logType == SOCK_DGRAM) {
		    logType = SOCK_STREAM;
		    if (LogFile != -1) {
				close(LogFile);
				LogFile = -1;
		    }
		    goto retry;
		} else {
		    if (LogFile != -1) {
				close(LogFile);
				LogFile = -1;
		    }
printf("[arcadyan_openlog] connect() fail, LogFile=%d\n", LogFile);
		}
    }
DONE:
    __UCLIBC_MUTEX_UNLOCK(mylock);
}
libc_hidden_def(openlog)

/*
 * syslog, vsyslog --
 *     print message on log file; output is intended for syslogd(8).
 */
void
arcadyan_vsyslog(const char *fmt, va_list ap )
{
	register char *p;
	char *last_chr, *head_end, *end;
	time_t now;
	int saved_errno;
	int rc;
	char tbuf[MAX_MESSAGE_CHAR_SIZE+3];	/* syslogd is unable to handle longer messages */

	struct sigaction action, oldaction;
	int sigpipe;
	memset (&action, 0, sizeof (action));
	action.sa_handler = sigpipe_handler;
	sigemptyset (&action.sa_mask);
	sigpipe = sigaction (SIGPIPE, &action, &oldaction);

	saved_errno = errno;

	__UCLIBC_MUTEX_LOCK(mylock);
	int localLogFile = LogFile;
	int localConnected = connected;
	__UCLIBC_MUTEX_UNLOCK(mylock);

	if (localLogFile < 0 || !localConnected)
		arcadyan_openlog(LogTag, LogStat | LOG_NDELAY, 0);

	(void)time(&now);
	head_end = p = tbuf;
	head_end += 3; // include log type, log level and message type

	/* We format the rest of the message. If the buffer becomes full, we mark
	 * the message as truncated. Note that we require at least 2 free bytes
	 * in the buffer as we might want to add "\r\n" there.
	 */

	end = tbuf + sizeof(tbuf) - 1;
#if 0 // bitonic
	__set_errno(saved_errno);
#endif // 0
	p += vsnprintf(p, end - p, fmt, ap);
	if (p >= end || p < head_end) {	/* Returned -1 in case of error... */
		static const char truncate_msg[12] = "[truncated] ";
		memmove(head_end + sizeof(truncate_msg), head_end,
				end - head_end - sizeof(truncate_msg));
		memcpy(head_end, truncate_msg, sizeof(truncate_msg));
		if (p < head_end) {
			while (p < end && *p) {
				p++;
			}
		}
		else {
			p = end - 1;
		}

	}
	last_chr = p;

	/* Output the message to the local logger using NUL as a message delimiter. */
	p = tbuf;
	*last_chr = 0;
	do {
		__UCLIBC_MUTEX_LOCK(mylock);
		rc = write(LogFile, p, last_chr + 1 - p);
		__UCLIBC_MUTEX_UNLOCK(mylock);
		if (rc < 0) {
			if ((errno==EAGAIN) || (errno==EINTR))
				rc=0;
			else {
				closelog_intern(0);
				break;
			}
		}
		p+=rc;
	} while (p <= last_chr);
	if (rc >= 0)
		goto getout;

getout:
	if (sigpipe == 0)
		sigaction (SIGPIPE, &oldaction,
				   (struct sigaction *) NULL);
}
libc_hidden_def(vsyslog)

void
arcadyan_syslog(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	arcadyan_vsyslog(fmt, ap);
	va_end(ap);
}
libc_hidden_def(syslog)

/*
 * CLOSELOG -- close the system log
 */
void
arcadyan_closelog( void )
{
	closelog_intern(1);
}
libc_hidden_def(closelog)

/* setlogmask -- set the log mask level */
int arcadyan_setlogmask(int pmask)
{
    int omask;

    omask = LogMask;
    __UCLIBC_MUTEX_LOCK(mylock);
    if (pmask != 0)
		LogMask = pmask;
    __UCLIBC_MUTEX_UNLOCK(mylock);
    return (omask);
}
// end of from linux syslog

void OpenSystemLog()
{
	libsyslog_init();

	arcadyan_openlog(LogTag, LogStat | LOG_NDELAY, 0);
	if (LogFile < 0 || !connected)
		error_exit("arcadyan_openlog() fail");
}

void CloseSystemLog()
{
	arcadyan_closelog();

	//release all acquired resources
	if (log_shmid != -1) {
		shmdt(shbuf);
	}
#if 0 // need to check whether we should remove these resources.
	if (log_shmid != -1) {
		shmctl(log_shmid, IPC_RMID, NULL);
	}
	if (log_semid != -1) {
		semctl(log_semid, 0, IPC_RMID, 0);
	}
#endif
}

// display
char *GetCurrentSysLog()
{
	if(sysLogPool==NULL)
		OpenSystemLog();
	return ( (((struct shbuf_ds *)syslog_shbuf)->head) ? SYSLOG_BUFFER_BY_INDEX( ((((struct shbuf_ds *)syslog_shbuf)->head)-1) ) : NULL );
}

void v_format_syslog(unsigned char log_type, unsigned char log_level, unsigned char log_facility, const char *fmt, va_list ap )
{
	register char *p;
	char *last_chr, *head_end, *end, *stdp;
	char tbuf[1024];	/* syslogd is unable to handle longer messages */

	head_end = stdp = p = tbuf;
	end = tbuf + sizeof(tbuf) - 1;
	p += vsnprintf(p, end - p, fmt, ap);
	if (p >= end) {	/* Returned -1 in case of error... */
		p = end - 1;
	}
	last_chr = p;

	/* Output the message to the local logger using NUL as a message delimiter. */
	*last_chr = 0;
//	printf("[v_format_syslog] tbuf=%s\n", tbuf);
	SetSystemLog(log_type, log_level, log_facility, tbuf);
}

void SetFormatSystemLog(unsigned char log_type, unsigned char log_level, unsigned char log_facility, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	v_format_syslog(log_type, log_level, log_facility, fmt, ap);
	va_end(ap);
}

char SetSystemLog(unsigned char log_type, unsigned char log_level, unsigned char log_facility, char* message) {
	if(sysLogPool==NULL)
		OpenSystemLog();

	arcadyan_syslog("%c%c%c%s", log_type, log_level, log_facility, message);
	return 1;
}

void ClearSystemLog()
{
	if(sysLogPool==NULL)
		OpenSystemLog();

	arcadyan_syslog("%c%c%c%s", LOG_TYPE_CLEAR_SYSLOG, LOG_LEVEL_DEFAULT, LOG_MESSAGE_TYPE_DEFAULT, "");
}
