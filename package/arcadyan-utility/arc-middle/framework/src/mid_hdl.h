/*
 * Arcadyan middle layer message handler prototype
 *
 * Copyright 2010, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _mid_hdl_h_
#define _mid_hdl_h_

#include "mid_fmwk.h"

typedef struct mid_hdl_arg {
	mid_msg_t       *msg;
	void            *mod_spc;   //use to carry address of the variable that
	                            //points to a memory block storing module-specific
	                            //access history information 
} mid_hdl_arg_t;

typedef int (*mod_hdl)(mid_hdl_arg_t *arg);

#define MID_ARG_MOD(pArg)	MID_MSG_MOD(pArg->msg)
#define MID_ARG_PID(pArg)	MID_MSG_PID(pArg->msg)
#define MID_ARG_RID(pArg)	MID_MSG_RID(pArg->msg)
#define MID_ARG_RSP(pArg)	MID_MSG_RSP(pArg->msg)
#define MID_ARG_LEN(pArg)	MID_MSG_LEN(pArg->msg)
#define MID_ARG_DAT(pArg)	MID_MSG_DAT(pArg->msg)
#define MID_ARG_REQ(pArg)	MID_MSG_REQ(pArg->msg)

#endif // _mid_hdl_h_
