#include "asm.h"

/*
 * This routine is called early in the
 * game to set up the hashtable. First all
 * buckets in the table are cleared.
 * Then a pass is made through the builtin
 * symbols, linking them into their hash
 * buckets.
 */
syminit()
{
	register struct sym  *sp;
	struct sym **spp;
	int ht;
	register char *p1;
	register nc;

	spp = &symhash[0];
	while (spp < &symhash[NHASH])
		*spp++ = NULL;
	sp = &sym[0];
	for (;;) {
		ht = 0;
		nc = NCPLN;
		p1 = sp->s_id;
		do {
			ht += *p1++;
		} while (--nc);
		sp->s_total = ht;
		ht &= HMASK;
		sp->s_sp = symhash[ht];
		symhash[ht] = sp;
		if ((sp->s_flag&S_END) != 0)
			break;
		++sp;
	}
}

/*
 * Mark all symbols of type `S_NEW'
 * global. Called at the end of pass
 * 0 if `-g'.
 */
symglob()
{
	register struct sym *sp;
	register i;

	for (i=0; i<NHASH; ++i) {
		sp = symhash[i];
		while (sp != NULL) {
			if (sp->s_kind == S_NEW)
				sp->s_flag |= S_GBL;
			sp = sp->s_sp;
		}
	}
}


/*
 * Allocate a block of space.
 * Leave if there is no space left
 * at all.
 */
char *
new(n)
{
	register char *p;

	if ((p = malloc(n)) == NULL) {
		fprintf(stderr, "Out of space.\n");
		exit(1);
	}
	return (p);
}

/*
 * Copy a name.
 */
symcopy(p1, p2)
register char *p1, *p2;
{
	register n;

	n = NCPLN;
	do {
		*p1++ = *p2++;
	} while (--n);
}
