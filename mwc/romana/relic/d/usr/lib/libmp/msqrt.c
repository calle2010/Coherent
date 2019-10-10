#include "mprec.h"


/*
 *	Msqrt sets the mint pointed to by "b" to the greatest integer
 *	which is less than or equal to the square root ot the mint
 *	pointed to by "a".  It sets the mint pointed to by "r" to the
 *	remainder (ie. r = a - b * b).  If "a" is negative, then mperr
 *	is called with the appropriate error message.  Note that no 
 *	assumption is made as to the distinction of "a", "b" and "r"
 *	except that "b" must not equal "r".
 */

void
msqrt(a, b, r)
register mint *a, *b, *r;
{
	mint	x, xs, tx, rem;

	if (!ispos(a))
		mperr("square root of negative number");

	/* initialize temps */
	minit(&x);
	minit(&xs);
	minit(&rem);
	minit(&tx);


	/* get initial guess */
	mitom(BASE, &x);
	spow(&x, a->len / 2, &x);

	/* loop till x * x <= a < (x + 1) * (x + 1) */
	for (;;) {
		mult(&x, &x, &xs);
		msub(a, &xs, &rem);
		madd(&x, &x, &tx);
		if (ispos(&rem) && mcmp(&rem, &tx) <= 0)
			break;
		madd(a, &xs, &xs);
		mdiv(&xs, &tx, &x, &xs);
	}

	/* throw away garbage and return results */
	mpfree(b->val);
	*b = x;
	mpfree(r->val);
	*r = rem;
	mpfree(xs.val);
	mpfree(tx.val);
}
