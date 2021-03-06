/* 06/02/2008 MK Fixed F4 oam. Modified ifx_find_atm_vcc() */
/* 06/19/2008 CZ Fixed oam infinite loop if DSL conn is down */


#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/net.h>		/* struct socket, struct proto_ops */
#include <linux/atm.h>		/* ATM stuff */
#include <linux/atmdev.h>
#include <linux/socket.h>	/* SOL_SOCKET */
#include <linux/errno.h>	/* error codes */
#include <linux/capability.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/time.h>		/* struct timeval */
#include <linux/skbuff.h>
#include <linux/bitops.h>
#include <linux/init.h>
#include <net/sock.h>		/* struct sock */

#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/poll.h>

struct ifx_koam_cell {
	unsigned char cell[52];
	unsigned int pid;
};
#define IFX_KOAM_CELL_SIZE	sizeof(struct ifx_koam_cell)

struct ifx_atm_cell_header {
#ifdef CONFIG_CPU_LITTLE_ENDIAN
	struct {
		u32 clp 	:1;	// Cell Loss Priority
		u32 pti		:3;	// Payload Type Identifier
		u32 vci 	:16;	// Virtual Channel Identifier
		u32 vpi		:8;	// Vitual Path Identifier
		u32 gfc 	:4;	// Generic Flow Control
	} bit;
#else
	struct {
		u32 gfc 	:4;	// Generic Flow Control
		u32 vpi		:8;	// Vitual Path Identifier
		u32 vci 	:16;	// Virtual Channel Identifier
		u32 pti		:3;	// Payload Type Identifier
		u32 clp 	:1;	// Cell Loss Priority
	} bit;
#endif

};

#define KOAM_CELL_HEADER_SIZE	sizeof(struct ifx_atm_cell_header)
#define OAM_TYPE_FN             (KOAM_CELL_HEADER_SIZE+0)

static struct sock *ifx_koam_sock = NULL;
static unsigned int ifx_koam_upid = 0;

static int ifx_push_oam_internal(struct atm_vcc *atmvcc, void *cell);
static int ifx_oam_failure(void *cell, int cmdId);

static struct atm_vcc *ifx_find_atm_vcc(u32 vpi, u32 vci)
{
	struct atm_vcc *vcc = NULL;
	struct sock *atm_sock = NULL;
	struct hlist_head *head;
	int i;
	
	write_lock_irq(&vcc_sklist_lock);
	
	if (vci == 0x3 || vci == 0x4)
	{
		for (i = 0 ; i < VCC_HTABLE_SIZE; i++) 
		{
			if (!(atm_sock = sk_head(&vcc_hash[i])))
				continue;			    	
			vcc = atm_sk(atm_sock);
			if(vcc->vpi == vpi)	
			    	break;
			else 
				vcc = NULL;
		}
	
		write_unlock_irq(&vcc_sklist_lock);
		return vcc;
	}	
	
	head = &vcc_hash[vci & (VCC_HTABLE_SIZE - 1)];
	atm_sock = sk_head(head);	
	for (; atm_sock != NULL; atm_sock=sk_next(atm_sock)) {
		vcc = atm_sk(atm_sock);
		if (vcc->dev == NULL)
			continue;
		if (vcc->vpi == (short)vpi)
			break;
	}
	write_unlock_irq(&vcc_sklist_lock);
	return atm_sk(atm_sock);
}

static void ifx_koam_rcv_skb(struct sk_buff *skb)
{
	struct ifx_koam_cell *pCell;
	struct ifx_atm_cell_header *cell_header;
	struct atm_vcc *vcc;
	u32 vpi, pti;
	u32 vci;

	pCell = (struct ifx_koam_cell *)skb->data;
	cell_header = (struct ifx_atm_cell_header*)pCell->cell;

	vpi = (u32)(cell_header->bit.vpi);
	vci = (u32)(cell_header->bit.vci);
	pti = (u32)(cell_header->bit.pti);
	
	ifx_koam_upid = pCell->pid;
	if (vpi == 0 && vci == 0) {
		printk(KERN_ERR "userspace registering PID\n");
		goto SEND_OAM_END;
	}
	vcc =ifx_find_atm_vcc(vpi, vci);
	if(!vcc){
		if (ifx_oam_failure(pCell, 8)) { /* cmdId 8: No vcc */
			printk(KERN_ERR "oam error: No vcc for vpi=%d, vci=%d\n", vpi, vci);
		}
		goto SEND_OAM_END;
	}
	if(vcc->dev->ops->send_oam(vcc, pCell->cell, 0)){
		if (ifx_oam_failure(pCell, 7)) { /* cmdId 7: No ppe conn */
			printk(KERN_ERR "oam error: send driver fail \n");
		}
	}
SEND_OAM_END:
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
	kfree_skb(skb);
#endif
	return;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
static void ifx_koam_rcv_sk(struct sock *sk, int len)
{
	struct sk_buff *skb;

	do {
		while((skb = skb_dequeue(&sk->sk_receive_queue)) !=NULL)
		{
			ifx_koam_rcv_skb(skb);
		}
	} while (ifx_koam_sock && ifx_koam_sock->sk_receive_queue.qlen);

	return;
}
#endif

static int ifx_push_oam_internal(struct atm_vcc *atmvcc, void *cell)
{
	struct task_struct *task;
	struct sk_buff *skb;
	int status=0;

	if(!ifx_koam_upid){
		return -1;
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
	task = find_task_by_pid(ifx_koam_upid);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
	task = find_task_by_vpid(ifx_koam_upid);
#else
	task = pid_task(find_vpid(ifx_koam_upid), PIDTYPE_PID);
#endif
	if(!task){
		return -1;
	}

	skb = alloc_skb(IFX_KOAM_CELL_SIZE, GFP_ATOMIC);

	skb_put(skb, IFX_KOAM_CELL_SIZE);
	memcpy(skb->data, (u8 *)cell, IFX_KOAM_CELL_SIZE);

	status = netlink_unicast(ifx_koam_sock, skb, ifx_koam_upid, MSG_DONTWAIT);
	if (status < 0){
		printk(KERN_ERR " koam send fail \n");
		kfree_skb(skb);
		return -1;
	}
	return 0;
}

extern int (*ifx_push_oam_pfn)(struct atm_vcc *atmvcc, void *cell);

static __init int ifx_koam_init(void)
{
	netlink_set_nonroot(NETLINK_USERSOCK, NL_NONROOT_RECV);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
	ifx_koam_sock = netlink_kernel_create(NETLINK_USERSOCK, 0, ifx_koam_rcv_sk, THIS_MODULE);
#else
	ifx_koam_sock = netlink_kernel_create(&init_net, NETLINK_USERSOCK, 0, ifx_koam_rcv_skb, NULL, THIS_MODULE);
#endif
	if (!ifx_koam_sock) {
		return -1;
	}
	ifx_push_oam_pfn = ifx_push_oam_internal;
	printk("KOAM is loaded successfully.\n");
	return 0;
}

static __exit void ifx_koam_exit(void)
{
	sock_release(ifx_koam_sock->sk_socket);
	ifx_push_oam_pfn = NULL;
	return;
}

static int ifx_oam_failure(void *cell, int cmdId)
{
	struct ifx_koam_cell *pCell;
	struct ifx_atm_cell_header *cell_header;
		
	pCell = (struct ifx_koam_cell *)cell;
	cell_header = (struct ifx_atm_cell_header*)pCell->cell;
   			    
	/* construct command failure */
	pCell->cell[OAM_TYPE_FN] = 0x18; /* oam cell */
	pCell->cell[1+OAM_TYPE_FN] = (u32)(cell_header->bit.vpi);  /* vpi*/
	pCell->cell[2+OAM_TYPE_FN] = (u32)(cell_header->bit.vci); /* vci */
	pCell->cell[5+OAM_TYPE_FN] = cmdId; /* command ID 7 or 8 */
	pCell->cell[7+OAM_TYPE_FN] = (u32)(cell_header->bit.pti); /* PTI value */

	cell_header->bit.vci = 0; /* 0 for command */

	if (ifx_push_oam_internal(NULL, (void *)pCell)) {
		printk(KERN_ERR "oam error: could not send failure\n");
		return -1;
	}
	return 0;
}

module_init(ifx_koam_init);
module_exit(ifx_koam_exit);
MODULE_LICENSE("GPL");
