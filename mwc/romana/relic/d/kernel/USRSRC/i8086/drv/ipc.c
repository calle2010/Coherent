/* $Header: /usr/src/sys/i8086/drv/RCS/ipc.c,v 2.1 88/09/03 13:06:15 src Exp $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1985
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 */

/*
 * Inter-Process Communication.
 *
 * $Log:	/usr/src/sys/i8086/drv/RCS/ipc.c,v $
 * Revision 2.1	88/09/03  13:06:15	src
 * *** empty log message ***
 * 
 * Revision 1.1	88/03/24  17:05:02	src
 * Initial revision
 * 
 */
#include <coherent.h>
#include <sys/ipc.h>
#include <sys/uproc.h>

/*
 * Determine Inter-Process Communication Access Permissions.
 *
 *	Input:	p = pointer to inter-process communication permission struct.
 *
 *	Action:	If super user, permissions are 0600.
 *		If uid is that of the creator or owner of the message id,
 *			use user permissions.
 *		If gid is that of the creator or owner of the message id,
 *			use group permissions.
 *		Otherwise, use others permissions.
 *
 *	Output:	0600 = Read/Alter permission.
 *		0400 = Read permission.
 *		0200 = Alter permission.
 *		   0 = No permission.
 */

ipcaccess( p )

register struct ipc_perm * p;

{
	if ( u.u_uid == 0 )
		return 0600;

	if ((u.u_uid == p->uid) || (u.u_uid == p->cuid))
		return p->mode & 0600;

	if ((u.u_gid == p->gid) || (u.u_gid == p->cgid))
		return (p->mode << 3) & 0600;

	return (p->mode << 6) & 0600;
}
