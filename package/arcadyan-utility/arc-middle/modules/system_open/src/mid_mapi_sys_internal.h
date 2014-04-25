#ifndef _MID_MAPI_SYS_INTERNAL_H_
#define _MID_MAPI_SYS_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	unsigned long long usr;
	unsigned long long sys;
	unsigned long long nice;
	unsigned long long idle;
	unsigned long long iowait;
	unsigned long long irq;
	unsigned long long softirq;
	/*
	 * Since Linux 2.6.11, there is an eighth column, steal - stolen time, 
	 * which is the time spent in other operating systems when running in a 
	 * the Linux kernel
	 */
	unsigned long long stead;
	/*
	 * Since Linux 2.6.24, there is a ninth column, guest, which is the time 
	 * spent running a virtual CPU for guest operating systems under the control of
	 * the Linux kernel
	 */
	unsigned long long guest;
} mapi_sys_cpu_stat_t;

typedef struct { /* Unit: KB */
	unsigned int total;
	unsigned int free;
	unsigned int buffers;
	unsigned int cached;
} mapi_sys_mem_stat_t;

#ifdef __cplusplus
}
#endif

#endif
