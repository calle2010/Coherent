/* $Header: /src386/STREAMS/coh.386/RCS/clist.c,v 2.2 93/08/09 13:35:14 bin Exp Locker: bin $ */
/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 5.0
 *	Copyright (c) 1982, 1983, 1984, 1993.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * Coherent.
 * Character list management.
 */
#include <sys/coherent.h>
#include <sys/clist.h>
#include <sys/sched.h>

#define	cvirt(p)	((CLIST *)(p))
#ifdef TRACER
int nclfree = 0;
#endif

/*
 * Initialise character list queues.
 *
 * Clist are not mapped.
 */
cltinit()
{
	register cmap_t cm;
	register cmap_t lm;
	register paddr_t p;
	register int s;

	s = sphi();
	lm = 0;
	for (p = clistp+NCLIST*sizeof(CLIST); (p-=sizeof(CLIST)) >= clistp; ) {
		cm = p;
		cvirt(cm)->cl_fp = lm;
		lm = cm;
	}
	cltfree = lm;
#ifdef TRACER
	nclfree = NCLIST;
#endif
	spl(s);
}

/*
 * Get a character from the given queue.
 */
cltgetq(cqp)
register CQUEUE *cqp;
{
	register cmap_t op;
	register cmap_t np;
	register int ox;
	register int c;
	register int s;

	if (cqp->cq_cc == 0)
		return (-1);
	s = sphi();
	op = cqp->cq_op;
	ox = cqp->cq_ox;
	c = cvirt(op)->cl_ch[ox]&0377;
	if (--cqp->cq_cc==0 || ++cqp->cq_ox==NCPCL) {
		cqp->cq_ox = 0;
		np = cvirt(op)->cl_fp;
		cvirt(op)->cl_fp = cltfree;
		cqp->cq_op = np;
		cltfree = op;
		if (np == 0) {
			cqp->cq_ip = 0;
			cqp->cq_ix = 0;
		}
		if (cltwant) {
			cltwant = 0;
			wakeup((char *)&cltwant);
		}
#ifdef TRACER
		T_HAL(0x0040, (nclfree++, printf("F%d ", nclfree)));
#endif
	}
	spl(s);
	return (c);
}

/*
 * Put a character on the given queue.
 */
cltputq(cqp, c)
register CQUEUE *cqp;
{
	register cmap_t ip;
	register int ix;
	register int s;
	register cmap_t np;

	s = sphi();
	ip = cqp->cq_ip;
	if ((ix=cqp->cq_ix) == 0) {
		if ((ip=cltfree) == 0) {
			spl(s);
			return (-1);
		}
		cltfree = cvirt(ip)->cl_fp;
		cvirt(ip)->cl_fp = 0;
		if ((np=cqp->cq_ip) == 0)
			cqp->cq_op = ip;
		else {
			cvirt(np)->cl_fp = ip;
		}
		cqp->cq_ip = ip;
		T_HAL(0x0040, (nclfree--, printf("f%d ", nclfree)));
	}
	cvirt(ip)->cl_ch[ix] = c;
	if (++cqp->cq_ix == NCPCL)
		cqp->cq_ix = 0;
	cqp->cq_cc++;
	spl(s);
	return c;
}

/*
 * Clear a character queue.
 */
clrq(cqp)
register CQUEUE *cqp;
{
	register int s;

	s = sphi();
	while (cltgetq(cqp) >= 0)
		;
	spl(s);
}

/*
 * Wait for more character queues to become available.
 */
waitq()
{
	while (cltfree == 0) {
		T_HAL(0x0080, putchar('+'));
		cltwant = 1;
		x_sleep((char *)&cltwant, pritty, slpriNoSig, "waitq");
		/* Wait for more character queues to become available.  */
	}
}
