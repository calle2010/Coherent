/*
 * Modify an expression.
 * All machines.
 *
 * modify() calls modtree() which generally does left before right
 *  deepest first traversal, including constant folding and machine
 *  dependent modifications.  modify() then walk()'s the tree with
 *  modswap() and modrneg() and finally calls modleaf() to insert
 *  leaf nodes where required.
 *
 * mflag -
 *	0 < mflag - prints trees before and after modification
 *		unless in MINIT context, where you get after only.
 *	1 < mflag - prints trees at end of modtree()
 *	2 < mflag - prints trees at beginning of modtree()
 *	8 < mflag - gets multiple intermediate trees.
 */
#ifdef   vax
#include "INC$LIB:cc1.h"
#else
#include "cc1.h"
#endif

int	modswap();
int	modrneg();

/*
 * Tree modify.
 */
TREE *
modify(tp, c)
register TREE	*tp;
{
#if !TINY
	if (mflag > 0 && c != MINIT)
		snapf("%W%E", "Before modify", tp);
#endif
	tp = modtree(tp, c, NULL);
	walk(tp, modswap);
	walk(tp, modrneg);
#if !TINY
	if (mflag > 8 && c != MINIT)
		snapf("%W%E", "Before modleaf", tp);
#endif
	tp = modleaf(tp, c, NULL);
#if !TINY
	if (mflag > 0)
		snapf("%W%E%W", "After modify", tp, NULL);
#endif
	return (tp);
}

/*
 * Tree modifier.
 * This routine performs a number of machine independent things.
 * Folding of constant expressions is done by the routine 'modfold()'.
 * After all the machine independent things have been done,
 * a call to 'modoper()' does machine-specific modifications.
 */
TREE *
modtree(tp, ac, ptp)
register TREE	*tp;
TREE		*ptp;
{
	register TREE	*lp;
	register TREE	*rp;
	register TREE	*tp1;
	register int	c;
	register int	left;
	register int	n;
	register int	op;
	register int	tt, ts;

	c = ac;
	if (c==MINIT || c==MRETURN || c==MSWITCH)
		c = MRVALUE;
#if !TINY
	if (mflag > 2 && ac != MINIT)
		snapf("%W%E", "modtree before modfold", tp);
#endif
again:
	tp = modfold(tp);
#if !TINY
	if (mflag > 8 && ac != MINIT)
		snapf("%W%E", "modtree after modfold", tp);
#endif
	op = tp->t_op;
	if (isleaf(op))
		goto done;
	lp = tp->t_lp;
	rp = NULL;
	if (op != FIELD)
		rp = tp->t_rp;
	tt = tp->t_type;
	ts = tp->t_size;
	/*
	 * Change '(reg = a) + b' into 'reg = a, reg + b'.
	 */
	if (isokasgn(op, lp)) {
		lp = leftnode(COMMA, lp, lp->t_type, lp->t_size);
		lp->t_rp = tp;
		tp->t_lp = copynode(lp->t_lp->t_lp);
		tp = lp;
		goto again;
	}
	/*
	 * Now change 'b + (reg = a)' into 'reg = a, b + reg'.
	 */
	if (isokasgn(op, rp)) {
		rp = leftnode(COMMA, rp, rp->t_type, rp->t_size);
		rp->t_rp = tp;
		tp->t_rp = copynode(rp->t_lp->t_lp);
		tp = rp;
		goto again;
	}
	switch (op) {

	case BLKMOVE:
		tp->t_lp = modtree(lp, MRVALUE, tp);
		tp->t_rp = modtree(rp, MRVALUE, tp);
		break;

	case CALL:
		tp = modcall(tp, c);
		break;

	case FIELD:
		tp->t_lp = modtree(lp, c, tp);
		break;

	case QUEST:
		tp->t_lp = modtree(lp, MFLOW, tp);
		if (rp->t_op != COLON)
			cbotch("no ':'");
		rp->t_lp = modtree(rp->t_lp, c, rp);
		rp->t_rp = modtree(rp->t_rp, c, rp);
		break;

	case COLON:
		cbotch("misplaced ':'");
		tp->t_op = COMMA;

	case COMMA:
		if (isfxcon(lp) || lp->t_op == DCON) {
			tp = rp;
			goto again;
		}
		tp->t_lp = modtree(lp, MEFFECT, tp);
		tp->t_rp = modtree(rp, c, tp);
		break;

	case NOT:
		if (c != MFLOW) {
			tp = modtruth(tp);
			goto again;
		}
		lp = modtree(lp, MFLOW, tp);
		goto unary;

	case COM:
	case NEG:
		lp = modtree(lp, (ac==MINIT?MINIT:MRVALUE), tp);
	unary:
		if (lp->t_op == op) {
			lp = lp->t_lp;
			lp->t_type = tt;
			lp->t_size = ts;
			tp = lp;
			goto again;
		}
		tp->t_lp = lp;
		break;

	case CONVERT:
	case CAST:
		/* Distribute conversion to last comma node. */
		if (lp->t_op==COMMA) {
			rp = lp;
			rp->t_type = tt;
			while (rp->t_rp->t_op==COMMA) {
				rp->t_type = tt;
				rp = rp->t_rp;
			}
			tp->t_lp = rp->t_rp;
			rp->t_rp = tp;
			tp = lp;
			goto again;
		}
		/* Distribute conversion to subtrees of COLON. */
		if (lp->t_op==QUEST) {
			rp = lp;
			rp->t_type = tt;
			rp = rp->t_rp;
			rp->t_type = tt;
			tp->t_lp = rp->t_lp;
			rp->t_lp = tp;
			rp->t_rp = leftnode(tp->t_op, rp->t_rp, tt);
			tp = lp;
			goto again;
		}
		lp = modtree(lp, MRVALUE, tp);
		if (tt==lp->t_type && ts==lp->t_size) {
			tp = lp;
			goto again;
		}
		tp->t_lp = lp;
		break;

	case OROR:
	case ANDAND:
		if (c != MFLOW) {
			tp = modtruth(tp);
			goto again;
		}
		tp->t_lp = modtree(lp, MFLOW, tp);
		tp->t_rp = modtree(rp, MFLOW, tp);
		break;

	case INCAFT:
	case DECAFT:
		if (c == MEFFECT)
			tp->t_op += INCBEF-INCAFT;

	case INCBEF:
	case DECBEF:
		tp->t_lp = modtree(lp, MLADDR, tp);
		if (isflt(tt)) {
			tp->t_op = (op==INCBEF || op==INCAFT) ? AADD : ASUB;
			if (op==INCAFT || op==DECAFT) {
				tp->t_lp = copynode(tp);
				tp->t_rp = leftnode(CONVERT, ivalnode(1), F64);
				tp->t_op = (op==INCAFT)?SUB:ADD;
			}
			goto again;
		}
		tp->t_rp = modtree(basenode(rp), MRADDR, tp);
		break;

	case ADDR:
		lp = modtree(lp, (ac==MINIT?MINIT:MLADDR), tp);
		if (lp->t_op == STAR) {
			lp = lp->t_lp;
			if ( ! isblkp(lp->t_type)) {
				lp->t_type = tt;
				lp->t_size = ts;
			}
			tp = lp;
			goto again;
		} else if (lp->t_op == QUEST) {
			/* Change '&(a?b:c)' to 'a?&b:&c'. */
			rp = lp->t_rp;		/* COLON node */
			rp->t_lp = leftnode(ADDR, rp->t_lp, tt, ts);
			rp->t_rp = leftnode(ADDR, rp->t_rp, tt, ts);
			rp->t_type = lp->t_type = tt;
			rp->t_size = lp->t_size = ts;
			tp = lp;
			goto again;
		}
		tp->t_lp = lp;
		break;

	case STAR:
		lp = modtree(lp, (ac==MINIT?MINIT:MRADDR), tp);
		if (lp->t_op == ADDR) {
			lp = lp->t_lp;
			if ( ! isblkp(lp->t_type)) {
				lp->t_type = tt;
				lp->t_size = ts;
			}
			tp = lp;
			goto again;
		}
		tp->t_lp = lp;
		break;

	case ASSIGN:
		tp->t_lp = modtree(lp, MLADDR,  tp);
		tp->t_rp = modtree(rp, MRVALUE, tp);
		break;

	case DIV:
	case REM:
	case ADIV:
	case AREM:
		lp = modtree(lp, lgoal(op), tp);
		rp = modtree(rp, MRADDR, tp);
		if (((n = ispow2(rp)) >= 0) && ((n==0) || isuns(tt))) {
			if (op==DIV || op==ADIV) {
				tp->t_op += SHR-DIV;
				tp->t_lp  = lp;
				tp->t_rp  = ivalnode(n);
				goto again;
			}
			else {
				tp->t_op += AND-REM;
				tp->t_lp  = lp;
				tp->t_rp = gvalnode(tt, ((long)01<<n)-1);
				goto again;
			}
		}
		tp->t_lp = lp;
		tp->t_rp = rp;
		break;

	case MUL:
	case AMUL:
		lp = modtree(lp, lgoal(op), tp);
		rp = modtree(rp, MRADDR, tp);
		if ((tp1=modmul(tp, lp, rp)) != NULL
		|| (op==MUL && (tp1=modmul(tp, rp, lp))!=NULL)) {
			tp = tp1;
			goto again;
		}
		tp->t_lp = lp;
		tp->t_rp = rp;
		break;

	case ADD:
	case SUB:
	case AND:
	case OR:
	case XOR:
	case SHR:
	case SHL:
		lp = modtree(lp, (ac==MINIT?MINIT:MRVALUE), tp);
		rp = modtree(rp, (ac==MINIT?MINIT:MRADDR),  tp);
		if (isnval(rp, 0)) {
			if (op == AND) {
				rp->t_type = tt;
				rp->t_size = ts;
				tp = rp;
				goto again;
			}
			lp->t_type = tt;
			lp->t_size = ts;
			tp = lp;
			goto again;
		}
		if (op == SUB
		&&  rp->t_op == ICON
		&& (isuns(rp->t_type)==0 || rp->t_ival>=0)) {
			tp->t_op += ADD-SUB;
			tp->t_type = IVAL_T;	/* Signed */
			rp->t_ival = -rp->t_ival;
			amd(rp);
			tp->t_lp  = lp;
			tp->t_rp  = rp;
			goto again;
		}
		tp->t_lp = lp;
		tp->t_rp = rp;
		break;

	case AADD:
	case ASUB:
	case AAND:
	case AOR:
	case AXOR:
	case ASHL:
	case ASHR:
		lp = modtree(lp, MLADDR, tp);
		rp = modtree(rp, MRADDR, tp);
		if (isnval(rp, 0)) {
			if (op == AAND) {
				tp->t_op = ASSIGN;
				tp->t_lp = lp;
				tp->t_rp = rp;
				goto again;
			}
			lp->t_type = tt;
			lp->t_size = ts;
			tp = lp;
			goto again;
		}
		tp->t_lp = lp;
		tp->t_rp = rp;
		break;

	case EQ:
	case NE:
	case GT:
	case LT:
	case GE:
	case LE:
	case UGT:
	case UGE:
	case ULT:
	case ULE:
		if (c != MFLOW) {
			tp = modtruth(tp);
			goto again;
		}
		lp = modfold(lp);
		rp = modfold(rp);
		if ((left=isnval(lp, 0)) || isnval(rp, 0)) {
			if (left) {
				tp->t_op = op = fliprel[op-EQ];
				tp1 = lp;
				lp  = rp;
				rp  = tp1;
			}
			if ((op==EQ || op==NE) && lp->t_op != QUEST)
				lp = modtree(lp, MFLOW,   tp);
			else
				lp = modtree(lp, MRVALUE, tp);
		} else
			lp = modtree(lp, MRVALUE, tp);
		tp->t_lp = lp;
		tp->t_rp = modtree(rp, MRADDR, tp);
		break;
	}
	tp = modfold(tp);
done:
	walk(tp, amd);
	if ((tp1 = modoper(tp, ac, ptp)) != NULL) {
		tp = tp1;
		goto again;
	}
#if !TINY
	if (mflag > 1 && ac != MINIT)
		snapf("%W%E", "modtree done", tp);
#endif
	return (tp);
}

/*
 * Modify multiply.
 * Called from the modify case for MUL and AMUL.
 * If MUL, called twice in both arrangements.
 */
TREE *
modmul(tp, lp, rp)
register TREE	*tp, *lp, *rp;
{
	register n;

	if (isnval(rp, 0)) {
		if (tp->t_op == AMUL) {
			tp->t_op = ASSIGN;
			return(tp);
		}
		rp->t_type = tp->t_type;
		rp->t_size = tp->t_size;
		return (rp);
	}
	if ((n=ispow2(rp)) >= 0) {
		tp->t_op += SHL-MUL;
		tp->t_lp = lp;
		tp->t_rp = ivalnode(n);
		return (tp);
	}
	return (NULL);
}

/*
 * This routine fabricates a fake question colon pair
 * to get the correct return value for !, &&, || and relational ops
 * in non flow contexts.
 */
TREE *
modtruth(tp)
register TREE	*tp;
{
	register TREE *tp1;

	tp1 = ivalnode(1);
	tp1 = leftnode(COLON, tp1, TRUTH);
	tp1->t_rp = ivalnode(0);
	tp  = leftnode(QUEST, tp,  TRUTH);
	tp->t_rp = tp1;
	return (tp);
}

/*
 * Find left subgoal.
 * This permits '+' and '+=' to share a lot of code.
 */
lgoal(op)
register op;
{
	if (op>=AADD && op<=ASHR)
		return (MLADDR);
	return (MRVALUE);
}

/*
 * Look for '+' of a negative constant and make it into a subtract.
 * Machines tend to have 'dec' instructions for small numbers.
 */
modrneg(tp, ptp)
register TREE	*tp;
TREE		*ptp;
{
	register TREE *rp;

	if (tp->t_op==ADD || tp->t_op==AADD) {
		rp = tp->t_rp;
		if (rp->t_op==ICON && !isuns(rp->t_type) && rp->t_ival<0) {
			tp->t_op += SUB-ADD;
			rp->t_ival = -rp->t_ival;
			amd(rp);
		}
	}
}

/*
 * Check assignment transformation for legality.
 */
isokasgn(op, tp)
register op;
register TREE	*tp;
{
	register top;

	if (op!=ASSIGN && (op<ADD || op>SHR))
		return (0);
	if (tp == NULL)
		return (0);
	top = tp->t_op;
	if (top!=ASSIGN && (top<AADD || top>ASHR))
		return (0);
	return (isokareg(tp, op));
}

/*
 * If the tree is a constant power of two, return the log
 * base two of the number.
 * If not, return -1.
 */
ispow2(tp)
register TREE	*tp;
{
	register i;
	register long n;

	if (!isfxcon(tp))
		return (-1);
	n = grabnval(tp);
	if (n<1 || (n&(n-1))!=0)
		return (-1);
	i = 0;
	while ((n&01) == 0) {
		++i;
		n >>= 1;
	}
	return (i);
}
