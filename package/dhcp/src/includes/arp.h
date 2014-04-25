/* 
 * Copyright (C) 2010, CyberTAN Corporation
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND CYBERTAN GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. CYBERTAN
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 */

 struct ether_header 
{
	u_int8_t  ether_dhost[6];
	u_int8_t  ether_shost[6];
	u_int16_t ether_type;
};
struct arpPacket
{
	struct ether_header ethhdr;	 			/* Ethernet header */
	unsigned short htype;						/* hardware type (must be ARPHRD_ETHER) */
	unsigned short ptype;						/* protocol type (must be ETH_P_IP) */
	unsigned char  hlen;						/* hardware address length (must be 6) */
	unsigned char  plen;						/* protocol address length (must be 4) */
	unsigned short operation;					/* ARP opcode */
	unsigned char  sHaddr[6];					/* sender's hardware address */
	unsigned char  sInaddr[4];					/* sender's IP address */
	unsigned char  tHaddr[6];					/* target's hardware address */
	unsigned char  tInaddr[4];					/* target's IP address */
	unsigned char  pad[18];					/* pad for min. Ethernet payload (60 bytes) */
};
#define	ETH_P_IP					0x0800	/* IP protocol */
#define	ETH_P_ARP					0x0806	/* address resolution protocol */
/* miscellaneous defines */
#define MAC_BCAST_ADDR_SERVER		(unsigned char *) "\xff\xff\xff\xff\xff\xff"
#define DEFAULT_TIMEOUT			1
#define ARPTIMES					3

void make_gratuitous_arp_packet(struct arpPacket *packet, struct client_state *client);
int send_arp_packet(struct arpPacket *packet, struct client_state *client);
int gratuitous_arp(struct client_state *client);