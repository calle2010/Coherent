/////////
/
/ Assigned add.
/
/////////

AADD:
%	PEFFECT|PRVALUE|PREL
	WORD		ANYR	*	*	TEMP
		ADR|LV		WORD
		1|MMX		*
%	PLVALUE
	WORD		ANYL	*	*	TEMP
		ADR|LV		WORD
		1|MMX		*
			[ZINC]	[AL]
		[IFV]	[ZMOV]	[R],[AL]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL
	WORD		AX	*	*	AX
		ADR|LV		FS8
		1|MMX		*
			[ZINCB]	[AL]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZCBW]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL
	WORD		AX	*	*	AX
		ADR|LV		FU8
		1|MMX		*
			[ZINCB]	[AL]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZSUBB]	[HI R],[HI R]
		[IFR]	[REL0]	[LAB]

#ifndef ONLYSMALL
%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		1|MMX		*
			[ZINC]	[AL]
			[ZMOV]	[LO R],[LO AL]
			[ZMOV]	[HI R],[HI AL]

%	PLVALUE|PEFFECT
	LPTX		ANYL	*	*	TEMP
		ADR|LV		LPTX
		1|MMX		*
			[ZINC]	[AL]
		[IFV]	[ZLDES]	[LO R],[AL]
#endif

%	PEFFECT|PRVALUE|PREL
	WORD		ANYR	*	*	TEMP
		REG|MMX		WORD
		ADR|IMM		WORD
%	PLVALUE
	WORD		ANYL	*	*	TEMP
		REG|MMX		WORD
		ADR|IMM		WORD
			[ZADD]	[AL],[AR]
		[IFV]	[ZMOV]	[R],[AL]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL
	WORD		ANYR	*	*	TEMP
		ADR|LV		WORD
		IMM|MMX		WORD
%	PEFFECT|PRVALUE|PREL
	WORD		ANYR	*	*	TEMP
		ADR|LV		WORD
		REG|MMX		WORD
%	PLVALUE
	WORD		ANYL	*	*	TEMP
		ADR|LV		WORD
		IMM|MMX		WORD
%	PLVALUE
	WORD		ANYL	*	*	TEMP
		ADR|LV		WORD
		REG|MMX		WORD
			[ZADD]	[AL],[AR]
		[IFV]	[ZMOV]	[R],[AL]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL
	WORD		AX	*	*	AX
		ADR|LV		FS8
		IMM|MMX		WORD
			[ZADDB]	[AL],[AR]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZCBW]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL
	WORD		AX	*	*	AX
		ADR|LV		FU8
		IMM|MMX		WORD
			[ZADDB]	[AL],[AR]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZSUBB]	[HI R],[HI R]
		[IFR]	[REL0]	[LAB]

#ifndef ONLYSMALL
%	PRVALUE
	LPTX		ANYR	*	*	TEMP
		ADR|LV		LPTX
		IMM|MMX		WORD
			[ZADD]	[AL],[AR]
			[ZMOV]	[LO R],[LO AL]
			[ZMOV]	[HI R],[HI AL]

%	PLVALUE|PEFFECT
	LPTX		ANYL	*	*	TEMP
		ADR|LV		LPTX
		IMM|MMX		WORD
			[ZADD]	[AL],[AR]
		[IFV]	[ZLDES]	[LO R],[AL]
#endif

%	PEFFECT|PRVALUE|PLT|PGE
	LONG		DXAX	*	*	DXAX
		ADR|LV		LONG
		LHC|MMX		LONG
			[ZADD]	[HI AL],[HI AR]
		[IFV]	[ZMOV]	[LO R],[LO AL]
		[IFV]	[ZMOV]	[HI R],[HI AL]
		[IFR]	[REL1]	[LAB]

%	PEFFECT|PRVALUE|PLT|PGE
	LONG		DXAX	*	*	DXAX
		ADR|LV		LONG
		IMM|MMX		LONG
			[ZADD]	[LO AL],[LO AR]
			[ZADC]	[HI AL],[HI AR]
		[IFV]	[ZMOV]	[LO R],[LO AL]
		[IFV]	[ZMOV]	[HI R],[HI AL]
		[IFR]	[REL1]	[LAB]

%	PEFFECT|PRVALUE|PREL|P_SRT
	WORD		ANYR	*	ANYR	TEMP
		ADR|LV		WORD
		TREG		WORD
%	PLVALUE|P_SRT
	WORD		ANYL	*	ANYL	TEMP
		ADR|LV		WORD
		TREG		WORD
			[ZADD]	[AL],[R]
		[IFV]	[ZMOV]	[R],[AL]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL|P_SRT
	WORD		AX	*	AX	AX
		ADR|LV		FS8
		TREG		WORD
			[ZADDB]	[AL],[LO R]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZCBW]
		[IFR]	[REL0]	[LAB]

%	PEFFECT|PRVALUE|PREL|P_SRT
	WORD		AX	AX	*	AX
		ADR|LV		FU8
		TREG		WORD
			[ZADDB]	[AL],[LO R]
		[IFV]	[ZMOVB]	[LO R],[AL]
		[IFV]	[ZSUBB]	[HI R],[HI R]
		[IFR]	[REL0]	[LAB]

#ifndef ONLYSMALL
%	PRVALUE
	LPTX		ANYR	*	ANYR	TEMP
		ADR|LV		LPTX
		TREG		WORD
			[ZADD]	[AL],[RR]
			[ZMOV]	[LO R],[LO AL]
			[ZMOV]	[HI R],[HI AL]

%	PLVALUE|PEFFECT
	LPTX		ANYL	*	ANYR	TEMP
		ADR|LV		LPTX
		TREG		WORD
			[ZADD]	[AL],[RR]
		[IFV]	[ZLDES]	[LO R],[AL]
#endif

%	PRVALUE|PREL
	WORD		ANYR	*	*	TEMP
		ADR|LV		WORD
		ADR|IMM		WORD
%	PLVALUE
	WORD		ANYL	*	*	TEMP
		ADR|LV		WORD
		ADR|IMM		WORD
			[ZMOV]	[R],[AL]
			[ZADD]	[R],[AR]
			[ZMOV]	[AL],[R]
		[IFR]	[REL0]	[LAB]

%	PRVALUE|PREL
	WORD		AX	*	*	AX
		ADR|LV		FS8
		ADR|IMM		WORD
			[ZMOVB]	[LO R],[AL]
			[ZCBW]
			[ZADD]	[R],[AR]
			[ZMOVB]	[AL],[LO R]
		[IFR]	[REL0]	[LAB]

%	PRVALUE|PREL
	WORD		AX	*	*	AX
		ADR|LV		FU8
		ADR|IMM		WORD
			[ZMOVB]	[LO R],[AL]
			[ZSUBB]	[HI R],[HI R]
			[ZADD]	[R],[AR]
			[ZMOVB]	[AL],[LO R]
		[IFR]	[REL0]	[LAB]

%	PEFFECT
	LONG		DXAX	*	*	NONE
		ADR|LV		LONG
		DIR|MMX		FS16
			[ZMOV]	[LO R],[AR]
			[ZCWD]
			[ZADD]	[LO AL],[LO R]
			[ZADC]	[HI AL],[HI R]

%	PEFFECT
	LONG		DXAX	*	*	NONE
		ADR|LV		LONG
		DIR|MMX		FU16
			[ZMOV]	[LO R],[AR]
			[ZSUB]	[HI R],[HI R]
			[ZADD]	[LO AL],[LO R]
			[ZADC]	[HI AL],[HI R]

%	PEFFECT
	LONG		DXAX	*	DXAX	NONE
		ADR|LV		LONG
		TREG		LONG
			[ZADD]	[LO AL],[LO R]
			[ZADC]	[HI AL],[HI R]

%	PRVALUE|PLT|PGE|P_SRT
	LONG		DXAX	*	DXAX	DXAX
		ADR|LV		LONG
		TREG		LONG
			[ZADD]	[LO R],[LO AL]
			[ZADC]	[HI R],[HI AL]
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[HI R]
		[IFR]	[REL1]	[LAB]

%	PRVALUE|PLT|PGE
	LONG		DXAX	*	*	DXAX
		ADR|LV		LONG
		ADR|IMM		LONG
			[ZMOV]	[LO R],[LO AL]
			[ZMOV]	[HI R],[HI AL]
			[ZADD]	[LO R],[LO AR]
			[ZADC]	[HI R],[HI AR]
			[ZMOV]	[LO AL],[LO R]
			[ZMOV]	[HI AL],[HI R]
		[IFR]	[REL1]	[LAB]

/////////
/
/ The left hand operand is an lvalue bit field.
/ The right operand has been preshifted to the appropriate place.
/ If the value is required, there is a shift on top of the '+=' or '-=' node.
/ Grab the machine object that holds the field, add or subtract
/ the right operand from it (performing the computation in the field),
/ extract the result and replace the value in the machine object.
/
/////////

%	PRVALUE
	WORD		AX	*	*	AX
		ADR|LV		FFLD8
		ADR|IMM		WORD
			[ZMOVB]	[LO R],[AL]
			[ZCBW]
			[ZADD]	[R],[AR]
			[ZAND]	[R],[LO EMASK]
			[ZANDB]	[AL],[LO CMASK]
			[ZORB]	[AL],[R]

%	PRVALUE
	WORD		ANYR	*	*	TEMP
		ADR|LV		FFLD16
		ADR|IMM		WORD
			[ZMOV]	[R],[AL]
			[ZADD]	[R],[AR]
			[ZAND]	[R],[LO EMASK]
			[ZAND]	[AL],[LO CMASK]
			[ZOR]	[AL],[R]

////////
/
/ Floating point, using the numeric data coprocessor (8087).
/
////////

#ifdef NDPDEF
%	PEFFECT|PRVALUE
	FF32|FF64	FPAC	*	FPAC	FPAC
		ADR|LV		FF32
		TREG		FF64
			[ZFADDF] [AL]
		[IFV]	[ZFSTF] [AL]
		[IFE]	[ZFSTPF] [AL]

%	PEFFECT|PRVALUE
	FF32|FF64	FPAC	*	FPAC	FPAC
		ADR|LV		FF64
		TREG		FF64
			[ZFADDD] [AL]
		[IFV]	[ZFSTD]	[AL]
		[IFE]	[ZFSTPD] [AL]
#endif
