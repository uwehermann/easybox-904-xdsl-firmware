#ifndef _ARCADYAN_SYSLOG_DATA_H_
#define _ARCADYAN_SYSLOG_DATA_H_

#include "umng_syslog_define.h"

#define _ARCADYAN_PATH_LOG "/dev/arc_log" // device name can not exceed 9 characters.

#define MAX_SYSTEM_LOG_POOL_SIZE	600
#define MAX_MESSAGE_CHAR_SIZE		100

#define MAX_READ 256

/* our shared key (syslogd.c and logread.c must be in sync) */
enum { KEY_ID = 0x12345677 }; /* "GENA" */

/* Semaphore operation structures */
struct shbuf_ds {
	int32_t log_pool_size;   /* log pool size = MAX_SYSTEM_LOG_POOL_SIZE */
	unsigned long	head; // 1-based index
	unsigned long	tail; // 1-based index
};

typedef struct {
	unsigned long index; // 0-based index
	unsigned char log_type; // 0: system log; 1: security log
	unsigned char log_level; // 0: debug; 1: info; 2: warning; 3: error
	unsigned char msg_type;	// message type
	unsigned char lang_type;// language type
	time_t time;			// alert time stamp
	char message[MAX_MESSAGE_CHAR_SIZE];
	unsigned short protocol;	// packet source field
	unsigned char source[4];	// packet source field
	unsigned char desti[4];	// packet destination field
	unsigned short srcPort;	// source port (ICMP type)
	unsigned short dstPort;	// destination port (ICMP code)
	unsigned long prev, next; // 1-based index
} SystemLogPool_s;

struct AlertMessage_s {
    char message[MAX_MESSAGE_CHAR_SIZE];   // alert type (0 for empty slot)
    struct tm time;   // alert time stamp
};

extern SystemLogPool_s *sysLogPool;
#define SYSLOG_BUFFER_BY_INDEX(index)		(((void *)sysLogPool) + (sizeof(SystemLogPool_s) * index))
#define SYSLOG_BUFFER_INDEX(mp)			(((SystemLogPool_s *)mp)->index) // 0-based index
#define SYSLOG_BUFFER_LANG_TYPE(mp)    	(((SystemLogPool_s *)mp)->lang_type)
#define SYSLOG_BUFFER_MSG_TYPE(mp)     	(((SystemLogPool_s *)mp)->msg_type)
#define SYSLOG_BUFFER_LOG_LEVEL(mp)     	(((SystemLogPool_s *)mp)->log_level)
#define SYSLOG_BUFFER_NEXT_INDEX(mp)	(((SystemLogPool_s *)mp)->next)
#define SYSLOG_BUFFER_PREV_INDEX(mp)	(((SystemLogPool_s *)mp)->prev)
#define SYSLOG_BUFFER_NEXT(mp)         	( (((SystemLogPool_s *)mp)->next) ? (((void *)sysLogPool) + sizeof(SystemLogPool_s) * (((SystemLogPool_s *)mp)->next - 1)) : NULL )
#define SYSLOG_BUFFER_PREV(mp)         	( (((SystemLogPool_s *)mp)->prev) ? (((void *)sysLogPool) + sizeof(SystemLogPool_s) * (((SystemLogPool_s *)mp)->prev - 1)) : NULL )
#define SYSLOG_BUFFER_LOG_TYPE(mp)     	(((SystemLogPool_s *)mp)->log_type)
#define SYSLOG_BUFFER_TIME(mp)         	(((SystemLogPool_s *)mp)->time)
#define SYSLOG_BUFFER_MESSAGE(mp)       (((SystemLogPool_s *)mp)->message)

#endif // _ARCADYAN_SYSLOG_DATA_H_
