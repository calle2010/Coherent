//////////
/ n1/i386/tables/ashift.t
//////////

/////////
/
/ Assigned shift left (<<=), assigned shift right (>>=).
/ Signed and unsigned right shifts are different (SAR != SHR).
/ Signed and unsigned left shifts are identical (SAL == SHL).
/ Shifts by byte immediates on are done directly.
/ Harder shifts use the CL shift opcode.
/ The [TL OP0] macro maps ZSAL or ZSAR into a signed or unsigned
/ shift of the appropriate width, e.g. ZSAR could become ZSHR or ZSARB, etc.
/ The [OP1] macro is just the unsigned shift ZSHL or ZSHR.
/ This makes this table much shorter; see n1/i386/outmch.c for the gory details.
/ The result of a shift by more than 32 bits is undefined behavior.
/ This table simply generates a shift by the arg,
/ and the i386 uses the shift count modulo 32.
/
/////////

ASHL:
ASHR:

//////////
/ Longs.
//////////

/ Integer op= byte constant.
%	PEFFECT|PVALUE|PREL
	LONG		ANYR	*	*	TEMP
		ADR|LV		SINT|UINT
		SHCNT|MMX	LONG
			[TL OP0]	[AL],[AR]
		[IFV]	[TL ZMOVSX]	[R],[AL]
		[IFR]	[REL0]		[LAB]

#ifdef	BOGUS
/ This entry generates much better code
/ than the non-BOGUS entries which replaces it,
/ but it is wrong when [AL] uses ECX for indexing (it gets clobbered).
/ Retained here in hope of seeing a better way out of this mess sometime.
/ Integer op= long.
%	PEFFECT|PVALUE
	LONG		ANYR	*	ECX	TEMP
		ADR|LV		SINT|UINT
		ADR		LONG
			[ZMOV]		[REGNO ECX],[AR]
			[TL OP0]	[AL],[REGNO CL]
		[IFV]	[TL ZMOVSX]	[R],[AL]
#endif

/ Long op= long.
/ Save and restore ECX in case [AL] uses it, cf. remarks above.
%	PEFFECT|PVALUE
	LONG		ANYR	*	*	TEMP
		ADR|LV		LONG
		ADR		LONG
			[ZMOV]		[R],[AL]
			[ZPUSH]		[REGNO ECX]	/ save in case [AL] uses it
			[ZMOV]		[REGNO ECX],[AR]
			[TL OP0]	[R],[REGNO CL]
			[ZPOP]		[REGNO ECX]	/ and restore
			[ZMOV]		[AL],[R]

//////////
/ Words and bytes.
//////////

/ Short op= long.
/ Save and restore ECX in case [AL] uses it, cf. remarks above.
/ The [IFV] is necessary e.g. when negative signed arg gets left shifted to 0.
%	PEFFECT|PVALUE|PBYTE
	LONG		ANYR	*	*	TEMP
		ADR|LV		SHORT
		ADR		LONG
			[TL ZMOVSX]	[R],[AL]
			[ZPUSH]		[REGNO ECX]
			[ZMOV]		[REGNO ECX],[AR]
			[TL OP0]	[TL R],[REGNO CL]
			[ZPOP]		[REGNO ECX]
			[TL ZMOV]	[AL],[TL R]
		[IFV]	[TL ZMOVSX]	[R], [TL R]

//////////
/ Fields.
//////////

/ Field op= byte constant.
%	PEFFECT|PVALUE|PBYTE
	LONG		ANYR	*	*	TEMP
		ADR|LV		FLD
		SHCNT|MMX	LONG
			[TL ZMOVZX]	[R],[AL]	/ fetch
			[ZAND]		[R],[EMASK]	/ extract
			[OP1]		[R],[AR]	/ shift
			[ZAND]		[R],[EMASK]	/ mask
			[TL ZAND]	[AL],[TL CMASK]	/ clear
			[TL ZOR]	[AL],[TL R]	/ store

/ Long field op= long.
%	PEFFECT|PVALUE|PBYTE
	LONG		ANYR	*	*	TEMP
		ADR|LV		FLD
		ADR		LONG
			[TL ZMOVZX]	[R],[AL]	/ fetch
			[ZAND]		[R],[EMASK]	/ extract
			[ZPUSH]		[REGNO ECX]	/ save in case [AL] uses it
			[ZMOV]		[REGNO ECX],[AR] / count to CL
			[OP1]		[R],[REGNO CL]	/ shift
			[ZPOP]		[REGNO ECX]	/ restore
			[ZAND]		[R],[EMASK]	/ mask
			[TL ZAND]	[AL],[TL CMASK]	/ clear
			[TL ZOR]	[AL],[TL R]	/ store

//////////
/ end of n1/i386/tables/ashift.t
//////////
