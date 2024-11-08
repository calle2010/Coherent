/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		LOCAL
 *		USE_PROTO
 *		ARGS ()
 *	<stdlib.h>
 *		NULL
 *		malloc ()
 *		free ()
 */

#include <sys/compat.h>
#include <stdlib.h>

#include "ehand.h"
#include "symbol.h"
#include "read.h"

#include "mtune.h"

LOCAL mtune_t *	_mtunes;


/*
 * Locate a parameter definition by symbol.
 */

#ifdef	USE_PROTO
mtune_t * (find_mtune) (symbol_t * sym)
#else
mtune_t *
find_mtune ARGS ((sym))
symbol_t      *	sym;
#endif
{
	mtune_t	      *	scan;

	for (scan = _mtunes ; scan != NULL ; scan = scan->mt_next) {

		if (scan->mt_name == sym)
			return scan;
	}

	return NULL;
}


/*
 * Read lines from an "mtune" file.
 */

#ifdef	USE_PROTO
LOCAL int (read_mtune) (input_t * input, lex_t * lexp)
#else
LOCAL int
read_mtune ARGS ((input, lexp))
input_t	      *	input;
lex_t	      *	lexp;
#endif
{
	int		ch = '\n';
	mtune_t	      *	mtunep;
	ehand_t		err;

	if ((mtunep = (mtune_t *) malloc (sizeof (* mtunep))) == NULL)
		throw_error ("out of memory in read_mtune ()");

	if (PUSH_HANDLER (err) == 0) {
		/*
		 * If the first thing on the line works out to be an EOF,
		 * then bail out without an error.
		 */

		ch = read_symbol (input, lexp, & mtunep->mt_name);

		if (mtunep->mt_name == NULL) {
			/*
			 * We allow an EOF at the beginning of a line and we
			 * also allow a blank line.
			 */

			goto at_eof;
		}
		check_not_eol (ch);

		if (mtunep->mt_name->s_size > MAX_PARAMNAME)
			throw_error ("parameter name must be <= %d characters",
				     MAX_PARAMNAME);

		if (find_mtune (mtunep->mt_name) != NULL)
			throw_error ("parameter name must be unique");

		/*
		 * Pull in the parameter values
		 */

		ch = read_longs (input, lexp, & mtunep->mt_min, NO_RANGE);
		check_not_eol (ch);

		ch = read_longs (input, lexp, & mtunep->mt_default, NO_RANGE);
		check_not_eol (ch);

		ch = read_longs (input, lexp, & mtunep->mt_max, NO_RANGE);
		ch = expect_eol (input, lexp, ch);


		/*
		 * Having passed all the reasonableness checks, we link the
		 * new entry into the chain.
		 */

		mtunep->mt_stune = NULL;

		mtunep->mt_next = _mtunes;
		_mtunes = mtunep;
	} else {

		free (mtunep);
		CHAIN_ERROR (err);
	}

at_eof:
	POP_HANDLER (err);
	return ch;
}


#if 0
/*
 * Regenerate an 'mtune' line from the stored record.
 */

#ifdef	USE_PROTO
void (write_mtune) (mtune_t * mtunep, FILE * out)
#else
void
write_mtune ARGS ((mtunep, out))
mtune_t	      *	mtunep;
FILE	      *	out;
#endif
{
	(void) fprintf (out, "%-8s %-7ld %-7ld %-7ld\n",
			mtunep->mt_name->s_data,
			mtunep->mt_min, mtunep->mt_default,
			mtunep->mt_max);
}
#endif


/*
 * Return the head of the list of all tuneable parameters.
 */

#ifdef	USE_PROTO
mtune_t * (mtunes) (void)
#else
mtune_t *
mtunes ARGS (())
#endif
{
	return _mtunes;
}


/*
 * Suck in a complete 'mtune' file.
 */

#ifdef	USE_PROTO
void (read_mtune_file) (CONST char * name)
#else
void
read_mtune_file ARGS ((name))
CONST char    *	name;
#endif
{
	read_dev_file (name, read_mtune);
}
