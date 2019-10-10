/*
 * Micro-EMACS extension for MWC -- Search for next/previous error in
 * error buffer.  This module must "know" the format of each type of
 * error message produced by the compiler that the error message is
 * generated by.
 */

#include	<stdio.h>
#include	"ed.h"

/*
 * Find a window associated with the buffer list.
 */

WINDOW *
wfind(bp)
BUFFER *bp;
{
	register WINDOW *wp;

	wp = wheadp;
	while (wp != NULL) {
		if (wp->w_bufp == bp)
			return wp;
		wp = wp->w_wndp;
	}
	return NULL;
}

/*
 * Search forward in the error buffer for an error message.
 */

forwerr(dir)
{
	register LINE	*lp;
	register WINDOW	*wp;
	register uchar	*tp;
	register int	c;
	register int	error_line;
	uchar		error_text[81];
	uchar		*et;

	if ((wp = wfind(errbp)) == NULL)
		return FALSE;

	error_line = 0;
	lp = wp->w_dotp;
	if (wp->w_doto)
		lp = dir ? lback(lp) : lforw(lp);

	for (; lp != errbp->b_linep; lp = dir ? lback(lp) : lforw(lp)) {
		int len, prev;

		tp = lp->l_text;
		for (len = llength(lp), prev = '(';
		    len && ((*tp < '0') || (*tp > '9') || (prev != '('));
		    prev = *tp++, len--)
			;
		while (len-- && ((c = *tp++ - '0') >= 0) && (c <= 9)) {
			error_line *= 10;
			error_line += c;
		}
		if (!error_line)
			continue;
		et = error_text;
		while ((len-- > 0) && ((*et++ = *tp++) != 0)) {
			if ((et - error_text) > (term.t_ncol - 15))
				break;
		}
		*et = 0;
		mlwrite("[%d:%s]",error_line, error_text);
		break;
	}

	if (!error_line) {
		mlwrite("[No more errors.]");
		return FALSE;
	}

	wp->w_dotp = lp;		/* set dot to this line	*/
	wp->w_doto = 1;
	wp->w_force = 1;
	wp->w_flag |= WFFORCE | WFMOVE;
	gotofline(TRUE, error_line);
	return TRUE;
}

/*
 * Make sure we have an error window.
 */

errwind()
{
	register BUFFER *bp;
	register BUFFER *obp;
	register WINDOW *wp;
	register int	r;

	if ((runswitch & CF_ERROR) == 0) {
		mlwrite("[no error buffer]");
		return FALSE;
	}

	if ((bp = errbp) == NULL) {
		mlwrite( "[No error buffer]" );
		return FALSE;
	}

	if (bp->b_nwnd == 0) {			/* If this buffer is not */
		if ((wp=wpopup()) == NULL) {	/* in a window, display it */
			mlwrite("[Can't open window for error buffer]");
			return FALSE;
		}
		obp = wp->w_bufp;		/* Get the old buffer ptr */
		if (--obp->b_nwnd == 0) {
			obp->b_dotp  = wp->w_dotp;
			obp->b_doto  = wp->w_doto;
			obp->b_markp = wp->w_markp;
			obp->b_marko = wp->w_marko;
		}
		wp->w_bufp = bp;
		++bp->b_nwnd;
		wp->w_linep = lback(bp->b_dotp);
		wp->w_dotp  = bp->b_dotp;
		wp->w_doto  = bp->b_doto;
		wp->w_markp = bp->b_markp;
		wp->w_marko = bp->b_marko;
		wp->w_flag |= WFMODE|WFHARD;
		if ((r = wp->w_ntrows - ERRLINES) > 0) {
			BUFFER *owp;

			owp = curwp;
			curwp = wp;
			shrinkwind(0,r);
			curwp = owp;
		}

	}
	return TRUE;
}

/*
 * Find the next error line in the error file.
 */
nexterr(f,n)
{
	if (n < 0)
		return preverr(f, -n);

	if (errwind() == FALSE)
		return FALSE;

	return forwerr(0);
}

/*
 * Find the previous error line in the error buffer.
 */
preverr(f,n)
{
	if (n < 0)
		return nexterr(f, -n);

	if (errwind() == FALSE)
		return FALSE;

	return forwerr(-1);
}

/*
 * Read in the error file, giving it a buffer.
 */
readerr()
{
	register BUFFER	*bp;
	uchar	bname[NBUFN];

	makename(bname, "error-buffer");	/* New buffer name.	*/
#if	GEM
	fixname(bname);
	fixname(errfile);
#endif
	if ((errbp = bp = bfind(bname, TRUE, 0)) == NULL) {
		mlwrite("Cannot create error buffer");
		return (FALSE);
	}
	if (--curbp->b_nwnd == 0) {		/* Undisplay.		*/
		curbp->b_dotp = curwp->w_dotp;
		curbp->b_doto = curwp->w_doto;
		curbp->b_markp = curwp->w_markp;
		curbp->b_marko = curwp->w_marko;
	}
	bp->b_flag |= BFNOWRT|BFERROR;		/* Mark the buffer...	*/
	curbp = bp;				/* Switch to it.	*/
	curwp->w_bufp = bp;
	curbp->b_nwnd++;
	return (readin(errfile));		/* Read it in.		*/
}
