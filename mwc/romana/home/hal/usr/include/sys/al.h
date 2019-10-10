/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_AL_H__
#define	__SYS_AL_H__

/*
 * Include file for com[1-4] driver modules al.c/alx.c
 */

#include <kernel/timeout.h>
#include <sys/ktty.h>
#include <sys/poll_clk.h>

#define NUM_AL_PORTS	4

typedef struct {
	unsigned short	port;
	unsigned short 	com_num;	/* 0..3 for com1..4, respectively */
} COM_DDP;

extern TTY	*(tp_table[NUM_AL_PORTS]);  /* table of pointers for polling */

#endif	/* ! defined (__SYS_AL_H__) */
