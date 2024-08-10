/*------------------------------------------------------------------------
 * primality search: SIMD-friendly representation of trial divisions
 *------------------------------------------------------------------------
 *  Author: Tamas Visegrady  <tamas@visegrady.ch>
 *----------------------------------------------------------------------*/

/* three prime-table sizes are predefined, with increasing
 * number of small primes:
 *   'S'  suitable for envs with fast modular exponentiation, such as 
 *        hardware security modules, or anything else with hw engines
 *   'M'  comparable to typical trial-division code, such as OpenSSL
 *   'L'  recommended for platforms with slow modular exponentiation,
 *        or for higher prime bitcounts.
 *
 * relative performance (change percentages are approximate):
 *   'M'  (safe or twin primes) XXX % slower than 'S'; XXX % less
 *        calls to mod. exponentiations
 *        (plain or FIPS 186 primes) XXX % slower, XXX % less mod.expn.
 *
 *   'L'  (safe or twin primes) XXX % slower than 'S'; XXX % less
 *        calls to mod. exponentiations
 *        (plain or FIPS 186 primes) XXX % slower, XXX % less mod.expn.
 *
 * can define NO_SIMDDIVIDE_S ..._M ..._L, respectively, to
 * prohibit specific sizes.
 *
 * conditional-disable prime-search types:
 *    NO_SIMD_TWINPRIME
 *    NO_SIMD_SAFEPRIME
 *    NO_SIMD_FIPS186PRIME  -- incremental search, FIPS 186-x sequence;
 *                          -- uses initial candidate plus P*Q increment
 *                          -- with P, Q auxiliary primes
 *    NO_SIMD_PLAINPRIME    -- incremental search checking 6k+1, 6k+5, 6k+7...
 *
 * runtime control: set
 *    PRIMES=...            -- nr. of primes to trial-divide against; must
 *                          -- be one of the S/M/L sizes compatible with
 *                          -- conditional-compiled restrictions
 *    LOG_ALL=...           -- do not trim even long lists
 */

#if defined(NO_SIMDDIVIDE_S) && defined(NO_SIMDDIVIDE_M)
#if defined(NO_SIMDDIVIDE_L)
#error "all SIMD-divide sizes are prohibited"
#endif
#endif

#if defined(NO_SIMD_TWINPRIME)    && defined(NO_SIMD_SAFEPRIME)
#if defined(NO_SIMD_FIPS186PRIME) && defined(NO_SIMD_PLAINPRIME)
#error "all prime subtypes are prohibited"
#endif
#endif


/*
 * hash together all divides-... checks:
 * <... previous sha256 ...>  <div-by-5?>  <div-by-7?> ...
 * initial hash is all-00
 */


/*----------------------------------------------------------------------------
 * References
 *
 * Lemire, Keiser, Kurz:
 *   Faster remainder by direct computation; applications to compilers
 *   and software libraries
 *     2019-11
 *     arxiv.org/abs/1902.01961
 *   section 3.2, "Fast divisibility check with a single multiplication"
 *   (referencing Granlund, Montgomery)
 *
 * Granlund, Montgomery:
 *   Division by invariant integers using multiplication
 *     1994
 *     SIGPLAN Not. 1994(29)
 *   section 9, "Exact division by constants"
 *--------------------------------------------------------------------------*/

#if 0
// safe primes:
//   distribution if smallest factors in array [5..], grouped into
//   units of 16; probability in parts-per-million
//   (in other words, cumulative probability distribution of SIMD divisors
//   falling in group N of 16x16 bits; 1: [5 .. 61], 2: [67 .. 139],
//   3: [149 .. 229]...)
// note: 3 is missing from list of small primes since safe-prime
// search only considers 6k+5 candidates.
//
// 862418[16] 901666[32] 917172[48] 925968[64] 931856[80]
// 936392[96] 940019[112] 943068[128] 945645[144] 947863[160]
// 949815[176] 951559[192] 953095[208] 954486[224] 955749[240]
// 956909[256] 957984[272] 958964[288] 959874[304] 960703[320]
// 961470[336] 962187[352] 962881[368] 963512[384] 964104[400]
// 964671[416] 965191[432] 965682[448] 966140[464] 966565[480]
// 966969[496] 967364[512] 967725[528] 968068[544] 968390[560]
// 968693[576] 968975[592] 969240[608] 969488[624] 969721[640]
// 969937[656] 970142[672] 970332[688] 970513[704] 970682[720]
// 970836[736] 970977[752] 971111[768] 971232[784] 971346[800]
// 971447[816] 971539[832] 971623[848] 971692[864] 971762[880]
// 971823[896] 971878[912] 971927[928] 971973[944] 972018[960]
// 972058[976] 972097[992] 972134[1008] 972166[1024]
// 972192[1040] 972212[1056] 972234[1072] 972252[1088]
// 972268[1104] 972282[1120] 972293[1136] 972301[1152]
// 972308[1168] 972313[1184] 972315[1200] 972315[1216]
// 972316[1232] 972316[1248] 972316[1264] 972316[1280]
// 972317[1296] 972317[1312] 972318[1328] 972318[1344]
// 972318[1360] 972318[1376] 972319[1392] 972319[1408]
// 972319[1424] 972320[1440] 972320[1456] 972320[1472]
// 972320[1488] 972320[1504] 972321[1520] 972321[1536] ...
//
// changes above approximately 1168 are only marginally relevant.
#endif


/*---  nothing user-serviceable below  -------------------------------------*/
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include "common-base.h"

#if defined(USE_CLIC)
#include "clic.h"
static void *dhpriv = NULL;
#endif

#define USE_HEX2BIN
#define USE_HEXDUMP  -1   /* no wrap */
#define USE_READINT
#define USE_BITMASK
#define USE_TIMEDIFF
#define USE_WRITEINT      /* log2 */
#define USE_ERR_ANNOTATE
#define USE_GCD
#include "common-util.h"

#include "common-prime-tools.h"

#include "modred-carmichael.h"

static inline void simd_report1(const char *msg,
                            const uint16_t *p, unsigned int pn) ;

static inline void simd_report_mod1(const char *msg,
                            const uint16_t *p, unsigned int pn) ;

static inline void simd_report_0ff(const char *msg,
                               const uint16_t *p, unsigned int pn) ;

//--------------------------------------
#define  PR64_RAW       0x80000000  /* no digit separator */
#define  PR64_NO_LEAD0  0x40000000  /* trim leading zeroes except for 00 */
#define  PR64_NO_LF     0x20000000  /* do not break line */

/* max(bignumber bitcount) */

#if !defined(PP_MAX_NR_BITS)
#define  PP_MAX_NR_BITS  ((unsigned int) 8192)        /* keep multiple of 64 */
#endif


//----------------------------------------------------------------------------
#if defined(__GNUC__)
// incl. gcc-compatible compilers, specifically clang and icc
#define  REALLY_FORCE_INLINE  __attribute__((always_inline))
#else
#define  REALLY_FORCE_INLINE  /**/
#endif


//--------------------------------------
// placeholder: indicate potentially-sensitive data wiped,
// not optimized away
//
// use your env-provided 'secure wipe' function here
//
static inline void *wipe(void *s, size_t n)
{
	memset(s, 0, n);

	return s;
}


// #if 1   //=====  safe prime search, SIMD-friendly form  ======================
#define  USE_SIMD  256   /* AVX256 etc. */

#include "firstprimes.h"

// reasonable for up to 512-bit SIMD
// TODO: infer max(align) from -mavx... etc. settings
#define  SIMD_ALIGN  __attribute__((aligned(64)))

// 64-bit arch only
#define  SAFEPRIME_MAX_BITS    ((unsigned int) 4096)
#define  SAFEPRIME_MAX_DIGITS  ((SAFEPRIME_MAX_BITS +64 -1) / 64)


// number of primes to consider; all primes fit uint16_t
// keep a multiple of 64, to accommodate up to 512-bit SIMD
//
#if !defined(NO_SIMDDIVIDE_L)
#define  SIMDPRIME_COUNT  ((unsigned int) 3456)
#elif !defined(NO_SIMDDIVIDE_M)
#define  SIMDPRIME_COUNT  ((unsigned int) 1856)
#else
#define  SIMDPRIME_COUNT  ((unsigned int) 576)
#endif
// see also report_table_primes() and override_table_size(),
// which must be updated in sync
//
struct PP_Mod16bit {
	SIMD_ALIGN
	uint16_t modn[ SIMDPRIME_COUNT ];         // candidate % first primes[]
	                                          // see firstprimes[]

	SIMD_ALIGN
	uint16_t incr[ SIMDPRIME_COUNT ];         // increment % [small primes]
	                                          // when searching with
	                                          // multi-digit increments,
	                                          // such as FIPS 186-x
	                                          // primality search
	unsigned int iused;

					// diagnostics use only: reserved for
					// all-hex-string of q[]
					// fits trailing \0
	char qstr[ ((SAFEPRIME_MAX_BITS +7) /8) *2 +1 ];
	size_t qsbytes;
					// when printing, we only update
					// least 64 bits' worth of hex digits
					// from .lsb.
					//
					// TODO: we do not handle wrap/carry
					// when .lsb wraps. Expect a proper
					// implementation (1) manage bignum-
					// printing on its own (2) skip
					// our debug-only features

	uint64_t lsb;         // of main number being tracked
	uint64_t lsbi;        // of increment
	                      // note: we do not manage wrapping, returning
	                      // just least significant 64 bits as 'index'

	uint64_t offset;
			// global 'offset' advance since initialization
			// reserved for end users; we just advance it

	unsigned int mod6;     // searches may be restricted to 6k+5 etc.
	unsigned int mod6incr; // increment % 6

	unsigned int mode;    // search type; see PrimeType_t,
	                      // SearchType_t and FieldSet_t
} ;
//
#define PP_MOD16BIT_INIT0  \
        { { 0, }, { 0, }, 0, { 0, }, 0, 0, 0, 0, 0, 0, 0, }

#define PP_MOD16_INVD_MODE  (~((unsigned int) 0))


/*--------------------------------------
 * recurring pattern: store an offset (array elements written) and
 * two SIMD/64-sized temp. arrays
 */
struct SIMD_Advance {
	unsigned long wr;              // index already written to lsb[] output

	SIMD_ALIGN uint16_t tmp[ 64 ];
	SIMD_ALIGN uint16_t tm2[ 64 ];
} ;
//
#define SIMD_ADVANCE_INIT0  { 0, { 0, }, { 0, } }


typedef enum {
	SIMD_PRIMETYPE_TWIN    = 1,
	SIMD_PRIMETYPE_SAFE    = 2,
	SIMD_PRIMETYPE_FIPS186 = 4,
	SIMD_PRIMETYPE_PLAIN   = 8,
	SIMD_PRIMETYPE_MASK    = 0xff
} PrimeType_t ;


typedef enum {
	SIMD_SEARCHTABLE_S    =  0x100,
	SIMD_SEARCHTABLE_M    =  0x200,
	SIMD_SEARCHTABLE_L    =  0x300,
	SIMD_SEARCHTABLE_MASK = 0xff00
} SearchType_t ;


typedef enum {
	SIMD_FIELDSET_INCR =  0x10000,   // increment field is set
	SIMD_FIELDSET_MASK = 0xff0000
} FieldSet_t ;


//--------------------------------------
static void wipe_advance_struct(struct SIMD_Advance *pa)
{
	if (pa) {
		wipe(pa->tmp, sizeof(pa->tmp));
		wipe(pa->tm2, sizeof(pa->tm2));
	}
}


/*--------------------------------------
 * res[i] = v[i] * coeff[i]
 */
static inline
REALLY_FORCE_INLINE
//
void simd_mul16x16(uint16_t res[16], const uint16_t v[16],
           const uint16_t coeff[16])
{
	res[  0 ] = v[  0 ] * coeff[  0 ];
	res[  1 ] = v[  1 ] * coeff[  1 ];
	res[  2 ] = v[  2 ] * coeff[  2 ];
	res[  3 ] = v[  3 ] * coeff[  3 ];
	res[  4 ] = v[  4 ] * coeff[  4 ];
	res[  5 ] = v[  5 ] * coeff[  5 ];
	res[  6 ] = v[  6 ] * coeff[  6 ];
	res[  7 ] = v[  7 ] * coeff[  7 ];
	res[  8 ] = v[  8 ] * coeff[  8 ];
	res[  9 ] = v[  9 ] * coeff[  9 ];
	res[ 10 ] = v[ 10 ] * coeff[ 10 ];
	res[ 11 ] = v[ 11 ] * coeff[ 11 ];
	res[ 12 ] = v[ 12 ] * coeff[ 12 ];
	res[ 13 ] = v[ 13 ] * coeff[ 13 ];
	res[ 14 ] = v[ 14 ] * coeff[ 14 ];
	res[ 15 ] = v[ 15 ] * coeff[ 15 ];
}


/*--------------------------------------
 * map A<B to all-1's (xffff), 0 otherwise
 *
 * since the division criteria is '...product... <= ...limit...' to
 * indicate that the prime divides the candidate, use 0 to indicate
 * non-division. (this way, an all-0 result mask indicates a possible
 * prime candidate.)
 */
static inline uint16_t le16mask(uint16_t a, uint16_t b)
{
	return (a <= b) ? ~((uint16_t) 0) : 0;
}


/*-------------------------------------- 
 * v[] += (vector of) adv
 */
static inline
REALLY_FORCE_INLINE
//
void simd_advance16x16_inpl(uint16_t v[16], uint16_t add)
{
	v[  0 ] += add;
	v[  1 ] += add;
	v[  2 ] += add;
	v[  3 ] += add;
	v[  4 ] += add;
	v[  5 ] += add;
	v[  6 ] += add;
	v[  7 ] += add;
	v[  8 ] += add;
	v[  9 ] += add;
	v[ 10 ] += add;
	v[ 11 ] += add;
	v[ 12 ] += add;
	v[ 13 ] += add;
	v[ 14 ] += add;
	v[ 15 ] += add;
}


/*--------------------------------------
 * v[] += adv[]
 */
static inline
REALLY_FORCE_INLINE
//
void simd_advance16x16_inpl_v(uint16_t v[16], const uint16_t add[16])
{
	v[  0 ] += add[  0 ];
	v[  1 ] += add[  1 ];
	v[  2 ] += add[  2 ];
	v[  3 ] += add[  3 ];
	v[  4 ] += add[  4 ];
	v[  5 ] += add[  5 ];
	v[  6 ] += add[  6 ];
	v[  7 ] += add[  7 ];
	v[  8 ] += add[  8 ];
	v[  9 ] += add[  9 ];
	v[ 10 ] += add[ 10 ];
	v[ 11 ] += add[ 11 ];
	v[ 12 ] += add[ 12 ];
	v[ 13 ] += add[ 13 ];
	v[ 14 ] += add[ 14 ];
	v[ 15 ] += add[ 15 ];
}


/*--------------------------------------
 * r[] = a[] | b[]
 */
static inline
REALLY_FORCE_INLINE
//
void simd_or16x16(uint16_t r[16], const uint16_t a[16], const uint16_t b[16])
{
	r[  0 ] = a[  0 ] | b[  0 ];
	r[  1 ] = a[  1 ] | b[  1 ];
	r[  2 ] = a[  2 ] | b[  2 ];
	r[  3 ] = a[  3 ] | b[  3 ];
	r[  4 ] = a[  4 ] | b[  4 ];
	r[  5 ] = a[  5 ] | b[  5 ];
	r[  6 ] = a[  6 ] | b[  6 ];
	r[  7 ] = a[  7 ] | b[  7 ];
	r[  8 ] = a[  8 ] | b[  8 ];
	r[  9 ] = a[  9 ] | b[  9 ];
	r[ 10 ] = a[ 10 ] | b[ 10 ];
	r[ 11 ] = a[ 11 ] | b[ 11 ];
	r[ 12 ] = a[ 12 ] | b[ 12 ];
	r[ 13 ] = a[ 13 ] | b[ 13 ];
	r[ 14 ] = a[ 14 ] | b[ 14 ];
	r[ 15 ] = a[ 15 ] | b[ 15 ];
}


/*--------------------------------------
 * return 'val +add'  if (val >= limit)
 *        'val'       otherwise
 *
 * casts and Booleans only; expect to compile to a conditional move etc.
 * please do not comment on readability
 *
 * note: restricted to two's complement representation
 */
static inline uint16_t add_if_ge(uint16_t val, uint16_t limit, uint16_t add)
{
	return val + (((uint16_t) -((int16_t) (limit <= val))) & add);
}


/*--------------------------------------
 * one u16-wide slice from m2r()
 *
 * subtract m2r[] value if val is >= 0x8000
 * m2r[] preserves mod-prime[] when subtracted from val >= 0x8000
 */
static inline uint16_t m2range_1unit(uint16_t val, uint16_t m2r)
{
	return add_if_ge(val, UINT16_C(0x8000), m2r);
}


/*--------------------------------------
 * conditionally subtract m2r[] from v[] if value is >= 0x8000
 * subtraction (1) preserves mod-prime[] (2) reduces to lowest non-negative
 * value with proper mod-prime
 *
 * note that we add a negated value instead of subtraction (no special
 * reason, just one primitive less)
 */
static inline
REALLY_FORCE_INLINE
//
void simd_m2range16x16(uint16_t r[16], const uint16_t v[16],
                 const uint16_t m2r[16])
{

	r[  0 ] = m2range_1unit(v[  0 ], m2r[  0 ]);
	r[  1 ] = m2range_1unit(v[  1 ], m2r[  1 ]);
	r[  2 ] = m2range_1unit(v[  2 ], m2r[  2 ]);
	r[  3 ] = m2range_1unit(v[  3 ], m2r[  3 ]);
	r[  4 ] = m2range_1unit(v[  4 ], m2r[  4 ]);
	r[  5 ] = m2range_1unit(v[  5 ], m2r[  5 ]);
	r[  6 ] = m2range_1unit(v[  6 ], m2r[  6 ]);
	r[  7 ] = m2range_1unit(v[  7 ], m2r[  7 ]);
	r[  8 ] = m2range_1unit(v[  8 ], m2r[  8 ]);
	r[  9 ] = m2range_1unit(v[  9 ], m2r[  9 ]);
	r[ 10 ] = m2range_1unit(v[ 10 ], m2r[ 10 ]);
	r[ 11 ] = m2range_1unit(v[ 11 ], m2r[ 11 ]);
	r[ 12 ] = m2range_1unit(v[ 12 ], m2r[ 12 ]);
	r[ 13 ] = m2range_1unit(v[ 13 ], m2r[ 13 ]);
	r[ 14 ] = m2range_1unit(v[ 14 ], m2r[ 14 ]);
	r[ 15 ] = m2range_1unit(v[ 15 ], m2r[ 15 ]);
}


/*--------------------------------------
 * reduce v[] in-place mod-small-primes[] (if entries are >= 0x8000)
 *
 * assume aggressive+forced inlining leads to recognized specialization
 * for src==dest
 */
static inline
REALLY_FORCE_INLINE
//
void simd_m2range16x16_inpl(uint16_t v[16], const uint16_t m2r[16])
{
	simd_m2range16x16(v, v, m2r);
}


/*--------------------------------------
 * r[] = 2 * v[] + add[]
 */
static inline
REALLY_FORCE_INLINE
//
void simd_shladd16x16(uint16_t r[ 16 ], const uint16_t v[ 16 ],
                                      const uint16_t add[ 16 ])
{
	r[  0 ] = (v[  0 ] << 1) + add[  0 ];
	r[  1 ] = (v[  1 ] << 1) + add[  1 ];
	r[  2 ] = (v[  2 ] << 1) + add[  2 ];
	r[  3 ] = (v[  3 ] << 1) + add[  3 ];
	r[  4 ] = (v[  4 ] << 1) + add[  4 ];
	r[  5 ] = (v[  5 ] << 1) + add[  5 ];
	r[  6 ] = (v[  6 ] << 1) + add[  6 ];
	r[  7 ] = (v[  7 ] << 1) + add[  7 ];
	r[  8 ] = (v[  8 ] << 1) + add[  8 ];
	r[  9 ] = (v[  9 ] << 1) + add[  9 ];
	r[ 10 ] = (v[ 10 ] << 1) + add[ 10 ];
	r[ 11 ] = (v[ 11 ] << 1) + add[ 11 ];
	r[ 12 ] = (v[ 12 ] << 1) + add[ 12 ];
	r[ 13 ] = (v[ 13 ] << 1) + add[ 13 ];
	r[ 14 ] = (v[ 14 ] << 1) + add[ 14 ];
	r[ 15 ] = (v[ 15 ] << 1) + add[ 15 ];
}


/*--------------------------------------
 * r[] = min(a[], b[])
 * expect repeated expressions to be recognized and min() substituted
 */
static inline
REALLY_FORCE_INLINE
//
void simd_min16x16(uint16_t r[ 16 ], const uint16_t a[ 16 ],
                                     const uint16_t b[ 16 ])
{
	r[  0 ] = (a[  0 ] < b[  0 ]) ? a[  0 ] : b[  0 ];
	r[  1 ] = (a[  1 ] < b[  1 ]) ? a[  1 ] : b[  1 ];
	r[  2 ] = (a[  2 ] < b[  2 ]) ? a[  2 ] : b[  2 ];
	r[  3 ] = (a[  3 ] < b[  3 ]) ? a[  3 ] : b[  3 ];
	r[  4 ] = (a[  4 ] < b[  4 ]) ? a[  4 ] : b[  4 ];
	r[  5 ] = (a[  5 ] < b[  5 ]) ? a[  5 ] : b[  5 ];
	r[  6 ] = (a[  6 ] < b[  6 ]) ? a[  6 ] : b[  6 ];
	r[  7 ] = (a[  7 ] < b[  7 ]) ? a[  7 ] : b[  7 ];
	r[  8 ] = (a[  8 ] < b[  8 ]) ? a[  8 ] : b[  8 ];
	r[  9 ] = (a[  9 ] < b[  9 ]) ? a[  9 ] : b[  9 ];
	r[ 10 ] = (a[ 10 ] < b[ 10 ]) ? a[ 10 ] : b[ 10 ];
	r[ 11 ] = (a[ 11 ] < b[ 11 ]) ? a[ 11 ] : b[ 11 ];
	r[ 12 ] = (a[ 12 ] < b[ 12 ]) ? a[ 12 ] : b[ 12 ];
	r[ 13 ] = (a[ 13 ] < b[ 13 ]) ? a[ 13 ] : b[ 13 ];
	r[ 14 ] = (a[ 14 ] < b[ 14 ]) ? a[ 14 ] : b[ 14 ];
	r[ 15 ] = (a[ 15 ] < b[ 15 ]) ? a[ 15 ] : b[ 15 ];
}


/*--------------------------------------
 * replace SIMD units with all-1 where r[] <= limit[]; 0 otherwise
 */
static inline
REALLY_FORCE_INLINE
//
void simd_lemask16x16_inpl(uint16_t r[ 16 ], const uint16_t limit[ 16 ])
{
	r[  0 ] = le16mask(r[  0 ], limit[  0 ]);
	r[  1 ] = le16mask(r[  1 ], limit[  1 ]);
	r[  2 ] = le16mask(r[  2 ], limit[  2 ]);
	r[  3 ] = le16mask(r[  3 ], limit[  3 ]);
	r[  4 ] = le16mask(r[  4 ], limit[  4 ]);
	r[  5 ] = le16mask(r[  5 ], limit[  5 ]);
	r[  6 ] = le16mask(r[  6 ], limit[  6 ]);
	r[  7 ] = le16mask(r[  7 ], limit[  7 ]);
	r[  8 ] = le16mask(r[  8 ], limit[  8 ]);
	r[  9 ] = le16mask(r[  9 ], limit[  9 ]);
	r[ 10 ] = le16mask(r[ 10 ], limit[ 10 ]);
	r[ 11 ] = le16mask(r[ 11 ], limit[ 11 ]);
	r[ 12 ] = le16mask(r[ 12 ], limit[ 12 ]);
	r[ 13 ] = le16mask(r[ 13 ], limit[ 13 ]);
	r[ 14 ] = le16mask(r[ 14 ], limit[ 14 ]);
	r[ 15 ] = le16mask(r[ 15 ], limit[ 15 ]);
}


/*--------------------------------------
 * safe-prime comparison: with v[] storing x * mod small-primes[],
 * report if any of the P|x or P|2x+1: r[] will be all-00 if no factor
 *
 * since comparing to 0 is expected intrinsic, report 0 if dividing, >0 if not
 * 'v' is x * 1/prime
 *
 * 'r' is filled with 16x16-bit result
 *     ffff  entry divides x  OR  divides 2x+1
 *        0  divides none
 *
 * (1) A1 =  x * 1/prime   mod 2^16
 * (2) A1 <= B             <->  prime divides x
 *
 * (3) A2 = (2x + 1) * 1/prime  mod 2^16  ==
 *          (2 * A1 + 1/prime)  mod 2^16
 * (4) A2 <= B             <->  prime divides 2x+1
 *
 *------------------------------------*/
static inline
REALLY_FORCE_INLINE
//
void simd_spcmp16x16(uint16_t r[16], const uint16_t v[16],
                                   const uint16_t inv[16],
                                   const uint16_t limit[16])
{
	uint16_t tmp[16];
// TODO: use external tmp[] which may be centrally wiped

	simd_shladd16x16(tmp, v, inv);                   // 2*v+inv[]  mod 2^16

	simd_min16x16(r, v, tmp);

	simd_lemask16x16_inpl(r, limit);           // divides -> ffff; not -> 0
}


//--------------------------------------
// report if P|x
// since comparing to 0 is expected intrinsic, report 0 if dividing, >0 if not
// 'v' is x * 1/prime
//
// 'r' is filled with 16x16-bit result
//     ffff  entry divides x
//        0  divides none
//--------------------------------------
static inline
REALLY_FORCE_INLINE
//
void simd_cmp16x16_plain(uint16_t r[16], const uint16_t limit[16])
{
	simd_lemask16x16_inpl(r, limit);
}


#if 1    //-----  delimiter: generated SIMD columns  -------------------------
#if 1   // delimiter: generated code
#if 1   // 256 bit: these are just pass-through functions

static const uint16_t simd_allzero256bits[ 16 ];
//
// comparison with all-zeroes, which is usually special-cased by SIMD
// instructions
//
// on AVX, this would be:
//     _mm256_testz_si256(x, x)     -- packed bit test, set zero flag (ZF)
//                                  -- AND two values; AND(X, X) <=> is 0?
// mnemonic, which compiles to VPTEST.
//
// temporarily, we just compare input vector to an all-zeroes known SIMD vector
//
// TODO: find portable C which compiles into ISA-specific VPTEST or equivalent
// TODO: document that we depend on a known binary representation
//
// 0 -> does not divide
//   -> trialdiv() functions return >0 if candidate may be prime
//                                  0  if at least one small prime divides


/*---------------------------------------*/
static inline
REALLY_FORCE_INLINE
//
unsigned int simd_is_all0(const uint16_t v[16])
{
	return !memcmp(simd_allzero256bits, v, sizeof(simd_allzero256bits));

#if 0
#if (SYS__X86_BITS >= 64)
	return _mm256_testz_si256((const __m256i) *v, (const __m256i) *v);

#elif (SYS__S390_BITS >= 64)
#error "supply S390 check-ZF intrinsic here"

#else
	return !!((((v[  0 ] & v[  1 ]) & (v[  2 ] & v[  3 ]))  &
	           ((v[  4 ] & v[  5 ]) & (v[  6 ] & v[  7 ]))) &
	          (((v[  8 ] & v[  9 ]) & (v[ 10 ] & v[ 11 ]))  &
	           ((v[ 12 ] & v[ 13 ]) & (v[ 14 ] & v[ 15 ]))));

#endif
#endif
}

#endif       // /256 bits


/*-----------------------------------------
 * is 768-bit (48x16) wide SIMD array all zeroes?
 *
 * NOTE: DESTROYS ORIGINAL INPUT
 */
static inline
REALLY_FORCE_INLINE
/**/
unsigned int simd_is_all0x48x16(uint16_t v[ 48 ])
{
	simd_or16x16(v, v, &(v[ 16 ]));
	simd_or16x16(v, v, &(v[ 32 ]));

	return simd_is_all0(v);
}


/*-----------------------------------------
 * is 64x16 wide SIMD array all zeroes?
 *
 * SECURITY NOTE: tmp[] is scratch which caller MUST wipe
 * NOTE: DESTROYS ORIGINAL INPUTS
 */
static inline
REALLY_FORCE_INLINE
/**/
unsigned int simd_is_all0x64x16_inpl(uint16_t v[64], uint16_t tmp[16])
{
	simd_or16x16(tmp, &(v[32]), &(v[48]));       // 2 3
	simd_or16x16(v,     v,      &(v[16]));       // 0 1
	simd_or16x16(v,     tmp,      v     );       // 0 1 2 3
// simd_report_0ff("## CMASK.OR=", v, 16);

	return simd_is_all0(v);
}


/*-----------------------------------------
 * v[] += adv; preserving mod-prime by subtracting m2r[] if applicable
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance64x16_m2r_inpl(uint16_t v[64], uint16_t adv,
                        const uint16_t m2r[64])
{
	simd_advance16x16_inpl(  v,          adv       );
	simd_m2range16x16_inpl(  v,          m2r       );

	simd_advance16x16_inpl(&(v[ 16 ]),   adv       );
	simd_m2range16x16_inpl(&(v[ 16 ]), &(m2r[ 16 ]));

	simd_advance16x16_inpl(&(v[ 32 ]),   adv       );
	simd_m2range16x16_inpl(&(v[ 32 ]), &(m2r[ 32 ]));

	simd_advance16x16_inpl(&(v[ 48 ]),   adv       );
	simd_m2range16x16_inpl(&(v[ 48 ]), &(m2r[ 48 ]));
}


/*---------------------------------------
 * v[] += adv[]; preserving mod-prime by subtracting m2r[] if applicable
 * 64 elements
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance64x16_m2r_inpl_v(uint16_t v[ 64 ],
                          const uint16_t adv[ 64 ],
                          const uint16_t m2r[ 64 ])
{
	simd_advance16x16_inpl_v(  v,          adv       );
	simd_m2range16x16_inpl  (  v,          m2r       );

	simd_advance16x16_inpl_v(&(v[ 16 ]), &(adv[ 16 ]));
	simd_m2range16x16_inpl  (&(v[ 16 ]), &(m2r[ 16 ]));

	simd_advance16x16_inpl_v(&(v[ 32 ]), &(adv[ 32 ]));
	simd_m2range16x16_inpl  (&(v[ 32 ]), &(m2r[ 32 ]));

	simd_advance16x16_inpl_v(&(v[ 48 ]), &(adv[ 48 ]));
	simd_m2range16x16_inpl  (&(v[ 48 ]), &(m2r[ 48 ]));
}


/*-----------------------------------------
 * 'plain' prime search: can modn[] indicate a prime?
 *
 * SECURITY NOTE: tmp[] and tmp2[] are scratch, filled with (potentially)secret
 * state-dependent data, and SHOULD be wiped upon return.
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_nofactor64x16(uint16_t tmp[ 64 ],
                           uint16_t tmp2[ 64 ],
                     const uint16_t modn[ 64 ], const uint16_t inv[64],
                    const uint16_t limit[ 64 ])
{
					// compute n * 1/prime  mod 2^16

	simd_mul16x16(  tmp,          modn,          inv       );
	simd_mul16x16(&(tmp[ 16 ]), &(modn[ 16 ]), &(inv[ 16 ]));
	simd_mul16x16(&(tmp[ 32 ]), &(modn[ 32 ]), &(inv[ 32 ]));
	simd_mul16x16(&(tmp[ 48 ]), &(modn[ 48 ]), &(inv[ 48 ]));

					// any of the products <= limit[ ]?
					// divides -> ffff; not -> 0

	simd_lemask16x16_inpl(  tmp,          limit       );
	simd_lemask16x16_inpl(&(tmp[ 16 ]), &(limit[ 16 ]));
	simd_lemask16x16_inpl(&(tmp[ 32 ]), &(limit[ 32 ]));
	simd_lemask16x16_inpl(&(tmp[ 48 ]), &(limit[ 48 ]));

	return simd_is_all0x64x16_inpl(tmp, tmp2);
}


/*--------------------------------------
 * does the first set of prime moduli (64 primes) indicate prime?
 *
 * SECURITY NOTE: tmp[] and tmp2[] are scratch areas, which caller
 * MUST maintain and wipe
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_nofactor_first(uint16_t tmp[ 64 ],
                             uint16_t tm2[ 64 ],
             const struct PP_Mod16bit *ps)
{
	return simd_nofactor64x16(tmp, tm2, ps->modn,
	                          firstprimes_inverse_simd,
	                          firstprimes_mullimit_simd);
}


/*--------------------------------------
 * does the 'not first' set of prime moduli indicate a possible prime?
 * checks up to the first 576 suitable primes
 *
 * SECURITY NOTE: tmp[] and tmp2[] are scratch areas, which caller
 * MUST maintain and wipe
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_nofactor_rest_s(uint16_t tmp[ 64 ],
                              uint16_t tm2[ 64 ],
              const struct PP_Mod16bit *ps)
{
	return (simd_nofactor64x16(tmp, tm2, &(ps->modn[  64 ]),
	                     &(firstprimes_inverse_simd[  64 ]),
	                    &(firstprimes_mullimit_simd[  64 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 128 ]),
	                     &(firstprimes_inverse_simd[ 128 ]),
	                    &(firstprimes_mullimit_simd[ 128 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 192 ]),
	                     &(firstprimes_inverse_simd[ 192 ]),
	                    &(firstprimes_mullimit_simd[ 192 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 256 ]),
	                     &(firstprimes_inverse_simd[ 256 ]),
	                    &(firstprimes_mullimit_simd[ 256 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 320 ]),
	                     &(firstprimes_inverse_simd[ 320 ]),
	                    &(firstprimes_mullimit_simd[ 320 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 384 ]),
	                     &(firstprimes_inverse_simd[ 384 ]),
	                    &(firstprimes_mullimit_simd[ 384 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 448 ]),
	                     &(firstprimes_inverse_simd[ 448 ]),
	                    &(firstprimes_mullimit_simd[ 448 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 512 ]),
	                     &(firstprimes_inverse_simd[ 512 ]),
	                    &(firstprimes_mullimit_simd[ 512 ]))
	        );
}


/*--------------------------------------
 * 1856-prime version of simd_nofactor_rest_s()
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_nofactor_rest_m(uint16_t tmp[ 64 ],
                              uint16_t tm2[ 64 ],
              const struct PP_Mod16bit *ps)
{
	return (simd_nofactor64x16(tmp, tm2, &(ps->modn[  576 ]),
	                     &(firstprimes_inverse_simd[  576 ]),
	                    &(firstprimes_mullimit_simd[  576 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[  640 ]),
	                     &(firstprimes_inverse_simd[  640 ]),
	                    &(firstprimes_mullimit_simd[  640 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[  704 ]),
	                     &(firstprimes_inverse_simd[  704 ]),
	                    &(firstprimes_mullimit_simd[  704 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[  768 ]),
	                     &(firstprimes_inverse_simd[  768 ]),
	                    &(firstprimes_mullimit_simd[  768 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[  832 ]),
	                     &(firstprimes_inverse_simd[  832 ]),
	                    &(firstprimes_mullimit_simd[  832 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[  896 ]),
	                     &(firstprimes_inverse_simd[  896 ]),
	                    &(firstprimes_mullimit_simd[  896 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[  960 ]),
	                     &(firstprimes_inverse_simd[  960 ]),
	                    &(firstprimes_mullimit_simd[  960 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1024 ]),
	                     &(firstprimes_inverse_simd[ 1024 ]),
	                    &(firstprimes_mullimit_simd[ 1024 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1088 ]),
	                     &(firstprimes_inverse_simd[ 1088 ]),
	                    &(firstprimes_mullimit_simd[ 1088 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1152 ]),
	                     &(firstprimes_inverse_simd[ 1152 ]),
	                    &(firstprimes_mullimit_simd[ 1152 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1216 ]),
	                     &(firstprimes_inverse_simd[ 1216 ]),
	                    &(firstprimes_mullimit_simd[ 1216 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1280 ]),
	                     &(firstprimes_inverse_simd[ 1280 ]),
	                    &(firstprimes_mullimit_simd[ 1280 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1344 ]),
	                     &(firstprimes_inverse_simd[ 1344 ]),
	                    &(firstprimes_mullimit_simd[ 1344 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1408 ]),
	                     &(firstprimes_inverse_simd[ 1408 ]),
	                    &(firstprimes_mullimit_simd[ 1408 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1472 ]),
	                     &(firstprimes_inverse_simd[ 1472 ]),
	                    &(firstprimes_mullimit_simd[ 1472 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1536 ]),
	                     &(firstprimes_inverse_simd[ 1536 ]),
	                    &(firstprimes_mullimit_simd[ 1536 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1600 ]),
	                     &(firstprimes_inverse_simd[ 1600 ]),
	                    &(firstprimes_mullimit_simd[ 1600 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1664 ]),
	                     &(firstprimes_inverse_simd[ 1664 ]),
	                    &(firstprimes_mullimit_simd[ 1664 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1728 ]),
	                     &(firstprimes_inverse_simd[ 1728 ]),
	                    &(firstprimes_mullimit_simd[ 1728 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1792 ]),
	                     &(firstprimes_inverse_simd[ 1792 ]),
	                    &(firstprimes_mullimit_simd[ 1792 ])));
}


/*--------------------------------------
 * 3456-prime version of simd_nofactor_rest_s()
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_nofactor_rest_l(uint16_t tmp[ 64 ],
                              uint16_t tm2[ 64 ],
              const struct PP_Mod16bit *ps)
{
	return (simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1856 ]),
	                     &(firstprimes_inverse_simd[ 1856 ]),
	                    &(firstprimes_mullimit_simd[ 1856 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1920 ]),
	                     &(firstprimes_inverse_simd[ 1920 ]),
	                    &(firstprimes_mullimit_simd[ 1920 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 1984 ]),
	                     &(firstprimes_inverse_simd[ 1984 ]),
	                    &(firstprimes_mullimit_simd[ 1984 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2048 ]),
	                     &(firstprimes_inverse_simd[ 2048 ]),
	                    &(firstprimes_mullimit_simd[ 2048 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2112 ]),
	                     &(firstprimes_inverse_simd[ 2112 ]),
	                    &(firstprimes_mullimit_simd[ 2112 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2176 ]),
	                     &(firstprimes_inverse_simd[ 2176 ]),
	                    &(firstprimes_mullimit_simd[ 2176 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2240 ]),
	                     &(firstprimes_inverse_simd[ 2240 ]),
	                    &(firstprimes_mullimit_simd[ 2240 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2304 ]),
	                     &(firstprimes_inverse_simd[ 2304 ]),
	                    &(firstprimes_mullimit_simd[ 2304 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2368 ]),
	                     &(firstprimes_inverse_simd[ 2368 ]),
	                    &(firstprimes_mullimit_simd[ 2368 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2432 ]),
	                     &(firstprimes_inverse_simd[ 2432 ]),
	                    &(firstprimes_mullimit_simd[ 2432 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2496 ]),
	                     &(firstprimes_inverse_simd[ 2496 ]),
	                    &(firstprimes_mullimit_simd[ 2496 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2560 ]),
	                     &(firstprimes_inverse_simd[ 2560 ]),
	                    &(firstprimes_mullimit_simd[ 2560 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2624 ]),
	                     &(firstprimes_inverse_simd[ 2624 ]),
	                    &(firstprimes_mullimit_simd[ 2624 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2688 ]),
	                     &(firstprimes_inverse_simd[ 2688 ]),
	                    &(firstprimes_mullimit_simd[ 2688 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2752 ]),
	                     &(firstprimes_inverse_simd[ 2752 ]),
	                    &(firstprimes_mullimit_simd[ 2752 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2816 ]),
	                     &(firstprimes_inverse_simd[ 2816 ]),
	                    &(firstprimes_mullimit_simd[ 2816 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2880 ]),
	                     &(firstprimes_inverse_simd[ 2880 ]),
	                    &(firstprimes_mullimit_simd[ 2880 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 2944 ]),
	                     &(firstprimes_inverse_simd[ 2944 ]),
	                    &(firstprimes_mullimit_simd[ 2944 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 3008 ]),
	                     &(firstprimes_inverse_simd[ 3008 ]),
	                    &(firstprimes_mullimit_simd[ 3008 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 3072 ]),
	                     &(firstprimes_inverse_simd[ 3072 ]),
	                    &(firstprimes_mullimit_simd[ 3072 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 3136 ]),
	                     &(firstprimes_inverse_simd[ 3136 ]),
	                    &(firstprimes_mullimit_simd[ 3136 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 3200 ]),
	                     &(firstprimes_inverse_simd[ 3200 ]),
	                    &(firstprimes_mullimit_simd[ 3200 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 3264 ]),
	                     &(firstprimes_inverse_simd[ 3264 ]),
	                    &(firstprimes_mullimit_simd[ 3264 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 3328 ]),
	                     &(firstprimes_inverse_simd[ 3328 ]),
	                    &(firstprimes_mullimit_simd[ 3328 ])) &&

	        simd_nofactor64x16(tmp, tm2, &(ps->modn[ 3392 ]),
	                     &(firstprimes_inverse_simd[ 3392 ]),
	                    &(firstprimes_mullimit_simd[ 3392 ])));
}


//--------------------------------------
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_spfactor64x16(uint16_t tmp[64],
                              uint16_t tmp2[64],
                        const uint16_t modn[64], const uint16_t inv[64],
                       const uint16_t limit[64]) ;


/*--------------------------------------
 * does the 'first' set of prime moduli indicate a possible safe prime?
 * see simd_no_spfactor_rest() for the other primes' remainders
 *
 * SECURITY NOTE: tmp[] and tmp2[] are scratch areas, which caller
 * MUST maintain and wipe
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_spfactor_first(uint16_t tmp[64],
                                uint16_t tm2[64],
                const struct PP_Mod16bit *ps)
{
	return simd_no_spfactor64x16(tmp, tm2, ps->modn,
		                     firstprimes_inverse_simd,
		                     firstprimes_mullimit_simd);
}


/*--------------------------------------
 * TODO: sync interfaces: see simd_no_spfactor64x16()
 * for counterpart
 *
 * SECURITY NOTE: tmp[] and tmp2[] are scratch, filled with
 * (potentially)secret state-dependent data, and SHOULD be wiped upon return.
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_spfactor_rest1(uint16_t tmp[  64 ],
                               uint16_t tmp2[  64 ],
                         const uint16_t modn[ 576 ])
{
	return (simd_no_spfactor64x16(tmp, tmp2, &(modn[  64 ]),
	                     &(firstprimes_inverse_simd[  64 ]),
	                    &(firstprimes_mullimit_simd[  64 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 128 ]),
	                     &(firstprimes_inverse_simd[ 128 ]),
	                    &(firstprimes_mullimit_simd[ 128 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 192 ]),
	                     &(firstprimes_inverse_simd[ 192 ]),
	                    &(firstprimes_mullimit_simd[ 192 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 256 ]),
	                     &(firstprimes_inverse_simd[ 256 ]),
	                    &(firstprimes_mullimit_simd[ 256 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 320 ]),
	                     &(firstprimes_inverse_simd[ 320 ]),
	                    &(firstprimes_mullimit_simd[ 320 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 384 ]),
	                     &(firstprimes_inverse_simd[ 384 ]),
	                    &(firstprimes_mullimit_simd[ 384 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 448 ]),
	                     &(firstprimes_inverse_simd[ 448 ]),
	                    &(firstprimes_mullimit_simd[ 448 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 512 ]),
	                     &(firstprimes_inverse_simd[ 512 ]),
	                    &(firstprimes_mullimit_simd[ 512 ]))) ;
}


/*--------------------------------------
 * mid-sized equivalent of simd_no_spfactor_rest1()
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_spfactor_rest_m(uint16_t tmp[   64 ],
                                uint16_t tmp2[   64 ],
                          const uint16_t modn[ 1856 ])
{
	return (simd_no_spfactor_rest1(tmp, tmp2, modn) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[  576 ]),
	                     &(firstprimes_inverse_simd[  576 ]),
	                    &(firstprimes_mullimit_simd[  576 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[  640 ]),
	                     &(firstprimes_inverse_simd[  640 ]),
	                    &(firstprimes_mullimit_simd[  640 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[  704 ]),
	                     &(firstprimes_inverse_simd[  704 ]),
	                    &(firstprimes_mullimit_simd[  704 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[  768 ]),
	                     &(firstprimes_inverse_simd[  768 ]),
	                    &(firstprimes_mullimit_simd[  768 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[  832 ]),
	                     &(firstprimes_inverse_simd[  832 ]),
	                    &(firstprimes_mullimit_simd[  832 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[  896 ]),
	                     &(firstprimes_inverse_simd[  896 ]),
	                    &(firstprimes_mullimit_simd[  896 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[  960 ]),
	                     &(firstprimes_inverse_simd[  960 ]),
	                    &(firstprimes_mullimit_simd[  960 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1024 ]),
	                     &(firstprimes_inverse_simd[ 1024 ]),
	                    &(firstprimes_mullimit_simd[ 1024 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1088 ]),
	                     &(firstprimes_inverse_simd[ 1088 ]),
	                    &(firstprimes_mullimit_simd[ 1088 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1152 ]),
	                     &(firstprimes_inverse_simd[ 1152 ]),
	                    &(firstprimes_mullimit_simd[ 1152 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1216 ]),
	                     &(firstprimes_inverse_simd[ 1216 ]),
	                    &(firstprimes_mullimit_simd[ 1216 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1280 ]),
	                     &(firstprimes_inverse_simd[ 1280 ]),
	                    &(firstprimes_mullimit_simd[ 1280 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1344 ]),
	                     &(firstprimes_inverse_simd[ 1344 ]),
	                    &(firstprimes_mullimit_simd[ 1344 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1408 ]),
	                     &(firstprimes_inverse_simd[ 1408 ]),
	                    &(firstprimes_mullimit_simd[ 1408 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1472 ]),
	                     &(firstprimes_inverse_simd[ 1472 ]),
	                    &(firstprimes_mullimit_simd[ 1472 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1536 ]),
	                     &(firstprimes_inverse_simd[ 1536 ]),
	                    &(firstprimes_mullimit_simd[ 1536 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1600 ]),
	                     &(firstprimes_inverse_simd[ 1600 ]),
	                    &(firstprimes_mullimit_simd[ 1600 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1664 ]),
	                     &(firstprimes_inverse_simd[ 1664 ]),
	                    &(firstprimes_mullimit_simd[ 1664 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1728 ]),
	                     &(firstprimes_inverse_simd[ 1728 ]),
	                    &(firstprimes_mullimit_simd[ 1728 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1792 ]),
	                     &(firstprimes_inverse_simd[ 1792 ]),
	                    &(firstprimes_mullimit_simd[ 1792 ])));
}


/*--------------------------------------
 * XXX
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_spfactor_rest_l(uint16_t tmp[   64 ],
                                 uint16_t tmp2[   64 ],
                           const uint16_t modn[ 3456 ])
{
	return (simd_no_spfactor_rest_m(tmp, tmp2, modn) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1856 ]),
	                     &(firstprimes_inverse_simd[ 1856 ]),
	                    &(firstprimes_mullimit_simd[ 1856 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1920 ]),
	                     &(firstprimes_inverse_simd[ 1920 ]),
	                    &(firstprimes_mullimit_simd[ 1920 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 1984 ]),
	                     &(firstprimes_inverse_simd[ 1984 ]),
	                    &(firstprimes_mullimit_simd[ 1984 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2048 ]),
	                     &(firstprimes_inverse_simd[ 2048 ]),
	                    &(firstprimes_mullimit_simd[ 2048 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2112 ]),
	                     &(firstprimes_inverse_simd[ 2112 ]),
	                    &(firstprimes_mullimit_simd[ 2112 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2176 ]),
	                     &(firstprimes_inverse_simd[ 2176 ]),
	                    &(firstprimes_mullimit_simd[ 2176 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2240 ]),
	                     &(firstprimes_inverse_simd[ 2240 ]),
	                    &(firstprimes_mullimit_simd[ 2240 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2304 ]),
	                     &(firstprimes_inverse_simd[ 2304 ]),
	                    &(firstprimes_mullimit_simd[ 2304 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2368 ]),
	                     &(firstprimes_inverse_simd[ 2368 ]),
	                    &(firstprimes_mullimit_simd[ 2368 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2432 ]),
	                     &(firstprimes_inverse_simd[ 2432 ]),
	                    &(firstprimes_mullimit_simd[ 2432 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2496 ]),
	                     &(firstprimes_inverse_simd[ 2496 ]),
	                    &(firstprimes_mullimit_simd[ 2496 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2560 ]),
	                     &(firstprimes_inverse_simd[ 2560 ]),
	                    &(firstprimes_mullimit_simd[ 2560 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2624 ]),
	                     &(firstprimes_inverse_simd[ 2624 ]),
	                    &(firstprimes_mullimit_simd[ 2624 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2688 ]),
	                     &(firstprimes_inverse_simd[ 2688 ]),
	                    &(firstprimes_mullimit_simd[ 2688 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2752 ]),
	                     &(firstprimes_inverse_simd[ 2752 ]),
	                    &(firstprimes_mullimit_simd[ 2752 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2816 ]),
	                     &(firstprimes_inverse_simd[ 2816 ]),
	                    &(firstprimes_mullimit_simd[ 2816 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2880 ]),
	                     &(firstprimes_inverse_simd[ 2880 ]),
	                    &(firstprimes_mullimit_simd[ 2880 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 2944 ]),
	                     &(firstprimes_inverse_simd[ 2944 ]),
	                    &(firstprimes_mullimit_simd[ 2944 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 3008 ]),
	                     &(firstprimes_inverse_simd[ 3008 ]),
	                    &(firstprimes_mullimit_simd[ 3008 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 3072 ]),
	                     &(firstprimes_inverse_simd[ 3072 ]),
	                    &(firstprimes_mullimit_simd[ 3072 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 3136 ]),
	                     &(firstprimes_inverse_simd[ 3136 ]),
	                    &(firstprimes_mullimit_simd[ 3136 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 3200 ]),
	                     &(firstprimes_inverse_simd[ 3200 ]),
	                    &(firstprimes_mullimit_simd[ 3200 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 3264 ]),
	                     &(firstprimes_inverse_simd[ 3264 ]),
	                    &(firstprimes_mullimit_simd[ 3264 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 3328 ]),
	                     &(firstprimes_inverse_simd[ 3328 ]),
	                    &(firstprimes_mullimit_simd[ 3328 ])) &&

	        simd_no_spfactor64x16(tmp, tmp2, &(modn[ 3392 ]),
	                     &(firstprimes_inverse_simd[ 3392 ]),
	                    &(firstprimes_mullimit_simd[ 3392 ])));
}


/*--------------------------------------
 * does any of the 'rest' of primes indicate a factor
 * of the main potential safe prime (Q) or that of (2Q+1)?
 * checks the first 576 primes
 *
 * see simd_no_spfactor_first(), which covers the first few small primes
 *
 * each of these groups becomes considerably less probable
 *
 * NOTE: DATA-DEPENDENT EXECUTION TIME DUE TO EARLY TERMINATION
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_spfactor_rest(uint16_t tmp[64],
                              uint16_t tmp2[64],
               const struct PP_Mod16bit *ps)
{
	return simd_no_spfactor_rest1(tmp, tmp2, ps->modn);
}


/*--------------------------------------
 * does any of the 'rest' of primes indicate a factor of a potential prime?
 * see simd_no_factor_first(), which covers the first few small primes
 *
 * NOTE: DATA-DEPENDENT EXECUTION TIME DUE TO EARLY TERMINATION
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_factor_rest(uint16_t tmp[64],
                            uint16_t tmp2[64],
             const struct PP_Mod16bit *ps)
{
// XXX
	return simd_no_spfactor_rest1(tmp, tmp2, ps->modn);
}


/*--------------------------------------
 * advance the 'first few' remainders in dst.modn[]
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance_remainders_first(struct PP_Mod16bit *dst, uint16_t adv)
{
	simd_advance64x16_m2r_inpl(dst->modn, adv,
	                           firstprimes_mod2range_simd);
}


/*--------------------------------------
 * advance rest of modn[]; together with simd_advance_remainders_first(),
 * all remainders---but none of the other fields---have been advanced
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance_remainders_rest(struct PP_Mod16bit *dst, uint16_t adv)
{
	simd_advance64x16_m2r_inpl(&(dst->modn[  64 ]), adv,
	          &(firstprimes_mod2range_simd[  64 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 128 ]), adv,
	          &(firstprimes_mod2range_simd[ 128 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 192 ]), adv,
	          &(firstprimes_mod2range_simd[ 192 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 256 ]), adv,
	          &(firstprimes_mod2range_simd[ 256 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 320 ]), adv,
	          &(firstprimes_mod2range_simd[ 320 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 384 ]), adv,
	          &(firstprimes_mod2range_simd[ 384 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 448 ]), adv,
	          &(firstprimes_mod2range_simd[ 448 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 512 ]), adv,
	          &(firstprimes_mod2range_simd[ 512 ]));
}


/*--------------------------------------
 * mid-sized equivalent of simd_advance_remainders_rest()
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance_remainders_rest_m(struct PP_Mod16bit *dst, uint16_t adv)
{
	simd_advance_remainders_rest(dst, adv);

	simd_advance64x16_m2r_inpl(&(dst->modn[  576 ]), adv,
	          &(firstprimes_mod2range_simd[  576 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[  640 ]), adv,
	          &(firstprimes_mod2range_simd[  640 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[  704 ]), adv,
	          &(firstprimes_mod2range_simd[  704 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[  768 ]), adv,
	          &(firstprimes_mod2range_simd[  768 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[  832 ]), adv,
	          &(firstprimes_mod2range_simd[  832 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[  896 ]), adv,
	          &(firstprimes_mod2range_simd[  896 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[  960 ]), adv,
	          &(firstprimes_mod2range_simd[  960 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1024 ]), adv,
	          &(firstprimes_mod2range_simd[ 1024 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1088 ]), adv,
	          &(firstprimes_mod2range_simd[ 1088 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1152 ]), adv,
	          &(firstprimes_mod2range_simd[ 1152 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1216 ]), adv,
	          &(firstprimes_mod2range_simd[ 1216 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1280 ]), adv,
	          &(firstprimes_mod2range_simd[ 1280 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1344 ]), adv,
	          &(firstprimes_mod2range_simd[ 1344 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1408 ]), adv,
	          &(firstprimes_mod2range_simd[ 1408 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1472 ]), adv,
	          &(firstprimes_mod2range_simd[ 1472 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1536 ]), adv,
	          &(firstprimes_mod2range_simd[ 1536 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1600 ]), adv,
	          &(firstprimes_mod2range_simd[ 1600 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1664 ]), adv,
	          &(firstprimes_mod2range_simd[ 1664 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1728 ]), adv,
	          &(firstprimes_mod2range_simd[ 1728 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1792 ]), adv,
	          &(firstprimes_mod2range_simd[ 1792 ]));
}


/*--------------------------------------
 * large-table equivalent of simd_advance_remainders_rest()
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance_remainders_rest_l(struct PP_Mod16bit *dst, uint16_t adv)
{
	simd_advance_remainders_rest_m(dst, adv);

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1856 ]), adv,
	          &(firstprimes_mod2range_simd[ 1856 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1920 ]), adv,
	          &(firstprimes_mod2range_simd[ 1920 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 1984 ]), adv,
	          &(firstprimes_mod2range_simd[ 1984 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2048 ]), adv,
	          &(firstprimes_mod2range_simd[ 2048 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2112 ]), adv,
	          &(firstprimes_mod2range_simd[ 2112 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2176 ]), adv,
	          &(firstprimes_mod2range_simd[ 2176 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2240 ]), adv,
	          &(firstprimes_mod2range_simd[ 2240 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2304 ]), adv,
	          &(firstprimes_mod2range_simd[ 2304 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2368 ]), adv,
	          &(firstprimes_mod2range_simd[ 2368 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2432 ]), adv,
	          &(firstprimes_mod2range_simd[ 2432 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2496 ]), adv,
	          &(firstprimes_mod2range_simd[ 2496 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2560 ]), adv,
	          &(firstprimes_mod2range_simd[ 2560 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2624 ]), adv,
	          &(firstprimes_mod2range_simd[ 2624 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2688 ]), adv,
	          &(firstprimes_mod2range_simd[ 2688 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2752 ]), adv,
	          &(firstprimes_mod2range_simd[ 2752 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2816 ]), adv,
	          &(firstprimes_mod2range_simd[ 2816 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2880 ]), adv,
	          &(firstprimes_mod2range_simd[ 2880 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 2944 ]), adv,
	          &(firstprimes_mod2range_simd[ 2944 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 3008 ]), adv,
	          &(firstprimes_mod2range_simd[ 3008 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 3072 ]), adv,
	          &(firstprimes_mod2range_simd[ 3072 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 3136 ]), adv,
	          &(firstprimes_mod2range_simd[ 3136 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 3200 ]), adv,
	          &(firstprimes_mod2range_simd[ 3200 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 3264 ]), adv,
	          &(firstprimes_mod2range_simd[ 3264 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 3328 ]), adv,
	          &(firstprimes_mod2range_simd[ 3328 ]));

	simd_advance64x16_m2r_inpl(&(dst->modn[ 3392 ]), adv,
	          &(firstprimes_mod2range_simd[ 3392 ]));
}


/*--------------------------------------
 * advance rest of state, which was skipped by first-rounds' checking
 * small table part
 *
 * dst->modn[] += adv[]
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance_rest_v_s(struct PP_Mod16bit *dst, const uint16_t adv[ 576 ])
{
	simd_advance64x16_m2r_inpl_v(&(dst->modn[  64 ]),
	                                   &(adv[  64 ]),
	            &(firstprimes_mod2range_simd[  64 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 128 ]),
	                                   &(adv[ 128 ]),
	            &(firstprimes_mod2range_simd[ 128 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 192 ]),
	                                   &(adv[ 192 ]),
	            &(firstprimes_mod2range_simd[ 192 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 256 ]),
	                                   &(adv[ 256 ]),
	            &(firstprimes_mod2range_simd[ 256 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 320 ]),
	                                   &(adv[ 320 ]),
	            &(firstprimes_mod2range_simd[ 320 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 384 ]),
	                                   &(adv[ 384 ]),
	            &(firstprimes_mod2range_simd[ 384 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 448 ]),
	                                   &(adv[ 448 ]),
	            &(firstprimes_mod2range_simd[ 448 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 512 ]),
	                                   &(adv[ 512 ]),
	            &(firstprimes_mod2range_simd[ 512 ]));
}


#if !defined(NO_SIMDDIVIDE_M) && !defined(NO_SIMDDIVIDE_L)  //---------------
/*--------------------------------------
 * mid-sized equivalent of simd_advance_rest_v_s()
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance_rest_v_m(struct PP_Mod16bit *dst, const uint16_t adv[ 1856 ])
{
	simd_advance_rest_v_s(dst, adv);

	simd_advance64x16_m2r_inpl_v(&(dst->modn[  576 ]),
	                                   &(adv[  576 ]),
	            &(firstprimes_mod2range_simd[  576 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[  640 ]),
	                                   &(adv[  640 ]),
	            &(firstprimes_mod2range_simd[  640 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[  704 ]),
	                                   &(adv[  704 ]),
	            &(firstprimes_mod2range_simd[  704 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[  768 ]),
	                                   &(adv[  768 ]),
	            &(firstprimes_mod2range_simd[  768 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[  832 ]),
	                                   &(adv[  832 ]),
	            &(firstprimes_mod2range_simd[  832 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[  896 ]),
	                                   &(adv[  896 ]),
	            &(firstprimes_mod2range_simd[  896 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[  960 ]),
	                                   &(adv[  960 ]),
	            &(firstprimes_mod2range_simd[  960 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1024 ]),
	                                   &(adv[ 1024 ]),
	            &(firstprimes_mod2range_simd[ 1024 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1088 ]),
	                                   &(adv[ 1088 ]),
	            &(firstprimes_mod2range_simd[ 1088 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1152 ]),
	                                   &(adv[ 1152 ]),
	            &(firstprimes_mod2range_simd[ 1152 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1216 ]),
	                                   &(adv[ 1216 ]),
	            &(firstprimes_mod2range_simd[ 1216 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1280 ]),
	                                   &(adv[ 1280 ]),
	            &(firstprimes_mod2range_simd[ 1280 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1344 ]),
	                                   &(adv[ 1344 ]),
	            &(firstprimes_mod2range_simd[ 1344 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1408 ]),
	                                   &(adv[ 1408 ]),
	            &(firstprimes_mod2range_simd[ 1408 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1472 ]),
	                                   &(adv[ 1472 ]),
	            &(firstprimes_mod2range_simd[ 1472 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1536 ]),
	                                   &(adv[ 1536 ]),
	            &(firstprimes_mod2range_simd[ 1536 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1600 ]),
	                                   &(adv[ 1600 ]),
	            &(firstprimes_mod2range_simd[ 1600 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1664 ]),
	                                   &(adv[ 1664 ]),
	            &(firstprimes_mod2range_simd[ 1664 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1728 ]),
	                                   &(adv[ 1728 ]),
	            &(firstprimes_mod2range_simd[ 1728 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1792 ]),
	                                   &(adv[ 1792 ]),
	            &(firstprimes_mod2range_simd[ 1792 ]));
}
#endif  // !NO_SIMDDIVIDE_M && NO_SIMDDIVIDE_L  //---------------------------


#if !defined(NO_SIMDDIVIDE_L)  //--------------------------------------------
/*--------------------------------------
 * large-table equivalent of simd_advance_rest_v_s()
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance_rest_v_l(struct PP_Mod16bit *dst, const uint16_t adv[ 3456 ])
{
	simd_advance_rest_v_m(dst, adv);

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1856 ]),
	                                   &(adv[ 1856 ]),
	            &(firstprimes_mod2range_simd[ 1856 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1920 ]),
	                                   &(adv[ 1920 ]),
	            &(firstprimes_mod2range_simd[ 1920 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 1984 ]),
	                                   &(adv[ 1984 ]),
	            &(firstprimes_mod2range_simd[ 1984 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2048 ]),
	                                   &(adv[ 2048 ]),
	            &(firstprimes_mod2range_simd[ 2048 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2112 ]),
	                                   &(adv[ 2112 ]),
	            &(firstprimes_mod2range_simd[ 2112 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2176 ]),
	                                   &(adv[ 2176 ]),
	            &(firstprimes_mod2range_simd[ 2176 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2240 ]),
	                                   &(adv[ 2240 ]),
	            &(firstprimes_mod2range_simd[ 2240 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2304 ]),
	                                   &(adv[ 2304 ]),
	            &(firstprimes_mod2range_simd[ 2304 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2368 ]),
	                                   &(adv[ 2368 ]),
	            &(firstprimes_mod2range_simd[ 2368 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2432 ]),
	                                   &(adv[ 2432 ]),
	            &(firstprimes_mod2range_simd[ 2432 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2496 ]),
	                                   &(adv[ 2496 ]),
	            &(firstprimes_mod2range_simd[ 2496 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2560 ]),
	                                   &(adv[ 2560 ]),
	            &(firstprimes_mod2range_simd[ 2560 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2624 ]),
	                                   &(adv[ 2624 ]),
	            &(firstprimes_mod2range_simd[ 2624 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2688 ]),
	                                   &(adv[ 2688 ]),
	            &(firstprimes_mod2range_simd[ 2688 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2752 ]),
	                                   &(adv[ 2752 ]),
	            &(firstprimes_mod2range_simd[ 2752 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2816 ]),
	                                   &(adv[ 2816 ]),
	            &(firstprimes_mod2range_simd[ 2816 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2880 ]),
	                                   &(adv[ 2880 ]),
	            &(firstprimes_mod2range_simd[ 2880 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 2944 ]),
	                                   &(adv[ 2944 ]),
	            &(firstprimes_mod2range_simd[ 2944 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 3008 ]),
	                                   &(adv[ 3008 ]),
	            &(firstprimes_mod2range_simd[ 3008 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 3072 ]),
	                                   &(adv[ 3072 ]),
	            &(firstprimes_mod2range_simd[ 3072 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 3136 ]),
	                                   &(adv[ 3136 ]),
	            &(firstprimes_mod2range_simd[ 3136 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 3200 ]),
	                                   &(adv[ 3200 ]),
	            &(firstprimes_mod2range_simd[ 3200 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 3264 ]),
	                                   &(adv[ 3264 ]),
	            &(firstprimes_mod2range_simd[ 3264 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 3328 ]),
	                                   &(adv[ 3328 ]),
	            &(firstprimes_mod2range_simd[ 3328 ]));

	simd_advance64x16_m2r_inpl_v(&(dst->modn[ 3392 ]),
	                                   &(adv[ 3392 ]),
	            &(firstprimes_mod2range_simd[ 3392 ]));
}
#endif    // !NO_SIMDDIVIDE_L  ----------------------------------------------
#endif      // /delimiter: generated code  -----------------------------------


/*-----------------------------------------
 * administer offset etc. changes when candidate has been advanced by 'adv'
 * assume .modn[] has been advanced
 */
static inline
uint64_t state_advance_nr(struct PP_Mod16bit *ps, unsigned int adv)
{
	uint64_t rc = 0;

	if (ps) {
		ps->offset += adv;
		ps->lsb    += adv;
		ps->mod6   =  (ps->mod6 + (adv % 6)) % 6;

		rc = ps->lsb;
	}

	return rc;
}


/*-----------------------------------------
 * vector[ SIMDPRIME_COUNT ] += adv; also updates general-nr counters
 * small prime-table size
 */
static inline
REALLY_FORCE_INLINE
//
void simd_advance_all_s(struct PP_Mod16bit *ps, uint16_t adv)
{
	state_advance_nr(ps, adv);

	simd_advance_remainders_first(ps, adv);
	simd_advance_remainders_rest(ps, adv);
}


/*-----------------------------------------
 * medium prime-table size equivalent of simd_advance_all()
 */
static inline
REALLY_FORCE_INLINE
//
void simd_advance_all_m(struct PP_Mod16bit *ps, uint16_t adv)
{
	state_advance_nr(ps, adv);

	simd_advance_remainders_first(ps, adv);
	simd_advance_remainders_rest_m(ps, adv);
}


/*-----------------------------------------
 * large prime-table size equivalent of simd_advance_all()
 */
static inline
REALLY_FORCE_INLINE
//
void simd_advance_all_l(struct PP_Mod16bit *ps, uint16_t adv)
{
	state_advance_nr(ps, adv);

	simd_advance_remainders_first(ps, adv);
	simd_advance_remainders_rest_l(ps, adv);
}


/*-----------------------------------------
 * autoselects size from ps->mode
 */
static inline
REALLY_FORCE_INLINE
//
void simd_advance_all(struct PP_Mod16bit *ps, uint16_t adv)
{
	switch (ps ? (ps->mode & SIMD_SEARCHTABLE_MASK) : 0) {
#if !defined(NO_SIMDDIVIDE_S)
	case SIMD_SEARCHTABLE_S:
		simd_advance_all_s(ps, adv);
		break;
#endif

#if !defined(NO_SIMDDIVIDE_M)
	case SIMD_SEARCHTABLE_M:
		simd_advance_all_m(ps, adv);
		break;
#endif

#if !defined(NO_SIMDDIVIDE_L)
	case SIMD_SEARCHTABLE_L:
		simd_advance_all_l(ps, adv);
		break;
#endif

	default:
		break;
	}
}


/*-----------------------------------------
 * ps->modn[] += ps->incr[] (vector addition)
 *
 * autoselects size from ps->mode
 */
static inline
REALLY_FORCE_INLINE
//
void simd_advance_rest_v_aw(struct PP_Mod16bit *ps)
{
	switch (ps ? (ps->mode & SIMD_SEARCHTABLE_MASK) : 0) {
#if !defined(NO_SIMDDIVIDE_S)
	case SIMD_SEARCHTABLE_S:
		simd_advance_rest_v_s(ps, ps->incr);
		break;
#endif

#if !defined(NO_SIMDDIVIDE_M)
	case SIMD_SEARCHTABLE_M:
		simd_advance_rest_v_m(ps, ps->incr);
		break;
#endif

#if !defined(NO_SIMDDIVIDE_L)
	case SIMD_SEARCHTABLE_L:
		simd_advance_rest_v_l(ps, ps->incr);
		break;
#endif

	default:
		break;
	}
}


/*-----------------------------------------
 * safe-prime search: can modn[] indicate a prime simultaneously
 * with 2*modn[]+1 also prime?
 *
 * SECURITY NOTE: tmp[] and tmp2[] are scratch, filled with
 * (potentially)secret state-dependent data, and SHOULD be wiped upon return.
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_spfactor64x16(uint16_t tmp[ 64 ],
                               uint16_t tm2[ 64 ],
                        const uint16_t modn[ 64 ], const uint16_t inv[64],
                       const uint16_t limit[ 64 ])
{
				// compute n * 1/prime  mod 2^16 products

	simd_mul16x16(  tmp,          modn,          inv       );
	simd_mul16x16(&(tmp[ 16 ]), &(modn[ 16 ]), &(inv[ 16 ]));
	simd_mul16x16(&(tmp[ 32 ]), &(modn[ 32 ]), &(inv[ 32 ]));
	simd_mul16x16(&(tmp[ 48 ]), &(modn[ 48 ]), &(inv[ 48 ]));

	simd_spcmp16x16(  tmp,         tmp,         inv,         limit       );
	simd_spcmp16x16(&(tmp[ 16 ]),&(tmp[ 16 ]),&(inv[ 16 ]),&(limit[ 16 ]));
	simd_spcmp16x16(&(tmp[ 32 ]),&(tmp[ 32 ]),&(inv[ 32 ]),&(limit[ 32 ]));
	simd_spcmp16x16(&(tmp[ 48 ]),&(tmp[ 48 ]),&(inv[ 48 ]),&(limit[ 48 ]));

// simd_report_0ff("## CMASK=", tmp, 64);

	return simd_is_all0x64x16_inpl(tmp, tm2);
}
#endif   //-----  /delimiter: generated SIMD columns  ------------------------


#if 1   //=====  Montgomery multiplication  ==================================
// build 16x16 -> 32 bit multiples
// Comba-add them at 16-bit offsets
// size-specialized: this is for 2048-bit modulus (128x16 bits)
//
// using 15-bit digits allows carry-less accumulation of 15x15-bit products
// 2048 bits -> 137x16 = 2192 bits -> 144x16 (= full SIMD units)
// only leading u16[] elements are used; [136] is the LS unit

// see Bos, Montgomery, Shumow, Zaverucha:
// Montgomery multiplication using vector instructions
// 2013-08
// XXX URL

#define  SIMD_MASK15BIT  UINT16_C(0x7fff)


//--------------------------------------
// r[] = v[] * s; 2048 -> SIMD operands
//
static inline
REALLY_FORCE_INLINE
//
void simd_montg2048_mul1(uint32_t r[144], const uint16_t v[144], uint16_t s)
{
	r[  0] = v[  0] * s;
	r[  1] = v[  1] * s;
	r[  2] = v[  2] * s;
	r[  3] = v[  3] * s;
	r[  4] = v[  4] * s;
	r[  5] = v[  5] * s;
	r[  6] = v[  6] * s;
	r[  7] = v[  7] * s;
	r[  8] = v[  8] * s;
	r[  9] = v[  9] * s;
	r[ 10] = v[ 10] * s;
	r[ 11] = v[ 11] * s;
	r[ 12] = v[ 12] * s;
	r[ 13] = v[ 13] * s;
	r[ 14] = v[ 14] * s;
	r[ 15] = v[ 15] * s;
	r[ 16] = v[ 16] * s;
	r[ 17] = v[ 17] * s;
	r[ 18] = v[ 18] * s;
	r[ 19] = v[ 19] * s;
	r[ 20] = v[ 20] * s;
	r[ 21] = v[ 21] * s;
	r[ 22] = v[ 22] * s;
	r[ 23] = v[ 23] * s;
	r[ 24] = v[ 24] * s;
	r[ 25] = v[ 25] * s;
	r[ 26] = v[ 26] * s;
	r[ 27] = v[ 27] * s;
	r[ 28] = v[ 28] * s;
	r[ 29] = v[ 29] * s;
	r[ 30] = v[ 30] * s;
	r[ 31] = v[ 31] * s;
	r[ 32] = v[ 32] * s;
	r[ 33] = v[ 33] * s;
	r[ 34] = v[ 34] * s;
	r[ 35] = v[ 35] * s;
	r[ 36] = v[ 36] * s;
	r[ 37] = v[ 37] * s;
	r[ 38] = v[ 38] * s;
	r[ 39] = v[ 39] * s;
	r[ 40] = v[ 40] * s;
	r[ 41] = v[ 41] * s;
	r[ 42] = v[ 42] * s;
	r[ 43] = v[ 43] * s;
	r[ 44] = v[ 44] * s;
	r[ 45] = v[ 45] * s;
	r[ 46] = v[ 46] * s;
	r[ 47] = v[ 47] * s;
	r[ 48] = v[ 48] * s;
	r[ 49] = v[ 49] * s;
	r[ 50] = v[ 50] * s;
	r[ 51] = v[ 51] * s;
	r[ 52] = v[ 52] * s;
	r[ 53] = v[ 53] * s;
	r[ 54] = v[ 54] * s;
	r[ 55] = v[ 55] * s;
	r[ 56] = v[ 56] * s;
	r[ 57] = v[ 57] * s;
	r[ 58] = v[ 58] * s;
	r[ 59] = v[ 59] * s;
	r[ 60] = v[ 60] * s;
	r[ 61] = v[ 61] * s;
	r[ 62] = v[ 62] * s;
	r[ 63] = v[ 63] * s;
	r[ 64] = v[ 64] * s;
	r[ 65] = v[ 65] * s;
	r[ 66] = v[ 66] * s;
	r[ 67] = v[ 67] * s;
	r[ 68] = v[ 68] * s;
	r[ 69] = v[ 69] * s;
	r[ 70] = v[ 70] * s;
	r[ 71] = v[ 71] * s;
	r[ 72] = v[ 72] * s;
	r[ 73] = v[ 73] * s;
	r[ 74] = v[ 74] * s;
	r[ 75] = v[ 75] * s;
	r[ 76] = v[ 76] * s;
	r[ 77] = v[ 77] * s;
	r[ 78] = v[ 78] * s;
	r[ 79] = v[ 79] * s;
	r[ 80] = v[ 80] * s;
	r[ 81] = v[ 81] * s;
	r[ 82] = v[ 82] * s;
	r[ 83] = v[ 83] * s;
	r[ 84] = v[ 84] * s;
	r[ 85] = v[ 85] * s;
	r[ 86] = v[ 86] * s;
	r[ 87] = v[ 87] * s;
	r[ 88] = v[ 88] * s;
	r[ 89] = v[ 89] * s;
	r[ 90] = v[ 90] * s;
	r[ 91] = v[ 91] * s;
	r[ 92] = v[ 92] * s;
	r[ 93] = v[ 93] * s;
	r[ 94] = v[ 94] * s;
	r[ 95] = v[ 95] * s;
	r[ 96] = v[ 96] * s;
	r[ 97] = v[ 97] * s;
	r[ 98] = v[ 98] * s;
	r[ 99] = v[ 99] * s;
	r[100] = v[100] * s;
	r[101] = v[101] * s;
	r[102] = v[102] * s;
	r[103] = v[103] * s;
	r[104] = v[104] * s;
	r[105] = v[105] * s;
	r[106] = v[106] * s;
	r[107] = v[107] * s;
	r[108] = v[108] * s;
	r[109] = v[109] * s;
	r[110] = v[110] * s;
	r[111] = v[111] * s;
	r[112] = v[112] * s;
	r[113] = v[113] * s;
	r[114] = v[114] * s;
	r[115] = v[115] * s;
	r[116] = v[116] * s;
	r[117] = v[117] * s;
	r[118] = v[118] * s;
	r[119] = v[119] * s;
	r[120] = v[120] * s;
	r[121] = v[121] * s;
	r[122] = v[122] * s;
	r[123] = v[123] * s;
	r[124] = v[124] * s;
	r[125] = v[125] * s;
	r[126] = v[126] * s;
	r[127] = v[127] * s;
	r[128] = v[128] * s;
	r[129] = v[129] * s;
	r[130] = v[130] * s;
	r[131] = v[131] * s;
	r[132] = v[132] * s;
	r[133] = v[133] * s;
	r[134] = v[134] * s;
	r[135] = v[135] * s;
	r[136] = v[136] * s;
	r[137] = v[137] * s;
	r[138] = v[138] * s;
	r[139] = v[139] * s;
	r[140] = v[140] * s;
	r[141] = v[141] * s;
	r[142] = v[142] * s;
	r[143] = v[143] * s;
}


//--------------------------------------
// r[] = a[] + b[]; 2048-bit -> SIMD operands
//
static inline
REALLY_FORCE_INLINE
//
void simd_montg2048_add1_inpl(uint32_t a[144], const uint32_t b[144])
{
	a[  0] += b[  0];
	a[  1] += b[  1];
	a[  2] += b[  2];
	a[  3] += b[  3];
	a[  4] += b[  4];
	a[  5] += b[  5];
	a[  6] += b[  6];
	a[  7] += b[  7];
	a[  8] += b[  8];
	a[  9] += b[  9];
	a[ 10] += b[ 10];
	a[ 11] += b[ 11];
	a[ 12] += b[ 12];
	a[ 13] += b[ 13];
	a[ 14] += b[ 14];
	a[ 15] += b[ 15];
	a[ 16] += b[ 16];
	a[ 17] += b[ 17];
	a[ 18] += b[ 18];
	a[ 19] += b[ 19];
	a[ 20] += b[ 20];
	a[ 21] += b[ 21];
	a[ 22] += b[ 22];
	a[ 23] += b[ 23];
	a[ 24] += b[ 24];
	a[ 25] += b[ 25];
	a[ 26] += b[ 26];
	a[ 27] += b[ 27];
	a[ 28] += b[ 28];
	a[ 29] += b[ 29];
	a[ 30] += b[ 30];
	a[ 31] += b[ 31];
	a[ 32] += b[ 32];
	a[ 33] += b[ 33];
	a[ 34] += b[ 34];
	a[ 35] += b[ 35];
	a[ 36] += b[ 36];
	a[ 37] += b[ 37];
	a[ 38] += b[ 38];
	a[ 39] += b[ 39];
	a[ 40] += b[ 40];
	a[ 41] += b[ 41];
	a[ 42] += b[ 42];
	a[ 43] += b[ 43];
	a[ 44] += b[ 44];
	a[ 45] += b[ 45];
	a[ 46] += b[ 46];
	a[ 47] += b[ 47];
	a[ 48] += b[ 48];
	a[ 49] += b[ 49];
	a[ 50] += b[ 50];
	a[ 51] += b[ 51];
	a[ 52] += b[ 52];
	a[ 53] += b[ 53];
	a[ 54] += b[ 54];
	a[ 55] += b[ 55];
	a[ 56] += b[ 56];
	a[ 57] += b[ 57];
	a[ 58] += b[ 58];
	a[ 59] += b[ 59];
	a[ 60] += b[ 60];
	a[ 61] += b[ 61];
	a[ 62] += b[ 62];
	a[ 63] += b[ 63];
	a[ 64] += b[ 64];
	a[ 65] += b[ 65];
	a[ 66] += b[ 66];
	a[ 67] += b[ 67];
	a[ 68] += b[ 68];
	a[ 69] += b[ 69];
	a[ 70] += b[ 70];
	a[ 71] += b[ 71];
	a[ 72] += b[ 72];
	a[ 73] += b[ 73];
	a[ 74] += b[ 74];
	a[ 75] += b[ 75];
	a[ 76] += b[ 76];
	a[ 77] += b[ 77];
	a[ 78] += b[ 78];
	a[ 79] += b[ 79];
	a[ 80] += b[ 80];
	a[ 81] += b[ 81];
	a[ 82] += b[ 82];
	a[ 83] += b[ 83];
	a[ 84] += b[ 84];
	a[ 85] += b[ 85];
	a[ 86] += b[ 86];
	a[ 87] += b[ 87];
	a[ 88] += b[ 88];
	a[ 89] += b[ 89];
	a[ 90] += b[ 90];
	a[ 91] += b[ 91];
	a[ 92] += b[ 92];
	a[ 93] += b[ 93];
	a[ 94] += b[ 94];
	a[ 95] += b[ 95];
	a[ 96] += b[ 96];
	a[ 97] += b[ 97];
	a[ 98] += b[ 98];
	a[ 99] += b[ 99];
	a[100] += b[100];
	a[101] += b[101];
	a[102] += b[102];
	a[103] += b[103];
	a[104] += b[104];
	a[105] += b[105];
	a[106] += b[106];
	a[107] += b[107];
	a[108] += b[108];
	a[109] += b[109];
	a[110] += b[110];
	a[111] += b[111];
	a[112] += b[112];
	a[113] += b[113];
	a[114] += b[114];
	a[115] += b[115];
	a[116] += b[116];
	a[117] += b[117];
	a[118] += b[118];
	a[119] += b[119];
	a[120] += b[120];
	a[121] += b[121];
	a[122] += b[122];
	a[123] += b[123];
	a[124] += b[124];
	a[125] += b[125];
	a[126] += b[126];
	a[127] += b[127];
	a[128] += b[128];
	a[129] += b[129];
	a[130] += b[130];
	a[131] += b[131];
	a[132] += b[132];
	a[133] += b[133];
	a[134] += b[134];
	a[135] += b[135];
	a[136] += b[136];
	a[137] += b[137];
	a[138] += b[138];
	a[139] += b[139];
	a[140] += b[140];
	a[141] += b[141];
	a[142] += b[142];
	a[143] += b[143];
}


//--------------------------------------
// split 30-bit products into 15+15-bit halves
//
static inline
REALLY_FORCE_INLINE
//
void simd_montg2048_split32x16(uint16_t r1[144],
                               uint16_t r2[144], const uint32_t a[144])
{
	r1[  0] = (uint16_t) (a[  0] >> 15);
	r1[  1] = (uint16_t) (a[  1] >> 15);
	r1[  2] = (uint16_t) (a[  2] >> 15);
	r1[  3] = (uint16_t) (a[  3] >> 15);
	r1[  4] = (uint16_t) (a[  4] >> 15);
	r1[  5] = (uint16_t) (a[  5] >> 15);
	r1[  6] = (uint16_t) (a[  6] >> 15);
	r1[  7] = (uint16_t) (a[  7] >> 15);
	r1[  8] = (uint16_t) (a[  8] >> 15);
	r1[  9] = (uint16_t) (a[  9] >> 15);
	r1[ 10] = (uint16_t) (a[ 10] >> 15);
	r1[ 11] = (uint16_t) (a[ 11] >> 15);
	r1[ 12] = (uint16_t) (a[ 12] >> 15);
	r1[ 13] = (uint16_t) (a[ 13] >> 15);
	r1[ 14] = (uint16_t) (a[ 14] >> 15);
	r1[ 15] = (uint16_t) (a[ 15] >> 15);
	r1[ 16] = (uint16_t) (a[ 16] >> 15);
	r1[ 17] = (uint16_t) (a[ 17] >> 15);
	r1[ 18] = (uint16_t) (a[ 18] >> 15);
	r1[ 19] = (uint16_t) (a[ 19] >> 15);
	r1[ 20] = (uint16_t) (a[ 20] >> 15);
	r1[ 21] = (uint16_t) (a[ 21] >> 15);
	r1[ 22] = (uint16_t) (a[ 22] >> 15);
	r1[ 23] = (uint16_t) (a[ 23] >> 15);
	r1[ 24] = (uint16_t) (a[ 24] >> 15);
	r1[ 25] = (uint16_t) (a[ 25] >> 15);
	r1[ 26] = (uint16_t) (a[ 26] >> 15);
	r1[ 27] = (uint16_t) (a[ 27] >> 15);
	r1[ 28] = (uint16_t) (a[ 28] >> 15);
	r1[ 29] = (uint16_t) (a[ 29] >> 15);
	r1[ 30] = (uint16_t) (a[ 30] >> 15);
	r1[ 31] = (uint16_t) (a[ 31] >> 15);
	r1[ 32] = (uint16_t) (a[ 32] >> 15);
	r1[ 33] = (uint16_t) (a[ 33] >> 15);
	r1[ 34] = (uint16_t) (a[ 34] >> 15);
	r1[ 35] = (uint16_t) (a[ 35] >> 15);
	r1[ 36] = (uint16_t) (a[ 36] >> 15);
	r1[ 37] = (uint16_t) (a[ 37] >> 15);
	r1[ 38] = (uint16_t) (a[ 38] >> 15);
	r1[ 39] = (uint16_t) (a[ 39] >> 15);
	r1[ 40] = (uint16_t) (a[ 40] >> 15);
	r1[ 41] = (uint16_t) (a[ 41] >> 15);
	r1[ 42] = (uint16_t) (a[ 42] >> 15);
	r1[ 43] = (uint16_t) (a[ 43] >> 15);
	r1[ 44] = (uint16_t) (a[ 44] >> 15);
	r1[ 45] = (uint16_t) (a[ 45] >> 15);
	r1[ 46] = (uint16_t) (a[ 46] >> 15);
	r1[ 47] = (uint16_t) (a[ 47] >> 15);
	r1[ 48] = (uint16_t) (a[ 48] >> 15);
	r1[ 49] = (uint16_t) (a[ 49] >> 15);
	r1[ 50] = (uint16_t) (a[ 50] >> 15);
	r1[ 51] = (uint16_t) (a[ 51] >> 15);
	r1[ 52] = (uint16_t) (a[ 52] >> 15);
	r1[ 53] = (uint16_t) (a[ 53] >> 15);
	r1[ 54] = (uint16_t) (a[ 54] >> 15);
	r1[ 55] = (uint16_t) (a[ 55] >> 15);
	r1[ 56] = (uint16_t) (a[ 56] >> 15);
	r1[ 57] = (uint16_t) (a[ 57] >> 15);
	r1[ 58] = (uint16_t) (a[ 58] >> 15);
	r1[ 59] = (uint16_t) (a[ 59] >> 15);
	r1[ 60] = (uint16_t) (a[ 60] >> 15);
	r1[ 61] = (uint16_t) (a[ 61] >> 15);
	r1[ 62] = (uint16_t) (a[ 62] >> 15);
	r1[ 63] = (uint16_t) (a[ 63] >> 15);
	r1[ 64] = (uint16_t) (a[ 64] >> 15);
	r1[ 65] = (uint16_t) (a[ 65] >> 15);
	r1[ 66] = (uint16_t) (a[ 66] >> 15);
	r1[ 67] = (uint16_t) (a[ 67] >> 15);
	r1[ 68] = (uint16_t) (a[ 68] >> 15);
	r1[ 69] = (uint16_t) (a[ 69] >> 15);
	r1[ 70] = (uint16_t) (a[ 70] >> 15);
	r1[ 71] = (uint16_t) (a[ 71] >> 15);
	r1[ 72] = (uint16_t) (a[ 72] >> 15);
	r1[ 73] = (uint16_t) (a[ 73] >> 15);
	r1[ 74] = (uint16_t) (a[ 74] >> 15);
	r1[ 75] = (uint16_t) (a[ 75] >> 15);
	r1[ 76] = (uint16_t) (a[ 76] >> 15);
	r1[ 77] = (uint16_t) (a[ 77] >> 15);
	r1[ 78] = (uint16_t) (a[ 78] >> 15);
	r1[ 79] = (uint16_t) (a[ 79] >> 15);
	r1[ 80] = (uint16_t) (a[ 80] >> 15);
	r1[ 81] = (uint16_t) (a[ 81] >> 15);
	r1[ 82] = (uint16_t) (a[ 82] >> 15);
	r1[ 83] = (uint16_t) (a[ 83] >> 15);
	r1[ 84] = (uint16_t) (a[ 84] >> 15);
	r1[ 85] = (uint16_t) (a[ 85] >> 15);
	r1[ 86] = (uint16_t) (a[ 86] >> 15);
	r1[ 87] = (uint16_t) (a[ 87] >> 15);
	r1[ 88] = (uint16_t) (a[ 88] >> 15);
	r1[ 89] = (uint16_t) (a[ 89] >> 15);
	r1[ 90] = (uint16_t) (a[ 90] >> 15);
	r1[ 91] = (uint16_t) (a[ 91] >> 15);
	r1[ 92] = (uint16_t) (a[ 92] >> 15);
	r1[ 93] = (uint16_t) (a[ 93] >> 15);
	r1[ 94] = (uint16_t) (a[ 94] >> 15);
	r1[ 95] = (uint16_t) (a[ 95] >> 15);
	r1[ 96] = (uint16_t) (a[ 96] >> 15);
	r1[ 97] = (uint16_t) (a[ 97] >> 15);
	r1[ 98] = (uint16_t) (a[ 98] >> 15);
	r1[ 99] = (uint16_t) (a[ 99] >> 15);
	r1[100] = (uint16_t) (a[100] >> 15);
	r1[101] = (uint16_t) (a[101] >> 15);
	r1[102] = (uint16_t) (a[102] >> 15);
	r1[103] = (uint16_t) (a[103] >> 15);
	r1[104] = (uint16_t) (a[104] >> 15);
	r1[105] = (uint16_t) (a[105] >> 15);
	r1[106] = (uint16_t) (a[106] >> 15);
	r1[107] = (uint16_t) (a[107] >> 15);
	r1[108] = (uint16_t) (a[108] >> 15);
	r1[109] = (uint16_t) (a[109] >> 15);
	r1[110] = (uint16_t) (a[110] >> 15);
	r1[111] = (uint16_t) (a[111] >> 15);
	r1[112] = (uint16_t) (a[112] >> 15);
	r1[113] = (uint16_t) (a[113] >> 15);
	r1[114] = (uint16_t) (a[114] >> 15);
	r1[115] = (uint16_t) (a[115] >> 15);
	r1[116] = (uint16_t) (a[116] >> 15);
	r1[117] = (uint16_t) (a[117] >> 15);
	r1[118] = (uint16_t) (a[118] >> 15);
	r1[119] = (uint16_t) (a[119] >> 15);
	r1[120] = (uint16_t) (a[120] >> 15);
	r1[121] = (uint16_t) (a[121] >> 15);
	r1[122] = (uint16_t) (a[122] >> 15);
	r1[123] = (uint16_t) (a[123] >> 15);
	r1[124] = (uint16_t) (a[124] >> 15);
	r1[125] = (uint16_t) (a[125] >> 15);
	r1[126] = (uint16_t) (a[126] >> 15);
	r1[127] = (uint16_t) (a[127] >> 15);
	r1[128] = (uint16_t) (a[128] >> 15);
	r1[129] = (uint16_t) (a[129] >> 15);
	r1[130] = (uint16_t) (a[130] >> 15);
	r1[131] = (uint16_t) (a[131] >> 15);
	r1[132] = (uint16_t) (a[132] >> 15);
	r1[133] = (uint16_t) (a[133] >> 15);
	r1[134] = (uint16_t) (a[134] >> 15);
	r1[135] = (uint16_t) (a[135] >> 15);
	r1[136] = (uint16_t) (a[136] >> 15);
	r1[137] = (uint16_t) (a[137] >> 15);
	r1[138] = (uint16_t) (a[138] >> 15);
	r1[139] = (uint16_t) (a[139] >> 15);
	r1[140] = (uint16_t) (a[140] >> 15);
	r1[141] = (uint16_t) (a[141] >> 15);
	r1[142] = (uint16_t) (a[142] >> 15);
	r1[143] = (uint16_t) (a[143] >> 15);

	r2[  0] = UINT16_C(0x7fff) & a[  0];
	r2[  1] = UINT16_C(0x7fff) & a[  1];
	r2[  2] = UINT16_C(0x7fff) & a[  2];
	r2[  3] = UINT16_C(0x7fff) & a[  3];
	r2[  4] = UINT16_C(0x7fff) & a[  4];
	r2[  5] = UINT16_C(0x7fff) & a[  5];
	r2[  6] = UINT16_C(0x7fff) & a[  6];
	r2[  7] = UINT16_C(0x7fff) & a[  7];
	r2[  8] = UINT16_C(0x7fff) & a[  8];
	r2[  9] = UINT16_C(0x7fff) & a[  9];
	r2[ 10] = UINT16_C(0x7fff) & a[ 10];
	r2[ 11] = UINT16_C(0x7fff) & a[ 11];
	r2[ 12] = UINT16_C(0x7fff) & a[ 12];
	r2[ 13] = UINT16_C(0x7fff) & a[ 13];
	r2[ 14] = UINT16_C(0x7fff) & a[ 14];
	r2[ 15] = UINT16_C(0x7fff) & a[ 15];
	r2[ 16] = UINT16_C(0x7fff) & a[ 16];
	r2[ 17] = UINT16_C(0x7fff) & a[ 17];
	r2[ 18] = UINT16_C(0x7fff) & a[ 18];
	r2[ 19] = UINT16_C(0x7fff) & a[ 19];
	r2[ 20] = UINT16_C(0x7fff) & a[ 20];
	r2[ 21] = UINT16_C(0x7fff) & a[ 21];
	r2[ 22] = UINT16_C(0x7fff) & a[ 22];
	r2[ 23] = UINT16_C(0x7fff) & a[ 23];
	r2[ 24] = UINT16_C(0x7fff) & a[ 24];
	r2[ 25] = UINT16_C(0x7fff) & a[ 25];
	r2[ 26] = UINT16_C(0x7fff) & a[ 26];
	r2[ 27] = UINT16_C(0x7fff) & a[ 27];
	r2[ 28] = UINT16_C(0x7fff) & a[ 28];
	r2[ 29] = UINT16_C(0x7fff) & a[ 29];
	r2[ 30] = UINT16_C(0x7fff) & a[ 30];
	r2[ 31] = UINT16_C(0x7fff) & a[ 31];
	r2[ 32] = UINT16_C(0x7fff) & a[ 32];
	r2[ 33] = UINT16_C(0x7fff) & a[ 33];
	r2[ 34] = UINT16_C(0x7fff) & a[ 34];
	r2[ 35] = UINT16_C(0x7fff) & a[ 35];
	r2[ 36] = UINT16_C(0x7fff) & a[ 36];
	r2[ 37] = UINT16_C(0x7fff) & a[ 37];
	r2[ 38] = UINT16_C(0x7fff) & a[ 38];
	r2[ 39] = UINT16_C(0x7fff) & a[ 39];
	r2[ 40] = UINT16_C(0x7fff) & a[ 40];
	r2[ 41] = UINT16_C(0x7fff) & a[ 41];
	r2[ 42] = UINT16_C(0x7fff) & a[ 42];
	r2[ 43] = UINT16_C(0x7fff) & a[ 43];
	r2[ 44] = UINT16_C(0x7fff) & a[ 44];
	r2[ 45] = UINT16_C(0x7fff) & a[ 45];
	r2[ 46] = UINT16_C(0x7fff) & a[ 46];
	r2[ 47] = UINT16_C(0x7fff) & a[ 47];
	r2[ 48] = UINT16_C(0x7fff) & a[ 48];
	r2[ 49] = UINT16_C(0x7fff) & a[ 49];
	r2[ 50] = UINT16_C(0x7fff) & a[ 50];
	r2[ 51] = UINT16_C(0x7fff) & a[ 51];
	r2[ 52] = UINT16_C(0x7fff) & a[ 52];
	r2[ 53] = UINT16_C(0x7fff) & a[ 53];
	r2[ 54] = UINT16_C(0x7fff) & a[ 54];
	r2[ 55] = UINT16_C(0x7fff) & a[ 55];
	r2[ 56] = UINT16_C(0x7fff) & a[ 56];
	r2[ 57] = UINT16_C(0x7fff) & a[ 57];
	r2[ 58] = UINT16_C(0x7fff) & a[ 58];
	r2[ 59] = UINT16_C(0x7fff) & a[ 59];
	r2[ 60] = UINT16_C(0x7fff) & a[ 60];
	r2[ 61] = UINT16_C(0x7fff) & a[ 61];
	r2[ 62] = UINT16_C(0x7fff) & a[ 62];
	r2[ 63] = UINT16_C(0x7fff) & a[ 63];
	r2[ 64] = UINT16_C(0x7fff) & a[ 64];
	r2[ 65] = UINT16_C(0x7fff) & a[ 65];
	r2[ 66] = UINT16_C(0x7fff) & a[ 66];
	r2[ 67] = UINT16_C(0x7fff) & a[ 67];
	r2[ 68] = UINT16_C(0x7fff) & a[ 68];
	r2[ 69] = UINT16_C(0x7fff) & a[ 69];
	r2[ 70] = UINT16_C(0x7fff) & a[ 70];
	r2[ 71] = UINT16_C(0x7fff) & a[ 71];
	r2[ 72] = UINT16_C(0x7fff) & a[ 72];
	r2[ 73] = UINT16_C(0x7fff) & a[ 73];
	r2[ 74] = UINT16_C(0x7fff) & a[ 74];
	r2[ 75] = UINT16_C(0x7fff) & a[ 75];
	r2[ 76] = UINT16_C(0x7fff) & a[ 76];
	r2[ 77] = UINT16_C(0x7fff) & a[ 77];
	r2[ 78] = UINT16_C(0x7fff) & a[ 78];
	r2[ 79] = UINT16_C(0x7fff) & a[ 79];
	r2[ 80] = UINT16_C(0x7fff) & a[ 80];
	r2[ 81] = UINT16_C(0x7fff) & a[ 81];
	r2[ 82] = UINT16_C(0x7fff) & a[ 82];
	r2[ 83] = UINT16_C(0x7fff) & a[ 83];
	r2[ 84] = UINT16_C(0x7fff) & a[ 84];
	r2[ 85] = UINT16_C(0x7fff) & a[ 85];
	r2[ 86] = UINT16_C(0x7fff) & a[ 86];
	r2[ 87] = UINT16_C(0x7fff) & a[ 87];
	r2[ 88] = UINT16_C(0x7fff) & a[ 88];
	r2[ 89] = UINT16_C(0x7fff) & a[ 89];
	r2[ 90] = UINT16_C(0x7fff) & a[ 90];
	r2[ 91] = UINT16_C(0x7fff) & a[ 91];
	r2[ 92] = UINT16_C(0x7fff) & a[ 92];
	r2[ 93] = UINT16_C(0x7fff) & a[ 93];
	r2[ 94] = UINT16_C(0x7fff) & a[ 94];
	r2[ 95] = UINT16_C(0x7fff) & a[ 95];
	r2[ 96] = UINT16_C(0x7fff) & a[ 96];
	r2[ 97] = UINT16_C(0x7fff) & a[ 97];
	r2[ 98] = UINT16_C(0x7fff) & a[ 98];
	r2[ 99] = UINT16_C(0x7fff) & a[ 99];
	r2[100] = UINT16_C(0x7fff) & a[100];
	r2[101] = UINT16_C(0x7fff) & a[101];
	r2[102] = UINT16_C(0x7fff) & a[102];
	r2[103] = UINT16_C(0x7fff) & a[103];
	r2[104] = UINT16_C(0x7fff) & a[104];
	r2[105] = UINT16_C(0x7fff) & a[105];
	r2[106] = UINT16_C(0x7fff) & a[106];
	r2[107] = UINT16_C(0x7fff) & a[107];
	r2[108] = UINT16_C(0x7fff) & a[108];
	r2[109] = UINT16_C(0x7fff) & a[109];
	r2[110] = UINT16_C(0x7fff) & a[110];
	r2[111] = UINT16_C(0x7fff) & a[111];
	r2[112] = UINT16_C(0x7fff) & a[112];
	r2[113] = UINT16_C(0x7fff) & a[113];
	r2[114] = UINT16_C(0x7fff) & a[114];
	r2[115] = UINT16_C(0x7fff) & a[115];
	r2[116] = UINT16_C(0x7fff) & a[116];
	r2[117] = UINT16_C(0x7fff) & a[117];
	r2[118] = UINT16_C(0x7fff) & a[118];
	r2[119] = UINT16_C(0x7fff) & a[119];
	r2[120] = UINT16_C(0x7fff) & a[120];
	r2[121] = UINT16_C(0x7fff) & a[121];
	r2[122] = UINT16_C(0x7fff) & a[122];
	r2[123] = UINT16_C(0x7fff) & a[123];
	r2[124] = UINT16_C(0x7fff) & a[124];
	r2[125] = UINT16_C(0x7fff) & a[125];
	r2[126] = UINT16_C(0x7fff) & a[126];
	r2[127] = UINT16_C(0x7fff) & a[127];
	r2[128] = UINT16_C(0x7fff) & a[128];
	r2[129] = UINT16_C(0x7fff) & a[129];
	r2[130] = UINT16_C(0x7fff) & a[130];
	r2[131] = UINT16_C(0x7fff) & a[131];
	r2[132] = UINT16_C(0x7fff) & a[132];
	r2[133] = UINT16_C(0x7fff) & a[133];
	r2[134] = UINT16_C(0x7fff) & a[134];
	r2[135] = UINT16_C(0x7fff) & a[135];
	r2[136] = UINT16_C(0x7fff) & a[136];
	r2[137] = UINT16_C(0x7fff) & a[137];
	r2[138] = UINT16_C(0x7fff) & a[138];
	r2[139] = UINT16_C(0x7fff) & a[139];
	r2[140] = UINT16_C(0x7fff) & a[140];
	r2[141] = UINT16_C(0x7fff) & a[141];
	r2[142] = UINT16_C(0x7fff) & a[142];
	r2[143] = UINT16_C(0x7fff) & a[143];
}


//--------------------------------------
// one step of Montgomery multiplication; r-bit digits
//     C += a * B
//     q =  C * inv mod 2^r 
//     C += q * M
//
// tmp[] and tm2[] are scratch registers
//
static inline
REALLY_FORCE_INLINE
//
void simd_montgomery2048_1step(uint16_t r  [144],
                               uint16_t r2 [144],  // scratch
                               uint32_t tmp[144],  // scratch
                               uint32_t tm2[144],  // scratch
                               uint16_t a,
                           const uint16_t b[144], uint16_t inv,
                           const uint16_t m[144])
{
	uint16_t cz =  a  * b[136],
	         q  = (cz * inv) & SIMD_MASK15BIT;

	simd_montg2048_mul1(tmp, b, a);
	simd_montg2048_mul1(tm2, m, q);
	simd_montg2048_add1_inpl(tmp, tm2);
	simd_montg2048_split32x16(r, r2, tmp);
}


//--------------------------------------
void simd_montgomery2048mul(uint16_t r[144],
                            uint16_t a,
                      const uint16_t b[144], uint16_t inv,
                      const uint16_t m[144])
{
	SIMD_ALIGN uint32_t tmp[144];
	SIMD_ALIGN uint32_t tm2[144];
	SIMD_ALIGN uint16_t  r2[144];

	simd_montgomery2048_1step(r, r2, tmp, tm2, a, b, inv, m);

	{
	unsigned int i;
	for (i=0; i<144; ++i)
		r[i] |= r2[i];
	}
}
#endif  //=====  /Montgomery multiplication  =================================


#if !defined(NO_SIMD_TWINPRIME)  //---- twin prime: u and (u + 2) primes -----
/*--------------------------------------
 * report if P|n or P|n+2 for P from prime[]
 *
 * rely on n*(1/P) <= limit(P) <=> P divides n
 * we check (n*(1/P) <= limit(P)) and ((2*n+1)*(1/P) <= limit(P)) by
 * checking min(n*(1/p), (2*n+1)*(1/P)) <= limit(P), saving a multiplication
 *
 * 'u' contains n * 1/prime[]
 * 'r' is filled with 16x16-bit result
 *     ffff  entry in prime[] divides n  OR  divides 2n+1
 *        0  prime[] is not a factor
 * 'r' and 'u' may point to the same memory; MUST NOT overlap with 'tmp'
 *
 * 'tmp' is centralized scratch, filled with (potentially)secret-dependent data
 *
 * TODO: check whether storing 2/p[] is more efficient than 2*(1/p)
 * unlikely: shift-by-1 only generates only one instruction; we are
 * probably better off than using another set of []-sized constants
 *--------------------------------------*/
static inline
REALLY_FORCE_INLINE
//
void simd_cmp16x16_twin(uint16_t   r[ 16 ], 
                        uint16_t tmp[ 16 ],
                  const uint16_t   u[ 16 ],
                  const uint16_t inv[ 16 ],
                const uint16_t limit[ 16 ])
{
	simd_shladd16x16(tmp, inv, u);          // (u * 1/p) + 2/p =~ u/p + 2/p

	simd_min16x16(r, tmp, u);               // min(u/p + 2/p, u/p)

	simd_lemask16x16_inpl(r, limit);
}


/*--------------------------------------
 * does this modn[] (->N) slice indicate a small-prime factor of N or 2N+1?
 *
 * tmp[] and tm2[] are different scratch registers
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_twinfactor64x16(uint16_t tmp[ 64 ],
                                 uint16_t tm2[ 64 ],
                          const uint16_t modn[ 64 ],
                           const uint16_t inv[ 64 ],
                         const uint16_t limit[ 64 ])
{
					// tmp[] = modn[] * 1/prime[]  mod 2^16

	simd_mul16x16(  tmp,          modn,          inv       );
	simd_mul16x16(&(tmp[ 16 ]), &(modn[ 16 ]), &(inv[ 16 ]));
	simd_mul16x16(&(tmp[ 32 ]), &(modn[ 32 ]), &(inv[ 32 ]));
	simd_mul16x16(&(tmp[ 48 ]), &(modn[ 48 ]), &(inv[ 48 ]));

	simd_cmp16x16_twin(  tmp,          tm2,         tmp,         inv,
	                   limit);
	simd_cmp16x16_twin(&(tmp[ 16 ]), &(tm2[ 16 ]),&(tmp[ 16 ]),&(inv[ 16 ]),
                         &(limit[ 16 ]));
	simd_cmp16x16_twin(&(tmp[ 32 ]), &(tm2[ 32 ]),&(tmp[ 32 ]),&(inv[ 32 ]),
                         &(limit[ 32 ]));
	simd_cmp16x16_twin(&(tmp[ 48 ]), &(tm2[ 48 ]),&(tmp[ 48 ]),&(inv[ 48 ]),
                         &(limit[ 48 ]));

	return simd_is_all0x64x16_inpl(tmp, tm2);
}


// included by any of the L/XL versions, therefore not ..._S conditional
/*--------------------------------------
 * is there a factor of x or x+2, after the first 64 primes have
 * been checked
 * this form is for small prime-tables
 *
 * tmp1[] and tmp2[] are different scratch registers
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_twinfactor_rest(uint16_t tmp[  64 ],
                                 uint16_t tm2[  64 ],
                          const uint16_t modn[ 576 ])
{
	return (simd_no_twinfactor64x16(tmp, tm2, &(modn[  64 ]),
	                      &(firstprimes_inverse_simd[  64 ]),
	                     &(firstprimes_mullimit_simd[  64 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 128 ]),
	                      &(firstprimes_inverse_simd[ 128 ]),
	                     &(firstprimes_mullimit_simd[ 128 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 192 ]),
	                      &(firstprimes_inverse_simd[ 192 ]),
	                     &(firstprimes_mullimit_simd[ 192 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 256 ]),
	                      &(firstprimes_inverse_simd[ 256 ]),
	                     &(firstprimes_mullimit_simd[ 256 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 320 ]),
	                      &(firstprimes_inverse_simd[ 320 ]),
	                     &(firstprimes_mullimit_simd[ 320 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 384 ]),
	                      &(firstprimes_inverse_simd[ 384 ]),
	                     &(firstprimes_mullimit_simd[ 384 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 448 ]),
	                      &(firstprimes_inverse_simd[ 448 ]),
	                     &(firstprimes_mullimit_simd[ 448 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 512 ]),
	                      &(firstprimes_inverse_simd[ 512 ]),
	                     &(firstprimes_mullimit_simd[ 512 ])));
}


#if !defined(NO_SIMDDIVIDE_M) && !defined(NO_SIMDDIVIDE_L)  //---------------
/*--------------------------------------
 * simd_no_twinfactor_rest() equivalent with mid-sized prime table
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_twinfactor_rest_m(uint16_t tmp[   64 ],
                                   uint16_t tm2[   64 ],
                            const uint16_t modn[ 1856 ])
{
	return (simd_no_twinfactor_rest(tmp, tm2, modn) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[  576 ]),
	                      &(firstprimes_inverse_simd[  576 ]),
	                     &(firstprimes_mullimit_simd[  576 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[  640 ]),
	                      &(firstprimes_inverse_simd[  640 ]),
	                     &(firstprimes_mullimit_simd[  640 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[  704 ]),
	                      &(firstprimes_inverse_simd[  704 ]),
	                     &(firstprimes_mullimit_simd[  704 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[  768 ]),
	                      &(firstprimes_inverse_simd[  768 ]),
	                     &(firstprimes_mullimit_simd[  768 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[  832 ]),
	                      &(firstprimes_inverse_simd[  832 ]),
	                     &(firstprimes_mullimit_simd[  832 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[  896 ]),
	                      &(firstprimes_inverse_simd[  896 ]),
	                     &(firstprimes_mullimit_simd[  896 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[  960 ]),
	                      &(firstprimes_inverse_simd[  960 ]),
	                     &(firstprimes_mullimit_simd[  960 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1024 ]),
	                      &(firstprimes_inverse_simd[ 1024 ]),
	                     &(firstprimes_mullimit_simd[ 1024 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1088 ]),
	                      &(firstprimes_inverse_simd[ 1088 ]),
	                     &(firstprimes_mullimit_simd[ 1088 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1152 ]),
	                      &(firstprimes_inverse_simd[ 1152 ]),
	                     &(firstprimes_mullimit_simd[ 1152 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1216 ]),
	                      &(firstprimes_inverse_simd[ 1216 ]),
	                     &(firstprimes_mullimit_simd[ 1216 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1280 ]),
	                      &(firstprimes_inverse_simd[ 1280 ]),
	                     &(firstprimes_mullimit_simd[ 1280 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1344 ]),
	                      &(firstprimes_inverse_simd[ 1344 ]),
	                     &(firstprimes_mullimit_simd[ 1344 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1408 ]),
	                      &(firstprimes_inverse_simd[ 1408 ]),
	                     &(firstprimes_mullimit_simd[ 1408 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1472 ]),
	                      &(firstprimes_inverse_simd[ 1472 ]),
	                     &(firstprimes_mullimit_simd[ 1472 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1536 ]),
	                      &(firstprimes_inverse_simd[ 1536 ]),
	                     &(firstprimes_mullimit_simd[ 1536 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1600 ]),
	                      &(firstprimes_inverse_simd[ 1600 ]),
	                     &(firstprimes_mullimit_simd[ 1600 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1664 ]),
	                      &(firstprimes_inverse_simd[ 1664 ]),
	                     &(firstprimes_mullimit_simd[ 1664 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1728 ]),
	                      &(firstprimes_inverse_simd[ 1728 ]),
	                     &(firstprimes_mullimit_simd[ 1728 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1792 ]),
	                      &(firstprimes_inverse_simd[ 1792 ]),
	                     &(firstprimes_mullimit_simd[ 1792 ])));
}
#endif    // !NO_SIMDDIVIDE_M && !NO_SIMDDIVIDE_L  --------------------------


#if !defined(NO_SIMDDIVIDE_L)  //--------------------------------------------
/*--------------------------------------
 * simd_no_twinfactor_rest() equivalent with large prime table
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_twinfactor_rest_l(uint16_t tmp[   64 ],
                                    uint16_t tm2[   64 ],
                             const uint16_t modn[ 3456 ])
{
	return (simd_no_twinfactor_rest_m(tmp, tm2, modn) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1856 ]),
	                      &(firstprimes_inverse_simd[ 1856 ]),
	                     &(firstprimes_mullimit_simd[ 1856 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1920 ]),
	                      &(firstprimes_inverse_simd[ 1920 ]),
	                     &(firstprimes_mullimit_simd[ 1920 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 1984 ]),
	                      &(firstprimes_inverse_simd[ 1984 ]),
	                     &(firstprimes_mullimit_simd[ 1984 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2048 ]),
	                      &(firstprimes_inverse_simd[ 2048 ]),
	                     &(firstprimes_mullimit_simd[ 2048 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2112 ]),
	                      &(firstprimes_inverse_simd[ 2112 ]),
	                     &(firstprimes_mullimit_simd[ 2112 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2176 ]),
	                      &(firstprimes_inverse_simd[ 2176 ]),
	                     &(firstprimes_mullimit_simd[ 2176 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2240 ]),
	                      &(firstprimes_inverse_simd[ 2240 ]),
	                     &(firstprimes_mullimit_simd[ 2240 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2304 ]),
	                      &(firstprimes_inverse_simd[ 2304 ]),
	                     &(firstprimes_mullimit_simd[ 2304 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2368 ]),
	                      &(firstprimes_inverse_simd[ 2368 ]),
	                     &(firstprimes_mullimit_simd[ 2368 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2432 ]),
	                      &(firstprimes_inverse_simd[ 2432 ]),
	                     &(firstprimes_mullimit_simd[ 2432 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2496 ]),
	                      &(firstprimes_inverse_simd[ 2496 ]),
	                     &(firstprimes_mullimit_simd[ 2496 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2560 ]),
	                      &(firstprimes_inverse_simd[ 2560 ]),
	                     &(firstprimes_mullimit_simd[ 2560 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2624 ]),
	                      &(firstprimes_inverse_simd[ 2624 ]),
	                     &(firstprimes_mullimit_simd[ 2624 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2688 ]),
	                      &(firstprimes_inverse_simd[ 2688 ]),
	                     &(firstprimes_mullimit_simd[ 2688 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2752 ]),
	                      &(firstprimes_inverse_simd[ 2752 ]),
	                     &(firstprimes_mullimit_simd[ 2752 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2816 ]),
	                      &(firstprimes_inverse_simd[ 2816 ]),
	                     &(firstprimes_mullimit_simd[ 2816 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2880 ]),
	                      &(firstprimes_inverse_simd[ 2880 ]),
	                     &(firstprimes_mullimit_simd[ 2880 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 2944 ]),
	                      &(firstprimes_inverse_simd[ 2944 ]),
	                     &(firstprimes_mullimit_simd[ 2944 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 3008 ]),
	                      &(firstprimes_inverse_simd[ 3008 ]),
	                     &(firstprimes_mullimit_simd[ 3008 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 3072 ]),
	                      &(firstprimes_inverse_simd[ 3072 ]),
	                     &(firstprimes_mullimit_simd[ 3072 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 3136 ]),
	                      &(firstprimes_inverse_simd[ 3136 ]),
	                     &(firstprimes_mullimit_simd[ 3136 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 3200 ]),
	                      &(firstprimes_inverse_simd[ 3200 ]),
	                     &(firstprimes_mullimit_simd[ 3200 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 3264 ]),
	                      &(firstprimes_inverse_simd[ 3264 ]),
	                     &(firstprimes_mullimit_simd[ 3264 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 3328 ]),
	                      &(firstprimes_inverse_simd[ 3328 ]),
	                     &(firstprimes_mullimit_simd[ 3328 ])) &&

	        simd_no_twinfactor64x16(tmp, tm2, &(modn[ 3392 ]),
	                      &(firstprimes_inverse_simd[ 3392 ]),
	                     &(firstprimes_mullimit_simd[ 3392 ])));
}
#endif    // !NO_SIMDDIVIDE_L  ----------------------------------------------
#endif    // !NO_SIMD_TWINPRIME  ---------------------------------------------


//----------------------------------------------------------------------------
static inline struct PP_Mod16bit *pp_mod16_init0(struct PP_Mod16bit *ps)
{
	if (ps)
		memset(ps, 0, sizeof(*ps));

	return ps;

}


/*--------------------------------------
 * reduce Q mod prime[] array to mod-prime range
 */
static inline void pp_mod16_mod_normalize(struct PP_Mod16bit *ps)
{
	if (ps) {
		unsigned int i;

		for (i=0; i<ARRAY_ELEMS(ps->modn); ++i)
			ps->modn[i] %= firstprimes[i];
	}
}


//--------------------------------------
static inline void simd_report1(const char *msg,
                            const uint16_t *p, unsigned int pn)
{
	unsigned int i;

	if (msg)
		printf("%s", msg);

	for (i=0; i<pn; ++i) {
		printf("%s", i ? "," : "");
		printf("%" PRIu16, p[i]);
	}
	printf("\n");
}


//--------------------------------------
static inline void simd_report_mod1(const char *msg,
                            const uint16_t *p, unsigned int pn)
{
	unsigned int i;

	if (msg)
		printf("MOD(%s)=", msg);

	for (i=0; i<pn; ++i) {
		printf("%s", i ? "," : "");
		printf("%" PRIu16, p[i] % firstprimes[i]);
	}
	printf("\n");
}


//--------------------------------------
static inline void simd_report_0ff(const char *msg,
                               const uint16_t *p, unsigned int pn)
{
	unsigned int i;

	if (msg)
		printf("%s", msg);

	for (i=0; i<pn; ++i) {
		printf("%s", i ? "," : "");
		printf("x%" PRIx16, p[i]);
	}
	printf("\n");
}


//--------------------------------------
static inline void pp_mod16report(const struct PP_Mod16bit *ps,
                                              unsigned int flags)
{
	if (ps) {
		unsigned int i, noncanon = 0;

		fflush(stdout);

				// reduction is not to canonical mod-prime,
				// since the is-multiple-of-P check tolerates
				// non-reduced inputs
		printf("## MOD16=");
		//
		for (i=0; i<ARRAY_ELEMS(ps->modn); ++i) {
			printf("%s", i ? "," : "");
			printf("%" PRIu16, ps->modn[i]);

			noncanon |= (ps->modn[i] >= firstprimes[i]);
		}
		printf("\n");

		if (noncanon) {
			printf("## MOD16.C=");
			//
			for (i=0; i<ARRAY_ELEMS(ps->modn); ++i) {
				printf("%s", i ? "," : "");
				printf("%" PRIu16, ps->modn[i] % firstprimes[i]);
			}
		}
		printf("\n");
	}

	(void) flags;
}


#if 0
//--------------------------------------
static int is_out_of_range(const struct PP_Mod16bit *ps)
{
	unsigned int i, fnd = 0;
	
	for (i=0; i<ARRAY_ELEMS(ps->modn); ++i) {
		if ((0x8000 & ps->modn[i]) && !fnd)
			fnd = 1+i;
	}

	return fnd;
}
#endif


/*--------------------------------------
 * check all possible values for each table entry
 *
 * returns >0 if all entries are correct
 *         0  if anything failed
 */
static int pp_simdtable_verifies(void)
{
	unsigned int i;

	BUILD_ASSERT(ARRAY_ELEMS(firstprimes_inverse_simd) ==
	             ARRAY_ELEMS(firstprimes_mullimit_simd));
	BUILD_ASSERT(ARRAY_ELEMS(firstprimes_inverse_simd) ==
	             ARRAY_ELEMS(firstprimes_mod2range_simd));
	//
	BUILD_ASSERT(ARRAY_ELEMS(firstprimes_inverse_simd) <=
	             ARRAY_ELEMS(firstprimes));

	for (i=0; i<ARRAY_ELEMS(firstprimes_inverse_simd); ++i) {
		uint32_t j;

				// inverse is correct
		if ((uint16_t) (firstprimes[i] * firstprimes_inverse_simd[i]) != 1) {
			uint32_t tmp = firstprimes[i];

			tmp *= firstprimes_inverse_simd[i];

			printf("I=%u INVERSE MISMATCH d*inv(d)=x%" PRIx16
			       "*x%" PRIx16 " = x%" PRIx16 "'%" PRIx16 "\n",
			       i, firstprimes[i],
			       firstprimes_inverse_simd[i],
			       (uint16_t) (tmp >> 16), (uint16_t) tmp);

			return 0;
		}

		for (j=0; j<UINT32_C(0x10000); ++j) {
			uint16_t mod1, mod2,        // actual Q, 2Q+1 mod-prime
			         mul1, mul2;
			uint16_t jj;
				// effective j: possibly after
				// x8000 reduction

			jj = (j >= firstprimes_mod2range_simd[i])
			      ? (j + firstprimes_mod2range_simd[i]) : j;

			mod1  =       jj  % firstprimes[i];
			mod2  = (jj+jj+1) % firstprimes[i];
					//
					// inv*Q, inv*(2Q+1), respectively
			mul1  =       jj  * firstprimes_inverse_simd[i];
			mul2  = (jj+jj+1) * firstprimes_inverse_simd[i];

// printf("xxx %u,%u mod=%u,%u mul=%x,%x lim=%x ", j, jj, mod1, mod2, mul1, mul2,
//      firstprimes_mullimit_simd[i]);

{
uint16_t maxlim = ~((uint16_t) 0);

maxlim /= firstprimes[i];

printf("max(lim)=%x inv=%x ", maxlim, firstprimes_inverse_simd[i]);
}

					// limit indicates divides(0) or
					// has >0 remainder(>0)
					//
			mul1  = (mul1 <= firstprimes_mullimit_simd[i])
			         ? 0 : mul1;
			mul2  = (mul2 <= firstprimes_mullimit_simd[i])
			         ? 0 : mul2;

printf("mul'=%x,%x div=%u,%u prime=%u\n", mul1, mul2, !mod1, !mod2,
       firstprimes[i]);

			if (!!mul1 != !!mod1) {
				printf("I=%u X=%u MISMATCH(Q) prime=%"
				       PRIu16 " div=%u,t=%u\n",
				       i, (unsigned int) j,
				       firstprimes[i],
				       !!mod1, (unsigned int) mul1);
				return 0;
			}

			if (!!mul2 != !!mod2) {
#if 0
				printf("I=%u X=%u MISMATCH(Q) prime=%"
				       PRIu16 " div=%u,t=%u\n",
				       i, (unsigned int) j,
				       firstprimes[i],
				       !!mod1, (unsigned int) mul1);
				return 0;
#endif
			}
		}
	}

printf("XXX TABLE OK\n");
fflush(stdout);
	return 1;
}


/*--------------------------------------
 * register 'current' into lsb[elems] at index 'wr' if possible
 * returns increased index
 */
static unsigned long report_current_lsb(uint64_t *lsb,    unsigned long elems,
                                        uint64_t current, unsigned long wr)
{
	if (lsb && (wr < elems))
		lsb[wr] = current;

	return ++wr;
}


// TODO: rest of comments
#if 0    //-----  Carmichael/3 search  ---------------------------------------
/*-----------------------------------------
 * safe-prime search: can modn[] indicate a prime simultaneously
 * with 2*modn[]+1 also prime?
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_nofactor48x16(const uint16_t modn[48], const uint16_t inv[48],
                           const uint16_t limit[48])
{
	uint16_t tmp[ 48 ];

				// compute n * 1/prime  mod 2^16 products

	simd_mul16x16(  tmp,        modn,        inv     );
	simd_mul16x16(&(tmp[16]), &(modn[16]), &(inv[16]));
	simd_mul16x16(&(tmp[32]), &(modn[32]), &(inv[32]));

	simd_spcmp16x16(  tmp,        tmp,        inv,        limit     );
	simd_spcmp16x16(&(tmp[16]), &(tmp[16]), &(inv[16]), &(limit[16]));
	simd_spcmp16x16(&(tmp[32]), &(tmp[32]), &(inv[32]), &(limit[32]));

	return simd_is_all0x48x16(tmp);
}


/*-----------------------------------------
 * v[] += adv; preserving mod-prime by subtracting m2r[] if applicable
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance48x16_m2r_inpl(uint16_t v[48], uint16_t adv,
                           const uint16_t m2r[48])
{
	simd_advance16x16_inpl(  v,          adv       );
	simd_m2range16x16_inpl(  v,          m2r       );

	simd_advance16x16_inpl(&(v[ 16 ]),   adv       );
	simd_m2range16x16_inpl(&(v[ 16 ]), &(m2r[ 16 ]));

	simd_advance16x16_inpl(&(v[ 32 ]),   adv       );
	simd_m2range16x16_inpl(&(v[ 32 ]), &(m2r[ 32 ]));
}
#endif   // 0


/*--------------------------------------
 * replicate 'src' to 'dst', or zero-initialize 'dst'
 */
static void scan_inherit_or0(struct PP_Mod16bit *dst,
                       const struct PP_Mod16bit *src)
{
	if (dst) {
		if (!src) {
			*dst = (struct PP_Mod16bit) PP_MOD16BIT_INIT0;

		} else if ((const struct PP_Mod16bit *) dst != src) {
			*dst = *src;
		}
	}
}


/*--------------------------------------
 * how much to advance from 6k+...mod6... to next possible prime?
 */
static unsigned int prime_mod6_advance(unsigned int mod6)
{
	switch (mod6) {
		case 3:
			return 2;         // 6k+3 -> 6k+5
		case 4:
		case 0:                   // fallthrough
			return 1;         // 6k -> 6k+1  or  6k+4 -> 6k+5
		case 2:
			return 3;         // 6k+2 -> 6k+5
		case 5:
		case 1:                   // fallthrough
		default:                  // fallthrough, in case your compiler
		                          // warns about no default
			return 0;         // 6k+1 or 6k+5: OK to start here
	}
}


//--------------------------------------
// advancing over each odd nr. of the form 6k+1 or 6k+5
//
static void scan_pkcs1_init(struct PP_Mod16bit *dst,
                      const struct PP_Mod16bit *src)
{
	if (dst) {
		scan_inherit_or0(dst, src);

		simd_advance_all(dst, prime_mod6_advance(dst->mod6));
	}
}


//--------------------------------------
#if !defined(NO_SIMD_TWINPRIME) && !defined(NO_SIMD_SAFEPRIME)  //------------
static void scan_twin_or_safeprime(struct PP_Mod16bit *dst,
                             const struct PP_Mod16bit *src) ;
#endif   //-----  !NO_SIMD_TWINPRIME && !NO_SIMD_SAFEPRIME  ------------------


/*--------------------------------------
 * initializes *dst as search-start state
 *
 * returns updated 'count' >0 if successful
 *         0  otherwise
 */
static
unsigned long init_search(const uint64_t *lsb, unsigned long count,
                      struct PP_Mod16bit *dst,
                const struct PP_Mod16bit *src,
                            unsigned int primetype)
{
	if (!src || !dst || !count)
		return 0;

	if (!lsb)
		count = 1;

	if ((const struct PP_Mod16bit *) dst != src)
		*dst = *src;                                    // work on *dst

	switch (primetype) {
#if !defined(NO_SIMD_TWINPRIME)
	case SIMD_PRIMETYPE_TWIN:
#endif
#if !defined(NO_SIMD_SAFEPRIME)
	case SIMD_PRIMETYPE_SAFE:            // [potential] fallthrough
#endif
#if !defined(NO_SIMD_TWINPRIME) && !defined(NO_SIMD_SAFEPRIME)  //------------
		scan_twin_or_safeprime(dst, dst);
		break;
#endif

	default:
#if !defined(NO_SIMD_PLAINPRIME)
		scan_pkcs1_init(dst, dst);
#endif
		break;
	}

	return count;
}


#if !defined(NO_SIMD_PLAINPRIME)  //------------------------------------------
/*--------------------------------------
 * is the current state divided by any of the first 576 primes?
 */
static inline
uint16_t simd_has_no_factor(uint16_t tmp[ 64 ],
                            uint16_t tm2[ 64 ],
            const struct PP_Mod16bit *ps)
{
	return (simd_nofactor_first(tmp, tm2, ps) &&
	        simd_nofactor_rest_s(tmp, tm2, ps)) ;
}


//---------------------------------------
static inline
uint16_t simd_has_nofactor_rest_w(uint16_t tmp[ 64 ],
                                  uint16_t tm2[ 64 ],
                  const struct PP_Mod16bit *ps)
{
	switch (ps ? (ps->mode & SIMD_SEARCHTABLE_MASK) : 0) {
#if !defined(NO_SIMDDIVIDE_L)
	case SIMD_SEARCHTABLE_L:
		return simd_nofactor_rest_l(tmp, tm2, ps);
#endif

#if !defined(NO_SIMDDIVIDE_M)
	case SIMD_SEARCHTABLE_M:
		return simd_nofactor_rest_m(tmp, tm2, ps);
#endif

#if !defined(NO_SIMDDIVIDE_S)
	case SIMD_SEARCHTABLE_S:
		return simd_nofactor_rest_s(tmp, tm2, ps);
#endif

	default:
		return 0;
	}
}


/*--------------------------------------
 * possibly write one more LSB64 to (lsb, count), depending on
 * 'ps' possibly being prime (S)
 *
 * returns updated 'wr', which may be original input or +1
 */
static inline
unsigned long simd_check_plain1(uint64_t *lsb, unsigned long count,
                           unsigned long wr,
                                uint16_t tmp[ 64 ],
                                uint16_t tm2[ 64 ],
                const struct PP_Mod16bit *ps)
{
	if (simd_has_no_factor(tmp, tm2, ps))
		wr = report_current_lsb(lsb, count, ps->lsb, wr);

	return wr;
}


#if !defined(NO_SIMDDIVIDE_M) && !defined(NO_SIMDDIVIDE_L)  //---------------
/*--------------------------------------
 * is the current state divided by any of the first 1856 primes?
 */
static inline
uint16_t simd_has_no_factor_m(uint16_t tmp[ 64 ],
                              uint16_t tm2[ 64 ],
              const struct PP_Mod16bit *ps)
{
	return (simd_nofactor_first(tmp, tm2, ps) &&
	        simd_nofactor_rest_m(tmp, tm2, ps)) ;
}


/*--------------------------------------
 * does the current 'ps' have a factor in the 'M' (mid-range) list?
 *
 * possibly write one more LSB64 to (lsb, count), if no factor
 *
 * returns updated 'wr', which may be original input or +1
 */
static inline
unsigned long simd_check_plain1_m(uint64_t *lsb, unsigned long count,
                             unsigned long wr,
                                  uint16_t tmp[ 64 ],
                                  uint16_t tm2[ 64 ],
                  const struct PP_Mod16bit *ps)
{
	if (simd_has_no_factor_m(tmp, tm2, ps))
		wr = report_current_lsb(lsb, count, ps->lsb, wr);

	return wr;
}
#endif    // !NO_SIMDDIVIDE_M && !NO_SIMDDIVIDE_L  --------------------------


#if !defined(NO_SIMDDIVIDE_L)  //--------------------------------------------
/*--------------------------------------
 * is the current state divided by any of the first 3456 primes?
 */
static inline
uint16_t simd_has_no_factor_l(uint16_t tmp[ 64 ],
                              uint16_t tm2[ 64 ],
              const struct PP_Mod16bit *ps)
{
	return (simd_nofactor_first(tmp, tm2, ps) &&
	        simd_nofactor_rest_l(tmp, tm2, ps)) ;
}


/*--------------------------------------
 * does the current 'ps' have a factor in the 'L' (large) list?
 *
 * possibly write one more LSB64 to (lsb, count), if no factor
 *
 * returns updated 'wr', which may be original input or +1
 */
static inline
unsigned long simd_check_plain1_l(uint64_t *lsb, unsigned long count,
                              unsigned long wr,
                                   uint16_t tmp[64],
                                   uint16_t tm2[64],
                   const struct PP_Mod16bit *ps)
{
	if (simd_has_no_factor_l(tmp, tm2, ps))
		wr = report_current_lsb(lsb, count, ps->lsb, wr);

	return wr;
}
#endif   // !NO_SIMDDIVIDE_L  -----------------------------------------------


#if 0
/*--------------------------------------
 * autoselect size from
 */
static inline
unsigned long simd_check_plain(uint64_t *lsb, unsigned long count,
                          unsigned long wr,
                               uint16_t tmp[ 64 ],
                               uint16_t tm2[ 64 ],
               const struct PP_Mod16bit *ps)
{
	if (simd_has_no_factor(tmp, tm2, ps))
		wr = report_current_lsb(lsb, count, ps->lsb, wr);
}
#endif


/*--------------------------------------
 * recurring theme: advance to first 6k+1 candidate, checking + possibly
 * registering initial state (which must have been 6k+5 if it has been
 * rechecked)
 *
 * possibly updates both 'ps' and 'adv'
 */
static inline
unsigned long plain_advance_to_6kp1(uint64_t *lsb, unsigned long count,
                          struct PP_Mod16bit *ps,
	                 struct SIMD_Advance *adv)
{
	static unsigned long rv = 0;

	if (ps && adv && (ps->mod6 == 5)) {         // advance 6k+5 -> 6(k+1)+1
		adv->wr = simd_check_plain1(lsb, count, adv->wr,
		                            adv->tmp, adv->tm2, ps);

		simd_advance_all(ps, 2);
		rv = adv->wr;
	}

	return rv;
}


#if defined(NO_SIMDDIVIDE_M) && defined(NO_SIMDDIVIDE_L)  //-----------------
//--------------------------------------
static
uint64_t plain_advance_s(uint64_t *lsb, unsigned long count,
               struct PP_Mod16bit *dst,
         const struct PP_Mod16bit *src)
{
	struct SIMD_Advance adv = SIMD_ADVANCE_INIT0;

	count = init_search(lsb, count, dst, src, 0);
	if (!count)
		return 0;

	adv.wr = plain_advance_to_6kp1(lsb, count, dst, &adv);

	while (adv.wr < count) {                // invariant: candidate is 6k+1
		adv.wr = simd_check_plain1(lsb, count, adv.wr,
		                           adv.tmp, adv.tm2, dst);

		if (adv.wr < count) {
			simd_advance_all(dst, 4);               // 6k+1 -> 6k+5

			adv.wr = simd_check_plain1(lsb, count, adv.wr,
			                           adv.tmp, adv.tm2, dst);
			if (adv.wr < count)
				simd_advance_all(dst, 2);   // 6k+5 -> 6(k+1)+1
		}
	}

	wipe_advance_struct(&adv);

	return dst->lsb;
}
#endif   // NO_SIMDDIVIDE_M && NO_SIMDDIVIDE_L  -----------------------------


#if !defined(NO_SIMDDIVIDE_M) && defined(NO_SIMDDIVIDE_L)  //----------------
//--------------------------------------
static
uint64_t plain_advance_m(uint64_t *lsb, unsigned long count,
               struct PP_Mod16bit *dst,
         const struct PP_Mod16bit *src)
{
	struct SIMD_Advance adv = SIMD_ADVANCE_INIT0;

	count = init_search(lsb, count, dst, src, 0);
	if (!count)
		return 0;

	adv.wr = plain_advance_to_6kp1(lsb, count, dst, &adv);

	while (adv.wr < count) {                // invariant: candidate is 6k+1
		adv.wr = simd_check_plain1_m(lsb, count, adv.wr,
		                             adv.tmp, adv.tm2, dst);
	
		if (adv.wr < count) {
			simd_advance_all(dst, 4);               // 6k+1 -> 6k+5

			adv.wr = simd_check_plain1_m(lsb, count, adv.wr,
		                                     adv.tmp, adv.tm2, dst);
			if (adv.wr < count)
				simd_advance_all(dst, 2);   // 6k+5 -> 6(k+1)+1
		}
	}

	wipe_advance_struct(&adv);

	return dst->lsb;
}
#endif   // !NO_SIMDDIVIDE_M && NO_SIMDDIVIDE_L  ----------------------------


#if !defined(NO_SIMDDIVIDE_L)  //--------------------------------------------
//--------------------------------------
static
uint64_t plain_advance_l(uint64_t *lsb, unsigned long count,
                struct PP_Mod16bit *dst,
          const struct PP_Mod16bit *src)
{
	struct SIMD_Advance adv = SIMD_ADVANCE_INIT0;

	count = init_search(lsb, count, dst, src, 0);
	if (!count)
		return 0;

	adv.wr = plain_advance_to_6kp1(lsb, count, dst, &adv);

	while (adv.wr < count) {                    // invariant: candidate is 6k+1
		adv.wr = simd_check_plain1_l(lsb, count, adv.wr,
		                              adv.tmp, adv.tm2, dst);
	
		if (adv.wr < count) {
			simd_advance_all(dst, 4);               // 6k+1 -> 6k+5

			adv.wr = simd_check_plain1_l(lsb, count, adv.wr,
		                                      adv.tmp, adv.tm2, dst);
			if (adv.wr < count)
				simd_advance_all(dst, 2);   // 6k+5 -> 6(k+1)+1
		}
	}

	wipe_advance_struct(&adv);

	return dst->lsb;
}
#endif   // !NO_SIMDDIVIDE_L  -----------------------------------------------


/*--------------------------------------
 * wrapper picking widest _advance() function
 */
static inline
uint64_t plain_advance(uint64_t *lsb, unsigned long count,
             struct PP_Mod16bit *dst,
       const struct PP_Mod16bit *src)
{
#if !defined(NO_SIMDDIVIDE_L)
	return plain_advance_l(lsb, count, dst, src);

#elif !defined(NO_SIMDDIVIDE_M)
	return plain_advance_m(lsb, count, dst, src);

#else
	return plain_advance_s(lsb, count, dst, src);
#endif
}
#endif  //-----  !NO_SIMD_PLAINPRIME  ----------------------------------------


#if !defined(NO_SIMD_TWINPRIME) && !defined(NO_SIMD_SAFEPRIME)  //------------
//--------------------------------------
// both safe prime and twin-prime search is restricted to p=6k+5
// advance to potential start position
//
static void scan_twin_or_safeprime(struct PP_Mod16bit *dst,
                             const struct PP_Mod16bit *src)
{
	if (dst) {
		uint16_t adv;

		scan_inherit_or0(dst, src);

		adv = (11 - dst->mod6) % 6;                  // advance to 6k+5

		simd_advance_all(dst, adv);
	}
}
#endif   //-----  !NO_SIMD_TWINPRIME && !NO_SIMD_SAFEPRIME  ------------------


#if !defined(NO_SIMD_TWINPRIME)  //---- twin prime: u and (u + 2) primes -----
// scanning primes of form 6k+5, for 'twin primes' where 6k+7 may also
// be a prime

#if defined(NO_SIMDDIVIDE_M) && defined(NO_SIMDDIVIDE_L)  //-----------------
// small prime-table
//
static
uint64_t twin_advance_s(uint64_t *lsb, unsigned long count,
              struct PP_Mod16bit *dst,
        const struct PP_Mod16bit *src)
{
	struct SIMD_Advance adv = SIMD_ADVANCE_INIT0;

	count = init_search(lsb, count, dst, src, SIMD_PRIMETYPE_TWIN);
	if (!count)
		return 0;

	for (adv.wr=0; adv.wr<count; ) {
		unsigned int skip = 0;

		// advance to first index which MAY be a prime based on
		// first N small primes, or adv is >= 2^16

		// count advances (6*number of elems tested)
		// up to 2^15 addition total
		//
		// this advance can still be added to mod-prime
		// array elems without generating carries

		while (!(skip >> 14) &&
		       !simd_no_twinfactor64x16(adv.tmp, adv.tm2, dst->modn,
		                                firstprimes_inverse_simd,
		                                firstprimes_mullimit_simd))
		{
			simd_advance64x16_m2r_inpl(dst->modn, 6,
			                      firstprimes_mod2range_simd);
			skip += 6;
		}

		state_advance_nr(dst, skip);

					// only first 64 have been advanced
					// now advance entries >64,
		if (skip)
			simd_advance_remainders_rest(dst, skip);

		if (skip >> 14)
			continue;

		if (simd_no_twinfactor_rest(adv.tmp, adv.tm2, dst->modn)) {
			adv.wr = report_current_lsb(lsb, count,
			                            dst->lsb, adv.wr);
		}

		simd_advance_all(dst, 6);
	}

	wipe_advance_struct(&adv);

	return dst->lsb;
}
#endif    // NO_SIMDDIVIDE_M && NO_SIMDDIVIDE_L  ----------------------------


#if !defined(NO_SIMDDIVIDE_M) && defined(NO_SIMDDIVIDE_L)  //----------------
/*--------------------------------------
 * mid-sized prime table, equivalent of twin_advance()
 */
static
uint64_t twin_advance_m(uint64_t *lsb, unsigned long count,
              struct PP_Mod16bit *dst,
        const struct PP_Mod16bit *src)
{
	struct SIMD_Advance adv = SIMD_ADVANCE_INIT0;

	count = init_search(lsb, count, dst, src, SIMD_PRIMETYPE_TWIN);
	if (!count)
		return 0;

	for (adv.wr=0; adv.wr<count; ) {
		unsigned int skip = 0;

		// advance to first index which MAY be a prime based on
		// first N small primes, or adv is >= 2^16

		// count advances (6*number of elems tested)
		// up to 2^15 addition total
		//
		// this advance can still be added to mod-prime
		// array elems without generating carries

		while (!(skip >> 14) &&
		       !simd_no_twinfactor64x16(adv.tmp, adv.tm2, dst->modn,
		                                firstprimes_inverse_simd,
		                                firstprimes_mullimit_simd))
		{
			simd_advance64x16_m2r_inpl(dst->modn, 6,
			                      firstprimes_mod2range_simd);
			skip += 6;
		}

		state_advance_nr(dst, skip);

					// only first 64 have been advanced
					// now advance entries >64,
		if (skip)
			simd_advance_remainders_rest_m(dst, skip);

		if (skip >> 14)
			continue;

		if (simd_no_twinfactor_rest_m(adv.tmp, adv.tm2, dst->modn)) {
			adv.wr = report_current_lsb(lsb, count,
			                            dst->lsb, adv.wr);
		}

		simd_advance_all(dst, 6);
	}

	wipe_advance_struct(&adv);

	return dst->lsb;
}
#endif    // !NO_SIMDDIVIDE_M && NO_SIMDDIVIDE_L  ---------------------------


#if !defined(NO_SIMDDIVIDE_L)  //--------------------------------------------
/*--------------------------------------
 * large prime table, equivalent of twin_advance()
 */
static
uint64_t twin_advance_l(uint64_t *lsb, unsigned long count,
               struct PP_Mod16bit *dst,
         const struct PP_Mod16bit *src)
{
	struct SIMD_Advance adv = SIMD_ADVANCE_INIT0;

	count = init_search(lsb, count, dst, src, SIMD_PRIMETYPE_TWIN);
	if (!count)
		return 0;

	for (adv.wr=0; adv.wr<count; ) {
		unsigned int skip = 0;

		// advance to first index which MAY be a prime based on
		// first N small primes, or adv is >= 2^16

		// count advances (6*number of elems tested)
		// up to 2^15 addition total
		//
		// this advance can still be added to mod-prime
		// array elems without generating carries

		while (!(skip >> 14) &&
		       !simd_no_twinfactor64x16(adv.tmp, adv.tm2, dst->modn,
		                                firstprimes_inverse_simd,
		                                firstprimes_mullimit_simd))
		{
			simd_advance64x16_m2r_inpl(dst->modn, 6,
			                      firstprimes_mod2range_simd);
			skip += 6;
		}

		state_advance_nr(dst, skip);

					// only first 64 have been advanced
					// now advance entries >64,
		if (skip)
			simd_advance_remainders_rest_l(dst, skip);

		if (skip >> 14)
			continue;

		if (simd_no_twinfactor_rest_l(adv.tmp, adv.tm2, dst->modn)) {
			adv.wr = report_current_lsb(lsb, count,
			                            dst->lsb, adv.wr);
		}

		simd_advance_all(dst, 6);
	}

	wipe_advance_struct(&adv);

	return dst->lsb;
}
#endif   // !NO_SIMDDIVIDE_L  -----------------------------------------------


/*--------------------------------------
 * wrapper picking widest _advance() function
 */
static inline
uint64_t twin_advance_w(uint64_t *lsb, unsigned long count,
              struct PP_Mod16bit *dst,
        const struct PP_Mod16bit *src)
{
#if !defined(NO_SIMDDIVIDE_L)
	return twin_advance_l(lsb, count, dst, src);

#elif !defined(NO_SIMDDIVIDE_M)
	return twin_advance_m(lsb, count, dst, src);

#else
	return twin_advance_s(lsb, count, dst, src);
#endif
}
#endif   //-----  !NO_SIMD_TWINPRIME)  ---------------------------------------


#if !defined(NO_SIMD_SAFEPRIME)  //-------------------------------------------

#if !defined(NO_SIMDDIVIDE_S)  //---------------------------------------------
/*--------------------------------------
 * list possible ls64(Q) of values where both Q and 2Q+1 MAY be primes
 * (have no small factors in our list of small primes)
 * [main entry point]
 *
 * advances from 'src' to 'dst'
 *
 * if non-NULL, fill lsb[] with 'count' next lsb64(Q) values,
 * in increasing order.
 * NOTE: WE DO NOT TRACK WRAPAROUND AT 2^64 BOUNDARIES.
 *
 * NULL 'lsb' returns the next possible lsb64(Q), and advances 'dst'
 * to next possible candidate.
 *
 * returns the last lsb64(Q) enumerated.
 *
 * 'src' and 'dst' may be the same struct
 */
static
uint64_t sfsieve_advance_s(uint64_t *lsb, unsigned long count,
                 struct PP_Mod16bit *dst,
           const struct PP_Mod16bit *src)
{
	struct SIMD_Advance adv = SIMD_ADVANCE_INIT0;

	count = init_search(lsb, count, dst, src, SIMD_PRIMETYPE_SAFE);
	if (!count)
		return 0;

	scan_twin_or_safeprime(dst, dst);

	while (adv.wr < count) {                // invariant: candidate is 6k+5
		unsigned int adv1 = 0;

		// advance to first index which MAY be a prime based on
		// first N small primes, or adv is >= 2^14

			// the first 64 primes, on the average, rule out
			// approx. 92.7% of all candidates (safe primes)

			// count advances (6*number of elems tested)
			// up to 2^14 addition total
			//
			// this advance can still be added to mod-prime
			// array elems without generating carries

		while (!(adv1 >> 14) &&
		       !simd_no_spfactor64x16(adv.tmp, adv.tm2, dst->modn,
		                           firstprimes_inverse_simd,
		                           firstprimes_mullimit_simd))
		{
// ...first...
			simd_advance64x16_m2r_inpl(dst->modn, 6,
			                  firstprimes_mod2range_simd);
			adv1 += 6;
		}

		state_advance_nr(dst, adv1);

		if (adv1)
			simd_advance_remainders_rest(dst, adv1);

		if (adv1 >> 14)
			continue;

		// advanced first [64] entries; these indicate
		// no small factor

// TODO: sync simd_no_spfactor_rest1() interface; remove ->modn indirection

		if (simd_no_spfactor_rest1(adv.tmp, adv.tm2, dst->modn)) {
			adv.wr = report_current_lsb(lsb, count, dst->lsb,
			                            adv.wr);
		}

		simd_advance_all(dst, 6);
	}

	wipe_advance_struct(&adv);

	return dst->lsb;
}
#endif  // !NO_SIMDDIVIDE_S //-----------------------------------------------


#if !defined(NO_SIMDDIVIDE_M) //---------------------------------------------
/*--------------------------------------
 * mid-sized equivalent of sfsieve_advance_s
 */
static
uint64_t sfsieve_advance_m(uint64_t *lsb, unsigned long count,
                 struct PP_Mod16bit *dst,
           const struct PP_Mod16bit *src)
{
	struct SIMD_Advance adv = SIMD_ADVANCE_INIT0;

	count = init_search(lsb, count, dst, src, SIMD_PRIMETYPE_SAFE);
	if (!count)
		return 0;

	scan_twin_or_safeprime(dst, dst);

	while (adv.wr < count) {                // invariant: candidate is 6k+5
		unsigned int adv1 = 0;

		while (!(adv1 >> 14) &&
		       !simd_no_spfactor64x16(adv.tmp, adv.tm2, dst->modn,
		                           firstprimes_inverse_simd,
		                           firstprimes_mullimit_simd))
		{
			simd_advance64x16_m2r_inpl(dst->modn, 6,
			                  firstprimes_mod2range_simd);
			adv1 += 6;
		}

		state_advance_nr(dst, adv1);

		if (adv1)
			simd_advance_remainders_rest_m(dst, adv1);

		if (adv1 >> 14)
			continue;

		if (simd_no_spfactor_rest_m(adv.tmp, adv.tm2, dst->modn)) {
			adv.wr = report_current_lsb(lsb, count, dst->lsb,
			                            adv.wr);
		}

		simd_advance_all_m(dst, 6);
	}

	wipe_advance_struct(&adv);

	return dst->lsb;
}
#endif  // !NO_SIMDDIVIDE_M  //----------------------------------------------


#if !defined(NO_SIMDDIVIDE_L)  //--------------------------------------------
/*--------------------------------------
 * large-table equivalent of sfsieve_advance_s
 */
static
uint64_t sfsieve_advance_l(uint64_t *lsb, unsigned long count,
                  struct PP_Mod16bit *dst,
            const struct PP_Mod16bit *src)
{
	struct SIMD_Advance adv = SIMD_ADVANCE_INIT0;

	count = init_search(lsb, count, dst, src, SIMD_PRIMETYPE_SAFE);
	if (!count)
		return 0;

	scan_twin_or_safeprime(dst, dst);

	while (adv.wr < count) {                // invariant: candidate is 6k+5
		unsigned int adv1 = 0;

		while (!(adv1 >> 14) &&
		       !simd_no_spfactor64x16(adv.tmp, adv.tm2, dst->modn,
		                           firstprimes_inverse_simd,
		                           firstprimes_mullimit_simd))
		{
			simd_advance64x16_m2r_inpl(dst->modn, 6,
			                  firstprimes_mod2range_simd);
			adv1 += 6;
		}

		state_advance_nr(dst, adv1);

		if (adv1)
			simd_advance_remainders_rest_l(dst, adv1);

		if (adv1 >> 14)
			continue;

		if (simd_no_spfactor_rest_l(adv.tmp, adv.tm2, dst->modn)) {
			adv.wr = report_current_lsb(lsb, count, dst->lsb,
			                            adv.wr);
		}

		simd_advance_all_l(dst, 6);
	}

	wipe_advance_struct(&adv);

	return dst->lsb;
}
#endif  // !NO_SIMDDIVIDE_L  //----------------------------------------------


//--------------------------------------
uint64_t sfsieve_advance(uint64_t *lsb, unsigned long count,
               struct PP_Mod16bit *dst,
         const struct PP_Mod16bit *src)
{
	switch (src ? (src->mode & SIMD_SEARCHTABLE_MASK) : 0) {
#if !defined(NO_SIMDDIVIDE_L)
	case SIMD_SEARCHTABLE_L:
		return sfsieve_advance_l(lsb, count, dst, src);
#endif

#if !defined(NO_SIMDDIVIDE_M)
	case SIMD_SEARCHTABLE_M:
		return sfsieve_advance_m(lsb, count, dst, src);
#endif

#if !defined(NO_SIMDDIVIDE_S)
	case SIMD_SEARCHTABLE_S:
		return sfsieve_advance_s(lsb, count, dst, src);
#endif

	default:
		return 0;
	}
}
#endif   //-----  !NO_SIMD_SAFEPRIME)  ---------------------------------------


#if 1

#define USE_OPENSSL

/*--------------------------------------
 * returns number of mode-specifying bits at start of 'base'
 * sets ps->mode if recognized
 *
 * pass through any unrecognized setting, let subsequent checks detect them
 */
static size_t callmode(struct PP_Mod16bit *ps, const char *base)
{
	size_t skip = 0;

	if (base) {
		int scolon_2nd = (base[0] != '\0') && (base[1] == ':');
		unsigned int mode = PP_MOD16_INVD_MODE;

		switch (base[0]) {
// TODO: constants
		case 't':
		case 'T':
			mode = scolon_2nd
			       ? SIMD_PRIMETYPE_TWIN
			       : PP_MOD16_INVD_MODE;
			skip = 2;
			break;
		case 's':
		case 'S':
			mode = scolon_2nd
			       ? SIMD_PRIMETYPE_SAFE
			       : PP_MOD16_INVD_MODE;
			skip = 2;
			break;
#if 0
		case 'f':
		case 'F':
			mode = scolon_2nd
			       ? SIMD_PRIMETYPE_FIPS186
			       : PP_MOD16_INVD_MODE;
			skip = 2;
			break;
#endif

// TODO: non-default prefix for plain-prime search?
		default:
			mode = SIMD_PRIMETYPE_PLAIN;
			break;
		}

		if (ps)
			ps->mode = mode;
	}

	return skip;
}


/*--------------------------------------
 * calculate (nr) mod mods[] to res[]; (nr) imported from hex string
 * other than avoiding division-by-zero, no sanity checking on modulus list
 *
 * opportunistically sets (nu64, u64s) to big-endian u64[] of (nr,nbytes),
 * if result fits. otherwise, non-NULL array is set to all-0.
 *
 * processes chunks in 2^48 units, so all operations require carry-less
 * u64 primitives only
 *
 * there is an upper limit on input size
 * efficiency is irrelevant
 */
static int mod16bits(uint16_t *res,
                     uint64_t *nu64, unsigned int u64s,
          const unsigned char *nr,     size_t nbytes,
               const uint16_t *mods,   size_t mcount)
{
	uint64_t n[ PP_MAX_NR_BITS / 64 ] = { 0, };
	unsigned int m, nrd = 0;

	if (!res || !nr || !nbytes || (nbytes > ((PP_MAX_NR_BITS +7) /8)) ||
	    !mods || !mcount || (mcount > SIMDPRIME_COUNT))
		return -1;

	if (nu64 && u64s)
		memset(nu64, 0, u64s*sizeof(uint64_t));

	nrd = bin2u64(n, ARRAY_ELEMS(n), nr, nbytes);
	if (nrd > ARRAY_ELEMS(n))
		return -2;

	if (nu64 && (nrd <= u64s)) {
		for (m=0; m<nrd; ++m)
			nu64[m] = n[m];
	}

	for (m=0; m<mcount; ++m) {
		if (!mods[m])
			return -3;

		res[m] = modn16(n, nrd, mods[m]);
	}

	return nrd;
}


//--------------------------------------
// 'base' read into ps->modn[]
// 'addl' read into ps->incr[] if non-NULL
// sets ps->mode to indicate search type
//
static int mod16read(struct PP_Mod16bit *ps, const char *base,
                                             const char *addl)
{
	unsigned char raw[ PP_MAX_NR_BITS /8 ] = { 0, };
	size_t rd, hexb;

	if (!ps || !base)
		return -1;

	*ps = (struct PP_Mod16bit) PP_MOD16BIT_INIT0;

	rd = callmode(ps, base);
	if (ps->mode == PP_MOD16_INVD_MODE)
		return cu_reportrc("unsupported search/mode", -1);

	base += rd + hexprefix_advance(base +rd);
	hexb =  strlen(base);

	if (hexb > sizeof(ps->qstr))
		return cu_reportrc("hex/value out of range", -1);

	rd = cu_hex2bin(raw, ARRAY_ELEMS(raw), base, hexb);
	if (rd > ARRAY_ELEMS(raw))
		return cu_reportrc("hex/value bitcount range", -1);

	memcpy(ps->qstr, base, hexb);

	{
	uint64_t ndigits[ (PP_MAX_NR_BITS +63) /64 ];          // scratch u64[]
	int nrd;

	nrd = mod16bits(ps->modn, ndigits, ARRAY_ELEMS(ndigits), raw, rd,
	                firstprimes, SIMDPRIME_COUNT);
	if (nrd < 0)
		return -1;

	ps->mod6 = modn16(ndigits, nrd, 6);
	ps->lsb  = ndigits[ nrd -1 ];

	if (addl) {
		addl += hexprefix_advance(addl);

		rd = cu_hex2bin(raw, ARRAY_ELEMS(raw), addl, 0);
		if (rd > ARRAY_ELEMS(raw))
			return -1;

		nrd = mod16bits(ps->incr, ndigits, ARRAY_ELEMS(ndigits),
		                raw, rd, firstprimes, SIMDPRIME_COUNT);
		if (nrd < 1)
			return -1;
		ps->iused = 666;       // TODO: real nr
// TODO: proper list of failure scenarios -> reportrc() markers

		ps->mod6incr = modn16(ndigits, nrd, 6);

		ps->lsbi =  ndigits[ nrd -1 ];
		ps->mode |= SIMD_FIELDSET_INCR;
	}
	}

	return 1;
}


//--------------------------------------
static void set_default_table_size(struct PP_Mod16bit *ps)
{
	if (ps) {
		ps->mode &= ~SIMD_SEARCHTABLE_MASK;

#if !defined(NO_SIMDDIVIDE_L)
		ps->mode |= SIMD_SEARCHTABLE_L;
#elif !defined(NO_SIMDDIVIDE_M)
		ps->mode |= SIMD_SEARCHTABLE_M;
#else
		ps->mode |= SIMD_SEARCHTABLE_S;
#endif
	}
}



/*--------------------------------------
 * opportunistic: possibly select nondefault prime count
 * returns >0  if ps->mode has been modified
 *         0   nothing to update
 *         <0  unsupported inputs
 *
 * MAY update 'ps' even if parsing fails
 */
static int override_table_size(struct PP_Mod16bit *ps, const char *primes)
{
	if (ps && primes) {
		uint64_t v = 0;

		ps->mode &= ~SIMD_SEARCHTABLE_MASK;

		v = cu_readuint(primes, 0);

		switch (v) {
#if !defined(NO_SIMDDIVIDE_L)
		case 3456:
			ps->mode |= SIMD_SEARCHTABLE_L;
			break;
#endif

#if !defined(NO_SIMDDIVIDE_M)
		case 1856:
			ps->mode |= SIMD_SEARCHTABLE_M;
			break;
#endif

#if !defined(NO_SIMDDIVIDE_S)
		case 576:
			ps->mode |= SIMD_SEARCHTABLE_S;
			break;
#endif

		default:
			return -1;
		}

		return 1;
	}

	return 0;
}


//--------------------------------------
// keep in sync with SIMDPRIME_COUNT
//
static unsigned int report_table_primes(const struct PP_Mod16bit *ps)
{
	switch (ps ? (ps->mode & SIMD_SEARCHTABLE_MASK) : 0) {
		case SIMD_SEARCHTABLE_L:
			return 3456;
		case SIMD_SEARCHTABLE_M:
			return 1856;
		case SIMD_SEARCHTABLE_S:
			return 576;
		default:
			return 0;
	}
}


//--------------------------------------
static const char *report_prime_type(const struct PP_Mod16bit *ps)
{
	switch (ps ? (ps->mode & SIMD_PRIMETYPE_MASK) : 0) {
		case SIMD_PRIMETYPE_TWIN:
			return "twin";
		case SIMD_PRIMETYPE_SAFE:
			return "safe";
		case SIMD_PRIMETYPE_FIPS186:
			return "FIPS-186";
		case SIMD_PRIMETYPE_PLAIN:
			return "plain(PKCS1)";
		default:
			return "UNKNOWN";
	}
}


//--------------------------------------
// allow measuring millions of iterations, w/o blowing up stack
//
#define  SF_TEST_UNITS  ((unsigned long) 10000000)
//
static uint64_t possible[ SF_TEST_UNITS ];


/* we hash together a concatenated be64[] version of all LSB 64 bits we
 * find.
 *
 * assume u64's are stored without padding; we compensate for host endianness.
 */


#if defined(USE_OPENSSL) || defined(USE_CLIC)
/*--------------------------------------
 * normalize u64[] to BE64[] in-place
 */
static void buffer2be64(uint64_t *p, unsigned int pn)
{
	if (p && pn) {
		unsigned int i;

		for (i=0; i<pn; ++i) {
			uint64_t v = p[i];
			MSBF8_WRITE(&( p[i] ), v);
		}
	}
}
#endif  // USE_OPENSSL || USE_CLIC


#if defined(USE_OPENSSL)  /*------------------------------------------------*/
#include <openssl/evp.h>

#if 0
int EVP_Q_digest(OSSL_LIB_CTX *libctx, const char *name, const char *propq,
                 const void *data, size_t datalen,
                 unsigned char *md, size_t *mdlen);
#endif


//--------------------------------------
// serialize be64 increments in-place; hash entire stream
// MAY change p[]
//
static void hash_buffer(uint64_t *p, unsigned int pn)
{
	if (p && pn) {
		unsigned char h[ EVP_MAX_MD_SIZE ];
		size_t hb;

		buffer2be64(p, pn);       // in-place BE64() || with no padding

		EVP_Q_digest(NULL, "SHA512", NULL, p, pn*8, h, &hb);
	}
}
#endif    /*-----  USE_OPENSSL  --------------------------------------------*/


#if defined(USE_CLIC)
//--------------------------------------
// serialize be64 increments in-place; hash entire stream
// MAY change p[]
//
static void hash_buffer(uint64_t *p, unsigned int pn)
{
	if (p && pn) {
		unsigned char h[ CLiC_DIGEST_MAX_SIZE ];
		long rc;

		buffer2be64(p, pn);

		rc = CLiC_sha512(NULL, (const unsigned char *) p, pn*8, h);
		if (rc > 0)
			cu_hexprint("## HASH(STATE)=", h, rc);
	}
}
#endif  /* defined(USE_CLIC) */


//--------------------------------------
int main(int argc, const char **argv)
{
	struct PP_Mod16bit ps = PP_MOD16BIT_INIT0;
	unsigned long i, pcount;
	uint64_t rc;

	--argc;
	++argv;

	if (argc < 1)
		return cu_reportrc("missing starting value", -1);

				// start prime always present
				// argv[1] is increment, if present
				//
	if (mod16read(&ps, argv[0], (argc > 1) ? argv[1] : NULL) <0)
		return -1;
	printf("## P0=%s\n", argv[0]);

#if 0
// TODO: centralized has-increment() etc. check
	if ((argc > 1) && (SIMD_PRIMETYPE_FIPS186 & ps.mode))
		printf("## INCR=%s\n", argv[1]);
#endif

	pcount = SF_TEST_UNITS;

	set_default_table_size(&ps);
	if (override_table_size(&ps, getenv("PRIMES")) <0)
		return cu_reportrc("invalid prime-count specified", -1);

	printf("## PRIMES=%u\n", report_table_primes(&ps));
	printf("## TYPE=%s\n", report_prime_type(&ps));

	{
	struct timespec start, end;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

	if (getenv("TWIN") || (SIMD_PRIMETYPE_TWIN & ps.mode)) {
		rc = twin_advance_w(possible, pcount, &ps, &ps);

	} else if (getenv("PLAIN") || (SIMD_PRIMETYPE_PLAIN & ps.mode)) {
pcount = 1000000;
		rc = plain_advance(possible, pcount, &ps, &ps);

	} else {
		rc = sfsieve_advance(possible, pcount, &ps, &ps);
	}

	printf("adv(%lu)=x%016" PRIx64 "\n", pcount, rc);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
	printf("DIFF=%.1fms\n", cu_msdelta2(&start, &end));
	}

	if (getenv("LOG_ALL") || (pcount < 100)) {
		for (i=0; i<pcount; ++i)
			printf("adv[%lu]=x%016" PRIx64 "\n", i, possible[i]);
	} else {
		for (i=0; i<100; ++i)
			printf("adv[%lu]=x%016" PRIx64 "\n", i, possible[i]);
		printf("...\n");

		for (i=0; i<40; ++i) {
			printf("adv[-%lu/%lu]=x%016" PRIx64 "\n", 40-i,
			       pcount-40+i, possible[ pcount-40+i ]);
		}
	}

			// note: ignores lead, nr. of candidates checked
			// before first possible entry is found
			//
			// essentially irrelevant for the 10K+ lists
			// we use as benchmarks
	if (possible[0] < possible[ pcount-1 ]) {
		rc = possible[ pcount-1 ] - possible[0];

						// min(diff) == pcount*6
		printf("## SKIPPED.RATIO=1:%.2f\n",
		       ((float) rc) / (pcount *6));

		printf("## SKIPPED.PPM=%" PRIu64 "\n",
		       UINT64_C(1000000) - (6000000 * pcount / rc));
	}

	hash_buffer(possible, pcount);

	printf("\n");

	return 0;
}
#endif   // 1

