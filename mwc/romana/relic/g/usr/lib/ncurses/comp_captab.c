/*
 *	comp_captab.c -- The names of the capabilities in a form ready for
 *		         the making of a hash table for the compiler.
 *
 */


#include "compiler.h"
#include "term.h"


struct name_table_entry	cap_table[] =
{
	0,           "bw",	BOOLEAN,	  0,
	0,           "am",	BOOLEAN,	  1,
	0,          "xsb",	BOOLEAN,	  2,
	0,          "xhp",	BOOLEAN,	  3,
	0,         "xenl",	BOOLEAN,	  4,
	0,           "eo",	BOOLEAN,	  5,
	0,           "gn",	BOOLEAN,	  6,
	0,           "hc",	BOOLEAN,	  7,
	0,           "km",	BOOLEAN,	  8,
	0,           "hs",	BOOLEAN,	  9,
	0,           "in",	BOOLEAN,	 10,
	0,           "da",	BOOLEAN,	 11,
	0,           "db",	BOOLEAN,	 12,
	0,          "mir",	BOOLEAN,	 13,
	0,         "msgr",	BOOLEAN,	 14,
	0,           "os",	BOOLEAN,	 15,
	0,        "eslok",	BOOLEAN,	 16,
	0,           "xt",	BOOLEAN,	 17,
	0,           "hz",	BOOLEAN,	 18,
	0,           "ul",	BOOLEAN,	 19,
	0,          "xon",	BOOLEAN,	 20,
	0,         "nxon",	BOOLEAN,	 21,
	0,         "mc5i",	BOOLEAN,	 22,
	0,         "chts",	BOOLEAN,	 23,
	0,        "nrrmc",	BOOLEAN,	 24,
	0,          "npc",	BOOLEAN,	 25,
	0,        "ndscr",	BOOLEAN,	 26,
	0,          "ccc",	BOOLEAN,	 27,
	0,          "bce",	BOOLEAN,	 28,
	0,          "hls",	BOOLEAN,	 29,
	0,         "xhpa",	BOOLEAN,	 30,
	0,         "crxm",	BOOLEAN,	 31,
	0,        "daisy",	BOOLEAN,	 32,
	0,         "xvpa",	BOOLEAN,	 33,
	0,          "sam",	BOOLEAN,	 34,
	0,         "cpix",	BOOLEAN,	 35,
	0,         "lpix",	BOOLEAN,	 36,
	0,         "cols",	NUMBER,		  0,
	0,           "it",	NUMBER,		  1,
	0,        "lines",	NUMBER,		  2,
	0,           "lm",	NUMBER,		  3,
	0,          "xmc",	NUMBER,		  4,
	0,           "pb",	NUMBER,		  5,
	0,           "vt",	NUMBER,		  6,
	0,          "wsl",	NUMBER,		  7,
	0,         "nlab",	NUMBER,		  8,
	0,           "lh",	NUMBER,		  9,
	0,           "lw",	NUMBER,		 10,
	0,           "ma",	NUMBER,		 11,
	0,         "wnum",	NUMBER,		 12,
	0,       "colors",	NUMBER,		 13,
	0,        "pairs",	NUMBER,		 14,
	0,          "ncv",	NUMBER,		 15,
	0,        "bufsz",	NUMBER,		 16,
	0,        "spinv",	NUMBER,		 17,
	0,        "spinh",	NUMBER,		 18,
	0,        "maddr",	NUMBER,		 19,
	0,        "mjump",	NUMBER,		 20,
	0,          "mcs",	NUMBER,		 21,
	0,          "mls",	NUMBER,		 22,
	0,        "npins",	NUMBER,		 23,
	0,          "orc",	NUMBER,		 24,
	0,          "orl",	NUMBER,		 25,
	0,         "orhi",	NUMBER,		 26,
	0,         "orvi",	NUMBER,		 27,
	0,          "cps",	NUMBER,		 28,
	0,        "widcs",	NUMBER,		 29,
	0,          "cbt",	STRING,		  0,
	0,          "bel",	STRING,		  1,
	0,           "cr",	STRING,		  2,
	0,          "csr",	STRING,		  3,
	0,          "tbc",	STRING,		  4,
	0,        "clear",	STRING,		  5,
	0,           "el",	STRING,		  6,
	0,           "ed",	STRING,		  7,
	0,          "hpa",	STRING,		  8,
	0,        "cmdch",	STRING,		  9,
	0,          "cup",	STRING,		 10,
	0,         "cud1",	STRING,		 11,
	0,         "home",	STRING,		 12,
	0,        "civis",	STRING,		 13,
	0,         "cub1",	STRING,		 14,
	0,        "mrcup",	STRING,		 15,
	0,        "cnorm",	STRING,		 16,
	0,         "cuf1",	STRING,		 17,
	0,           "ll",	STRING,		 18,
	0,         "cuu1",	STRING,		 19,
	0,        "cvvis",	STRING,		 20,
	0,         "dch1",	STRING,		 21,
	0,          "dl1",	STRING,		 22,
	0,          "dsl",	STRING,		 23,
	0,           "hd",	STRING,		 24,
	0,        "smacs",	STRING,		 25,
	0,        "blink",	STRING,		 26,
	0,         "bold",	STRING,		 27,
	0,        "smcup",	STRING,		 28,
	0,         "smdc",	STRING,		 29,
	0,          "dim",	STRING,		 30,
	0,         "smir",	STRING,		 31,
	0,        "invis",	STRING,		 32,
	0,         "prot",	STRING,		 33,
	0,          "rev",	STRING,		 34,
	0,         "smso",	STRING,		 35,
	0,         "smul",	STRING,		 36,
	0,          "ech",	STRING,		 37,
	0,        "rmacs",	STRING,		 38,
	0,         "sgr0",	STRING,		 39,
	0,        "rmcup",	STRING,		 40,
	0,         "rmdc",	STRING,		 41,
	0,         "rmir",	STRING,		 42,
	0,         "rmso",	STRING,		 43,
	0,         "rmul",	STRING,		 44,
	0,        "flash",	STRING,		 45,
	0,           "ff",	STRING,		 46,
	0,          "fsl",	STRING,		 47,
	0,          "is1",	STRING,		 48,
	0,          "is2",	STRING,		 49,
	0,          "is3",	STRING,		 50,
	0,           "if",	STRING,		 51,
	0,         "ich1",	STRING,		 52,
	0,          "il1",	STRING,		 53,
	0,           "ip",	STRING,		 54,
	0,          "kbs",	STRING,		 55,
	0,         "ktbc",	STRING,		 56,
	0,         "kclr",	STRING,		 57,
	0,        "kctab",	STRING,		 58,
	0,        "kdch1",	STRING,		 59,
	0,         "kdl1",	STRING,		 60,
	0,        "kcud1",	STRING,		 61,
	0,        "krmir",	STRING,		 62,
	0,          "kel",	STRING,		 63,
	0,          "ked",	STRING,		 64,
	0,          "kf0",	STRING,		 65,
	0,          "kf1",	STRING,		 66,
	0,         "kf10",	STRING,		 67,
	0,          "kf2",	STRING,		 68,
	0,          "kf3",	STRING,		 69,
	0,          "kf4",	STRING,		 70,
	0,          "kf5",	STRING,		 71,
	0,          "kf6",	STRING,		 72,
	0,          "kf7",	STRING,		 73,
	0,          "kf8",	STRING,		 74,
	0,          "kf9",	STRING,		 75,
	0,        "khome",	STRING,		 76,
	0,        "kich1",	STRING,		 77,
	0,         "kil1",	STRING,		 78,
	0,        "kcub1",	STRING,		 79,
	0,          "kll",	STRING,		 80,
	0,          "knp",	STRING,		 81,
	0,          "kpp",	STRING,		 82,
	0,        "kcuf1",	STRING,		 83,
	0,         "kind",	STRING,		 84,
	0,          "kri",	STRING,		 85,
	0,         "khts",	STRING,		 86,
	0,        "kcuu1",	STRING,		 87,
	0,         "rmkx",	STRING,		 88,
	0,         "smkx",	STRING,		 89,
	0,          "lf0",	STRING,		 90,
	0,          "lf1",	STRING,		 91,
	0,         "lf10",	STRING,		 92,
	0,          "lf2",	STRING,		 93,
	0,          "lf3",	STRING,		 94,
	0,          "lf4",	STRING,		 95,
	0,          "lf5",	STRING,		 96,
	0,          "lf6",	STRING,		 97,
	0,          "lf7",	STRING,		 98,
	0,          "lf8",	STRING,		 99,
	0,          "lf9",	STRING,		100,
	0,          "rmm",	STRING,		101,
	0,          "smm",	STRING,		102,
	0,          "nel",	STRING,		103,
	0,          "pad",	STRING,		104,
	0,          "dch",	STRING,		105,
	0,           "dl",	STRING,		106,
	0,          "cud",	STRING,		107,
	0,          "ich",	STRING,		108,
	0,         "indn",	STRING,		109,
	0,           "il",	STRING,		110,
	0,          "cub",	STRING,		111,
	0,          "cuf",	STRING,		112,
	0,          "rin",	STRING,		113,
	0,          "cuu",	STRING,		114,
	0,        "pfkey",	STRING,		115,
	0,        "pfloc",	STRING,		116,
	0,          "pfx",	STRING,		117,
	0,          "mc0",	STRING,		118,
	0,          "mc4",	STRING,		119,
	0,          "mc5",	STRING,		120,
	0,          "rep",	STRING,		121,
	0,          "rs1",	STRING,		122,
	0,          "rs2",	STRING,		123,
	0,          "rs3",	STRING,		124,
	0,           "rf",	STRING,		125,
	0,           "rc",	STRING,		126,
	0,          "vpa",	STRING,		127,
	0,           "sc",	STRING,		128,
	0,          "ind",	STRING,		129,
	0,           "ri",	STRING,		130,
	0,          "sgr",	STRING,		131,
	0,          "hts",	STRING,		132,
	0,         "wind",	STRING,		133,
	0,           "ht",	STRING,		134,
	0,          "tsl",	STRING,		135,
	0,           "uc",	STRING,		136,
	0,           "hu",	STRING,		137,
	0,        "iprog",	STRING,		138,
	0,          "ka1",	STRING,		139,
	0,          "ka3",	STRING,		140,
	0,          "kb2",	STRING,		141,
	0,          "kc1",	STRING,		142,
	0,          "kc3",	STRING,		143,
	0,         "mc5p",	STRING,		144,
	0,          "rmp",	STRING,		145,
	0,         "acsc",	STRING,		146,
	0,          "pln",	STRING,		147,
	0,         "kcbt",	STRING,		148,
	0,        "smxon",	STRING,		149,
	0,        "rmxon",	STRING,		150,
	0,         "smam",	STRING,		151,
	0,         "rmam",	STRING,		152,
	0,         "xonc",	STRING,		153,
	0,        "xoffc",	STRING,		154,
	0,        "enacs",	STRING,		155,
	0,         "smln",	STRING,		156,
	0,         "rmln",	STRING,		157,
	0,         "kbeg",	STRING,		158,
	0,         "kcan",	STRING,		159,
	0,         "kclo",	STRING,		160,
	0,         "kcmd",	STRING,		161,
	0,         "kcpy",	STRING,		162,
	0,         "kcrt",	STRING,		163,
	0,         "kend",	STRING,		164,
	0,         "kent",	STRING,		165,
	0,         "kext",	STRING,		166,
	0,         "kfnd",	STRING,		167,
	0,         "khlp",	STRING,		168,
	0,         "kmrk",	STRING,		169,
	0,         "kmsg",	STRING,		170,
	0,         "kmov",	STRING,		171,
	0,         "knxt",	STRING,		172,
	0,         "kopn",	STRING,		173,
	0,         "kopt",	STRING,		174,
	0,         "kprv",	STRING,		175,
	0,         "kprt",	STRING,		176,
	0,         "krdo",	STRING,		177,
	0,         "kref",	STRING,		178,
	0,         "krfr",	STRING,		179,
	0,         "krpl",	STRING,		180,
	0,         "krst",	STRING,		181,
	0,         "kres",	STRING,		182,
	0,         "ksav",	STRING,		183,
	0,         "kspd",	STRING,		184,
	0,         "kund",	STRING,		185,
	0,         "kBEG",	STRING,		186,
	0,         "kCAN",	STRING,		187,
	0,         "kCMD",	STRING,		188,
	0,         "kCPY",	STRING,		189,
	0,         "kCRT",	STRING,		190,
	0,          "kDC",	STRING,		191,
	0,          "kDL",	STRING,		192,
	0,         "kslt",	STRING,		193,
	0,         "kEND",	STRING,		194,
	0,         "kEOL",	STRING,		195,
	0,         "kEXT",	STRING,		196,
	0,         "kFND",	STRING,		197,
	0,         "kHLP",	STRING,		198,
	0,         "kHOM",	STRING,		199,
	0,          "kIC",	STRING,		200,
	0,         "kLFT",	STRING,		201,
	0,         "kMSG",	STRING,		202,
	0,         "kMOV",	STRING,		203,
	0,         "kNXT",	STRING,		204,
	0,         "kOPT",	STRING,		205,
	0,         "kPRV",	STRING,		206,
	0,         "kPRT",	STRING,		207,
	0,         "kRDO",	STRING,		208,
	0,         "kRPL",	STRING,		209,
	0,         "kRIT",	STRING,		210,
	0,         "kRES",	STRING,		211,
	0,         "kSAV",	STRING,		212,
	0,         "kSPD",	STRING,		213,
	0,         "kUND",	STRING,		214,
	0,          "rfi",	STRING,		215,
	0,         "kf11",	STRING,		216,
	0,         "kf12",	STRING,		217,
	0,         "kf13",	STRING,		218,
	0,         "kf14",	STRING,		219,
	0,         "kf15",	STRING,		220,
	0,         "kf16",	STRING,		221,
	0,         "kf17",	STRING,		222,
	0,         "kf18",	STRING,		223,
	0,         "kf19",	STRING,		224,
	0,         "kf20",	STRING,		225,
	0,         "kf21",	STRING,		226,
	0,         "kf22",	STRING,		227,
	0,         "kf23",	STRING,		228,
	0,         "kf24",	STRING,		229,
	0,         "kf25",	STRING,		230,
	0,         "kf26",	STRING,		231,
	0,         "kf27",	STRING,		232,
	0,         "kf28",	STRING,		233,
	0,         "kf29",	STRING,		234,
	0,         "kf30",	STRING,		235,
	0,         "kf31",	STRING,		236,
	0,         "kf32",	STRING,		237,
	0,         "kf33",	STRING,		238,
	0,         "kf34",	STRING,		239,
	0,         "kf35",	STRING,		240,
	0,         "kf36",	STRING,		241,
	0,         "kf37",	STRING,		242,
	0,         "kf38",	STRING,		243,
	0,         "kf39",	STRING,		244,
	0,         "kf40",	STRING,		245,
	0,         "kf41",	STRING,		246,
	0,         "kf42",	STRING,		247,
	0,         "kf43",	STRING,		248,
	0,         "kf44",	STRING,		249,
	0,         "kf45",	STRING,		250,
	0,         "kf46",	STRING,		251,
	0,         "kf47",	STRING,		252,
	0,         "kf48",	STRING,		253,
	0,         "kf49",	STRING,		254,
	0,         "kf50",	STRING,		255,
	0,         "kf51",	STRING,		256,
	0,         "kf52",	STRING,		257,
	0,         "kf53",	STRING,		258,
	0,         "kf54",	STRING,		259,
	0,         "kf55",	STRING,		260,
	0,         "kf56",	STRING,		261,
	0,         "kf57",	STRING,		262,
	0,         "kf58",	STRING,		263,
	0,         "kf59",	STRING,		264,
	0,         "kf60",	STRING,		265,
	0,         "kf61",	STRING,		266,
	0,         "kf62",	STRING,		267,
	0,         "kf63",	STRING,		268,
	0,          "el1",	STRING,		269,
	0,          "mgc",	STRING,		270,
	0,         "smgl",	STRING,		271,
	0,         "smgr",	STRING,		272,
	0,          "fln",	STRING,		273,
	0,         "sclk",	STRING,		274,
	0,         "dclk",	STRING,		275,
	0,        "rmclk",	STRING,		276,
	0,         "cwin",	STRING,		277,
	0,        "wingo",	STRING,		278,
	0,          "hup",	STRING,		279,
	0,         "dial",	STRING,		280,
	0,        "qdial",	STRING,		281,
	0,         "tone",	STRING,		282,
	0,        "pulse",	STRING,		283,
	0,         "hook",	STRING,		284,
	0,        "pause",	STRING,		285,
	0,         "wait",	STRING,		286,
	0,           "u0",	STRING,		287,
	0,           "u1",	STRING,		288,
	0,           "u2",	STRING,		289,
	0,           "u3",	STRING,		290,
	0,           "u4",	STRING,		291,
	0,           "u5",	STRING,		292,
	0,           "u6",	STRING,		293,
	0,           "u7",	STRING,		294,
	0,           "u8",	STRING,		295,
	0,           "u9",	STRING,		296,
	0,           "op",	STRING,		297,
	0,           "oc",	STRING,		298,
	0,        "initc",	STRING,		299,
	0,        "initp",	STRING,		300,
	0,          "scp",	STRING,		301,
	0,         "setf",	STRING,		302,
	0,         "setb",	STRING,		303,
	0,          "cpi",	STRING,		304,
	0,          "lpi",	STRING,		305,
	0,          "chr",	STRING,		306,
	0,          "cvr",	STRING,		307,
	0,         "defc",	STRING,		308,
	0,        "swidm",	STRING,		309,
	0,        "sdrfq",	STRING,		310,
	0,         "sitm",	STRING,		311,
	0,          "slm",	STRING,		312,
	0,        "smicm",	STRING,		313,
	0,         "snlq",	STRING,		314,
	0,        "snrmq",	STRING,		315,
	0,         "sshm",	STRING,		316,
	0,        "ssubm",	STRING,		317,
	0,        "ssupm",	STRING,		318,
	0,          "sum",	STRING,		319,
	0,        "rwidm",	STRING,		320,
	0,         "ritm",	STRING,		321,
	0,          "rlm",	STRING,		322,
	0,        "rmicm",	STRING,		323,
	0,         "rshm",	STRING,		324,
	0,        "rsubm",	STRING,		325,
	0,        "rsupm",	STRING,		326,
	0,          "rum",	STRING,		327,
	0,         "mhpa",	STRING,		328,
	0,        "mcud1",	STRING,		329,
	0,        "mcub1",	STRING,		330,
	0,        "mcuf1",	STRING,		331,
	0,         "mvpa",	STRING,		332,
	0,        "mcuu1",	STRING,		333,
	0,       "porder",	STRING,		334,
	0,         "mcud",	STRING,		335,
	0,         "mcub",	STRING,		336,
	0,         "mcuf",	STRING,		337,
	0,         "mcuu",	STRING,		338,
	0,          "scs",	STRING,		339,
	0,         "smgb",	STRING,		340,
	0,        "smgbp",	STRING,		341,
	0,        "smglp",	STRING,		342,
	0,        "smgrp",	STRING,		343,
	0,         "smgt",	STRING,		344,
	0,        "smgtp",	STRING,		345,
	0,         "sbim",	STRING,		346,
	0,         "scsd",	STRING,		347,
	0,         "rbim",	STRING,		348,
	0,         "rcsd",	STRING,		349,
	0,        "subcs",	STRING,		350,
	0,        "supcs",	STRING,		351,
	0,         "docr",	STRING,		352,
	0,        "zerom",	STRING,		353,
	0,         "csnm",	STRING,		354,
};

struct name_table_entry *cap_hash_table[844];

int	Hashtabsize = 844;
int	Captabsize = 422;


#if (BOOLCOUNT!=37)||(NUMCOUNT!=30)||(STRCOUNT!=355)
	--> term.h and comp_captab.c disagree about the <--
	--> numbers of booleans, numbers and/or strings <--
#endif
