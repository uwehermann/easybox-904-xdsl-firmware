/* 
 * Copyright (C) 2010, CyberTAN Corporation
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND CYBERTAN GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. CYBERTAN
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 */

 #include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "dhcpd.h"
#include "arp.h"

void make_gratuitous_arp_packet(packet, client)
	struct arpPacket *packet;
	struct client_state *client;
{
	/* make arp request packet*/
	//memset(packet, 0, sizeof(struct arpPacket));
	memcpy(packet->ethhdr.ether_dhost, MAC_BCAST_ADDR_SERVER, 6);						/* MAC DA */
	memcpy(packet->ethhdr.ether_shost, &(client->interface -> hw_address.hbuf [1]), 6);		/* MAC SA */
	packet->ethhdr.ether_type = htons(ETH_P_ARP);										/* protocol type (Ethernet) */
	packet->htype = htons(ARPHRD_ETHER);												/* hardware type */
	packet->ptype = htons(ETH_P_IP);													/* protocol type (ARP message) */
	packet->hlen = 6;																	/* hardware address length */
	packet->plen = 4;																	/* protocol address length */
	packet->operation = htons(ARPOP_REQUEST);		/* ARP op code */
	memcpy(packet->sInaddr, &(client ->new -> address.iabuf[0]), 4);						/* source IP address */
	memcpy(packet->sHaddr, &(client -> interface -> hw_address.hbuf [1]), 6);				/* source hardware address */
	memcpy(packet->tInaddr, &(client ->new -> address.iabuf[0]), 4);						/* target IP address */

	return ;
}

int send_arp_packet(packet, client)
	struct arpPacket *packet;
	struct client_state *client;
{

	int	timeout = DEFAULT_TIMEOUT;			// we need to reduce time
	int 	optval = 1;
	int	s;									/* socket */
	int	ret = 1;								/* return value */
	struct sockaddr addr;						/* for interface name */
	fd_set fdset;
	struct timeval tm;
	time_t prevTime;
	struct in_addr ipaddr;						// add by honor

	if ((s = socket (PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) == -1) 
	{
		cprintf("Could not open raw socket\n");
		return -1;
	}
	
	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) 
	{
		cprintf("Could not setsocketopt on raw socket\n");
		close(s);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	strcpy(addr.sa_data, client->interface->name);
	if (sendto(s, packet, sizeof(struct arpPacket), 0, &addr, sizeof(addr)) < 0)
		ret = 0;
	/* wait arp reply, and check it */
	tm.tv_usec = 0;
	time(&prevTime);
	while (timeout > 0 && timeout <= DEFAULT_TIMEOUT) 
	{
		FD_ZERO(&fdset);
		FD_SET(s, &fdset);
		tm.tv_sec = timeout;
		if (select(s + 1, &fdset, (fd_set *) NULL, (fd_set *) NULL, &tm) < 0) 
		{
			cprintf("Error on ARPING request: %s\n", strerror(errno));
			if (errno != EINTR) 
				ret = 0;
		}
		else if (FD_ISSET(s, &fdset))
		{
			if (recv(s, packet, sizeof(struct arpPacket), 0) < 0 ) 
				ret = 0;
			if (packet->operation == htons(ARPOP_REPLY) 
				&& bcmp(packet->tHaddr, &(client -> interface -> hw_address.hbuf [1]), 6) == 0 
				&& bcmp(packet->sInaddr, &(client ->new -> address.iabuf[0]), 4) == 0)
			{
				cprintf("Valid arp reply receved for this address\n");
				ret = 0;
			}
		}
		timeout -= time(NULL) - prevTime;
		time(&prevTime);
	}
	close(s);
	return ret;
}

int gratuitous_arp(client)
	struct client_state *client;
{
	struct arpPacket packet;
	int result = 0;
	int times = 0;

	if(client ->new -> address.len == 16)	// do not support ipv6
	{
		return 0;
	}
	for(;times < ARPTIMES; times++)
	{
		memset(&packet, 0, sizeof(struct arpPacket));
		make_gratuitous_arp_packet(&packet, client);
		if(send_arp_packet(&packet, client) == 0)
		{
			result = 1;
		}
	}

	return result;
}
