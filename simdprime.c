/*------------------------------------------------------------------------
 * primality search: SIMD-friendly representation of trial divisions
 *------------------------------------------------------------------------
 * SPDX: MIT
 *------------------------------------------------------------------------
 * Author: Tamas Visegrady  <tamas@visegrady.ch>
 *----------------------------------------------------------------------*/

/* Sample implementation of the SIMD-ification techniques
 * described in 'Portable, fully autovectorizable trial division
 * implementations for efficient primality testing'.
 *
 * These routines are SIMD-friendly implementations of trial divisions,
 * filtering out prime candidates if they have small (prime) factor(s).
 */

/* three prime-table sizes are predefined, with increasing
 * number of small primes:
 *   'S'  suitable for envs with fast modular exponentiation, such as
 *        hardware security modules, or anything else with hw engines
 *   'M'  comparable to typical trial-division code, such as OpenSSL's
 *        2048-prime tables
 *   'L'  recommended for platforms with slow modular exponentiation,
 *        or for higher prime bitcounts.
 *
 * can define NO_SIMDDIVIDE_S ..._M ..._L, respectively, to prohibit
 * specific sizes. Expect only S and L in practice: those with fast
 * engines would prefer S; with SIMD-acceleration, the incremental time
 * between M and L is expected to be ~negligible. (The only practical
 * reason for M is to achieve a small-factor-rejection ratio comparable
 * to typical sw implementations which sieve with ~2k primes.)
 *
 * conditional-disable prime-search types:
 *    NO_SIMD_SAFEPRIME    -- p and 2p+1 are both primes
 *    NO_SIMD_TWINPRIME    -- p and p+2 are both primes
 *    NO_SIMD_FIPS186PRIME -- incremental search, FIPS 186-x sequence;
 *                         -- uses initial candidate plus (2*)P*Q increment
 *                         -- with P, Q auxiliary primes
 *    NO_SIMD_PLAINPRIME   -- incremental search checking 6k+1, 6k+5, 6k+7...
 *
 * runtime control: set
 *    PRIMES=...            -- nr. of primes to trial-divide against; must
 *                          -- be one of the S/M/L sizes compatible with
 *                          -- conditional-compiled restrictions
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


/*----------------------------------------------------------------------------
 * References
 *
 * Granlund, Montgomery:
 *     Division by invariant integers using multiplication
 *       1994
 *       SIGPLAN Not. 1994(29)
 *     section 9, "Exact division by constants"
 *
 * Lemire, Keiser, Kurz:
 *     Faster remainder by direct computation; applications to compilers
 *     and software libraries
 *       2019-11
 *       arxiv.org/abs/1902.01961
 *     section 3.2, "Fast divisibility check with a single multiplication"
 *     (referencing Granlund, Montgomery)
 * Note that the direct-remainder method is not directly applicable to
 * automatic SIMD-vectorization since it relies on different widths of
 * multiplication.
 *--------------------------------------------------------------------------*/

#if 0
// safe primes:
//   distribution if smallest factors in array [5..], grouped into
//   units of 16; approximate probability of dividing an odd number in
//   parts-per-million. In other words, cumulative distribution of SIMD
//   divisors falling in group N of 16x16 bits; 1: [5 .. 61], 2: [67 ..
//   139], 3: [149 .. 229]...:
//     [  5.. 61]: p=.862418
//     [ 67..139]: p=.901666
//     [149..229]: p=.917172
//     [233..317]: p=.925968
//     ...
//
// note: 3 is missing from list of small primes; we only try feasible
// 'm' values for 6k+m etc.; divide-by-3 is never needed.
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
// changes above approximately 1168 are essentially relevant. for
// non-SIMD environments where many short multiplications' cost is not
// negligible, consider using fewer small primes.
#endif


/*---  nothing user-serviceable below  -------------------------------------*/
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "common-base.h"

#define USE_HEX2BIN
#define USE_HEXDUMP -1
#define USE_READINT
#define USE_TIMEDIFF
#define USE_ERR_ANNOTATE
#include "common-util.h"

#include "common-prime-tools.h"


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
// 2x256 does not really get impacted by forcing 512-suitable min. alignment
#define  SIMD_ALIGN  __attribute__((aligned(64)))

// 64-bit arch only, 'large enough' for our illustration
// note: real loop is size-oblivious; difference only relevant
// during bignum->modP table conversion
//
#define  SAFEPRIME_MAX_BITS    ((unsigned int) 4096)
#define  SAFEPRIME_MAX_DIGITS  ((SAFEPRIME_MAX_BITS +64 -1) / 64)


// number of primes to consider; all primes fit uint16_t
// keep a multiple of 64, to accommodate up to 512-bit SIMD (2 x256=2 x16 x16)
//
#if !defined(NO_SIMDDIVIDE_L)
#define  SIMDPRIME_COUNT  ((unsigned int) 3456)
#elif !defined(NO_SIMDDIVIDE_M)
#define  SIMDPRIME_COUNT  ((unsigned int) 1856)
#else
#define  SIMDPRIME_COUNT  ((unsigned int) 576)
#endif
// see also report_table_prime_count() and override_table_size(),
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
	unsigned int mod6incr; // increment % 6; used only for cases
	                       // with bignumber increment (FIPS 186-5)

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
void simd_mul16x16(uint16_t res[static 16], const uint16_t v[static 16],
           const uint16_t coeff[static 16])
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
 * v[] += (vector of) adv x 16
 */
static inline
REALLY_FORCE_INLINE
//
void simd_advance16x16_inpl(uint16_t v[static 16], uint16_t add)
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


#if 0
// FIPS 186-x only
/*--------------------------------------
 * v[] += adv[]
 */
static inline
REALLY_FORCE_INLINE
//
void simd_advance16x16_inpl_v(uint16_t v[static 16],
                      const uint16_t add[static 16])
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
#endif   // 0: FIPS 186-x only


/*--------------------------------------
 * r[] = a[] | b[]
 */
static inline
REALLY_FORCE_INLINE
//
void simd_or16x16(uint16_t r[static 16], const uint16_t a[static 16],
                                         const uint16_t b[static 16])
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
void simd_m2range16x16(uint16_t r[static 16], const uint16_t v[static 16],
               const uint16_t m2r[static 16])
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
 * for src==dest; we do not replicate the simd_m2range16x16() fn. body
 * here
 */
static inline
REALLY_FORCE_INLINE
//
void simd_m2range16x16_inpl(uint16_t v[static 16],
                    const uint16_t m2r[static 16])
{
	simd_m2range16x16(v, v, m2r);
}


/*--------------------------------------
 * r[] = 2 * v[] + add[]
 */
static inline
REALLY_FORCE_INLINE
//
void simd_shladd16x16(uint16_t r[static 16], const uint16_t v[static 16],
                                           const uint16_t add[static 16])
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
void simd_min16x16(uint16_t r[static 16], const uint16_t a[static 16],
                                          const uint16_t b[static 16])
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
void simd_lemask16x16_inpl(uint16_t r[static 16],
                 const uint16_t limit[static 16])
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
void simd_spcmp16x16(uint16_t r[static 16], const uint16_t v[static 16],
             const uint16_t inv[static 16],
           const uint16_t limit[static 16])
{
	uint16_t tmp[16];
// TODO: use external tmp[] which may be centrally wiped

	simd_shladd16x16(tmp, v, inv);                   // 2*v+inv[]  mod 2^16

	simd_min16x16(r, v, tmp);

	simd_lemask16x16_inpl(r, limit);           // divides -> ffff; not -> 0
}


#if 1    //-----  delimiter: generated SIMD columns  -------------------------
#if 1   // delimiter: generated code
#if 1   // 256 bit: these are just pass-through functions

SIMD_ALIGN
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
unsigned int simd_is_all0(const uint16_t v[static 16])
{
	return !memcmp(simd_allzero256bits, v, sizeof(simd_allzero256bits));
}


// note: zero-flag (ZF) checking for 256-bit wide SIMD units
// above, we rely on the all-0 16x16-bit representation 
//
// there are equivalent intrinsics, or even the straightforward AND-all
// of 16x16 bits may turn into a short construct. TBD
//
#if 0
#if (SYS__X86_BITS >= 64)
	return _mm256_testz_si256((const __m256i) *v, (const __m256i) *v);
		// 
		// compare-all-0 compiles into vpxor, vptest "close enough"

#elif (SYS__AARCH >= 64)
	return (vmaxvq_u16(v) == 0);
		// MAX(..16x16 bits..); does not require use of all-0
		// aux. register
		//
		// compiles into:
		//     umaxv hd,vn.8h   "Unsigned Maximum across Vector"
		//
		// see developer.arm.com/architectures/instruction-sets/
		//     intrinsics/vmaxvq_u16

#elif (SYS__S390_BITS >= 64)
#error "supply S390 check-ZF(compare-0) intrinsic here"

#else
	return !!((((v[  0 ] | v[  1 ]) | (v[  2 ] | v[  3 ]))  &
	           ((v[  4 ] | v[  5 ]) | (v[  6 ] | v[  7 ]))) &
	          (((v[  8 ] | v[  9 ]) | (v[ 10 ] | v[ 11 ]))  &
	           ((v[ 12 ] | v[ 13 ]) | (v[ 14 ] | v[ 15 ]))));
		// note the 8x2 split
		// expect to be specialized into faster-than-16x expression

#endif
#endif
#endif       // /256 bits


/*-----------------------------------------
 * is 64x16 wide SIMD array all zeroes?
 *
 * SECURITY NOTE: tmp[] is scratch which caller MUST wipe
 * NOTE: DESTROYS ORIGINAL INPUTS
 */
static inline
REALLY_FORCE_INLINE
/**/
unsigned int simd_is_all0x64x16_inpl(uint16_t v[static 64],
                                   uint16_t tmp[static 16])
{
	simd_or16x16(tmp, &(v[32]), &(v[48]));       // 2 3
	simd_or16x16(v,     v,      &(v[16]));       // 0 1
	simd_or16x16(v,     tmp,      v     );       // 0 1 2 3

	return simd_is_all0(v);
}


/*-----------------------------------------
 * v[] += adv; preserving mod-prime by subtracting m2r[] if applicable
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance64x16_m2r_inpl(uint16_t v[static 64], uint16_t adv,
                        const uint16_t m2r[static 64])
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


#if 0    // FIPS 186-x only
/*---------------------------------------
 * v[] += adv[]; preserving mod-prime by subtracting m2r[] if applicable
 * 64 elements
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance64x16_m2r_inpl_v(uint16_t v[static 64],
                          const uint16_t adv[static 64],
                          const uint16_t m2r[static 64])
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
#endif      // 0: FIPS 186-x


/*-----------------------------------------
 * 'plain' prime search: can modn[] indicate a prime?
 *
 * SECURITY NOTE: tmp[] and tmp2[] are scratch, filled with (potentially)secret
 * state-dependent data, and SHOULD be wiped upon return.
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_nofactor64x16(uint16_t tmp[static 64],
                           uint16_t tmp2[static 64],
                     const uint16_t modn[static 64],
                      const uint16_t inv[static 64],
                    const uint16_t limit[static 64])
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
uint16_t simd_nofactor_first(uint16_t tmp[static 64],
                             uint16_t tm2[static 64],
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
uint16_t simd_nofactor_rest_s(uint16_t tmp[static 64],
                              uint16_t tm2[static 64],
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


#if !defined(NO_SIMDDIVIDE_M) && defined(NO_SIMDDIVIDE_L)  //----------------
/*--------------------------------------
 * 1856-prime version of simd_nofactor_rest_s()
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_nofactor_rest_m(uint16_t tmp[static 64],
                              uint16_t tm2[static 64],
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
#endif  //-----  !NO_SIMDDIVIDE_M) && NO_SIMDDIVIDE_L  //---------------------


/*--------------------------------------
 * 3456-prime version of simd_nofactor_rest_s()
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_nofactor_rest_l(uint16_t tmp[static 64],
                              uint16_t tm2[static 64],
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


/*--------------------------------------
 * does the current modn[] indicate a small factor among the 64 small
 * primes in the corresponding firstprimes[]?
 *
 * uses inverses and comparison limits from matching inv[] and limit[]
 */
static inline
REALLY_FORCE_INLINE
/**/
uint16_t simd_no_spfactor64x16(uint16_t tmp[static 64],
                              uint16_t tmp2[static 64],
                        const uint16_t modn[static 64],
                         const uint16_t inv[static 64],
                       const uint16_t limit[static 64]) ;


#if 0
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
uint16_t simd_no_spfactor_first(uint16_t tmp[static 64],
                                uint16_t tm2[static 64],
                const struct PP_Mod16bit *ps)
{
	return simd_no_spfactor64x16(tmp, tm2, ps->modn,
		                     firstprimes_inverse_simd,
		                     firstprimes_mullimit_simd);
}
#endif


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
uint16_t simd_no_spfactor_rest1(uint16_t tmp[static  64],
                               uint16_t tmp2[static  64],
                         const uint16_t modn[static 576])
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
uint16_t simd_no_spfactor_rest_m(uint16_t tmp[static   64],
                                uint16_t tmp2[static   64],
                          const uint16_t modn[static 1856])
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
uint16_t simd_no_spfactor_rest_l(uint16_t tmp[static   64],
                                uint16_t tmp2[static   64],
                          const uint16_t modn[static 3456])
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


#if 0
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
uint16_t simd_no_spfactor_rest(uint16_t tmp[static 64],
                              uint16_t tmp2[static 64],
               const struct PP_Mod16bit *ps)
{
	return simd_no_spfactor_rest1(tmp, tmp2, ps->modn);
}
#endif


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


#if 0     // FIPS 186-x only
/*--------------------------------------
 * advance rest of state, which was skipped by first-rounds' checking
 * small table part
 *
 * dst->modn[] += adv[]
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance_rest_v_s(struct PP_Mod16bit *dst,
                               const uint16_t adv[static 576])
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
#endif   // 0: FIPS 186-x


#if !defined(NO_SIMDDIVIDE_M) && !defined(NO_SIMDDIVIDE_L)  //---------------
#if 0   // FIPS 186-x only
/*--------------------------------------
 * mid-sized equivalent of simd_advance_rest_v_s()
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance_rest_v_m(struct PP_Mod16bit *dst,
                               const uint16_t adv[static 1856])
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
#endif  // 0: FIPS 186-x only
#endif  // !NO_SIMDDIVIDE_M && NO_SIMDDIVIDE_L  //---------------------------


#if 0   // FIPS 186-x only
#if !defined(NO_SIMDDIVIDE_L)  //--------------------------------------------
/*--------------------------------------
 * large-table equivalent of simd_advance_rest_v_s()
 */
static inline
REALLY_FORCE_INLINE
/**/
void simd_advance_rest_v_l(struct PP_Mod16bit *dst,
                               const uint16_t adv[static 3456])
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
#endif    // 0: FIPS 186-x
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


#if 0
// FIPS 186-x only

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
#endif


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
uint16_t simd_no_spfactor64x16(uint16_t tmp[static 64],
                               uint16_t tm2[static 64],
                        const uint16_t modn[static 64], 
                         const uint16_t inv[static 64],
                       const uint16_t limit[static 64])
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

	return simd_is_all0x64x16_inpl(tmp, tm2);
}
#endif   //-----  /delimiter: generated SIMD columns  ------------------------


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
void simd_cmp16x16_twin(uint16_t   r[static  16],
                        uint16_t tmp[static  16],
                  const uint16_t   u[static  16],
                  const uint16_t inv[static  16],
                const uint16_t limit[static  16])
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
uint16_t simd_no_twinfactor64x16(uint16_t tmp[static 64],
                                 uint16_t tm2[static 64],
                          const uint16_t modn[static 64],
                           const uint16_t inv[static 64],
                         const uint16_t limit[static 64])
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


#if 0
//----------------------------------------------------------------------------
static inline struct PP_Mod16bit *pp_mod16_init0(struct PP_Mod16bit *ps)
{
	if (ps)
		memset(ps, 0, sizeof(*ps));

	return ps;
}
#endif


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
 * how much to advance from current 6k+...mod6... to next possible prime?
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
uint16_t simd_has_no_factor(uint16_t tmp[static 64],
                            uint16_t tm2[static 64],
            const struct PP_Mod16bit *ps)
{
	return (simd_nofactor_first(tmp, tm2, ps) &&
	        simd_nofactor_rest_s(tmp, tm2, ps)) ;
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
                                uint16_t tmp[static 64],
                                uint16_t tm2[static 64],
                const struct PP_Mod16bit *ps)
{
	if (simd_has_no_factor(tmp, tm2, ps))
		wr = report_current_lsb(lsb, count, ps->lsb, wr);

	return wr;
}


#if !defined(NO_SIMDDIVIDE_M) && defined(NO_SIMDDIVIDE_L)  //----------------
/*--------------------------------------
 * is the current state divided by any of the first 1856 primes?
 */
static inline
uint16_t simd_has_no_factor_m(uint16_t tmp[static 64],
                              uint16_t tm2[static 64],
              const struct PP_Mod16bit *ps)
{
	return (simd_nofactor_first(tmp, tm2, ps) &&
	        simd_nofactor_rest_m(tmp, tm2, ps)) ;
}
#endif    // !NO_SIMDDIVIDE_M && NO_SIMDDIVIDE_L  ---------------------------


#if !defined(NO_SIMDDIVIDE_M) && defined(NO_SIMDDIVIDE_L)  //----------------
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
                                  uint16_t tmp[static 64],
                                  uint16_t tm2[static 64],
                  const struct PP_Mod16bit *ps)
{
	if (simd_has_no_factor_m(tmp, tm2, ps))
		wr = report_current_lsb(lsb, count, ps->lsb, wr);

	return wr;
}
#endif    // !NO_SIMDDIVIDE_M && NO_SIMDDIVIDE_L  ---------------------------


#if !defined(NO_SIMDDIVIDE_L)  //--------------------------------------------
/*--------------------------------------
 * is the current state divided by any of the first 3456 primes?
 */
static inline
uint16_t simd_has_no_factor_l(uint16_t tmp[static 64],
                              uint16_t tm2[static 64],
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
                                   uint16_t tmp[static 64],
                                   uint16_t tm2[static 64],
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
// shared code: twin primes, safe primes

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
/* static inline */
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

// TODO: this is essentially simd_no_spfactor_first(); deduplicate
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


// we search for safe primes starting at the following 2048-bit number:
//
// ff31415926535897932384626433832795028841971693993751058209749445
// 9230781640628620899862803482534211706798214808651328230664709384
// 4609550582231725359408128481117450284102701938521105559644622948
// 9549303819644288109756659334461284756482337867831652712019091456
// 4856692346034861045432664821339360726024914127372458700660631558
// 8174881520920962829254091715364367892590360011330530548820466521
// 3841469519415116094330572703657595919530921861173819326117931051
// 185480744623799627495673518857527248912279381830119491298336733b
//
// (decimal 3221505644354..46517451797307)
#define  KAT_START_LSB  UINT64_C(0x119491298336733b)
//
// P mod (small prime) (5, 7, 11, 13, 17, ..., 32213, 32233, 32237)
// see firstprimes[]
static const uint16_t start_mod_firstprimes[3456] = {
	2, 1, 4, 2, 8, 12, 8, 7, 4, 3, 31, 22, 20, 5, 34, 4, 40, 62,
	1, 12, 13, 80, 2, 37, 39, 70, 54, 48, 65, 40, 90, 22, 8, 54,
	118, 130, 100, 103, 12, 174, 138, 136, 154, 110, 169, 199,
	103, 110, 76, 113, 183, 27, 110, 1, 173, 117, 192, 187, 68,
	181, 262, 222, 89, 132, 224, 191, 283, 102, 312, 285, 137,
	225, 77, 56, 100, 200, 21, 147, 92, 385, 277, 215, 267, 364,
	47, 14, 150, 67, 29, 10, 468, 191, 316, 78, 240, 100, 76,
	154, 105, 100, 439, 6, 122, 56, 304, 253, 96, 134, 303, 530,
	374, 217, 620, 589, 217, 388, 507, 541, 306, 74, 116, 300,
	70, 439, 571, 406, 354, 598, 660, 239, 82, 206, 477, 743,
	525, 754, 92, 386, 228, 287, 482, 542, 23, 334, 679, 238,
	684, 308, 242, 14, 279, 447, 712, 112, 365, 544, 882, 446,
	694, 267, 438, 130, 908, 468, 382, 557, 358, 487, 449, 39,
	772, 465, 210, 532, 920, 10, 853, 421, 288, 290, 613, 44, 33,
	36, 291, 969, 16, 1013, 221, 198, 677, 271, 717, 913, 524,
	487, 406, 288, 623, 297, 143, 1015, 824, 714, 687, 1227, 491,
	313, 168, 530, 504, 1058, 771, 965, 855, 615, 1256, 739, 734,
	1294, 533, 762, 1251, 828, 1389, 1431, 726, 504, 836, 1071,
	254, 1158, 902, 852, 1019, 693, 631, 1451, 1050, 757, 985,
	82, 423, 209, 687, 7, 1052, 1426, 1205, 642, 213, 1260, 744,
	1075, 771, 1616, 994, 1102, 1610, 1252, 442, 737, 68, 520,
	1020, 949, 1703, 176, 379, 152, 352, 1296, 1166, 250, 948,
	1465, 422, 677, 1052, 131, 743, 190, 236, 1062, 65, 462, 787,
	176, 1288, 1720, 813, 969, 36, 1483, 497, 468, 1735, 1078,
	1352, 1230, 185, 832, 417, 257, 1036, 353, 669, 1317, 1608,
	908, 1377, 818, 1216, 2034, 874, 471, 953, 177, 3, 49, 797,
	828, 223, 245, 1278, 1990, 1407, 734, 1419, 307, 1838, 1594,
	346, 68, 4, 243, 1330, 585, 420, 457, 1715, 252, 1332, 191,
	1049, 967, 1599, 1238, 1568, 2296, 846, 1989, 113, 663, 2154,
	2145, 28, 283, 1089, 34, 1448, 847, 1179, 1253, 1935, 2055,
	2481, 1135, 2219, 1779, 2236, 450, 1006, 1134, 1657, 825, 901,
	1562, 2215, 571, 1504, 481, 2329, 340, 1591, 621, 67, 540,
	335, 1781, 2665, 3, 189, 2683, 1129, 2570, 2683, 2200, 525,
	80, 1280, 477, 1369, 1009, 1600, 975, 2396, 840, 2811, 2682,
	2388, 1844, 167, 1377, 192, 587, 1619, 1361, 2068, 2451, 535,
	2201, 1612, 540, 896, 328, 641, 833, 2637, 2982, 931, 2055,
	746, 1884, 1586, 1913, 2535, 1773, 2639, 644, 2133, 2759,
	1776, 2596, 1721, 1388, 994, 1995, 2296, 2070, 629, 1560,
	1049, 2180, 167, 2775, 1304, 1297, 1516, 3050, 2791, 516,
	2767, 2076, 462, 2367, 3304, 2232, 1423, 1779, 1329, 2409,
	1715, 935, 2261, 3098, 1754, 2684, 2447, 1765, 620, 1660,
	1408, 93, 467, 125, 2295, 1712, 3504, 3213, 2450, 2209, 813,
	1266, 3417, 831, 1130, 2535, 247, 1497, 1434, 1071, 1982, 422,
	2250, 186, 3546, 2697, 555, 402, 2, 2529, 3230, 941, 701, 2219,
	1794, 3266, 3271, 448, 713, 741, 1527, 2501, 1590, 287, 3192,
	239, 2640, 2284, 661, 1562, 2974, 3113, 602, 3847, 11, 3707,
	2891, 3545, 1488, 959, 1772, 3216, 2148, 3904, 564, 3188,
	3999, 2029, 2678, 1727, 790, 2358, 3928, 3661, 2362, 1345,
	834, 3131, 3603, 3253, 1740, 970, 303, 459, 2640, 862, 3786,
	1809, 224, 636, 755, 2250, 2944, 1918, 1984, 529, 1182, 2682,
	1878, 834, 3291, 1777, 1792, 1558, 2188, 2936, 955, 3574, 4006,
	2161, 310, 3421, 3582, 2325, 376, 3051, 2474, 2837, 693, 923,
	3020, 3835, 3152, 528, 2313, 4051, 1627, 3152, 3899, 3949,
	377, 4450, 4408, 2135, 3664, 4579, 2522, 3458, 2982, 3767,
	3081, 585, 1717, 1561, 4455, 2890, 3026, 783, 3875, 2812,
	3244, 2503, 3974, 2942, 2375, 2469, 2644, 782, 4437, 1118,
	3188, 3954, 1423, 811, 3926, 3240, 1865, 2027, 4479, 175,
	3471, 4114, 736, 3886, 1250, 2080, 4249, 480, 2118, 1175,
	2517, 3996, 3658, 2512, 1237, 4874, 1763, 2551, 3599, 567,
	4170, 4560, 1973, 3065, 4754, 196, 4052, 1424, 3042, 2222,
	1637, 3468, 4556, 3434, 3791, 5107, 67, 3587, 5026, 1894,
	4301, 1272, 4568, 1354, 2541, 1075, 229, 3261, 5101, 788,
	2239, 3038, 4418, 2851, 2478, 174, 416, 4757, 2500, 3082,
	1503, 3581, 3702, 3245, 1119, 2057, 5430, 300, 3955, 884,
	112, 1654, 3831, 1785, 1188, 2581, 2178, 966, 2532, 5218,
	4310, 3249, 4050, 1549, 3826, 5079, 952, 3761, 679, 4635,
	757, 762, 5264, 1457, 2732, 2041, 14, 5305, 4285, 3903, 5368,
	5087, 5699, 3940, 1146, 868, 1250, 451, 3223, 1495, 328, 1057,
	1266, 44, 3949, 2015, 4293, 527, 1627, 673, 3648, 5246, 521,
	3139, 4040, 5771, 2189, 3242, 1166, 1784, 293, 1679, 4755,
	4405, 3619, 3324, 1998, 4844, 5035, 4541, 2831, 210, 1282,
	1876, 1015, 5750, 5280, 1328, 5559, 5889, 5706, 4457, 3708,
	5906, 4843, 1700, 3980, 1537, 5344, 5649, 63, 6186, 2196,
	233, 3572, 1943, 4208, 47, 5256, 3132, 4011, 761, 5551, 5112,
	1053, 1753, 6172, 3940, 5117, 2159, 1368, 1447, 892, 3012,
	5495, 2676, 4457, 5909, 2343, 2044, 6420, 2520, 5235, 3790,
	504, 3007, 1473, 1463, 5977, 3971, 3685, 1350, 4834, 967,
	3246, 2222, 5833, 3343, 4885, 598, 5157, 6593, 3458, 5327,
	1413, 992, 5258, 4545, 4056, 3425, 609, 6223, 3919, 5069,
	1239, 4280, 6015, 5756, 3115, 129, 6669, 4902, 4548, 6834,
	5130, 3556, 100, 332, 3401, 5540, 963, 591, 1590, 1787, 1280,
	3698, 6762, 4996, 4447, 3466, 1764, 2084, 1546, 6267, 966,
	6813, 3099, 3610, 3321, 6103, 4768, 3882, 2487, 74, 3683,
	1029, 1390, 152, 4295, 4855, 5160, 3313, 1406, 3059, 3984,
	4772, 4953, 2718, 6649, 1080, 26, 5067, 769, 717, 3788, 1222,
	2306, 1104, 3123, 2983, 4561, 4633, 4069, 326, 5675, 2065,
	4478, 4716, 5684, 2951, 6708, 2030, 5157, 3151, 380, 1960,
	722, 2162, 6958, 4679, 1336, 5671, 6201, 2931, 2352, 1045,
	6425, 1047, 7273, 6020, 4163, 5415, 3527, 5123, 5104, 198,
	5424, 4416, 6539, 1464, 5987, 2206, 2362, 4285, 4155, 7104,
	7060, 694, 6889, 6145, 7860, 1346, 6229, 1210, 5688, 2073,
	2355, 363, 2145, 1572, 7546, 3281, 1326, 6189, 4837, 5287,
	4685, 993, 5315, 6800, 3410, 5554, 3356, 552, 5656, 1387,
	1229, 4361, 2426, 4515, 4795, 7906, 2175, 5766, 400, 6340,
	6180, 1355, 5358, 8123, 8125, 8024, 6918, 6873, 1051, 6344,
	6338, 1940, 94, 3284, 685, 2104, 5758, 6070, 8064, 5638, 2162,
	2913, 1217, 4298, 3183, 1689, 8222, 2067, 7614, 2153, 2619,
	7233, 4189, 4061, 4454, 4864, 6136, 780, 8013, 8285, 4381,
	18, 1632, 6841, 5057, 1179, 7249, 6181, 4858, 6441, 1203,
	322, 3602, 5361, 3274, 685, 6655, 8695, 2956, 6418, 7659,
	2069, 6015, 7233, 3688, 3153, 2737, 3310, 5775, 5694, 160,
	1068, 7091, 210, 2465, 7047, 753, 5800, 2584, 1852, 6822,
	2787, 7183, 162, 2858, 6802, 1551, 4556, 921, 4534, 4748,
	1480, 6470, 7575, 5864, 4832, 6967, 9065, 3259, 4528, 8935,
	1339, 7588, 2314, 8752, 8604, 5993, 7346, 7515, 8363, 8233,
	34, 5951, 8044, 2150, 7546, 6696, 2509, 3421, 3864, 4332,
	2385, 2934, 102, 355, 8366, 710, 8103, 5907, 9268, 4114,
	6560, 5057, 543, 1243, 7756, 9162, 5986, 4443, 701, 6806,
	6881, 5369, 5173, 4354, 6636, 7253, 6623, 5964, 785, 6672,
	86, 1848, 7382, 1201, 3710, 4317, 7848, 6411, 6237, 4058,
	8587, 2010, 5029, 6467, 6825, 6669, 6388, 595, 5388, 6851,
	7816, 1826, 469, 771, 8801, 1321, 154, 9279, 5096, 6280,
	7954, 6059, 1924, 2581, 112, 1778, 8452, 5408, 2984, 6634,
	5057, 8324, 8462, 4960, 1877, 9043, 2522, 491, 3510, 9067,
	7891, 6891, 7604, 6100, 2768, 477, 7814, 3880, 4306, 1944,
	8168, 1355, 5568, 3936, 8788, 8980, 7195, 2001, 18, 5357,
	1145, 8521, 6095, 6466, 6953, 5937, 7642, 7096, 2247, 7732,
	10206, 5480, 5943, 3498, 6061, 6146, 1189, 3917, 2393, 5304,
	5363, 5522, 377, 3226, 8037, 5213, 8606, 1156, 6609, 3365,
	6784, 9891, 656, 2898, 9758, 5722, 2535, 4808, 10565, 2226,
	8463, 3541, 6046, 7765, 6014, 8501, 8582, 3039, 1917, 6302,
	3520, 7918, 5543, 1565, 1429, 10669, 5625, 9514, 2079, 6270,
	5934, 860, 1402, 9041, 829, 10339, 6558, 2138, 7009, 10525,
	10612, 818, 8593, 8716, 7670, 9470, 623, 4262, 5345, 6599,
	1342, 2497, 1543, 7677, 9898, 185, 3145, 1999, 6897, 3815,
	3193, 101, 1437, 8439, 8825, 10713, 2016, 4047, 4718, 8625,
	6926, 8613, 2256, 3640, 8792, 8996, 6667, 5225, 6483, 6824,
	2210, 4513, 5144, 4196, 1477, 9839, 3065, 8206, 873, 1883,
	2072, 6761, 10149, 9558, 10416, 1370, 3347, 3303, 10045,
	6621, 4143, 5217, 964, 10424, 253, 1693, 329, 9897, 4162,
	3627, 2102, 2480, 9581, 2130, 11367, 1664, 6619, 10931,
	11243, 5921, 2512, 8457, 7603, 61, 11028, 4831, 2273, 6126,
	1090, 3080, 6685, 9609, 1591, 10693, 8824, 6420, 1658, 9018,
	10425, 10796, 5652, 93, 6320, 11696, 10639, 1793, 7535, 10538,
	3053, 6563, 1758, 6962, 5065, 1900, 6268, 1921, 8110, 267,
	204, 5298, 3468, 6480, 9214, 5709, 297, 4724, 8561, 8702,
	9810, 6191, 302, 10443, 3567, 8733, 4990, 3606, 6120, 6065,
	10354, 7282, 8213, 6162, 1710, 10121, 3845, 7547, 11967,
	9968, 2653, 1855, 12138, 1300, 10716, 5655, 7209, 12185,
	6245, 7544, 3961, 6177, 5001, 750, 2841, 6001, 8189, 8895,
	11252, 6504, 10046, 2092, 11584, 2391, 8315, 7702, 10074,
	5541, 3903, 8774, 4558, 12375, 2398, 5418, 6780, 3950, 1968,
	4434, 8120, 2902, 8893, 10385, 11146, 2226, 6665, 7191, 11917,
	4149, 4659, 12132, 9850, 11498, 11409, 4981, 4741, 6114, 6080,
	7115, 9120, 7702, 3422, 4492, 3506, 7032, 4560, 10177, 5949,
	5639, 704, 11086, 2899, 7522, 10164, 10897, 8300, 6721, 3582,
	6303, 1855, 652, 8184, 10790, 9449, 7766, 9460, 408, 4387,
	4846, 8266, 9583, 935, 3359, 12754, 3212, 13115, 7321, 6063,
	12190, 469, 2123, 8959, 10203, 11744, 11069, 627, 5615, 3799,
	7719, 3110, 11104, 12892, 68, 1614, 7193, 9816, 5352, 2743,
	10517, 3166, 12297, 2331, 7982, 8837, 5921, 4411, 2630, 1436,
	2323, 768, 6671, 3582, 4398, 11517, 6720, 807, 6227, 13070,
	11789, 6875, 10449, 1279, 8735, 6298, 13551, 10076, 7485,
	9340, 11467, 12077, 2720, 11181, 2219, 12326, 12392, 4542,
	5655, 4640, 4644, 10405, 4040, 11162, 3812, 4092, 10414, 5516,
	887, 8490, 5891, 8927, 12042, 7971, 4093, 12778, 3146, 6391,
	6726, 8126, 13349, 9907, 7065, 5029, 12583, 11905, 9212, 9545,
	4448, 8720, 13706, 1821, 12799, 3090, 4280, 7627, 377, 13231,
	9068, 2010, 6455, 11161, 5166, 8453, 7021, 13767, 13492, 755,
	12478, 12303, 2709, 1125, 1282, 10241, 2721, 12460, 2710,
	7440, 10441, 5389, 14341, 4024, 10785, 6731, 21, 11882, 944,
	9294, 7893, 1241, 866, 2756, 6849, 13776, 6411, 9666, 7596,
	13629, 7265, 10207, 1090, 1389, 14329, 12437, 9952, 13102,
	9735, 3304, 13854, 3485, 743, 12341, 3730, 3516, 11842, 3966,
	1102, 4202, 658, 12054, 7696, 392, 5929, 9299, 3519, 9071,
	9036, 11035, 261, 9796, 4167, 14174, 4302, 2110, 6251, 7606,
	4423, 8970, 13013, 7799, 3120, 9072, 7347, 1579, 3341, 14542,
	6538, 9915, 4028, 9556, 9356, 1950, 111, 12223, 11051, 578,
	8008, 7504, 2816, 9512, 11173, 13999, 70, 3838, 5811, 944,
	1230, 4155, 14680, 8490, 13796, 885, 8951, 2833, 1929, 1113,
	8951, 15035, 789, 1893, 1124, 344, 1475, 1608, 9374, 1654,
	8666, 11444, 1763, 6537, 14648, 6342, 10538, 1023, 8810,
	11632, 5467, 4115, 14150, 4392, 7161, 7149, 13593, 4545,
	4951, 385, 4812, 9426, 13324, 10227, 6218, 10978, 4035, 8643,
	6894, 2256, 2061, 5688, 8137, 6370, 8400, 15068, 3098, 4230,
	15169, 3744, 15593, 2600, 2037, 13113, 8442, 15667, 15488,
	3618, 11142, 8887, 5111, 1091, 10355, 8891, 10240, 11259,
	6375, 12213, 9745, 5499, 10351, 13878, 10908, 10211, 9452,
	10658, 13037, 11669, 13577, 11694, 15362, 22, 14880, 11214,
	13340, 12282, 552, 2869, 11191, 944, 11895, 687, 4654, 7444,
	15859, 10730, 8167, 12139, 15487, 11710, 9903, 16124, 458,
	5584, 6235, 11979, 13336, 2722, 2907, 16125, 15227, 1493,
	11998, 489, 10276, 1451, 7528, 4870, 6601, 5535, 13313, 8657,
	3628, 440, 12206, 13267, 10957, 14054, 5385, 3017, 7935, 7229,
	7731, 14124, 7473, 6944, 15459, 9635, 13520, 335, 4888, 10698,
	14180, 169, 13499, 12974, 14584, 6201, 12510, 16086, 1322,
	6007, 15697, 13196, 14962, 6939, 9935, 9432, 12965, 27, 1592,
	7739, 13279, 875, 11449, 11898, 12485, 6207, 7045, 3016, 833,
	13328, 423, 1661, 4656, 16570, 3051, 14248, 9085, 6455, 15792,
	3323, 2452, 14801, 16307, 14164, 3726, 6369, 12117, 8600,
	5752, 1049, 5997, 6189, 15790, 13041, 6030, 12820, 6648,
	16223, 13903, 17139, 1984, 10725, 157, 3198, 14682, 3815,
	5839, 1287, 16316, 12662, 5386, 11878, 15883, 12209, 2902,
	4256, 7368, 14959, 3455, 12058, 13672, 16146, 10510, 10193,
	15407, 12916, 7479, 16434, 12634, 13907, 3899, 5132, 12527,
	3472, 2524, 9955, 8136, 2863, 8853, 16973, 7484, 15912,
	17323, 10935, 590, 9545, 1754, 3, 5841, 13106, 5125, 7314,
	3918, 6906, 7435, 15619, 974, 8254, 8423, 12985, 12034, 9353,
	16714, 11536, 15210, 10179, 2227, 5968, 16371, 8571, 5531,
	8619, 5871, 15956, 15016, 5690, 3071, 823, 11820, 6578,
	5409, 11579, 5418, 5866, 18002, 9434, 17722, 14191, 3350,
	4034, 8504, 752, 5893, 11587, 9287, 514, 751, 13610, 2166,
	5649, 410, 11327, 9920, 5884, 15563, 13314, 14194, 180, 7231,
	9855, 4471, 5767, 16530, 6026, 3389, 9197, 2875, 6705, 13064,
	17194, 16579, 5499, 12319, 8593, 8469, 13976, 3284, 7338,
	6610, 11929, 17340, 6686, 14051, 12370, 17665, 2067, 8117,
	1785, 9654, 5082, 10353, 9328, 13913, 16881, 10646, 16419,
	14037, 8009, 10781, 3820, 64, 2911, 5827, 1643, 11009, 15591,
	3924, 5174, 7817, 11361, 3607, 3765, 16783, 2969, 747, 406,
	14521, 7342, 12432, 3998, 17182, 11114, 14572, 11819, 6186,
	10169, 17463, 13495, 13076, 14682, 16087, 12408, 15668, 3870,
	2988, 4742, 11746, 6930, 19060, 4939, 694, 1539, 10720, 1644,
	9495, 1949, 4246, 6566, 6358, 7975, 9335, 4879, 17785, 19022,
	6187, 76, 18257, 919, 11984, 173, 14023, 17263, 6309, 4695,
	16529, 6079, 18874, 8079, 9204, 13162, 12120, 15128, 14764,
	18550, 13272, 7242, 13319, 8575, 5988, 9854, 1265, 1349,
	4720, 13634, 14757, 2623, 15702, 9622, 14792, 12068, 490,
	10082, 12335, 18235, 11927, 519, 6458, 5233, 18919, 1519,
	15510, 6042, 18537, 15199, 6704, 10119, 14125, 11567, 3310,
	16843, 12175, 9615, 14149, 16036, 1841, 10870, 68, 15138,
	17693, 1391, 5953, 6750, 3975, 10203, 4581, 3135, 4771, 3008,
	17194, 2666, 3303, 18535, 17170, 7368, 592, 16686, 13759,
	1031, 19042, 9056, 3510, 2859, 17509, 11457, 3084, 16698,
	16670, 15191, 6676, 11863, 6678, 15776, 7644, 4132, 8315,
	824, 261, 16902, 16143, 7906, 6372, 19283, 14813, 2707, 3453,
	9916, 214, 13049, 19027, 4618, 6731, 8909, 10994, 16053, 2506,
	16459, 16065, 1338, 5996, 13319, 2324, 16181, 5748, 12099,
	6536, 9007, 7346, 5219, 8012, 18249, 15158, 19275, 19612,
	12061, 19408, 17206, 7003, 10538, 6501, 13412, 3563, 5575,
	7317, 6502, 19688, 2974, 11224, 20544, 12698, 6168, 11229,
	13380, 16152, 14100, 2093, 16314, 15911, 554, 437, 3452,
	1745, 7435, 12967, 12657, 12294, 5059, 14477, 6317, 2048,
	1469, 14129, 11643, 5686, 323, 15729, 13092, 14045, 13482,
	251, 4348, 6917, 1807, 20225, 18736, 323, 20236, 8615, 17786,
	8890, 19451, 11839, 13704, 14996, 678, 9553, 8109, 13850,
	9310, 13013, 2667, 1012, 4574, 10909, 10645, 5151, 6078,
	2896, 17702, 2277, 18099, 15942, 5300, 2317, 15384, 1636,
	8894, 20468, 14293, 12445, 3978, 18560, 9239, 3014, 6332,
	8634, 13451, 21170, 9822, 3872, 19380, 4193, 832, 147, 7686,
	12704, 20695, 15192, 21569, 17317, 9961, 13992, 10876, 9625,
	1190, 4539, 7651, 16831, 13069, 19737, 12448, 18988, 10540,
	15083, 9610, 20760, 1343, 8445, 17002, 20390, 14940, 11442,
	8853, 20820, 3630, 18583, 15423, 291, 17534, 17095, 7275,
	18110, 10905, 2902, 17692, 6270, 17949, 9272, 6659, 9500,
	3228, 21139, 7168, 22049, 7481, 18780, 21439, 2404, 2660,
	17681, 4870, 16922, 9150, 8342, 3326, 10156, 5439, 14330,
	15301, 6197, 9895, 19948, 8262, 849, 2543, 8554, 10226,
	2092, 21396, 21015, 4863, 19253, 13145, 21005, 123, 17455,
	8348, 12447, 7047, 15288, 8414, 4319, 19874, 9242, 20420,
	7554, 2628, 2418, 22463, 18265, 21181, 22474, 21088, 15943,
	19353, 21275, 9532, 15178, 22356, 3274, 2741, 10739, 13520,
	17569, 15691, 16204, 20248, 17183, 4186, 5461, 21608, 3537,
	17885, 19365, 4402, 5300, 22225, 3460, 9540, 13948, 2027,
	14002, 6946, 9816, 10749, 11679, 6716, 2633, 732, 21178,
	9355, 14339, 5175, 20431, 9308, 4411, 15901, 9489, 16600,
	2512, 4422, 11168, 17644, 17072, 20481, 1472, 11679, 8697,
	14124, 20549, 6714, 15616, 296, 5319, 20759, 7119, 9135,
	6049, 10644, 4531, 574, 16146, 19807, 3935, 5945, 5746,
	14539, 18750, 8945, 7334, 12594, 23117, 6567, 10702, 4820,
	8314, 18804, 17113, 22765, 19339, 20880, 19802, 10621, 8673,
	19217, 7903, 15887, 90, 13314, 17658, 9109, 4915, 11810, 3041,
	8511, 8573, 14585, 16633, 23608, 2132, 2451, 7069, 11135,
	10802, 5001, 20068, 10772, 13345, 19720, 3882, 4688, 12893,
	4354, 368, 21219, 11479, 7503, 5427, 22863, 5390, 4072, 3092,
	7519, 12933, 3838, 16179, 14164, 1712, 22262, 1114, 1456,
	442, 16498, 1346, 11915, 20955, 6053, 15866, 21056, 13813,
	22506, 872, 2816, 6818, 16218, 12892, 15546, 23226, 1164,
	5564, 8913, 11796, 9985, 18670, 9760, 13322, 18357, 20623,
	5954, 19522, 17116, 15248, 2172, 15332, 21314, 8797, 18848,
	13645, 3778, 10856, 8857, 16544, 6599, 12621, 5183, 7975,
	17455, 20267, 11005, 311, 22664, 18123, 14665, 24340, 1292,
	9577, 1021, 9021, 8376, 6831, 16569, 20991, 11906, 5351,
	2225, 19997, 3344, 17826, 15533, 21206, 6358, 12663, 8302,
	5830, 16568, 11020, 16591, 6084, 14643, 4599, 1606, 9726,
	7301, 167, 13339, 20018, 1257, 5744, 5348, 441, 4634, 10526,
	598, 13241, 19473, 15674, 18213, 18398, 22439, 12946, 20742,
	21403, 22024, 23908, 8702, 19767, 13147, 20252, 4898, 20312,
	22243, 6570, 17581, 9420, 194, 15105, 17581, 20504, 3001,
	15558, 10099, 12407, 1132, 20768, 9692, 15542, 2009, 11672,
	12206, 17574, 23931, 18818, 23438, 11555, 2927, 15652, 1554,
	9230, 1108, 22388, 17407, 1714, 13290, 8916, 6798, 23850,
	4966, 21204, 15948, 9868, 17559, 15788, 16918, 16505, 14188,
	7844, 22182, 15664, 4174, 17633, 10254, 25216, 14460, 9237,
	14716, 23496, 17273, 14576, 9323, 5977, 13348, 7936, 14050,
	8791, 4250, 10951, 17288, 8210, 20316, 21403, 22441, 16001,
	11250, 768, 7087, 9357, 6318, 11203, 11017, 3920, 6705, 5276,
	7028, 9821, 20354, 6397, 6618, 23607, 10848, 2126, 14492,
	1758, 10089, 5708, 26037, 3312, 20564, 15701, 16074, 7902,
	14757, 21800, 24986, 8641, 15650, 23286, 6687, 17110, 15619,
	3488, 24405, 9680, 417, 20321, 10272, 5860, 18617, 18349,
	25966, 9835, 10231, 8359, 9934, 5005, 19713, 17603, 21228,
	11312, 5445, 26007, 4513, 7829, 6119, 10990, 18424, 15035,
	8856, 23098, 19986, 13422, 20196, 12202, 5137, 21603, 22411,
	11502, 6970, 15696, 20792, 23651, 25847, 13812, 3786, 9017,
	6506, 21581, 12337, 10015, 22009, 22726, 15219, 3026, 4989,
	5601, 10684, 1942, 17394, 8227, 23615, 5848, 15373, 15927,
	22533, 3635, 9652, 18787, 17523, 12838, 2396, 21939, 20489,
	4676, 15544, 24090, 10995, 9774, 16283, 7367, 15656, 6627,
	22542, 25957, 12015, 17935, 18183, 25131, 14797, 26315,
	26497, 9894, 22759, 25343, 2719, 13614, 7756, 19016, 19073,
	8963, 24165, 12851, 11354, 3910, 14951, 25017, 19362, 16145,
	19761, 20323, 2000, 19768, 1404, 24570, 7506, 4161, 26311,
	13204, 5951, 14793, 22597, 532, 10377, 21971, 1797, 20074,
	15878, 1034, 6840, 1626, 22549, 24396, 24101, 16525, 11265,
	15016, 12485, 1480, 11390, 2982, 26299, 12292, 17247, 13632,
	18707, 21687, 7959, 16771, 9713, 17429, 9042, 16430, 1514,
	18041, 12525, 20165, 25009, 20356, 16766, 21380, 4224, 14703,
	10716, 25710, 4286, 11248, 22589, 22309, 13214, 18778, 19924,
	8714, 11171, 7704, 607, 22017, 24078, 17808, 19289, 21940,
	11640, 16840, 28035, 23721, 22342, 19828, 1822, 14746, 26274,
	19756, 20896, 12170, 7596, 3754, 18310, 1239, 7843, 14764,
	11111, 20839, 8494, 19904, 24809, 1411, 18591, 22384, 16840,
	12375, 5155, 19689, 18371, 22751, 9989, 24118, 1246, 13671,
	18708, 8224, 6657, 16968, 28413, 19386, 26965, 10196, 15613,
	20759, 14128, 13384, 17822, 8902, 23243, 18367, 16419, 7850,
	2951, 21144, 28071, 24952, 11834, 23029, 13655, 26258, 22829,
	23741, 18973, 17378, 14619, 13024, 7764, 18259, 20122, 18880,
	24310, 1735, 11888, 18680, 20051, 25964, 26044, 9514, 14178,
	19681, 16157, 19302, 9566, 18860, 27740, 3977, 8516, 4677,
	2485, 15536, 23843, 10199, 6597, 18461, 10788, 13118, 16476,
	26864, 16891, 24076, 8572, 2236, 13126, 6844, 21589, 19244,
	25013, 24783, 26397, 16867, 28787, 25452, 4844, 7588, 19824,
	26415, 22636, 23453, 20470, 19921, 14160, 16044, 17983, 28157,
	18247, 17294, 13714, 26050, 10365, 19177, 25602, 23567, 6129,
	20118, 22198, 27126, 27014, 16873, 9486, 29170, 24648, 27441,
	9794, 24327, 16982, 27332, 2632, 23854, 8671, 14740, 11958,
	1947, 7752, 17230, 21814, 14824, 490, 25215, 24216, 26467,
	20725, 10681, 11602, 17353, 17827, 21581, 24999, 23025, 20756,
	16439, 3869, 26350, 11395, 13171, 1419, 14382, 15568, 18724,
	17234, 15495, 27289, 2022, 1504, 2307, 15379, 15678, 758,
	1105, 21255, 2487, 9709, 5155, 19469, 14705, 21353, 26609,
	28741, 20895, 5335, 537, 10996, 19809, 25125, 22960, 17279,
	6535, 18643, 9592, 26277, 3805, 21727, 19106, 9688, 8272, 254,
	3113, 15416, 27120, 6328, 26060, 12088, 24498, 8363, 24776,
	17380, 24094, 10872, 17126, 9732, 7763, 10611, 18178, 4929,
	630, 9843, 9127, 5445, 16833, 22644, 26951, 26853, 17164,
	6216, 16474, 3697, 17253, 16703, 26055, 16986, 22011, 7407,
	28379, 21007, 4090, 15188, 24332, 28107, 9154, 853, 9175,
	8073, 27305, 16559, 1778, 27657, 13880, 22656, 8016, 11794,
	7045, 27455, 7060, 19322, 8682, 28017, 5360, 25777, 12553,
	26112, 14824, 333, 13336, 8204, 28954, 17031, 9884, 11610,
	11925, 8165, 11083, 10692, 15491, 24680, 19620, 17579, 25201,
	30738, 18630, 16663, 22939, 2533, 12566, 19954, 18013, 27320,
	3023, 13011, 22716, 3436, 30942, 16624, 15156, 26617, 9448,
	16204, 30363, 24214, 27159, 22616, 8007, 22319, 20920, 14424,
	4301, 16900, 12697, 21042, 31004, 26036, 3828, 10517, 22275,
	31200, 17184, 20348, 21792, 30051, 17905, 24690, 9362, 20736,
	9305, 25680, 15173, 23664, 13842, 18114, 17536, 18128, 4964,
	9117, 1534, 14070, 13317, 19282, 12414, 8779, 24681, 27013,
	19624, 8804, 6931, 16075, 21782, 25998, 25174, 8068, 2234,
	12611, 24685, 28143, 22170, 19315, 28295, 14206, 27442, 30651,
	23272, 18046, 12872, 1159, 15089, 15466, 28036, 3789, 25555,
	83, 5140, 20149, 18267, 30747, 9117, 26980, 27249, 19905
} ;


#if 1  //=====  delimiter: standalone test code  =============================
#define USE_OPENSSL   /* check conditionals below; add other CSPs if needed */

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

	memmove(ps->qstr, base, hexb);

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
static int searchtype_uses_increment(const struct PP_Mod16bit *ps)
{
	return (ps && (SIMD_PRIMETYPE_FIPS186 & ps->mode));
}


//--------------------------------------
// keep in sync with SIMDPRIME_COUNT
//
static unsigned int report_table_prime_count(const struct PP_Mod16bit *ps)
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


/*--------------------------------------
 * normalize u64[] to BE64[]
 * tolerate in-place
 */
static void buffer2be64(unsigned char *pb, const uint64_t *arr, size_t count)
{
	if (pb && arr && count) {
		unsigned int i;

		for (i=0; i<count; ++i) {
			uint64_t curr = arr[i];

			MSBF8_WRITE(&(pb[ 8*i ]), curr);
		}
	}
}


#if defined(USE_OPENSSL)  /*------------------------------------------------*/
#include <openssl/evp.h>


//--------------------------------------
// return nr. of bytes written to start of (h, hbytes)
// note: no error handling; we only use this to demonstrate
//
static size_t hash_buffer(unsigned char *h, size_t hbytes,
                    const unsigned char *p, size_t bytes)
{
	size_t rc = 0;

	if (p && h && hbytes && bytes) {
		unsigned char hash[ EVP_MAX_MD_SIZE ];
		size_t hb = sizeof(hash);

		EVP_Q_digest(NULL, "SHA512", NULL, p, bytes, hash, &hb);

		rc = (hb < hbytes) ? hb : hbytes;

		if (rc)
			memmove(h, hash, rc);
	}

	return rc;
}
#endif    /*-----  USE_OPENSSL  --------------------------------------------*/


//--------------------------------------
// initialize safe/twin-prime search for the 2048-bit KAT start state
// returns number of filtering small primes, or 0 if anything failed
//
static unsigned int init0_kat_search_state(struct PP_Mod16bit *ps,
                                                  PrimeType_t type)
{
	if (ps) {
		unsigned int i, filter_primes;

		*ps = (struct PP_Mod16bit) PP_MOD16BIT_INIT0;

		set_default_table_size(ps);
		ps->mode |= type;

		filter_primes = report_table_prime_count(ps);
		if (!filter_primes)
			return 0;

		for (i=0; i<filter_primes; ++i)
			ps->modn[i] = start_mod_firstprimes[i];

		ps->lsb  = KAT_START_LSB;
		ps->mod6 = 5;

		return filter_primes;
	}

	return 0;
}


//--------------------------------------
static inline struct timespec clock_marked_now(void)
{
	struct timespec now;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);

	return now;
}


//--------------------------------------
// run one of the known type's known-answer test
// type restricted to SIMD_PRIMETYPE_SAFE or SIMD_PRIMETYPE_TWIN
//
// lsbs[candidates] set aside to store LS 64 bits of each filted candidate
// -> buffer may be scratch-used during our current test, not returning
//
// non-NULL hash MUST be >=64 bytes; stores SHA512(..BE64(LSBs)..) if non-NULL
//
// returns >0 if successful
//
static unsigned int
init0_simd_kat1(unsigned char *hash,
                     uint64_t *lsbs, unsigned int candidates, PrimeType_t type)
{
	unsigned char *be64s = (unsigned char *) lsbs;
	struct PP_Mod16bit ps = PP_MOD16BIT_INIT0;
	unsigned int filter_primes, res;
	struct timespec start, end;
	size_t hbytes;

	if (hash)
		memset(hash, 0, 64);

	if (!lsbs || !candidates)
		return 0;

	filter_primes = init0_kat_search_state(&ps, type);
	if (!filter_primes)
		return 0;

	start = clock_marked_now();

	if (type == SIMD_PRIMETYPE_TWIN) {
		res = twin_advance_w(lsbs, candidates, &ps, &ps);
	} else if (type == SIMD_PRIMETYPE_SAFE) {
		res = sfsieve_advance(lsbs, candidates, &ps, &ps);
	} else {
		return 0;
	}
	end = clock_marked_now();

	if (!res)
		return 0;

	printf("SIMD.SMALL_PRIMES=%u\n", filter_primes);
	printf("SIMD.CANDIDATES=%u\n", candidates);
	printf("SIMD.PRIME.TYPE=%s\n", report_prime_type(&ps));
	printf("SIMD.LSBS x%" PRIx64 ", x%" PRIx64 ", x%" PRIx64 ", x%" PRIx64
	        " .. x%" PRIx64 ", x%" PRIx64 "\n",
		lsbs[0], lsbs[1], lsbs[2], lsbs[3],
		lsbs[candidates-2], lsbs[candidates-1]);
	printf("SIMD.TIME.DIFF=%.1fms\n", cu_msdelta2(&start, &end));

	if (hash) {
		buffer2be64(be64s, lsbs, candidates);
		cu_hexprint("## TIME.BIN ", be64s, 4*8);
		cu_hexprint("## ...      ", &(be64s[candidates*8 - 3*8]), 3*8);

		hbytes = hash_buffer(hash, 64, be64s, candidates*8);
		if (!hbytes)
			return 0;
		cu_hexprint("SIMD.HASH(LSBS) ", hash, hbytes);
	}

	printf("\n");
	fflush(stdout);

	return candidates;
}


//--------------------------------------
// find the first million safe-prime/twin-prime candidates with no factors of
// P/2P+1 (or P/P+2) in the first SIMDPRIME_COUNT entries of firstprimes[]
//
// collect LS 64 bits of each accepted candidate; concatenate as BE64
// entries; hash together
//
// lsbs[] initialized as u64[] returned from search
// we then revert entries in-place to force be64[], then hash together
// be64s is alias to allow in-place rewriting
//
static int safe_n_twinprime_kat(void)
{
	unsigned int candidates = 1000000;
	unsigned char hash[ 64 ];
	uint64_t *lsbs = NULL;
	int rc = -1;

	do {
	lsbs  = calloc(candidates, 8);
	if (!lsbs)
		break;

	if (!init0_simd_kat1(hash, lsbs, candidates, SIMD_PRIMETYPE_SAFE))
		break;

	if (!init0_simd_kat1(hash, lsbs, candidates, SIMD_PRIMETYPE_TWIN))
		break;

	rc = 0;
	} while (0);

	free(lsbs);

	return rc;
}


//--------------------------------------
int main(int argc, const char **argv)
{
	struct PP_Mod16bit ps = PP_MOD16BIT_INIT0;
	unsigned long i, pcount;
	uint64_t rc;

	--argc;
	++argv;

	{
	int k = safe_n_twinprime_kat();
	if (k <0)
		return cu_reportrc("twin/safe-prime search failed", -1);
	return 0;
	}

	if (argc < 1)
		return cu_reportrc("missing starting value", -1);

				// start prime always present
				// argv[1] is increment, if present
				//
	if (mod16read(&ps, argv[0], (argc > 1) ? argv[1] : NULL) <0)
		return -1;
	printf("## P0=%s\n", argv[0]);

	if ((argc > 1) && searchtype_uses_increment(&ps))
		printf("## INCR=%s\n", argv[1]);

	pcount = SF_TEST_UNITS;

	set_default_table_size(&ps);
	if (override_table_size(&ps, getenv("PRIMES")) <0)
		return cu_reportrc("invalid prime-count specified", -1);

	printf("## PRIMES=%u\n", report_table_prime_count(&ps));
	printf("## TYPE=%s\n",   report_prime_type(&ps));

	{
	struct timespec start, end;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

	if (getenv("TWIN") || (SIMD_PRIMETYPE_TWIN & ps.mode)) {
		rc = twin_advance_w(possible, pcount, &ps, &ps);

	} else if (getenv("PLAIN") || (SIMD_PRIMETYPE_PLAIN & ps.mode)) {
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
		for (i=0; i<60; ++i)
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
			//
	if (possible[0] < possible[ pcount-1 ]) {
		rc = possible[ pcount-1 ] - possible[0];

						// min(diff) == pcount*6
		printf("## SKIPPED.RATIO=1:%.2f\n",
		       ((float) rc) / (pcount *6));

		printf("## SKIPPED.PPM=%" PRIu64 "\n",
		       UINT64_C(1000000) - (6000000 * pcount / rc));
	}

//	hash_buffer(possible, pcount);

	printf("\n");

	return 0;
}
#endif   //=====  /delimiter: standalone test code  ==========================

