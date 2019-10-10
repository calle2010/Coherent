/*
 * aha_dsl.c - routines for manipulating AHA-154x Data Segment Lists.
 * Part of the Adaptec SCSI driver.
 */

#include <sys/coherent.h>
#include <sys/buf.h>
#include <sys/scsiwork.h>
#include <sys/aha154x.h>
#include <sys/errno.h>

#ifndef LESSER
#define LESSER(a, b)	(((a)<(b))?(a):(b))
#endif /* LESSER */

extern unsigned long aha_p3_to_l();

/*
 * void
 * dsl_free( P3 dsl_ptr )
 *
 * Free a DSL generated by dsl_gen().
 */
void
dsl_free( dsl_ptr )
	P3 dsl_ptr;
{
	caddr_t dsl_vaddr;	/* Virtual address of DSL.  */

	T_PIGGY(0x4000, printf("dsl_free(%x:%x:%x)",
				dsl_ptr[0], dsl_ptr[1], dsl_ptr[2] ));

	dsl_vaddr = mem_recall( aha_p3_to_l( dsl_ptr ) );

	T_PIGGY(0x4000, printf(":%x, ", dsl_vaddr) );

	pfree( dsl_vaddr );
	mem_forget( dsl_vaddr );

} /* dsl_free() */

#if 1
#define MAX_ENTRIES	16	/* At most 16 DSL entries are allowed.  */
/*
 * void
 * dsl_gen(P3 new_dsl, paddr_t buffer, unsigned len)
 *
 * Generate a Data Segment List for virtual-physical buffer 'buffer' with
 * length 'len'.  Return a P3 pointer to this DSL in new_dsl.
 */
void
dsl_gen( new_dsl, new_len, buffer, len)
	P3 new_dsl, new_len;
	paddr_t buffer;
	unsigned long len;
{
	unsigned first;		/* Size of first chunk in bytes.  */
	unsigned rest;		/* len - first */
	unsigned middle;	/* Size of middle (aligned) chunk in bytes. */
	unsigned middle_clicks;	/* middle/NBPC */
	unsigned last;		/* Size of last chunk in bytes.  */

	unsigned total_entries;	/* Total number of entries in DSL.  */
	unsigned table_len;	/* total_entries * sizeof(DSL_ENTRY) */

	DSL_ENTRY *tmp_dsl;	/* Build DSL here.  */

	unsigned len_to_stuff;	/* How much buffer left to stuff into DSL?  */
	unsigned addr_to_stuff;	/* What address goes in the current DSL entry?  */

	int i;			/* Handy counter/index.  */

	T_PIGGY( 0x4000,
		 printf("dsl_gen(dsl: %x, new_len: %x, buf: %x, len: %x)", 
			new_dsl, new_len, buffer, len));

	/*
	 * The buffer can have up to three sections:
	 * first	-- from the start of the buffer
	 *		   to the start of the next click.
	 * middle	-- all full sized clicks
	 * last		-- the final partial click.
	 *
	 * A buffer must have at least a first section, may have only
	 * a first and last, first and middle, or may have all three.
	 */

	/*
	 * Calculate the sizes for all three sections in bytes.
	 */

	/*
	 * The first chunk is either the entire buffer, or up to then
	 * end of the first click.
	 */
	first = LESSER(len, ctob(btocrd(buffer + NBPC)) - buffer);

	rest = len - first;
	middle_clicks = rest / NBPC;	/* First calculate length in clicks.  */
	middle = middle_clicks * NBPC;	/* Convert to bytes.  */

	last = rest % NBPC;

	/* How many entries do we need in the DSL?  */
	total_entries = ((first > 0)?1:0) + middle_clicks + ((last > 0)?1:0);

	/* The AHA-154xB permits no more than MAX_ENTRIES DSL entries.  */
	if ( total_entries > MAX_ENTRIES ) {
		u.u_error = EINVAL;
		return;
	}

	/* How big a table do we need?  */
	table_len = total_entries * sizeof(DSL_ENTRY);

	/* Allocate the DSL.  */
	if (NULL == (tmp_dsl = (DSL_ENTRY *) palloc(table_len))) {
		u.u_error = ENOMEM;
		return;
	}
	
	/* Fill in the DSL.  */
	len_to_stuff = len;
	addr_to_stuff = buffer;

	/* First Part */
	aha_l_to_p3( first, tmp_dsl[0].size );
	aha_l_to_p3( vptop( addr_to_stuff ), tmp_dsl[0].addr );

	T_PIGGY(0x4000,
		printf("DSL first(size: %x:%x:%x, addr: %x:%x:%x)",
		tmp_dsl[0].size[0], tmp_dsl[0].size[1], tmp_dsl[0].size[2],
		tmp_dsl[0].addr[0], tmp_dsl[0].addr[1], tmp_dsl[0].addr[2]));
	len_to_stuff -= first;
	addr_to_stuff += first;

	/* Middle Part */
	i = 1;
	while (len_to_stuff > NBPC) {
		aha_l_to_p3( NBPC, tmp_dsl[i].size );
		aha_l_to_p3( vptop( addr_to_stuff  ), tmp_dsl[i].addr );

T_PIGGY(0x4000,
	printf("DSL middle:%x(size: %x:%x:%x, addr: %x:%x:%x)", i,
	tmp_dsl[i].size[0], tmp_dsl[i].size[1], tmp_dsl[i].size[2],
	tmp_dsl[i].addr[0], tmp_dsl[i].addr[1], tmp_dsl[i].addr[2]));
		len_to_stuff -= NBPC;
		addr_to_stuff += NBPC;
		++i;
	}

	/* Last Part */
	if (len_to_stuff > 0) {
		aha_l_to_p3( last, tmp_dsl[i].size );
		aha_l_to_p3( vptop( addr_to_stuff  ), tmp_dsl[i].addr );

T_PIGGY(0x4000,
	printf("DSL last:%x(size: %x:%x:%x, addr: %x:%x:%x)", i,
	tmp_dsl[i].size[0], tmp_dsl[i].size[1], tmp_dsl[i].size[2],
	tmp_dsl[i].addr[0], tmp_dsl[i].addr[1], tmp_dsl[i].addr[2]));

		len_to_stuff -= last;
		addr_to_stuff += last;
		++i;
	}

#if	0
	/*
	 * NIGEL: Trace macros must now be given expressions... this one
	 * wasn't worth cleaning up.
	 */

	T_PIGGY(0x4000, {
		/* Sanity Check.  */
		if ( i != total_entries ) {
			printf(
		"\ndsl_gen() insane: i: %x != total_entries: %x\n",
			i, total_entries );
		}
		if ( len_to_stuff != 0 ) {
			printf(
		"\ndsl_gen() insane: len_to_stuff: %x != 0\n",
			len_to_stuff);
		}
		if ( addr_to_stuff != (buffer + len) ) {
			printf(
		"\ndsl_gen() insane: addr_to_stuff: %x != (buffer + len): %x\n",
			addr_to_stuff, (buffer + len));
		}
	} ); /* T_PIGGY() */
#endif

	/* Fill in return values.  */
	aha_l_to_p3( table_len, new_len );
	aha_l_to_p3( vtop(tmp_dsl), new_dsl );
	mem_remember( tmp_dsl, aha_p3_to_l( new_dsl ));


	T_PIGGY( 0x4000, printf("new_dsl: %x:%x:%x, new_len: %x:%x:%x, ",
				new_dsl[0], new_dsl[1], new_dsl[2],
				new_len[0], new_len[1], new_len[2]));
} /* dsl_gen() */
#endif /* 1 */

