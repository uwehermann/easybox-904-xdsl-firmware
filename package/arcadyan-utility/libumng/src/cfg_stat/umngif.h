/*
 * user space APIs to retrieve system status
 *
 * Copyright 2009, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _umngif_h_
#define _umngif_h_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <net/if.h>
    
#define MAX_IF_NUM  32
#define MACADDR_LEN 6

typedef struct umngif_state {
    unsigned char   ifname[IFNAMSIZ];
    unsigned char   mac[MACADDR_LEN];
	struct in_addr  ip4addr;
	struct in_addr  ip4mask;
	struct in_addr  ip4peer;
	int             mtu;
	short           flags;  //check http://linux.die.net/man/7/netdevice for the flags definition
} umngif_state_t;

// get state of an interface
int umngif_get_state(const char *ifname, umngif_state_t *ifstate);
int umngif_get_dftroute(char *ifname, char *gateway);
/* not open
// get names of all active interfaces
int umngif_get_allnames(char *ifnames, int ifnum);
*/

#endif // _umngif_h_
