/* generated code follows, please do not modify it */

/*
 * build-time controls
 *   define RBSS_USE_NO_128BIT
 *                 prevent use of 128-bit types even if supported by compiler
 *                 gcc/icc/clang-compatible support is autodetected.
 *
 *   define RBSS_NO_NULL_CHECKS
 *                 remove (redundant) NULL checks from static-inline 'macros'
 *                 note: expect those checks to be specialized away by
 *                 by any reasonable compiler; useful if the functions
 *                 are removed and use separately.
 */

#if 1  /*---  delimiter; nothing user-serviceable below  ------------------*/
#include <stdint.h>

/*
 * note that this file uses identifiers containing double-underscores
 * (but not starting with them). Such identifiers are reserved in C++
 * (ISO/IEC 14882:2011, 17.6.4.3.2, Global names), but not in C (ISO/IEC
 * 9899:2011, 7.1.3, Reserved identifiers). You MAY need to convince
 * your compiler to prevent compilation as C++ if the __cplusplus
 * countermeasure does not work below.
 */

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(RBSS__LIKELY)
#if (__GNUC__ >= 3)
#define RBSS__LIKELY(cond) (__builtin_expect (!!(cond), 1))
#endif
#endif
/**/
#if !defined(RBSS__LIKELY)
#define RBSS__LIKELY(cond)  (cond)
#endif

/* 128-bit u/int support, conditional, gcc/clang */
#undef RBSS__USE_128BIT

#if !defined(RBSS_USE_NO_128BIT)
#if defined(__SIZEOF_INT128__)
typedef unsigned uint128_t __attribute__((mode(TI))) ;
#define RBSS__USE_128BIT 128
#endif
#endif    /* !defined(RBSS_USE_NO_128BIT) */

#if defined(__GNUC__)
#define RBSS__PURE_FN __attribute__ ((pure))
	/* not bothering with version checking, please do not comment on it */
#else
#define RBSS__PURE_FN /* pure function, no marker */
#endif

static inline void bn_64to61rx_343(uint64_t r[6], const uint64_t a[6])
{
        r[0] =       UINT64_C(0x3fffffffff) & ((a[1] >> 49) | (a[0] << 15));
        r[1] = UINT64_C(0x1fffffffffffffff) & ((a[2] >> 52) | (a[1] << 12));
        r[2] = UINT64_C(0x1fffffffffffffff) & ((a[3] >> 55) | (a[2] <<  9));
        r[3] = UINT64_C(0x1fffffffffffffff) & ((a[4] >> 58) | (a[3] <<  6));
        r[4] = UINT64_C(0x1fffffffffffffff) & ((a[5] >> 61) | (a[4] <<  3));
        r[5] = UINT64_C(0x1fffffffffffffff) &   a[5];
}


//--------------------------------------
static inline void bn_64to61rx_513(uint64_t r[9], const uint64_t a[8])
{
	r[8] = UINT64_C(0x1fffffffffffffff) &   a[7];
	r[7] = UINT64_C(0x1fffffffffffffff) & ((a[7] >> 61) | (a[6] <<  3));
	r[6] = UINT64_C(0x1fffffffffffffff) & ((a[6] >> 58) | (a[5] <<  6));
	r[5] = UINT64_C(0x1fffffffffffffff) & ((a[5] >> 55) | (a[4] <<  9));
	r[4] = UINT64_C(0x1fffffffffffffff) & ((a[4] >> 52) | (a[3] << 12));
	r[3] = UINT64_C(0x1fffffffffffffff) & ((a[3] >> 49) | (a[2] << 15));
	r[2] = UINT64_C(0x1fffffffffffffff) & ((a[2] >> 46) | (a[1] << 18));
	r[1] = UINT64_C(0x1fffffffffffffff) & ((a[1] >> 43) | (a[0] << 21));
	r[0] =          UINT32_C(0x1ffffff) &  (a[0] >> 40) ;
}


//--------------------------------------
static inline void bn_64to61rx_684(uint64_t r[12], const uint64_t a[11])
{
	r[11] = UINT64_C(0x1fffffffffffffff) &   a[10];
	r[10] = UINT64_C(0x1fffffffffffffff) & ((a[10] >> 61) | (a[ 9] <<  3));
	r[ 9] = UINT64_C(0x1fffffffffffffff) & ((a[ 9] >> 58) | (a[ 8] <<  6));
	r[ 8] = UINT64_C(0x1fffffffffffffff) & ((a[ 8] >> 55) | (a[ 7] <<  9));
	r[ 7] = UINT64_C(0x1fffffffffffffff) & ((a[ 7] >> 52) | (a[ 6] << 12));
	r[ 6] = UINT64_C(0x1fffffffffffffff) & ((a[ 6] >> 49) | (a[ 5] << 15));
	r[ 5] = UINT64_C(0x1fffffffffffffff) & ((a[ 5] >> 46) | (a[ 4] << 18));
	r[ 4] = UINT64_C(0x1fffffffffffffff) & ((a[ 4] >> 43) | (a[ 3] << 21));
	r[ 3] = UINT64_C(0x1fffffffffffffff) & ((a[ 3] >> 40) | (a[ 2] << 24));
	r[ 2] = UINT64_C(0x1fffffffffffffff) & ((a[ 2] >> 37) | (a[ 1] << 27));
	r[ 1] = UINT64_C(0x1fffffffffffffff) & ((a[ 1] >> 34) | (a[ 0] << 30));
	r[ 0] =             UINT32_C(0x1fff) &  (a[ 0] >> 31);
}


//--------------------------------------
// r[] and a[] may be same pointer
// other overlap is undefined
static inline void bn_64to61rx_1025(uint64_t r[17], const uint64_t a[16])
{
	r[16] = UINT64_C(0x1fffffffffffffff) &   a[15];
	r[15] = UINT64_C(0x1fffffffffffffff) & ((a[15] >> 61) | (a[14] <<  3));
	r[14] = UINT64_C(0x1fffffffffffffff) & ((a[14] >> 58) | (a[13] <<  6));
	r[13] = UINT64_C(0x1fffffffffffffff) & ((a[13] >> 55) | (a[12] <<  9));
	r[12] = UINT64_C(0x1fffffffffffffff) & ((a[12] >> 52) | (a[11] << 12));
	r[11] = UINT64_C(0x1fffffffffffffff) & ((a[11] >> 49) | (a[10] << 15));
	r[10] = UINT64_C(0x1fffffffffffffff) & ((a[10] >> 46) | (a[ 9] << 18));
	r[ 9] = UINT64_C(0x1fffffffffffffff) & ((a[ 9] >> 43) | (a[ 8] << 21));
	r[ 8] = UINT64_C(0x1fffffffffffffff) & ((a[ 8] >> 40) | (a[ 7] << 24));
	r[ 7] = UINT64_C(0x1fffffffffffffff) & ((a[ 7] >> 37) | (a[ 6] << 27));
	r[ 6] = UINT64_C(0x1fffffffffffffff) & ((a[ 6] >> 34) | (a[ 5] << 30));
	r[ 5] = UINT64_C(0x1fffffffffffffff) & ((a[ 5] >> 31) | (a[ 4] << 33));
	r[ 4] = UINT64_C(0x1fffffffffffffff) & ((a[ 4] >> 28) | (a[ 3] << 36));
	r[ 3] = UINT64_C(0x1fffffffffffffff) & ((a[ 3] >> 25) | (a[ 2] << 39));
	r[ 2] = UINT64_C(0x1fffffffffffffff) & ((a[ 2] >> 22) | (a[ 1] << 42));
	r[ 1] = UINT64_C(0x1fffffffffffffff) & ((a[ 1] >> 19) | (a[ 0] << 45));
	r[ 0] =    UINT64_C(0x1ffffffffffff) &  (a[ 0] >> 16);
}


//--------------------------------------
// r[] and a[] may be same pointer
// other overlap is undefined
static inline void bn_64to61rx_1367(uint64_t r[23], const uint64_t a[22])
{
	r[22] = UINT64_C(0x1fffffffffffffff) &   a[21];
	r[21] = UINT64_C(0x1fffffffffffffff) & ((a[20] <<  3) | (a[21] >> 61));
	r[20] = UINT64_C(0x1fffffffffffffff) & ((a[19] <<  6) | (a[20] >> 58));
	r[19] = UINT64_C(0x1fffffffffffffff) & ((a[18] <<  9) | (a[19] >> 55));
	r[18] = UINT64_C(0x1fffffffffffffff) & ((a[17] << 12) | (a[18] >> 52));
	r[17] = UINT64_C(0x1fffffffffffffff) & ((a[16] << 15) | (a[17] >> 49));
	r[16] = UINT64_C(0x1fffffffffffffff) & ((a[15] << 18) | (a[16] >> 46));
	r[15] = UINT64_C(0x1fffffffffffffff) & ((a[14] << 21) | (a[15] >> 43));
	r[14] = UINT64_C(0x1fffffffffffffff) & ((a[13] << 24) | (a[14] >> 40));
	r[13] = UINT64_C(0x1fffffffffffffff) & ((a[12] << 27) | (a[13] >> 37));
	r[12] = UINT64_C(0x1fffffffffffffff) & ((a[11] << 30) | (a[12] >> 34));
	r[11] = UINT64_C(0x1fffffffffffffff) & ((a[10] << 33) | (a[11] >> 31));
	r[10] = UINT64_C(0x1fffffffffffffff) & ((a[ 9] << 36) | (a[10] >> 28));
	r[ 9] = UINT64_C(0x1fffffffffffffff) & ((a[ 8] << 39) | (a[ 9] >> 25));
	r[ 8] = UINT64_C(0x1fffffffffffffff) & ((a[ 7] << 42) | (a[ 8] >> 22));
	r[ 7] = UINT64_C(0x1fffffffffffffff) & ((a[ 6] << 45) | (a[ 7] >> 19));
	r[ 6] = UINT64_C(0x1fffffffffffffff) & ((a[ 5] << 48) | (a[ 6] >> 16));
	r[ 5] = UINT64_C(0x1fffffffffffffff) & ((a[ 4] << 51) | (a[ 5] >> 13));
	r[ 4] = UINT64_C(0x1fffffffffffffff) & ((a[ 3] << 54) | (a[ 4] >> 10));
	r[ 3] = UINT64_C(0x1fffffffffffffff) & ((a[ 2] << 57) | (a[ 3] >>  7));
	r[ 2] = UINT64_C(0x1fffffffffffffff) & ((a[ 1] << 60) | (a[ 2] >>  4));

// note processing order! [0] MUST be updated before [1], in case of overlap

	r[ 0] =          UINT64_C(0x1ffffff) & ((a[ 0] <<  2) | (a[ 1] >> 62));
	r[ 1] = UINT64_C(0x1fffffffffffffff) &  (a[ 1] >>  1);
}


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_384(uint64_t r[8], const uint64_t a[6])
{
	r[7] = UINT64_C(0xfffffffffffff) &                  a[5];
	r[6] = UINT64_C(0xfffffffffffff) & ((a[4] << 12) | (a[5] >> 52));
	r[5] = UINT64_C(0xfffffffffffff) & ((a[3] << 24) | (a[4] >> 40));
	r[4] = UINT64_C(0xfffffffffffff) & ((a[2] << 36) | (a[3] >> 28));
	r[3] = UINT64_C(0xfffffffffffff) & ((a[1] << 48) | (a[2] >> 16));
	r[2] = UINT64_C(0xfffffffffffff) &                 (a[1] >>  4);
	r[1] = UINT64_C(0xfffffffffffff) & ((a[0] <<  8) | (a[1] >> 56));
	r[0] =         UINT64_C(0xfffff) &                 (a[0] >> 44);
}


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_448(uint64_t r[9], const uint64_t a[7])
{
	r[8] = UINT64_C(0xfffffffffffff) &                   a[6];
	r[7] = UINT64_C(0xfffffffffffff) &  ((a[5] << 12) | (a[6] >> 52));
	r[6] = UINT64_C(0xfffffffffffff) &  ((a[4] << 24) | (a[5] >> 40));
	r[5] = UINT64_C(0xfffffffffffff) &  ((a[3] << 36) | (a[4] >> 28));
	r[4] = UINT64_C(0xfffffffffffff) &  ((a[2] << 48) | (a[3] >> 16));
	r[3] = UINT64_C(0xfffffffffffff) &                  (a[2] >>  4);
	r[2] = UINT64_C(0xfffffffffffff) &  ((a[1] <<  8) | (a[2] >> 56));
	r[1] = UINT64_C(0xfffffffffffff) &  ((a[0] << 20) | (a[1] >> 44));
	r[0] =                                               a[0] >> 32;
}


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_506(uint64_t r[10], const uint64_t a[8])
{
	r[9] = UINT64_C(0xfffffffffffff) &                  a[7];
	r[8] = UINT64_C(0xfffffffffffff) & ((a[6] << 12) | (a[7] >> 52));
	r[7] = UINT64_C(0xfffffffffffff) & ((a[5] << 24) | (a[6] >> 40));
	r[6] = UINT64_C(0xfffffffffffff) & ((a[4] << 36) | (a[5] >> 28));
	r[5] = UINT64_C(0xfffffffffffff) & ((a[3] << 48) | (a[4] >> 16));
	r[4] = UINT64_C(0xfffffffffffff) &                 (a[3] >>  4);
	r[3] = UINT64_C(0xfffffffffffff) & ((a[2] <<  8) | (a[3] >> 56));
	r[2] = UINT64_C(0xfffffffffffff) & ((a[1] << 20) | (a[2] >> 44));
	r[1] = UINT64_C(0xfffffffffffff) & ((a[0] << 32) | (a[1] >> 32));
	r[0] =    UINT64_C(0x3fffffffff) &                 (a[0] >> 20);
}


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_704(uint64_t r[14], const uint64_t a[11])
{
	r[13] = UINT64_C(0xfffffffffffff) &   a[10];
	r[12] = UINT64_C(0xfffffffffffff) & ((a[10] >> 52) | (a[9] << 12));
	r[11] = UINT64_C(0xfffffffffffff) & ((a[ 9] >> 40) | (a[8] << 24));
	r[10] = UINT64_C(0xfffffffffffff) & ((a[ 8] >> 28) | (a[7] << 36));
	r[ 9] = UINT64_C(0xfffffffffffff) & ((a[ 7] >> 16) | (a[6] << 48));
	r[ 8] = UINT64_C(0xfffffffffffff) &  (a[ 6] >>  4);
	r[ 7] = UINT64_C(0xfffffffffffff) & ((a[ 6] >> 56) | (a[5] <<  8));
	r[ 6] = UINT64_C(0xfffffffffffff) & ((a[ 5] >> 44) | (a[4] << 20));
	r[ 5] = UINT64_C(0xfffffffffffff) & ((a[ 4] >> 32) | (a[3] << 32));
	r[ 4] = UINT64_C(0xfffffffffffff) & ((a[ 3] >> 20) | (a[2] << 44));
	r[ 3] = UINT64_C(0xfffffffffffff) &  (a[ 2] >>  8);
	r[ 2] = UINT64_C(0xfffffffffffff) & ((a[ 2] >> 60) | (a[1] <<  4));
	r[ 1] = UINT64_C(0xfffffffffffff) & ((a[ 1] >> 48) | (a[0] << 16));
	r[ 0] =       UINT32_C(0xfffffff) &  (a[ 0] >> 36);
}


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_1024(uint64_t r[20], const uint64_t a[16])
{
	r[19] = UINT64_C(0xfffffffffffff) &                   a[15];
	r[18] = UINT64_C(0xfffffffffffff) & ((a[14] << 12) | (a[15] >> 52));
	r[17] = UINT64_C(0xfffffffffffff) & ((a[13] << 24) | (a[14] >> 40));
	r[16] = UINT64_C(0xfffffffffffff) & ((a[12] << 36) | (a[13] >> 28));
	r[15] = UINT64_C(0xfffffffffffff) & ((a[11] << 48) | (a[12] >> 16));
	r[14] = UINT64_C(0xfffffffffffff) &                  (a[11] >>  4);
	r[13] = UINT64_C(0xfffffffffffff) & ((a[10] <<  8) | (a[11] >> 56));
	r[12] = UINT64_C(0xfffffffffffff) & ((a[ 9] << 20) | (a[10] >> 44));
	r[11] = UINT64_C(0xfffffffffffff) & ((a[ 8] << 32) | (a[ 9] >> 32));
	r[10] = UINT64_C(0xfffffffffffff) & ((a[ 7] << 44) | (a[ 8] >> 20));
	r[ 9] = UINT64_C(0xfffffffffffff) &                  (a[ 7] >>  8);
	r[ 8] = UINT64_C(0xfffffffffffff) & ((a[ 6] <<  4) | (a[ 7] >> 60));
	r[ 7] = UINT64_C(0xfffffffffffff) & ((a[ 5] << 16) | (a[ 6] >> 48));
	r[ 6] = UINT64_C(0xfffffffffffff) & ((a[ 4] << 28) | (a[ 5] >> 36));
	r[ 5] = UINT64_C(0xfffffffffffff) & ((a[ 3] << 40) | (a[ 4] >> 24));
	r[ 4] = UINT64_C(0xfffffffffffff) &                  (a[ 3] >> 12);
	r[ 3] = UINT64_C(0xfffffffffffff) &                   a[ 2];
	r[ 2] = UINT64_C(0xfffffffffffff) & ((a[ 1] << 12) | (a[ 2] >> 52));
	r[ 1] = UINT64_C(0xfffffffffffff) & ((a[ 0] << 24) | (a[ 1] >> 40));
	r[ 0] =                                               a[ 0] >> 28;
}


#if 0
/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_1028(uint64_t r[20], const uint64_t a[17])
{
	r[19] = UINT64_C(0xfffffffffffff) &                   a[16];
	r[18] = UINT64_C(0xfffffffffffff) & ((a[15] << 12) | (a[16] >> 52));
	r[17] = UINT64_C(0xfffffffffffff) & ((a[14] << 24) | (a[15] >> 40));
	r[16] = UINT64_C(0xfffffffffffff) & ((a[13] << 36) | (a[14] >> 28));
	r[15] = UINT64_C(0xfffffffffffff) & ((a[12] << 48) | (a[13] >> 16));
	r[14] = UINT64_C(0xfffffffffffff) &                  (a[12] >>  4);
	r[13] = UINT64_C(0xfffffffffffff) & ((a[11] <<  8) | (a[12] >> 56));
	r[12] = UINT64_C(0xfffffffffffff) & ((a[10] << 20) | (a[11] >> 44));
	r[11] = UINT64_C(0xfffffffffffff) & ((a[ 9] << 32) | (a[10] >> 32));
	r[10] = UINT64_C(0xfffffffffffff) & ((a[ 8] << 44) | (a[ 9] >> 20));
	r[ 9] = UINT64_C(0xfffffffffffff) &                  (a[ 8] >>  8);
	r[ 8] = UINT64_C(0xfffffffffffff) & ((a[ 7] <<  4) | (a[ 8] >> 60));
	r[ 7] = UINT64_C(0xfffffffffffff) & ((a[ 6] << 16) | (a[ 7] >> 48));
	r[ 6] = UINT64_C(0xfffffffffffff) & ((a[ 5] << 28) | (a[ 6] >> 36));
	r[ 5] = UINT64_C(0xfffffffffffff) & ((a[ 4] << 40) | (a[ 5] >> 24));
	r[ 4] = UINT64_C(0xfffffffffffff) &                  (a[ 4] >> 12);
	r[ 3] = UINT64_C(0xfffffffffffff) &                   a[ 3];
	r[ 2] = UINT64_C(0xfffffffffffff) & ((a[ 2] << 12) | (a[ 3] >> 52));
	r[ 1] = UINT64_C(0xfffffffffffff) & ((a[ 1] << 24) | (a[ 2] >> 40));
	r[ 0] =    UINT64_C(0xffffffffff) & ((a[ 0] << 36) | (a[ 1] >> 28));
}
#endif


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_1088(uint64_t r[21], const uint64_t a[17])
{
	r[20] = UINT64_C(0xfffffffffffff) &                   a[16];
	r[19] = UINT64_C(0xfffffffffffff) & ((a[15] << 12) | (a[16] >> 52));
	r[18] = UINT64_C(0xfffffffffffff) & ((a[14] << 24) | (a[15] >> 40));
	r[17] = UINT64_C(0xfffffffffffff) & ((a[13] << 36) | (a[14] >> 28));
	r[16] = UINT64_C(0xfffffffffffff) & ((a[12] << 48) | (a[13] >> 16));
	r[15] = UINT64_C(0xfffffffffffff) &                  (a[12] >>  4);
	r[14] = UINT64_C(0xfffffffffffff) & ((a[11] <<  8) | (a[12] >> 56));
	r[13] = UINT64_C(0xfffffffffffff) & ((a[10] << 20) | (a[11] >> 44));
	r[12] = UINT64_C(0xfffffffffffff) & ((a[ 9] << 32) | (a[10] >> 32));
	r[11] = UINT64_C(0xfffffffffffff) & ((a[ 8] << 44) | (a[ 9] >> 20));
	r[10] = UINT64_C(0xfffffffffffff) &                  (a[ 8] >>  8);
	r[ 9] = UINT64_C(0xfffffffffffff) & ((a[ 7] <<  4) | (a[ 8] >> 60));
	r[ 8] = UINT64_C(0xfffffffffffff) & ((a[ 6] << 16) | (a[ 7] >> 48));
	r[ 7] = UINT64_C(0xfffffffffffff) & ((a[ 5] << 28) | (a[ 6] >> 36));
	r[ 6] = UINT64_C(0xfffffffffffff) & ((a[ 4] << 40) | (a[ 5] >> 24));
	r[ 5] = UINT64_C(0xfffffffffffff) &                  (a[ 4] >> 12);
	r[ 4] = UINT64_C(0xfffffffffffff) &                   a[ 3];
	r[ 3] = UINT64_C(0xfffffffffffff) & ((a[ 2] << 12) | (a[ 3] >> 52));
	r[ 2] = UINT64_C(0xfffffffffffff) & ((a[ 1] << 24) | (a[ 2] >> 40));
	r[ 1] = UINT64_C(0xfffffffffffff) & ((a[ 0] << 36) | (a[ 1] >> 28));
	r[ 0] =         UINT64_C(0x7ffff) &                  (a[ 0] >> 16);
}


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_1361(uint64_t r[27], const uint64_t a[22])
{
	r[26] = UINT64_C(0xfffffffffffff) &                   a[21];
	r[25] = UINT64_C(0xfffffffffffff) & ((a[20] << 12) | (a[21] >> 52));
	r[24] = UINT64_C(0xfffffffffffff) & ((a[19] << 24) | (a[20] >> 40));
	r[23] = UINT64_C(0xfffffffffffff) & ((a[18] << 36) | (a[19] >> 28));
	r[22] = UINT64_C(0xfffffffffffff) & ((a[17] << 48) | (a[18] >> 16));
	r[21] = UINT64_C(0xfffffffffffff) &                  (a[17] >>  4);
	r[20] = UINT64_C(0xfffffffffffff) & ((a[16] <<  8) | (a[17] >> 56));
	r[19] = UINT64_C(0xfffffffffffff) & ((a[15] << 20) | (a[16] >> 44));
	r[18] = UINT64_C(0xfffffffffffff) & ((a[14] << 32) | (a[15] >> 32));
	r[17] = UINT64_C(0xfffffffffffff) & ((a[13] << 44) | (a[14] >> 20));
	r[16] = UINT64_C(0xfffffffffffff) &                  (a[13] >>  8);
	r[15] = UINT64_C(0xfffffffffffff) & ((a[12] <<  4) | (a[13] >> 60));
	r[14] = UINT64_C(0xfffffffffffff) & ((a[11] << 16) | (a[12] >> 48));
	r[13] = UINT64_C(0xfffffffffffff) & ((a[10] << 28) | (a[11] >> 36));
	r[12] = UINT64_C(0xfffffffffffff) & ((a[ 9] << 40) | (a[10] >> 24));
	r[11] = UINT64_C(0xfffffffffffff) &                  (a[ 9] >> 12);
	r[10] = UINT64_C(0xfffffffffffff) &                   a[ 8];
	r[ 9] = UINT64_C(0xfffffffffffff) & ((a[ 7] << 12) | (a[ 8] >> 52));
	r[ 8] = UINT64_C(0xfffffffffffff) & ((a[ 6] << 24) | (a[ 7] >> 40));
	r[ 7] = UINT64_C(0xfffffffffffff) & ((a[ 5] << 36) | (a[ 6] >> 28));
	r[ 6] = UINT64_C(0xfffffffffffff) & ((a[ 4] << 48) | (a[ 5] >> 16));
	r[ 5] = UINT64_C(0xfffffffffffff) &                  (a[ 4] >>  4);
	r[ 4] = UINT64_C(0xfffffffffffff) & ((a[ 3] <<  8) | (a[ 4] >> 56));
	r[ 3] = UINT64_C(0xfffffffffffff) & ((a[ 2] << 20) | (a[ 3] >> 44));
	r[ 2] = UINT64_C(0xfffffffffffff) & ((a[ 1] << 32) | (a[ 2] >> 32));
	r[ 1] = UINT64_C(0xfffffffffffff) & ((a[ 0] << 44) | (a[ 1] >> 20));
	r[ 0] =           UINT64_C(0x1ff) &                  (a[ 0] >>  8);
}


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_1536(uint64_t r[30], const uint64_t a[24])
{
	r[29] = UINT64_C(0xfffffffffffff) &                   a[23];
	r[28] = UINT64_C(0xfffffffffffff) & ((a[22] << 12) | (a[23] >> 52));
	r[27] = UINT64_C(0xfffffffffffff) & ((a[21] << 24) | (a[22] >> 40));
	r[26] = UINT64_C(0xfffffffffffff) & ((a[20] << 36) | (a[21] >> 28));
	r[25] = UINT64_C(0xfffffffffffff) & ((a[19] << 48) | (a[20] >> 16));
	r[24] = UINT64_C(0xfffffffffffff) &                  (a[19] >>  4);
	r[23] = UINT64_C(0xfffffffffffff) & ((a[18] <<  8) | (a[19] >> 56));
	r[22] = UINT64_C(0xfffffffffffff) & ((a[17] << 20) | (a[18] >> 44));
	r[21] = UINT64_C(0xfffffffffffff) & ((a[16] << 32) | (a[17] >> 32));
	r[20] = UINT64_C(0xfffffffffffff) & ((a[15] << 44) | (a[16] >> 20));
	r[19] = UINT64_C(0xfffffffffffff) &                  (a[15] >>  8);
	r[18] = UINT64_C(0xfffffffffffff) & ((a[14] <<  4) | (a[15] >> 60));
	r[17] = UINT64_C(0xfffffffffffff) & ((a[13] << 16) | (a[14] >> 48));
	r[16] = UINT64_C(0xfffffffffffff) & ((a[12] << 28) | (a[13] >> 36));
	r[15] = UINT64_C(0xfffffffffffff) & ((a[11] << 40) | (a[12] >> 24));
	r[14] = UINT64_C(0xfffffffffffff) &                  (a[11] >> 12);
	r[13] = UINT64_C(0xfffffffffffff) &                   a[10];
	r[12] = UINT64_C(0xfffffffffffff) & ((a[ 9] << 12) | (a[10] >> 52));
	r[11] = UINT64_C(0xfffffffffffff) & ((a[ 8] << 24) | (a[ 9] >> 40));
	r[10] = UINT64_C(0xfffffffffffff) & ((a[ 7] << 36) | (a[ 8] >> 28));
	r[ 9] = UINT64_C(0xfffffffffffff) & ((a[ 6] << 48) | (a[ 7] >> 16));
	r[ 8] = UINT64_C(0xfffffffffffff) &                  (a[ 6] >>  4);
	r[ 7] = UINT64_C(0xfffffffffffff) & ((a[ 5] <<  8) | (a[ 6] >> 56));
	r[ 6] = UINT64_C(0xfffffffffffff) & ((a[ 4] << 20) | (a[ 5] >> 44));
	r[ 5] = UINT64_C(0xfffffffffffff) & ((a[ 3] << 32) | (a[ 4] >> 32));
	r[ 4] = UINT64_C(0xfffffffffffff) & ((a[ 2] << 44) | (a[ 3] >> 20));
	r[ 3] = UINT64_C(0xfffffffffffff) &                  (a[ 2] >>  8);
	r[ 2] = UINT64_C(0xfffffffffffff) & ((a[ 1] <<  4) | (a[ 2] >> 60));
	r[ 1] = UINT64_C(0xfffffffffffff) & ((a[ 0] << 16) | (a[ 1] >> 48));
	r[ 0] =                                               a[ 0] >> 36;
}


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_2048(uint64_t r[40], const uint64_t a[32])
{
	r[39] = UINT64_C(0xfffffffffffff) &                   a[31];
	r[38] = UINT64_C(0xfffffffffffff) & ((a[30] << 12) | (a[31] >> 52));
	r[37] = UINT64_C(0xfffffffffffff) & ((a[29] << 24) | (a[30] >> 40));
	r[36] = UINT64_C(0xfffffffffffff) & ((a[28] << 36) | (a[29] >> 28));
	r[35] = UINT64_C(0xfffffffffffff) & ((a[27] << 48) | (a[28] >> 16));
	r[34] = UINT64_C(0xfffffffffffff) &                  (a[27] >>  4);
	r[33] = UINT64_C(0xfffffffffffff) & ((a[26] <<  8) | (a[27] >> 56));
	r[32] = UINT64_C(0xfffffffffffff) & ((a[25] << 20) | (a[26] >> 44));
	r[31] = UINT64_C(0xfffffffffffff) & ((a[24] << 32) | (a[25] >> 32));
	r[30] = UINT64_C(0xfffffffffffff) & ((a[23] << 44) | (a[24] >> 20));
	r[29] = UINT64_C(0xfffffffffffff) &                  (a[23] >>  8);
	r[28] = UINT64_C(0xfffffffffffff) & ((a[22] <<  4) | (a[23] >> 60));
	r[27] = UINT64_C(0xfffffffffffff) & ((a[21] << 16) | (a[22] >> 48));
	r[26] = UINT64_C(0xfffffffffffff) & ((a[20] << 28) | (a[21] >> 36));
	r[25] = UINT64_C(0xfffffffffffff) & ((a[19] << 40) | (a[20] >> 24));
	r[24] = UINT64_C(0xfffffffffffff) &                  (a[19] >> 12);
	r[23] = UINT64_C(0xfffffffffffff) &                   a[18];
	r[22] = UINT64_C(0xfffffffffffff) & ((a[17] << 12) | (a[18] >> 52));
	r[21] = UINT64_C(0xfffffffffffff) & ((a[16] << 24) | (a[17] >> 40));
	r[20] = UINT64_C(0xfffffffffffff) & ((a[15] << 36) | (a[16] >> 28));
	r[19] = UINT64_C(0xfffffffffffff) & ((a[14] << 48) | (a[15] >> 16));
	r[18] = UINT64_C(0xfffffffffffff) &                  (a[14] >>  4);
	r[17] = UINT64_C(0xfffffffffffff) & ((a[13] <<  8) | (a[14] >> 56));
	r[16] = UINT64_C(0xfffffffffffff) & ((a[12] << 20) | (a[13] >> 44));
	r[15] = UINT64_C(0xfffffffffffff) & ((a[11] << 32) | (a[12] >> 32));
	r[14] = UINT64_C(0xfffffffffffff) & ((a[10] << 44) | (a[11] >> 20));
	r[13] = UINT64_C(0xfffffffffffff) &                  (a[10] >>  8);
	r[12] = UINT64_C(0xfffffffffffff) & ((a[ 9] <<  4) | (a[10] >> 60));
	r[11] = UINT64_C(0xfffffffffffff) & ((a[ 8] << 16) | (a[ 9] >> 48));
	r[10] = UINT64_C(0xfffffffffffff) & ((a[ 7] << 28) | (a[ 8] >> 36));
	r[ 9] = UINT64_C(0xfffffffffffff) & ((a[ 6] << 40) | (a[ 7] >> 24));
	r[ 8] = UINT64_C(0xfffffffffffff) &                  (a[ 6] >> 12);
	r[ 7] = UINT64_C(0xfffffffffffff) &                   a[ 5];
	r[ 6] = UINT64_C(0xfffffffffffff) & ((a[ 4] << 12) | (a[ 5] >> 52));
	r[ 5] = UINT64_C(0xfffffffffffff) & ((a[ 3] << 24) | (a[ 4] >> 40));
	r[ 4] = UINT64_C(0xfffffffffffff) & ((a[ 2] << 36) | (a[ 3] >> 28));
	r[ 3] = UINT64_C(0xfffffffffffff) & ((a[ 1] << 48) | (a[ 2] >> 16));
	r[ 2] = UINT64_C(0xfffffffffffff) &                  (a[ 1] >>  4);
	r[ 1] = UINT64_C(0xfffffffffffff) & ((a[ 0] <<  8) | (a[ 1] >> 56));
	r[ 0] =                                               a[ 0] >> 44;
}


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_3072(uint64_t r[60], const uint64_t a[48])
{
	r[59] = UINT64_C(0xfffffffffffff) &                   a[47];
	r[58] = UINT64_C(0xfffffffffffff) & ((a[46] << 12) | (a[47] >> 52));
	r[57] = UINT64_C(0xfffffffffffff) & ((a[45] << 24) | (a[46] >> 40));
	r[56] = UINT64_C(0xfffffffffffff) & ((a[44] << 36) | (a[45] >> 28));
	r[55] = UINT64_C(0xfffffffffffff) & ((a[43] << 48) | (a[44] >> 16));
	r[54] = UINT64_C(0xfffffffffffff) &                  (a[43] >>  4);
	r[53] = UINT64_C(0xfffffffffffff) & ((a[42] <<  8) | (a[43] >> 56));
	r[52] = UINT64_C(0xfffffffffffff) & ((a[41] << 20) | (a[42] >> 44));
	r[51] = UINT64_C(0xfffffffffffff) & ((a[40] << 32) | (a[41] >> 32));
	r[50] = UINT64_C(0xfffffffffffff) & ((a[39] << 44) | (a[40] >> 20));
	r[49] = UINT64_C(0xfffffffffffff) &                  (a[39] >>  8);
	r[48] = UINT64_C(0xfffffffffffff) & ((a[38] <<  4) | (a[39] >> 60));
	r[47] = UINT64_C(0xfffffffffffff) & ((a[37] << 16) | (a[38] >> 48));
	r[46] = UINT64_C(0xfffffffffffff) & ((a[36] << 28) | (a[37] >> 36));
	r[45] = UINT64_C(0xfffffffffffff) & ((a[35] << 40) | (a[36] >> 24));
	r[44] = UINT64_C(0xfffffffffffff) &                  (a[35] >> 12);
	r[43] = UINT64_C(0xfffffffffffff) &                   a[34];
	r[42] = UINT64_C(0xfffffffffffff) & ((a[33] << 12) | (a[34] >> 52));
	r[41] = UINT64_C(0xfffffffffffff) & ((a[32] << 24) | (a[33] >> 40));
	r[40] = UINT64_C(0xfffffffffffff) & ((a[31] << 36) | (a[32] >> 28));
	r[39] = UINT64_C(0xfffffffffffff) & ((a[30] << 48) | (a[31] >> 16));
	r[38] = UINT64_C(0xfffffffffffff) &                  (a[30] >>  4);
	r[37] = UINT64_C(0xfffffffffffff) & ((a[29] <<  8) | (a[30] >> 56));
	r[36] = UINT64_C(0xfffffffffffff) & ((a[28] << 20) | (a[29] >> 44));
	r[35] = UINT64_C(0xfffffffffffff) & ((a[27] << 32) | (a[28] >> 32));
	r[34] = UINT64_C(0xfffffffffffff) & ((a[26] << 44) | (a[27] >> 20));
	r[33] = UINT64_C(0xfffffffffffff) &                  (a[26] >>  8);
	r[32] = UINT64_C(0xfffffffffffff) & ((a[25] <<  4) | (a[26] >> 60));
	r[31] = UINT64_C(0xfffffffffffff) & ((a[24] << 16) | (a[25] >> 48));
	r[30] = UINT64_C(0xfffffffffffff) & ((a[23] << 28) | (a[24] >> 36));
	r[29] = UINT64_C(0xfffffffffffff) & ((a[22] << 40) | (a[23] >> 24));
	r[28] = UINT64_C(0xfffffffffffff) &                  (a[22] >> 12);
	r[27] = UINT64_C(0xfffffffffffff) &                   a[21];
	r[26] = UINT64_C(0xfffffffffffff) & ((a[20] << 12) | (a[21] >> 52));
	r[25] = UINT64_C(0xfffffffffffff) & ((a[19] << 24) | (a[20] >> 40));
	r[24] = UINT64_C(0xfffffffffffff) & ((a[18] << 36) | (a[19] >> 28));
	r[23] = UINT64_C(0xfffffffffffff) & ((a[17] << 48) | (a[18] >> 16));
	r[22] = UINT64_C(0xfffffffffffff) &                  (a[17] >>  4);
	r[21] = UINT64_C(0xfffffffffffff) & ((a[16] <<  8) | (a[17] >> 56));
	r[20] = UINT64_C(0xfffffffffffff) & ((a[15] << 20) | (a[16] >> 44));
	r[19] = UINT64_C(0xfffffffffffff) & ((a[14] << 32) | (a[15] >> 32));
	r[18] = UINT64_C(0xfffffffffffff) & ((a[13] << 44) | (a[14] >> 20));
	r[17] = UINT64_C(0xfffffffffffff) &                  (a[13] >>  8);
	r[16] = UINT64_C(0xfffffffffffff) & ((a[12] <<  4) | (a[13] >> 60));
	r[15] = UINT64_C(0xfffffffffffff) & ((a[11] << 16) | (a[12] >> 48));
	r[14] = UINT64_C(0xfffffffffffff) & ((a[10] << 28) | (a[11] >> 36));
	r[13] = UINT64_C(0xfffffffffffff) & ((a[ 9] << 40) | (a[10] >> 24));
	r[12] = UINT64_C(0xfffffffffffff) &                  (a[ 9] >> 12);
	r[11] = UINT64_C(0xfffffffffffff) &                   a[ 8];
	r[10] = UINT64_C(0xfffffffffffff) & ((a[ 7] << 12) | (a[ 8] >> 52));
	r[ 9] = UINT64_C(0xfffffffffffff) & ((a[ 6] << 24) | (a[ 7] >> 40));
	r[ 8] = UINT64_C(0xfffffffffffff) & ((a[ 5] << 36) | (a[ 6] >> 28));
	r[ 7] = UINT64_C(0xfffffffffffff) & ((a[ 4] << 48) | (a[ 5] >> 16));
	r[ 6] = UINT64_C(0xfffffffffffff) &                  (a[ 4] >>  4);
	r[ 5] = UINT64_C(0xfffffffffffff) & ((a[ 3] <<  8) | (a[ 4] >> 56));
	r[ 4] = UINT64_C(0xfffffffffffff) & ((a[ 2] << 20) | (a[ 3] >> 44));
	r[ 3] = UINT64_C(0xfffffffffffff) & ((a[ 1] << 32) | (a[ 2] >> 32));
	r[ 2] = UINT64_C(0xfffffffffffff) & ((a[ 0] << 44) | (a[ 1] >> 20));
	r[ 1] = UINT64_C(0xfffffffffffff) &                  (a[ 0] >>  8);
	r[ 0] =                                              (a[ 0] >> 60);
}


/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_4096(uint64_t r[79], const uint64_t a[64])
{
	r[78] = UINT64_C(0xfffffffffffff) &                   a[63];
	r[77] = UINT64_C(0xfffffffffffff) & ((a[62] << 12) | (a[63] >> 52));
	r[76] = UINT64_C(0xfffffffffffff) & ((a[61] << 24) | (a[62] >> 40));
	r[75] = UINT64_C(0xfffffffffffff) & ((a[60] << 36) | (a[61] >> 28));
	r[74] = UINT64_C(0xfffffffffffff) & ((a[59] << 48) | (a[60] >> 16));
	r[73] = UINT64_C(0xfffffffffffff) &                  (a[59] >>  4);
	r[72] = UINT64_C(0xfffffffffffff) & ((a[58] <<  8) | (a[59] >> 56));
	r[71] = UINT64_C(0xfffffffffffff) & ((a[57] << 20) | (a[58] >> 44));
	r[70] = UINT64_C(0xfffffffffffff) & ((a[56] << 32) | (a[57] >> 32));
	r[69] = UINT64_C(0xfffffffffffff) & ((a[55] << 44) | (a[56] >> 20));
	r[68] = UINT64_C(0xfffffffffffff) &                  (a[55] >>  8);
	r[67] = UINT64_C(0xfffffffffffff) & ((a[54] <<  4) | (a[55] >> 60));
	r[66] = UINT64_C(0xfffffffffffff) & ((a[53] << 16) | (a[54] >> 48));
	r[65] = UINT64_C(0xfffffffffffff) & ((a[52] << 28) | (a[53] >> 36));
	r[64] = UINT64_C(0xfffffffffffff) & ((a[51] << 40) | (a[52] >> 24));
	r[63] = UINT64_C(0xfffffffffffff) &                  (a[51] >> 12);
	r[62] = UINT64_C(0xfffffffffffff) &                   a[50];
	r[61] = UINT64_C(0xfffffffffffff) & ((a[49] << 12) | (a[50] >> 52));
	r[60] = UINT64_C(0xfffffffffffff) & ((a[48] << 24) | (a[49] >> 40));
	r[59] = UINT64_C(0xfffffffffffff) & ((a[47] << 36) | (a[48] >> 28));
	r[58] = UINT64_C(0xfffffffffffff) & ((a[46] << 48) | (a[47] >> 16));
	r[57] = UINT64_C(0xfffffffffffff) &                  (a[46] >>  4);
	r[56] = UINT64_C(0xfffffffffffff) & ((a[45] <<  8) | (a[46] >> 56));
	r[55] = UINT64_C(0xfffffffffffff) & ((a[44] << 20) | (a[45] >> 44));
	r[54] = UINT64_C(0xfffffffffffff) & ((a[43] << 32) | (a[44] >> 32));
	r[53] = UINT64_C(0xfffffffffffff) & ((a[42] << 44) | (a[43] >> 20));
	r[52] = UINT64_C(0xfffffffffffff) &                  (a[42] >>  8);
	r[51] = UINT64_C(0xfffffffffffff) & ((a[41] <<  4) | (a[42] >> 60));
	r[50] = UINT64_C(0xfffffffffffff) & ((a[40] << 16) | (a[41] >> 48));
	r[49] = UINT64_C(0xfffffffffffff) & ((a[39] << 28) | (a[40] >> 36));
	r[48] = UINT64_C(0xfffffffffffff) & ((a[38] << 40) | (a[39] >> 24));
	r[47] = UINT64_C(0xfffffffffffff) &                  (a[38] >> 12);
	r[46] = UINT64_C(0xfffffffffffff) &                   a[37];
	r[45] = UINT64_C(0xfffffffffffff) & ((a[36] << 12) | (a[37] >> 52));
	r[44] = UINT64_C(0xfffffffffffff) & ((a[35] << 24) | (a[36] >> 40));
	r[43] = UINT64_C(0xfffffffffffff) & ((a[34] << 36) | (a[35] >> 28));
	r[42] = UINT64_C(0xfffffffffffff) & ((a[33] << 48) | (a[34] >> 16));
	r[41] = UINT64_C(0xfffffffffffff) &                  (a[33] >>  4);
	r[40] = UINT64_C(0xfffffffffffff) & ((a[32] <<  8) | (a[33] >> 56));
	r[39] = UINT64_C(0xfffffffffffff) & ((a[31] << 20) | (a[32] >> 44));
	r[38] = UINT64_C(0xfffffffffffff) & ((a[30] << 32) | (a[31] >> 32));
	r[37] = UINT64_C(0xfffffffffffff) & ((a[29] << 44) | (a[30] >> 20));
	r[36] = UINT64_C(0xfffffffffffff) &                  (a[29] >>  8);
	r[35] = UINT64_C(0xfffffffffffff) & ((a[28] <<  4) | (a[29] >> 60));
	r[34] = UINT64_C(0xfffffffffffff) & ((a[27] << 16) | (a[28] >> 48));
	r[33] = UINT64_C(0xfffffffffffff) & ((a[26] << 28) | (a[27] >> 36));
	r[32] = UINT64_C(0xfffffffffffff) & ((a[25] << 40) | (a[26] >> 24));
	r[31] = UINT64_C(0xfffffffffffff) &                  (a[25] >> 12);
	r[30] = UINT64_C(0xfffffffffffff) &                   a[24];
	r[29] = UINT64_C(0xfffffffffffff) & ((a[23] << 12) | (a[24] >> 52));
	r[28] = UINT64_C(0xfffffffffffff) & ((a[22] << 24) | (a[23] >> 40));
	r[27] = UINT64_C(0xfffffffffffff) & ((a[21] << 36) | (a[22] >> 28));
	r[26] = UINT64_C(0xfffffffffffff) & ((a[20] << 48) | (a[21] >> 16));
	r[25] = UINT64_C(0xfffffffffffff) &                  (a[20] >>  4);
	r[24] = UINT64_C(0xfffffffffffff) & ((a[19] <<  8) | (a[20] >> 56));
	r[23] = UINT64_C(0xfffffffffffff) & ((a[18] << 20) | (a[19] >> 44));
	r[22] = UINT64_C(0xfffffffffffff) & ((a[17] << 32) | (a[18] >> 32));
	r[21] = UINT64_C(0xfffffffffffff) & ((a[16] << 44) | (a[17] >> 20));
	r[20] = UINT64_C(0xfffffffffffff) &                  (a[16] >>  8);
	r[19] = UINT64_C(0xfffffffffffff) & ((a[15] <<  4) | (a[16] >> 60));
	r[18] = UINT64_C(0xfffffffffffff) & ((a[14] << 16) | (a[15] >> 48));
	r[17] = UINT64_C(0xfffffffffffff) & ((a[13] << 28) | (a[14] >> 36));
	r[16] = UINT64_C(0xfffffffffffff) & ((a[12] << 40) | (a[13] >> 24));
	r[15] = UINT64_C(0xfffffffffffff) &                  (a[12] >> 12);
	r[14] = UINT64_C(0xfffffffffffff) &                   a[11];
	r[13] = UINT64_C(0xfffffffffffff) & ((a[10] << 12) | (a[11] >> 52));
	r[12] = UINT64_C(0xfffffffffffff) & ((a[ 9] << 24) | (a[10] >> 40));
	r[11] = UINT64_C(0xfffffffffffff) & ((a[ 8] << 36) | (a[ 9] >> 28));
	r[10] = UINT64_C(0xfffffffffffff) & ((a[ 7] << 48) | (a[ 8] >> 16));
	r[ 9] = UINT64_C(0xfffffffffffff) &                  (a[ 7] >>  4);
	r[ 8] = UINT64_C(0xfffffffffffff) & ((a[ 6] <<  8) | (a[ 7] >> 56));
	r[ 7] = UINT64_C(0xfffffffffffff) & ((a[ 5] << 20) | (a[ 6] >> 44));
	r[ 6] = UINT64_C(0xfffffffffffff) & ((a[ 4] << 32) | (a[ 5] >> 32));
	r[ 5] = UINT64_C(0xfffffffffffff) & ((a[ 3] << 44) | (a[ 4] >> 20));
	r[ 4] = UINT64_C(0xfffffffffffff) &                  (a[ 3] >>  8);
	r[ 3] = UINT64_C(0xfffffffffffff) & ((a[ 2] <<  4) | (a[ 3] >> 60));
	r[ 2] = UINT64_C(0xfffffffffffff) & ((a[ 1] << 16) | (a[ 2] >> 48));
	r[ 1] = UINT64_C(0xfffffffffffff) & ((a[ 0] << 28) | (a[ 1] >> 36));
	r[ 0] =    UINT64_C(0xffffffffff) &                  (a[ 0] >> 24);
}


#if 0
/*--------------------------------------
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_64to52rx_454(uint64_t r[9], const uint64_t a[8])
{
	r[8] = UINT64_C(0xfffffffffffff) &                  a[7];
	r[7] = UINT64_C(0xfffffffffffff) & ((a[6] << 12) | (a[7] >> 52));
	r[6] = UINT64_C(0xfffffffffffff) & ((a[5] << 24) | (a[6] >> 40));
	r[5] = UINT64_C(0xfffffffffffff) & ((a[4] << 36) | (a[5] >> 28));
	r[4] = UINT64_C(0xfffffffffffff) & ((a[3] << 48) | (a[4] >> 16));

				// note: ordered to allow in-place update

	r[0] =    UINT64_C(0x3fffffffff) & ((a[0] << 32) | (a[1] >> 32));
	r[1] = UINT64_C(0xfffffffffffff) & ((a[1] << 20) | (a[2] >> 44));
	r[2] = UINT64_C(0xfffffffffffff) & ((a[2] <<  8) | (a[3] >> 56));
	r[3] = UINT64_C(0xfffffffffffff) &                 (a[3] >>  4);
}
#endif


/*--------------------------------------
 * r[0] always 0 (TODO: actual limit? see bitcounts)
 *
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_rx52to64_8(uint64_t r[8], const uint64_t a[8])
{
	r[7] =                (a[6] << 52) +  a[7];
	r[6] =                (a[5] << 40) + (a[6] >> 12);
	r[5] =                (a[4] << 28) + (a[5] >> 24);
	r[4] =                (a[3] << 16) + (a[4] >> 36);
	r[3] = (a[1] << 56) + (a[2] <<  4) + (a[3] >> 48);
	r[2] =                (a[0] << 44) + (a[1] >>  8);
	r[1] =                                a[0] >> 20;
	r[0] =                                0;
}


/*--------------------------------------
 * r[0] always 0 (TODO: actual limit? see bitcounts)
 *
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_rx52to64_9(uint64_t r[9], const uint64_t a[9])
{
	r[8] = (a[7] << 52) +  a[8];
	r[7] = (a[6] << 40) + (a[7] >> 12);
	r[6] = (a[5] << 28) + (a[6] >> 24);
	r[5] = (a[4] << 16) + (a[5] >> 36);
	r[4] = (a[2] << 56) + (a[3] <<  4) + (a[4] >> 48);
	r[3] = (a[1] << 44) + (a[2] >>  8);
	r[2] = (a[0] << 32) + (a[1] >> 20);
	r[1] =                 a[0] >> 32;
	r[0] =                 0;
}


/*--------------------------------------
 * r[0] always 0 (TODO: actual limit? see bitcounts)
 *
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_rx52to64_10(uint64_t r[10], const uint64_t a[10])
{
	r[9] = (a[8] << 52) +  a[9];
	r[8] = (a[7] << 40) + (a[8] >> 12);
	r[7] = (a[6] << 28) + (a[7] >> 24);
	r[6] = (a[5] << 16) + (a[6] >> 36);
	r[5] = (a[3] << 56) + (a[4] <<  4) + (a[5] >> 48);
	r[4] = (a[2] << 44) + (a[3] >>  8);
	r[3] = (a[1] << 32) + (a[2] >> 20);
	r[2] = (a[0] << 20) + (a[1] >> 32);
	r[1] =                 a[0] >> 44;
	r[0] =                 0;
}


/*--------------------------------------
 * r[0],r[1] always 0 (TODO: actual limit? see bitcounts)
 *
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_rx52to64_14(uint64_t r[14], const uint64_t a[14])
{
	r[13] =                 (a[12] << 52) +  a[13];
	r[12] =                 (a[11] << 40) + (a[12] >> 12);
	r[11] =                 (a[10] << 28) + (a[11] >> 24);
	r[10] =                 (a[ 9] << 16) + (a[10] >> 36);
	r[ 9] = (a[ 7] << 56) + (a[ 8] <<  4) + (a[ 9] >> 48);
	r[ 8] =                 (a[ 6] << 44) + (a[ 7] >>  8);
	r[ 7] =                 (a[ 5] << 32) + (a[ 6] >> 20);
	r[ 6] =                 (a[ 4] << 20) + (a[ 5] >> 32);
	r[ 5] = (a[ 2] << 60) + (a[ 3] <<  8) + (a[ 4] >> 44);
	r[ 4] =                 (a[ 1] << 48) + (a[ 2] >>  4);
	r[ 3] =                 (a[ 0] << 36) + (a[ 1] >> 16);
	r[ 2] =                                  a[ 0] >> 28;
	r[ 1] =                                  0;
	r[ 0] =                                  0;
}


/*--------------------------------------
 * r[0],r[1],r[2] always 0 (TODO: actual limit? see bitcounts)
 *
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_rx52to64_20(uint64_t r[20], const uint64_t a[20])
{
	r[19] =                 (a[18] << 52) +  a[19];
	r[18] =                 (a[17] << 40) + (a[18] >> 12);
	r[17] =                 (a[16] << 28) + (a[17] >> 24);
	r[16] =                 (a[15] << 16) + (a[16] >> 36);
	r[15] = (a[13] << 56) + (a[14] <<  4) + (a[15] >> 48);
	r[14] =                 (a[12] << 44) + (a[13] >>  8);
	r[13] =                 (a[11] << 32) + (a[12] >> 20);
	r[12] =                 (a[10] << 20) + (a[11] >> 32);
	r[11] = (a[ 8] << 60) + (a[ 9] <<  8) + (a[10] >> 44);
	r[10] =                 (a[ 7] << 48) + (a[ 8] >>  4);
	r[ 9] =                 (a[ 6] << 36) + (a[ 7] >> 16);
	r[ 8] =                 (a[ 5] << 24) + (a[ 6] >> 28);
	r[ 7] =                 (a[ 4] << 12) + (a[ 5] >> 40);
	r[ 6] =                 (a[ 2] << 52) +  a[ 3];
	r[ 5] =                 (a[ 1] << 40) + (a[ 2] >> 12);
	r[ 4] =                 (a[ 0] << 28) + (a[ 1] >> 24);
	r[ 3] =                                 (a[ 0] >> 36);
	r[ 2] =                                  0;
	r[ 1] =                                  0;
	r[ 0] =                                  0;
}


/*--------------------------------------
 * r[0..4] always 0 (TODO: actual limit? see bitcounts)
 *
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_rx52to64_27(uint64_t r[27], const uint64_t a[27])
{
	r[26] =                 (a[25] << 52) +  a[26];
	r[25] =                 (a[24] << 40) + (a[25] >> 12);
	r[24] =                 (a[23] << 28) + (a[24] >> 24);
	r[23] =                 (a[22] << 16) + (a[23] >> 36);
	r[22] = (a[20] << 56) + (a[21] <<  4) + (a[22] >> 48);
	r[21] =                 (a[19] << 44) + (a[20] >>  8);
	r[20] =                 (a[18] << 32) + (a[19] >> 20);
	r[19] =                 (a[17] << 20) + (a[18] >> 32);
	r[18] = (a[15] << 60) + (a[16] <<  8) + (a[17] >> 44);
	r[17] =                 (a[14] << 48) + (a[15] >>  4);
	r[16] =                 (a[13] << 36) + (a[14] >> 16);
	r[15] =                 (a[12] << 24) + (a[13] >> 28);
	r[14] =                 (a[11] << 12) + (a[12] >> 40);
	r[13] =                 (a[ 9] << 52) +  a[10];
	r[12] =                 (a[ 8] << 40) + (a[ 9] >> 12);
	r[11] =                 (a[ 7] << 28) + (a[ 8] >> 24);
	r[10] =                 (a[ 6] << 16) + (a[ 7] >> 36);
	r[ 9] = (a[ 4] << 56) + (a[ 5] <<  4) + (a[ 6] >> 48);
	r[ 8] =                 (a[ 3] << 44) + (a[ 4] >>  8);
	r[ 7] =                 (a[ 2] << 32) + (a[ 3] >> 20);
	r[ 6] =                 (a[ 1] << 20) + (a[ 2] >> 32);
	r[ 5] =                 (a[ 0] <<  8) + (a[ 1] >> 44);
	r[ 4] =                                  0;
	r[ 3] =                                  0;
	r[ 2] =                                  0;
	r[ 1] =                                  0;
	r[ 0] =                                  0;
}


/*--------------------------------------
 * r[0..7] always 0 (TODO: actual limit? see bitcounts)
 *
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_rx52to64_30(uint64_t r[30], const uint64_t a[30])
{
	r[29] =                 (a[28] << 52) +  a[29];
	r[28] =                 (a[27] << 40) + (a[28] >> 12);
	r[27] =                 (a[26] << 28) + (a[27] >> 24);
	r[26] =                 (a[25] << 16) + (a[26] >> 36);
	r[25] = (a[23] << 56) + (a[24] <<  4) + (a[25] >> 48);
	r[24] =                 (a[22] << 44) + (a[23] >>  8);
	r[23] =                 (a[21] << 32) + (a[22] >> 20);
	r[22] =                 (a[20] << 20) + (a[21] >> 32);
	r[21] = (a[18] << 60) + (a[19] <<  8) + (a[20] >> 44);
	r[20] =                 (a[17] << 48) + (a[18] >>  4);
	r[19] =                 (a[16] << 36) + (a[17] >> 16);
	r[18] =                 (a[15] << 24) + (a[16] >> 28);
	r[17] =                 (a[14] << 12) + (a[15] >> 40);
	r[16] =                 (a[12] << 52) +  a[13];
	r[15] =                 (a[11] << 40) + (a[12] >> 12);
	r[14] =                 (a[10] << 28) + (a[11] >> 24);
	r[13] =                 (a[ 9] << 16) + (a[10] >> 36);
	r[12] = (a[ 7] << 56) + (a[ 8] <<  4) + (a[ 9] >> 48);
	r[11] =                 (a[ 6] << 44) + (a[ 7] >>  8);
	r[10] =                 (a[ 5] << 32) + (a[ 6] >> 20);
	r[ 9] =                 (a[ 4] << 20) + (a[ 5] >> 32);
	r[ 8] = (a[ 2] << 60) + (a[ 3] <<  8) + (a[ 4] >> 44);
	r[ 7] =                 (a[ 1] << 48) + (a[ 2] >>  4);
	r[ 6] =                 (a[ 0] << 36) + (a[ 1] >> 16);
	r[ 5] =                                   a[ 0] >> 28;
	r[ 4] = 0;
	r[ 3] = 0;
	r[ 2] = 0;
	r[ 1] = 0;
	r[ 0] = 0;
}


/*--------------------------------------
 * r[0..7] always 0 (TODO: actual limit? see bitcounts)
 *
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_rx52to64_40(uint64_t r[40], const uint64_t a[40])
{
	r[39] =                 (a[38] << 52) +  a[39];
	r[38] =                 (a[37] << 40) + (a[38] >> 12);
	r[37] =                 (a[36] << 28) + (a[37] >> 24);
	r[36] =                 (a[35] << 16) + (a[36] >> 36);
	r[35] = (a[33] << 56) + (a[34] <<  4) + (a[35] >> 48);
	r[34] =                 (a[32] << 44) + (a[33] >>  8);
	r[33] =                 (a[31] << 32) + (a[32] >> 20);
	r[32] =                 (a[30] << 20) + (a[31] >> 32);
	r[31] = (a[28] << 60) + (a[29] <<  8) + (a[30] >> 44);
	r[30] =                 (a[27] << 48) + (a[28] >>  4);
	r[29] =                 (a[26] << 36) + (a[27] >> 16);
	r[28] =                 (a[25] << 24) + (a[26] >> 28);
	r[27] =                 (a[24] << 12) + (a[25] >> 40);
	r[26] =                 (a[22] << 52) +  a[23];
	r[25] =                 (a[21] << 40) + (a[22] >> 12);
	r[24] =                 (a[20] << 28) + (a[21] >> 24);
	r[23] =                 (a[19] << 16) + (a[20] >> 36);
	r[22] = (a[17] << 56) + (a[18] <<  4) + (a[19] >> 48);
	r[21] =                 (a[16] << 44) + (a[17] >>  8);
	r[20] =                 (a[15] << 32) + (a[16] >> 20);
	r[19] =                 (a[14] << 20) + (a[15] >> 32);
	r[18] = (a[12] << 60) + (a[13] <<  8) + (a[14] >> 44);
	r[17] =                 (a[11] << 48) + (a[12] >>  4);
	r[16] =                 (a[10] << 36) + (a[11] >> 16);
	r[15] =                 (a[ 9] << 24) + (a[10] >> 28);
	r[14] =                 (a[ 8] << 12) + (a[ 9] >> 40);
	r[13] =                 (a[ 6] << 52) +  a[ 7];
	r[12] =                 (a[ 5] << 40) + (a[ 6] >> 12);
	r[11] =                 (a[ 4] << 28) + (a[ 5] >> 24);
	r[10] =                 (a[ 3] << 16) + (a[ 4] >> 36);
	r[ 9] = (a[ 1] << 56) + (a[ 2] <<  4) + (a[ 3] >> 48);
	r[ 8] =                 (a[ 0] << 44) + (a[ 1] >>  8);
	r[ 7] =                                 (a[ 0] >> 20);
	r[ 6] = 0;
	r[ 5] = 0;
	r[ 4] = 0;
	r[ 3] = 0;
	r[ 2] = 0;
	r[ 1] = 0;
	r[ 0] = 0;
}


/*--------------------------------------
 * r[0..11] always 0 (TODO: actual limit? see bitcounts)
 *
 * r[] and a[] may be same pointer
 * other overlap is undefined
 */
static inline void bn_rx52to64_60(uint64_t r[60], const uint64_t a[60])
{
	r[59] =                 (a[58] << 52) +  a[59];
	r[58] =                 (a[57] << 40) + (a[58] >> 12);
	r[57] =                 (a[56] << 28) + (a[57] >> 24);
	r[56] =                 (a[55] << 16) + (a[56] >> 36);
	r[55] = (a[53] << 56) + (a[54] <<  4) + (a[55] >> 48);
	r[54] =                 (a[52] << 44) + (a[53] >>  8);
	r[53] =                 (a[51] << 32) + (a[52] >> 20);
	r[52] =                 (a[50] << 20) + (a[51] >> 32);
	r[51] = (a[48] << 60) + (a[49] <<  8) + (a[50] >> 44);
	r[50] =                 (a[47] << 48) + (a[48] >>  4);
	r[49] =                 (a[46] << 36) + (a[47] >> 16);
	r[48] =                 (a[45] << 24) + (a[46] >> 28);
	r[47] =                 (a[44] << 12) + (a[45] >> 40);
	r[46] =                 (a[42] << 52) +  a[43];
	r[45] =                 (a[41] << 40) + (a[42] >> 12);
	r[44] =                 (a[40] << 28) + (a[41] >> 24);
	r[43] =                 (a[39] << 16) + (a[40] >> 36);
	r[42] = (a[37] << 56) + (a[38] <<  4) + (a[39] >> 48);
	r[41] =                 (a[36] << 44) + (a[37] >>  8);
	r[40] =                 (a[35] << 32) + (a[36] >> 20);
	r[39] =                 (a[34] << 20) + (a[35] >> 32);
	r[38] = (a[32] << 60) + (a[33] <<  8) + (a[34] >> 44);
	r[37] =                 (a[31] << 48) + (a[32] >>  4);
	r[36] =                 (a[30] << 36) + (a[31] >> 16);
	r[35] =                 (a[29] << 24) + (a[30] >> 28);
	r[34] =                 (a[28] << 12) + (a[29] >> 40);
	r[33] =                 (a[26] << 52) +  a[27];
	r[32] =                 (a[25] << 40) + (a[26] >> 12);
	r[31] =                 (a[24] << 28) + (a[25] >> 24);
	r[30] =                 (a[23] << 16) + (a[24] >> 36);
	r[29] = (a[21] << 56) + (a[22] <<  4) + (a[23] >> 48);
	r[28] =                 (a[20] << 44) + (a[21] >>  8);
	r[27] =                 (a[19] << 32) + (a[20] >> 20);
	r[26] =                 (a[18] << 20) + (a[19] >> 32);
	r[25] = (a[16] << 60) + (a[17] <<  8) + (a[18] >> 44);
	r[24] =                 (a[15] << 48) + (a[16] >>  4);
	r[23] =                 (a[14] << 36) + (a[15] >> 16);
	r[22] =                 (a[13] << 24) + (a[14] >> 28);
	r[21] =                 (a[12] << 12) + (a[13] >> 40);
	r[20] =                 (a[10] << 52) +  a[11];
	r[19] =                 (a[ 9] << 40) + (a[10] >> 12);
	r[18] =                 (a[ 8] << 28) + (a[ 9] >> 24);
	r[17] =                 (a[ 7] << 16) + (a[ 8] >> 36);
	r[16] = (a[ 5] << 56) + (a[ 6] <<  4) + (a[ 7] >> 48);
	r[15] =                 (a[ 4] << 44) + (a[ 5] >>  8);
	r[14] =                 (a[ 3] << 32) + (a[ 4] >> 20);
	r[13] =                 (a[ 2] << 20) + (a[ 3] >> 32);
	r[12] = (a[ 0] << 60) + (a[ 1] <<  8) + (a[ 2] >> 44);
	r[11] =                                  a[ 0] >>  4;
	r[10] = 0;
	r[ 9] = 0;
	r[ 8] = 0;
	r[ 7] = 0;
	r[ 6] = 0;
	r[ 5] = 0;
	r[ 4] = 0;
	r[ 3] = 0;
	r[ 2] = 0;
	r[ 1] = 0;
	r[ 0] = 0;
}


//--------------------------------------
static inline void bn_64to52rx_carry_30inpl(uint64_t a[30])
{
	a[28] += (a[29] >> 52);
	a[29] &= UINT64_C(0xfffffffffffff);
	a[27] += (a[28] >> 52);
	a[28] &= UINT64_C(0xfffffffffffff);
	a[26] += (a[27] >> 52);
	a[27] &= UINT64_C(0xfffffffffffff);
	a[25] += (a[26] >> 52);
	a[26] &= UINT64_C(0xfffffffffffff);
	a[24] += (a[25] >> 52);
	a[25] &= UINT64_C(0xfffffffffffff);
	a[23] += (a[24] >> 52);
	a[24] &= UINT64_C(0xfffffffffffff);
	a[22] += (a[23] >> 52);
	a[23] &= UINT64_C(0xfffffffffffff);
	a[21] += (a[22] >> 52);
	a[22] &= UINT64_C(0xfffffffffffff);
	a[20] += (a[21] >> 52);
	a[21] &= UINT64_C(0xfffffffffffff);
	a[19] += (a[20] >> 52);
	a[20] &= UINT64_C(0xfffffffffffff);
	a[18] += (a[19] >> 52);
	a[19] &= UINT64_C(0xfffffffffffff);
	a[17] += (a[18] >> 52);
	a[18] &= UINT64_C(0xfffffffffffff);
	a[16] += (a[17] >> 52);
	a[17] &= UINT64_C(0xfffffffffffff);
	a[15] += (a[16] >> 52);
	a[16] &= UINT64_C(0xfffffffffffff);
	a[14] += (a[15] >> 52);
	a[15] &= UINT64_C(0xfffffffffffff);
	a[13] += (a[14] >> 52);
	a[14] &= UINT64_C(0xfffffffffffff);
	a[12] += (a[13] >> 52);
	a[13] &= UINT64_C(0xfffffffffffff);
	a[11] += (a[12] >> 52);
	a[12] &= UINT64_C(0xfffffffffffff);
	a[10] += (a[11] >> 52);
	a[11] &= UINT64_C(0xfffffffffffff);
	a[ 9] += (a[10] >> 52);
	a[10] &= UINT64_C(0xfffffffffffff);
	a[ 8] += (a[ 9] >> 52);
	a[ 9] &= UINT64_C(0xfffffffffffff);
	a[ 7] += (a[ 8] >> 52);
	a[ 8] &= UINT64_C(0xfffffffffffff);
	a[ 6] += (a[ 7] >> 52);
	a[ 7] &= UINT64_C(0xfffffffffffff);
	a[ 5] += (a[ 6] >> 52);
	a[ 6] &= UINT64_C(0xfffffffffffff);
	a[ 4] += (a[ 5] >> 52);
	a[ 5] &= UINT64_C(0xfffffffffffff);
	a[ 3] += (a[ 4] >> 52);
	a[ 4] &= UINT64_C(0xfffffffffffff);
	a[ 2] += (a[ 3] >> 52);
	a[ 3] &= UINT64_C(0xfffffffffffff);
	a[ 1] += (a[ 2] >> 52);
	a[ 2] &= UINT64_C(0xfffffffffffff);
	a[ 0] += (a[ 1] >> 52);
	a[ 1] &= UINT64_C(0xfffffffffffff);
}


//--------------------------------------
static inline void bn_64to52rx_carry_40inpl(uint64_t a[40])
{
	a[38] += (a[39] >> 52);
	a[39] &= UINT64_C(0xfffffffffffff);
	a[37] += (a[38] >> 52);
	a[38] &= UINT64_C(0xfffffffffffff);
	a[36] += (a[37] >> 52);
	a[37] &= UINT64_C(0xfffffffffffff);
	a[35] += (a[36] >> 52);
	a[36] &= UINT64_C(0xfffffffffffff);
	a[34] += (a[35] >> 52);
	a[35] &= UINT64_C(0xfffffffffffff);
	a[33] += (a[34] >> 52);
	a[34] &= UINT64_C(0xfffffffffffff);
	a[32] += (a[33] >> 52);
	a[33] &= UINT64_C(0xfffffffffffff);
	a[31] += (a[32] >> 52);
	a[32] &= UINT64_C(0xfffffffffffff);
	a[30] += (a[31] >> 52);
	a[31] &= UINT64_C(0xfffffffffffff);
	a[29] += (a[30] >> 52);
	a[30] &= UINT64_C(0xfffffffffffff);
	a[28] += (a[29] >> 52);
	a[29] &= UINT64_C(0xfffffffffffff);
	a[27] += (a[28] >> 52);
	a[28] &= UINT64_C(0xfffffffffffff);
	a[26] += (a[27] >> 52);
	a[27] &= UINT64_C(0xfffffffffffff);
	a[25] += (a[26] >> 52);
	a[26] &= UINT64_C(0xfffffffffffff);
	a[24] += (a[25] >> 52);
	a[25] &= UINT64_C(0xfffffffffffff);
	a[23] += (a[24] >> 52);
	a[24] &= UINT64_C(0xfffffffffffff);
	a[22] += (a[23] >> 52);
	a[23] &= UINT64_C(0xfffffffffffff);
	a[21] += (a[22] >> 52);
	a[22] &= UINT64_C(0xfffffffffffff);
	a[20] += (a[21] >> 52);
	a[21] &= UINT64_C(0xfffffffffffff);
	a[19] += (a[20] >> 52);
	a[20] &= UINT64_C(0xfffffffffffff);
	a[18] += (a[19] >> 52);
	a[19] &= UINT64_C(0xfffffffffffff);
	a[17] += (a[18] >> 52);
	a[18] &= UINT64_C(0xfffffffffffff);
	a[16] += (a[17] >> 52);
	a[17] &= UINT64_C(0xfffffffffffff);
	a[15] += (a[16] >> 52);
	a[16] &= UINT64_C(0xfffffffffffff);
	a[14] += (a[15] >> 52);
	a[15] &= UINT64_C(0xfffffffffffff);
	a[13] += (a[14] >> 52);
	a[14] &= UINT64_C(0xfffffffffffff);
	a[12] += (a[13] >> 52);
	a[13] &= UINT64_C(0xfffffffffffff);
	a[11] += (a[12] >> 52);
	a[12] &= UINT64_C(0xfffffffffffff);
	a[10] += (a[11] >> 52);
	a[11] &= UINT64_C(0xfffffffffffff);
	a[ 9] += (a[10] >> 52);
	a[10] &= UINT64_C(0xfffffffffffff);
	a[ 8] += (a[ 9] >> 52);
	a[ 9] &= UINT64_C(0xfffffffffffff);
	a[ 7] += (a[ 8] >> 52);
	a[ 8] &= UINT64_C(0xfffffffffffff);
	a[ 6] += (a[ 7] >> 52);
	a[ 7] &= UINT64_C(0xfffffffffffff);
	a[ 5] += (a[ 6] >> 52);
	a[ 6] &= UINT64_C(0xfffffffffffff);
	a[ 4] += (a[ 5] >> 52);
	a[ 5] &= UINT64_C(0xfffffffffffff);
	a[ 3] += (a[ 4] >> 52);
	a[ 4] &= UINT64_C(0xfffffffffffff);
	a[ 2] += (a[ 3] >> 52);
	a[ 3] &= UINT64_C(0xfffffffffffff);
	a[ 1] += (a[ 2] >> 52);
	a[ 2] &= UINT64_C(0xfffffffffffff);
	a[ 0] += (a[ 1] >> 52);
	a[ 1] &= UINT64_C(0xfffffffffffff);
}


//--------------------------------------
static inline void bn_64to52rx_carry_60inpl(uint64_t a[60])
{
	a[58] += (a[59] >> 52);
	a[59] &= UINT64_C(0xfffffffffffff);
	a[57] += (a[58] >> 52);
	a[58] &= UINT64_C(0xfffffffffffff);
	a[56] += (a[57] >> 52);
	a[57] &= UINT64_C(0xfffffffffffff);
	a[55] += (a[56] >> 52);
	a[56] &= UINT64_C(0xfffffffffffff);
	a[54] += (a[55] >> 52);
	a[55] &= UINT64_C(0xfffffffffffff);
	a[53] += (a[54] >> 52);
	a[54] &= UINT64_C(0xfffffffffffff);
	a[52] += (a[53] >> 52);
	a[53] &= UINT64_C(0xfffffffffffff);
	a[51] += (a[52] >> 52);
	a[52] &= UINT64_C(0xfffffffffffff);
	a[50] += (a[51] >> 52);
	a[51] &= UINT64_C(0xfffffffffffff);
	a[49] += (a[50] >> 52);
	a[50] &= UINT64_C(0xfffffffffffff);
	a[48] += (a[49] >> 52);
	a[49] &= UINT64_C(0xfffffffffffff);
	a[47] += (a[48] >> 52);
	a[48] &= UINT64_C(0xfffffffffffff);
	a[46] += (a[47] >> 52);
	a[47] &= UINT64_C(0xfffffffffffff);
	a[45] += (a[46] >> 52);
	a[46] &= UINT64_C(0xfffffffffffff);
	a[44] += (a[45] >> 52);
	a[45] &= UINT64_C(0xfffffffffffff);
	a[43] += (a[44] >> 52);
	a[44] &= UINT64_C(0xfffffffffffff);
	a[42] += (a[43] >> 52);
	a[43] &= UINT64_C(0xfffffffffffff);
	a[41] += (a[42] >> 52);
	a[42] &= UINT64_C(0xfffffffffffff);
	a[40] += (a[41] >> 52);
	a[41] &= UINT64_C(0xfffffffffffff);
	a[39] += (a[40] >> 52);
	a[40] &= UINT64_C(0xfffffffffffff);
	a[38] += (a[39] >> 52);
	a[39] &= UINT64_C(0xfffffffffffff);
	a[37] += (a[38] >> 52);
	a[38] &= UINT64_C(0xfffffffffffff);
	a[36] += (a[37] >> 52);
	a[37] &= UINT64_C(0xfffffffffffff);
	a[35] += (a[36] >> 52);
	a[36] &= UINT64_C(0xfffffffffffff);
	a[34] += (a[35] >> 52);
	a[35] &= UINT64_C(0xfffffffffffff);
	a[33] += (a[34] >> 52);
	a[34] &= UINT64_C(0xfffffffffffff);
	a[32] += (a[33] >> 52);
	a[33] &= UINT64_C(0xfffffffffffff);
	a[31] += (a[32] >> 52);
	a[32] &= UINT64_C(0xfffffffffffff);
	a[30] += (a[31] >> 52);
	a[31] &= UINT64_C(0xfffffffffffff);
	a[29] += (a[30] >> 52);
	a[30] &= UINT64_C(0xfffffffffffff);
	a[28] += (a[29] >> 52);
	a[29] &= UINT64_C(0xfffffffffffff);
	a[27] += (a[28] >> 52);
	a[28] &= UINT64_C(0xfffffffffffff);
	a[26] += (a[27] >> 52);
	a[27] &= UINT64_C(0xfffffffffffff);
	a[25] += (a[26] >> 52);
	a[26] &= UINT64_C(0xfffffffffffff);
	a[24] += (a[25] >> 52);
	a[25] &= UINT64_C(0xfffffffffffff);
	a[23] += (a[24] >> 52);
	a[24] &= UINT64_C(0xfffffffffffff);
	a[22] += (a[23] >> 52);
	a[23] &= UINT64_C(0xfffffffffffff);
	a[21] += (a[22] >> 52);
	a[22] &= UINT64_C(0xfffffffffffff);
	a[20] += (a[21] >> 52);
	a[21] &= UINT64_C(0xfffffffffffff);
	a[19] += (a[20] >> 52);
	a[20] &= UINT64_C(0xfffffffffffff);
	a[18] += (a[19] >> 52);
	a[19] &= UINT64_C(0xfffffffffffff);
	a[17] += (a[18] >> 52);
	a[18] &= UINT64_C(0xfffffffffffff);
	a[16] += (a[17] >> 52);
	a[17] &= UINT64_C(0xfffffffffffff);
	a[15] += (a[16] >> 52);
	a[16] &= UINT64_C(0xfffffffffffff);
	a[14] += (a[15] >> 52);
	a[15] &= UINT64_C(0xfffffffffffff);
	a[13] += (a[14] >> 52);
	a[14] &= UINT64_C(0xfffffffffffff);
	a[12] += (a[13] >> 52);
	a[13] &= UINT64_C(0xfffffffffffff);
	a[11] += (a[12] >> 52);
	a[12] &= UINT64_C(0xfffffffffffff);
	a[10] += (a[11] >> 52);
	a[11] &= UINT64_C(0xfffffffffffff);
	a[ 9] += (a[10] >> 52);
	a[10] &= UINT64_C(0xfffffffffffff);
	a[ 8] += (a[ 9] >> 52);
	a[ 9] &= UINT64_C(0xfffffffffffff);
	a[ 7] += (a[ 8] >> 52);
	a[ 8] &= UINT64_C(0xfffffffffffff);
	a[ 6] += (a[ 7] >> 52);
	a[ 7] &= UINT64_C(0xfffffffffffff);
	a[ 5] += (a[ 6] >> 52);
	a[ 6] &= UINT64_C(0xfffffffffffff);
	a[ 4] += (a[ 5] >> 52);
	a[ 5] &= UINT64_C(0xfffffffffffff);
	a[ 3] += (a[ 4] >> 52);
	a[ 4] &= UINT64_C(0xfffffffffffff);
	a[ 2] += (a[ 3] >> 52);
	a[ 3] &= UINT64_C(0xfffffffffffff);
	a[ 1] += (a[ 2] >> 52);
	a[ 2] &= UINT64_C(0xfffffffffffff);
	a[ 0] += (a[ 1] >> 52);
	a[ 1] &= UINT64_C(0xfffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 52 bits per digit; in-place
 * does not reduce MS digit [0]
 */
static inline void bn_64to52rx_carry_27inpl(uint64_t a[27])
{
	a[25] += (a[26] >> 52);
	a[26] &= UINT64_C(0xfffffffffffff);

	a[24] += (a[25] >> 52);
	a[25] &= UINT64_C(0xfffffffffffff);

	a[23] += (a[24] >> 52);
	a[24] &= UINT64_C(0xfffffffffffff);

	a[22] += (a[23] >> 52);
	a[23] &= UINT64_C(0xfffffffffffff);

	a[21] += (a[22] >> 52);
	a[22] &= UINT64_C(0xfffffffffffff);

	a[20] += (a[21] >> 52);
	a[21] &= UINT64_C(0xfffffffffffff);

	a[19] += (a[20] >> 52);
	a[20] &= UINT64_C(0xfffffffffffff);

	a[18] += (a[19] >> 52);
	a[19] &= UINT64_C(0xfffffffffffff);

	a[17] += (a[18] >> 52);
	a[18] &= UINT64_C(0xfffffffffffff);

	a[16] += (a[17] >> 52);
	a[17] &= UINT64_C(0xfffffffffffff);

	a[15] += (a[16] >> 52);
	a[16] &= UINT64_C(0xfffffffffffff);

	a[14] += (a[15] >> 52);
	a[15] &= UINT64_C(0xfffffffffffff);

	a[13] += (a[14] >> 52);
	a[14] &= UINT64_C(0xfffffffffffff);

	a[12] += (a[13] >> 52);
	a[13] &= UINT64_C(0xfffffffffffff);

	a[11] += (a[12] >> 52);
	a[12] &= UINT64_C(0xfffffffffffff);

	a[10] += (a[11] >> 52);
	a[11] &= UINT64_C(0xfffffffffffff);

	a[ 9] += (a[10] >> 52);
	a[10] &= UINT64_C(0xfffffffffffff);

	a[ 8] += (a[ 9] >> 52);
	a[ 9] &= UINT64_C(0xfffffffffffff);

	a[ 7] += (a[ 8] >> 52);
	a[ 8] &= UINT64_C(0xfffffffffffff);

	a[ 6] += (a[ 7] >> 52);
	a[ 7] &= UINT64_C(0xfffffffffffff);

	a[ 5] += (a[ 6] >> 52);
	a[ 6] &= UINT64_C(0xfffffffffffff);

	a[ 4] += (a[ 5] >> 52);
	a[ 5] &= UINT64_C(0xfffffffffffff);

	a[ 3] += (a[ 4] >> 52);
	a[ 4] &= UINT64_C(0xfffffffffffff);

	a[ 2] += (a[ 3] >> 52);
	a[ 3] &= UINT64_C(0xfffffffffffff);

	a[ 1] += (a[ 2] >> 52);
	a[ 2] &= UINT64_C(0xfffffffffffff);

	a[ 0] += (a[ 1] >> 52);
	a[ 1] &= UINT64_C(0xfffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 52 bits per digit; in-place
 * does not reduce MS digit [0]
 */
static inline void bn_64to52rx_carry_20inpl(uint64_t a[20])
{
	a[18] += (a[19] >> 52);
	a[19] &= UINT64_C(0xfffffffffffff);

	a[17] += (a[18] >> 52);
	a[18] &= UINT64_C(0xfffffffffffff);

	a[16] += (a[17] >> 52);
	a[17] &= UINT64_C(0xfffffffffffff);

	a[15] += (a[16] >> 52);
	a[16] &= UINT64_C(0xfffffffffffff);

	a[14] += (a[15] >> 52);
	a[15] &= UINT64_C(0xfffffffffffff);

	a[13] += (a[14] >> 52);
	a[14] &= UINT64_C(0xfffffffffffff);

	a[12] += (a[13] >> 52);
	a[13] &= UINT64_C(0xfffffffffffff);

	a[11] += (a[12] >> 52);
	a[12] &= UINT64_C(0xfffffffffffff);

	a[10] += (a[11] >> 52);
	a[11] &= UINT64_C(0xfffffffffffff);

	a[ 9] += (a[10] >> 52);
	a[10] &= UINT64_C(0xfffffffffffff);

	a[ 8] += (a[ 9] >> 52);
	a[ 9] &= UINT64_C(0xfffffffffffff);

	a[ 7] += (a[ 8] >> 52);
	a[ 8] &= UINT64_C(0xfffffffffffff);

	a[ 6] += (a[ 7] >> 52);
	a[ 7] &= UINT64_C(0xfffffffffffff);

	a[ 5] += (a[ 6] >> 52);
	a[ 6] &= UINT64_C(0xfffffffffffff);

	a[ 4] += (a[ 5] >> 52);
	a[ 5] &= UINT64_C(0xfffffffffffff);

	a[ 3] += (a[ 4] >> 52);
	a[ 4] &= UINT64_C(0xfffffffffffff);

	a[ 2] += (a[ 3] >> 52);
	a[ 3] &= UINT64_C(0xfffffffffffff);

	a[ 1] += (a[ 2] >> 52);
	a[ 2] &= UINT64_C(0xfffffffffffff);

	a[ 0] += (a[ 1] >> 52);
	a[ 1] &= UINT64_C(0xfffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 52 bits per digit; in-place
 * does not reduce MS digit [0]
 */
static inline void bn_64to52rx_carry_21inpl(uint64_t a[21])
{
	a[19] += (a[20] >> 52);
	a[20] &= UINT64_C(0xfffffffffffff);

	a[18] += (a[19] >> 52);
	a[19] &= UINT64_C(0xfffffffffffff);

	a[17] += (a[18] >> 52);
	a[18] &= UINT64_C(0xfffffffffffff);

	a[16] += (a[17] >> 52);
	a[17] &= UINT64_C(0xfffffffffffff);

	a[15] += (a[16] >> 52);
	a[16] &= UINT64_C(0xfffffffffffff);

	a[14] += (a[15] >> 52);
	a[15] &= UINT64_C(0xfffffffffffff);

	a[13] += (a[14] >> 52);
	a[14] &= UINT64_C(0xfffffffffffff);

	a[12] += (a[13] >> 52);
	a[13] &= UINT64_C(0xfffffffffffff);

	a[11] += (a[12] >> 52);
	a[12] &= UINT64_C(0xfffffffffffff);

	a[10] += (a[11] >> 52);
	a[11] &= UINT64_C(0xfffffffffffff);

	a[ 9] += (a[10] >> 52);
	a[10] &= UINT64_C(0xfffffffffffff);

	a[ 8] += (a[ 9] >> 52);
	a[ 9] &= UINT64_C(0xfffffffffffff);

	a[ 7] += (a[ 8] >> 52);
	a[ 8] &= UINT64_C(0xfffffffffffff);

	a[ 6] += (a[ 7] >> 52);
	a[ 7] &= UINT64_C(0xfffffffffffff);

	a[ 5] += (a[ 6] >> 52);
	a[ 6] &= UINT64_C(0xfffffffffffff);

	a[ 4] += (a[ 5] >> 52);
	a[ 5] &= UINT64_C(0xfffffffffffff);

	a[ 3] += (a[ 4] >> 52);
	a[ 4] &= UINT64_C(0xfffffffffffff);

	a[ 2] += (a[ 3] >> 52);
	a[ 3] &= UINT64_C(0xfffffffffffff);

	a[ 1] += (a[ 2] >> 52);
	a[ 2] &= UINT64_C(0xfffffffffffff);

	a[ 0] += (a[ 1] >> 52);
	a[ 1] &= UINT64_C(0xfffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 52 bits per digit; in-place
 * does not reduce MS digit [0]
 */
static inline void bn_64to52rx_carry_14inpl(uint64_t a[14])
{
	a[12] += (a[13] >> 52);
	a[13] &= UINT64_C(0xfffffffffffff);

	a[11] += (a[12] >> 52);
	a[12] &= UINT64_C(0xfffffffffffff);

	a[10] += (a[11] >> 52);
	a[11] &= UINT64_C(0xfffffffffffff);

	a[ 9] += (a[10] >> 52);
	a[10] &= UINT64_C(0xfffffffffffff);

	a[ 8] += (a[ 9] >> 52);
	a[ 9] &= UINT64_C(0xfffffffffffff);

	a[ 7] += (a[ 8] >> 52);
	a[ 8] &= UINT64_C(0xfffffffffffff);

	a[ 6] += (a[ 7] >> 52);
	a[ 7] &= UINT64_C(0xfffffffffffff);

	a[ 5] += (a[ 6] >> 52);
	a[ 6] &= UINT64_C(0xfffffffffffff);

	a[ 4] += (a[ 5] >> 52);
	a[ 5] &= UINT64_C(0xfffffffffffff);

	a[ 3] += (a[ 4] >> 52);
	a[ 4] &= UINT64_C(0xfffffffffffff);

	a[ 2] += (a[ 3] >> 52);
	a[ 3] &= UINT64_C(0xfffffffffffff);

	a[ 1] += (a[ 2] >> 52);
	a[ 2] &= UINT64_C(0xfffffffffffff);

	a[ 0] += (a[ 1] >> 52);
	a[ 1] &= UINT64_C(0xfffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 52 bits per digit; in-place
 * does not reduce MS digit [0]
 */
static inline void bn_64to52rx_carry_10inpl(uint64_t a[10])
{
	a[8] += (a[9] >> 52);
	a[9] &= UINT64_C(0xfffffffffffff);

	a[7] += (a[8] >> 52);
	a[8] &= UINT64_C(0xfffffffffffff);

	a[6] += (a[7] >> 52);
	a[7] &= UINT64_C(0xfffffffffffff);

	a[5] += (a[6] >> 52);
	a[6] &= UINT64_C(0xfffffffffffff);

	a[4] += (a[5] >> 52);
	a[5] &= UINT64_C(0xfffffffffffff);

	a[3] += (a[4] >> 52);
	a[4] &= UINT64_C(0xfffffffffffff);

	a[2] += (a[3] >> 52);
	a[3] &= UINT64_C(0xfffffffffffff);

	a[1] += (a[2] >> 52);
	a[2] &= UINT64_C(0xfffffffffffff);

	a[0] += (a[1] >> 52);
	a[1] &= UINT64_C(0xfffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 52 bits per digit; in-place
 */
static inline void bn_64to52rx_carry_8inpl(uint64_t a[8])
{
	a[6] += (a[7] >> 52);
	a[7] &= UINT64_C(0xfffffffffffff);

	a[5] += (a[6] >> 52);
	a[6] &= UINT64_C(0xfffffffffffff);

	a[4] += (a[5] >> 52);
	a[5] &= UINT64_C(0xfffffffffffff);

	a[3] += (a[4] >> 52);
	a[4] &= UINT64_C(0xfffffffffffff);

	a[2] += (a[3] >> 52);
	a[3] &= UINT64_C(0xfffffffffffff);

	a[1] += (a[2] >> 52);
	a[2] &= UINT64_C(0xfffffffffffff);

	a[0] += (a[1] >> 52);
	a[1] &= UINT64_C(0xfffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 52 bits per digit; in-place
 * does not reduce MS digit [0]
 */
static inline void bn_64to52rx_carry_9inpl(uint64_t a[9])
{
	a[7] += (a[8] >> 52);
	a[8] &= UINT64_C(0xfffffffffffff);

	a[6] += (a[7] >> 52);
	a[7] &= UINT64_C(0xfffffffffffff);

	a[5] += (a[6] >> 52);
	a[6] &= UINT64_C(0xfffffffffffff);

	a[4] += (a[5] >> 52);
	a[5] &= UINT64_C(0xfffffffffffff);

	a[3] += (a[4] >> 52);
	a[4] &= UINT64_C(0xfffffffffffff);

	a[2] += (a[3] >> 52);
	a[3] &= UINT64_C(0xfffffffffffff);

	a[1] += (a[2] >> 52);
	a[2] &= UINT64_C(0xfffffffffffff);

	a[0] += (a[1] >> 52);
	a[1] &= UINT64_C(0xfffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 61 bits per digit; in-place
 * does not reduce MS digit [0]
 */
static inline void bn_64to61rx_carry_6inpl(uint64_t a[6])
{
	a[4] += (a[5] >> 61);
	a[5] &= UINT64_C(0x1fffffffffffffff);

	a[3] += (a[4] >> 61);
	a[4] &= UINT64_C(0x1fffffffffffffff);

	a[2] += (a[3] >> 61);
	a[3] &= UINT64_C(0x1fffffffffffffff);

	a[1] += (a[2] >> 61);
	a[2] &= UINT64_C(0x1fffffffffffffff);

	a[0] += (a[1] >> 61);
	a[1] &= UINT64_C(0x1fffffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 61 bits per digit; in-place
 * does not reduce MS digit [0]
 */
static inline void bn_64to61rx_carry_9inpl(uint64_t a[9])
{
	a[7] += (a[8] >> 61);
	a[8] &= UINT64_C(0x1fffffffffffffff);

	a[6] += (a[7] >> 61);
	a[7] &= UINT64_C(0x1fffffffffffffff);

	a[5] += (a[6] >> 61);
	a[6] &= UINT64_C(0x1fffffffffffffff);

	a[4] += (a[5] >> 61);
	a[5] &= UINT64_C(0x1fffffffffffffff);

	a[3] += (a[4] >> 61);
	a[4] &= UINT64_C(0x1fffffffffffffff);

	a[2] += (a[3] >> 61);
	a[3] &= UINT64_C(0x1fffffffffffffff);

	a[1] += (a[2] >> 61);
	a[2] &= UINT64_C(0x1fffffffffffffff);

	a[0] += (a[1] >> 61);
	a[1] &= UINT64_C(0x1fffffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 61 bits per digit; in-place
 * does not reduce MS digit [0]
 */
static inline void bn_64to61rx_carry_12inpl(uint64_t a[12])
{
	a[10] += (a[11] >> 61);
	a[11] &= UINT64_C(0x1fffffffffffffff);

	a[ 9] += (a[10] >> 61);
	a[10] &= UINT64_C(0x1fffffffffffffff);

	a[ 8] += (a[ 9] >> 61);
	a[ 9] &= UINT64_C(0x1fffffffffffffff);

	a[ 7] += (a[ 8] >> 61);
	a[ 8] &= UINT64_C(0x1fffffffffffffff);

	a[ 6] += (a[ 7] >> 61);
	a[ 7] &= UINT64_C(0x1fffffffffffffff);

	a[ 5] += (a[ 6] >> 61);
	a[ 6] &= UINT64_C(0x1fffffffffffffff);

	a[ 4] += (a[ 5] >> 61);
	a[ 5] &= UINT64_C(0x1fffffffffffffff);

	a[ 3] += (a[ 4] >> 61);
	a[ 4] &= UINT64_C(0x1fffffffffffffff);

	a[ 2] += (a[ 3] >> 61);
	a[ 3] &= UINT64_C(0x1fffffffffffffff);

	a[ 1] += (a[ 2] >> 61);
	a[ 2] &= UINT64_C(0x1fffffffffffffff);

	a[ 0] += (a[ 1] >> 61);
	a[ 1] &= UINT64_C(0x1fffffffffffffff);
}


/*--------------------------------------
 * carry-propagate 64 to 61 bits per digit; in-place
 * does not reduce MS digit [0]
 */
static inline void bn_64to61rx_carry_17inpl(uint64_t a[17])
{
	a[15] += (a[16] >> 61);
	a[16] &= UINT64_C(0x1fffffffffffffff);

	a[14] += (a[15] >> 61);
	a[15] &= UINT64_C(0x1fffffffffffffff);

	a[13] += (a[14] >> 61);
	a[14] &= UINT64_C(0x1fffffffffffffff);

	a[12] += (a[13] >> 61);
	a[13] &= UINT64_C(0x1fffffffffffffff);

	a[11] += (a[12] >> 61);
	a[12] &= UINT64_C(0x1fffffffffffffff);

	a[10] += (a[11] >> 61);
	a[11] &= UINT64_C(0x1fffffffffffffff);

	a[ 9] += (a[10] >> 61);
	a[10] &= UINT64_C(0x1fffffffffffffff);

	a[ 8] += (a[ 9] >> 61);
	a[ 9] &= UINT64_C(0x1fffffffffffffff);

	a[ 7] += (a[ 8] >> 61);
	a[ 8] &= UINT64_C(0x1fffffffffffffff);

	a[ 6] += (a[ 7] >> 61);
	a[ 7] &= UINT64_C(0x1fffffffffffffff);

	a[ 5] += (a[ 6] >> 61);
	a[ 6] &= UINT64_C(0x1fffffffffffffff);

	a[ 4] += (a[ 5] >> 61);
	a[ 5] &= UINT64_C(0x1fffffffffffffff);

	a[ 3] += (a[ 4] >> 61);
	a[ 4] &= UINT64_C(0x1fffffffffffffff);

	a[ 2] += (a[ 3] >> 61);
	a[ 3] &= UINT64_C(0x1fffffffffffffff);

	a[ 1] += (a[ 2] >> 61);
	a[ 2] &= UINT64_C(0x1fffffffffffffff);

	a[ 0] += (a[ 1] >> 61);
	a[ 1] &= UINT64_C(0x1fffffffffffffff);
}


//--------------------------------------
static inline void bn_64to61rx_carry_23inpl(uint64_t a[23])
{
	a[21] += (a[22] >> 61);
	a[22] &= UINT64_C(0x1fffffffffffffff);

	a[20] += (a[21] >> 61);
	a[21] &= UINT64_C(0x1fffffffffffffff);

	a[19] += (a[20] >> 61);
	a[20] &= UINT64_C(0x1fffffffffffffff);

	a[18] += (a[19] >> 61);
	a[19] &= UINT64_C(0x1fffffffffffffff);

	a[17] += (a[18] >> 61);
	a[18] &= UINT64_C(0x1fffffffffffffff);

	a[16] += (a[17] >> 61);
	a[17] &= UINT64_C(0x1fffffffffffffff);

	a[15] += (a[16] >> 61);
	a[16] &= UINT64_C(0x1fffffffffffffff);

	a[14] += (a[15] >> 61);
	a[15] &= UINT64_C(0x1fffffffffffffff);

	a[13] += (a[14] >> 61);
	a[14] &= UINT64_C(0x1fffffffffffffff);

	a[12] += (a[13] >> 61);
	a[13] &= UINT64_C(0x1fffffffffffffff);

	a[11] += (a[12] >> 61);
	a[12] &= UINT64_C(0x1fffffffffffffff);

	a[10] += (a[11] >> 61);
	a[11] &= UINT64_C(0x1fffffffffffffff);

	a[ 9] += (a[10] >> 61);
	a[10] &= UINT64_C(0x1fffffffffffffff);

	a[ 8] += (a[ 9] >> 61);
	a[ 9] &= UINT64_C(0x1fffffffffffffff);

	a[ 7] += (a[ 8] >> 61);
	a[ 8] &= UINT64_C(0x1fffffffffffffff);

	a[ 6] += (a[ 7] >> 61);
	a[ 7] &= UINT64_C(0x1fffffffffffffff);

	a[ 5] += (a[ 6] >> 61);
	a[ 6] &= UINT64_C(0x1fffffffffffffff);

	a[ 4] += (a[ 5] >> 61);
	a[ 5] &= UINT64_C(0x1fffffffffffffff);

	a[ 3] += (a[ 4] >> 61);
	a[ 4] &= UINT64_C(0x1fffffffffffffff);

	a[ 2] += (a[ 3] >> 61);
	a[ 3] &= UINT64_C(0x1fffffffffffffff);

	a[ 1] += (a[ 2] >> 61);
	a[ 2] &= UINT64_C(0x1fffffffffffffff);

	a[ 0] += (a[ 1] >> 61);
	a[ 1] &= UINT64_C(0x1fffffffffffffff);
}


//--------------------------------------
static inline int bn_64to52rx_carry_inpl(uint64_t *a, unsigned int an)
{
	switch (a ? an : 0) {
//		case  6: bn_64to52rx_carry_6inpl(a);  return  6;
		case  8: bn_64to52rx_carry_8inpl(a);  return  8;
		case  9: bn_64to52rx_carry_9inpl(a);  return  9;
		case 10: bn_64to52rx_carry_10inpl(a); return 10;
//		case 12: bn_64to52rx_carry_12inpl(a); return 12;
		case 14: bn_64to52rx_carry_14inpl(a); return 14;
//		case 17: bn_64to52rx_carry_17inpl(a); return 17;
		case 20: bn_64to52rx_carry_20inpl(a); return 20;
		case 21: bn_64to52rx_carry_21inpl(a); return 21;
		case 27: bn_64to52rx_carry_27inpl(a); return 27;
		case 30: bn_64to52rx_carry_30inpl(a); return 30;
		case 40: bn_64to52rx_carry_40inpl(a); return 40;
		case 60: bn_64to52rx_carry_60inpl(a); return 60;

		default:
			return 0;
	}
}


//--------------------------------------
// r[0] >0 only if a[] had >= 321 bits (a[0] >= 16 b)
//
static inline void bn_rx61to64_6(uint64_t r[6], const uint64_t a[6])
{
	r[5] = (a[4] << 61) +  a[5];
	r[4] = (a[3] << 58) + (a[4] >>  3);
	r[3] = (a[2] << 55) + (a[3] >>  6);
	r[2] = (a[1] << 52) + (a[2] >>  9);
	r[1] = (a[0] << 49) + (a[1] >> 12);
	r[0] =                 a[0] >> 15;
}


//--------------------------------------
// r[0] >0 only if a[] had >= 512 bits (a[0] >= 24 b)
//
static inline void bn_rx61to64_9(uint64_t r[9], const uint64_t a[9])
{
	r[8] = (a[7] << 61) +  a[8];
	r[7] = (a[6] << 58) + (a[7] >>  3);
	r[6] = (a[5] << 55) + (a[6] >>  6);
	r[5] = (a[4] << 52) + (a[5] >>  9);
	r[4] = (a[3] << 49) + (a[4] >> 12);
	r[3] = (a[2] << 46) + (a[3] >> 15);
	r[2] = (a[1] << 43) + (a[2] >> 18);
	r[1] = (a[0] << 40) + (a[1] >> 21);
	r[0] =                 a[0] >> 24;
}


//--------------------------------------
// r[0] >0 only if a[] had >= 704 bits (a[0] >= 34 b)
//
static inline void bn_rx61to64_12(uint64_t r[12], const uint64_t a[12])
{
	r[11] = (a[10] << 61) +  a[11];
	r[10] = (a[ 9] << 58) + (a[10] >>  3);
	r[ 9] = (a[ 8] << 55) + (a[ 9] >>  6);
	r[ 8] = (a[ 7] << 52) + (a[ 8] >>  9);
	r[ 7] = (a[ 6] << 49) + (a[ 7] >> 12);
	r[ 6] = (a[ 5] << 46) + (a[ 6] >> 15);
	r[ 5] = (a[ 4] << 43) + (a[ 5] >> 18);
	r[ 4] = (a[ 3] << 40) + (a[ 4] >> 21);
	r[ 3] = (a[ 2] << 37) + (a[ 3] >> 24);
	r[ 2] = (a[ 1] << 34) + (a[ 2] >> 27);
	r[ 1] = (a[ 0] << 31) + (a[ 1] >> 30);
	r[ 0] =                  a[ 0] >> 33;
}


//--------------------------------------
// r[0] >0 only if a[] had >= 976 bits (a[0] >= 17 b)
//
static inline void bn_rx61to64_17(uint64_t r[17], const uint64_t a[17])
{
	r[16] = (a[15] << 61) +  a[16];
	r[15] = (a[14] << 58) + (a[15] >>  3);
	r[14] = (a[13] << 55) + (a[14] >>  6);
	r[13] = (a[12] << 52) + (a[13] >>  9);
	r[12] = (a[11] << 49) + (a[12] >> 12);
	r[11] = (a[10] << 46) + (a[11] >> 15);
	r[10] = (a[ 9] << 43) + (a[10] >> 18);
	r[ 9] = (a[ 8] << 40) + (a[ 9] >> 21);
	r[ 8] = (a[ 7] << 37) + (a[ 8] >> 24);
	r[ 7] = (a[ 6] << 34) + (a[ 7] >> 27);
	r[ 6] = (a[ 5] << 31) + (a[ 6] >> 30);
	r[ 5] = (a[ 4] << 28) + (a[ 5] >> 33);
	r[ 4] = (a[ 3] << 25) + (a[ 4] >> 36);
	r[ 3] = (a[ 2] << 22) + (a[ 3] >> 39);
	r[ 2] = (a[ 1] << 19) + (a[ 2] >> 42);
	r[ 1] = (a[ 0] << 16) + (a[ 1] >> 45);
	r[ 0] =                  a[ 0] >> 48;
}


//--------------------------------------
// r[0] >0 only if a[] had >= 1343 bits (a[0] >= 17 b)
// TODO: exact limits under our reduced range
//
static inline void bn_rx61to64_23(uint64_t r[23], const uint64_t a[23])
{
	r[22] = (a[21] << 61) +  a[22];
	r[21] = (a[20] << 58) + (a[21] >>  3);
	r[20] = (a[19] << 55) + (a[20] >>  6);
	r[19] = (a[18] << 52) + (a[19] >>  9);
	r[18] = (a[17] << 49) + (a[18] >> 12);
	r[17] = (a[16] << 46) + (a[17] >> 15);
	r[16] = (a[15] << 43) + (a[16] >> 18);
	r[15] = (a[14] << 40) + (a[15] >> 21);
	r[14] = (a[13] << 37) + (a[14] >> 24);
	r[13] = (a[12] << 34) + (a[13] >> 27);
	r[12] = (a[11] << 31) + (a[12] >> 30);
	r[11] = (a[10] << 28) + (a[11] >> 33);
	r[10] = (a[ 9] << 25) + (a[10] >> 36);
	r[ 9] = (a[ 8] << 22) + (a[ 9] >> 39);
	r[ 8] = (a[ 7] << 19) + (a[ 8] >> 42);
	r[ 7] = (a[ 6] << 16) + (a[ 7] >> 45);
	r[ 6] = (a[ 5] << 13) + (a[ 6] >> 48);
	r[ 5] = (a[ 4] << 10) + (a[ 5] >> 51);
	r[ 4] = (a[ 3] <<  7) + (a[ 4] >> 54);
	r[ 3] = (a[ 2] <<  4) + (a[ 3] >> 57);
							// note three terms
	r[ 2] = (a[ 1] <<  1) + (a[ 2] >> 60) + (a[ 0] << 62);
	r[ 1] = (a[ 0] >>  2) + (a[ 1] >> 63);      // note: >>, >>, not a typo
	r[ 0] = 0;                // a[0] >>2 consumed all non-zero bits of [0]
}


//--------------------------------------
static inline void bn_rx61to64_6inpl(uint64_t a[6])
{
	a[5] = (a[4] << 61) +  a[5];
	a[4] = (a[3] << 58) + (a[4] >>  3);
	a[3] = (a[2] << 55) + (a[3] >>  6);
	a[2] = (a[1] << 52) + (a[2] >>  9);
	a[1] = (a[0] << 49) + (a[1] >> 12);
	a[0] =                 a[0] >> 15;
}


/*--------------------------------------
 * full-radix add-digit
 */
static inline uint64_t bn_add1_6(uint64_t r[6],
                           const uint64_t a[6], uint64_t b)
{
	uint64_t c = 0;

	if (a && r) {
		r[5] =  a[5] +b;
		c    = (r[5] < b);

		r[4] =  a[4] +c;
		c    = (r[4] < c);

		r[3] =  a[3] +c;
		c    = (r[3] < c);

		r[2] =  a[2] +c;
		c    = (r[2] < c);

		r[1] =  a[1] +c;
		c    = (r[1] < c);

		r[0] =  a[0] +c;
		c    = (r[0] < c);
	}

	return c;
}


/*--------------------------------------
 * full-radix add-digit
 */
static inline uint64_t bn_add1_8(uint64_t r[8],
                           const uint64_t a[8], uint64_t b)
{
	uint64_t c = 0;

	if (a && r) {
		r[7] =  a[7] +b;
		c    = (r[7] < b);

		r[6] =  a[6] +c;
		c    = (r[6] < c);

		r[5] =  a[5] +c;
		c    = (r[5] < c);

		r[4] =  a[4] +c;
		c    = (r[4] < c);

		r[3] =  a[3] +c;
		c    = (r[3] < c);

		r[2] =  a[2] +b;
		c    = (r[2] < b);

		r[1] =  a[1] +c;
		c    = (r[1] < c);

		r[0] =  a[0] +c;
		c    = (r[0] < c);
	}

	return c;
}


/*--------------------------------------
 * full-radix add-digit, no carry propagation
 * caller must track above-radix bits' state
 */
static inline void bn_add_8nc(uint64_t r[8],
                        const uint64_t a[8],
                        const uint64_t b[8])
{
	if LIKELY(a && r && b) {
		r[7] = a[7] + b[7];
		r[6] = a[6] + b[6];
		r[5] = a[5] + b[5];
		r[4] = a[4] + b[4];
		r[3] = a[3] + b[3];
		r[2] = a[2] + b[2];
		r[1] = a[1] + b[1];
		r[0] = a[0] + b[0];
	}
}


/*--------------------------------------
 * full-radix add-digit, no carry propagation
 * caller must track above-radix bits' state
 */
static inline void bn_add_9nc(uint64_t r[9],
                        const uint64_t a[9],
                        const uint64_t b[9])
{
	if LIKELY(a && r && b) {
		r[8] = a[8] + b[8];
		r[7] = a[7] + b[7];
		r[6] = a[6] + b[6];
		r[5] = a[5] + b[5];
		r[4] = a[4] + b[4];
		r[3] = a[3] + b[3];
		r[2] = a[2] + b[2];
		r[1] = a[1] + b[1];
		r[0] = a[0] + b[0];
	}
}


/*--------------------------------------
 * full-radix add-digit, no carry propagation
 * caller must track above-radix bits' state
 */
static inline void bn_add_10nc(uint64_t r[10],
                         const uint64_t a[10],
                         const uint64_t b[10])
{
	if LIKELY(a && r && b) {
		r[9] = a[9] + b[9];
		r[8] = a[8] + b[8];
		r[7] = a[7] + b[7];
		r[6] = a[6] + b[6];
		r[5] = a[5] + b[5];
		r[4] = a[4] + b[4];
		r[3] = a[3] + b[3];
		r[2] = a[2] + b[2];
		r[1] = a[1] + b[1];
		r[0] = a[0] + b[0];
	}
}


/*--------------------------------------
 * full-radix add-digit, no carry propagation
 * caller must track above-radix bits' state
 */
static inline void bn_add_14nc(uint64_t r[14],
                         const uint64_t a[14],
                         const uint64_t b[14])
{
	if LIKELY(a && r && b) {
		r[13] = a[13] + b[13];
		r[12] = a[12] + b[12];
		r[11] = a[11] + b[11];
		r[10] = a[10] + b[10];
		r[ 9] = a[ 9] + b[ 9];
		r[ 8] = a[ 8] + b[ 8];
		r[ 7] = a[ 7] + b[ 7];
		r[ 6] = a[ 6] + b[ 6];
		r[ 5] = a[ 5] + b[ 5];
		r[ 4] = a[ 4] + b[ 4];
		r[ 3] = a[ 3] + b[ 3];
		r[ 2] = a[ 2] + b[ 2];
		r[ 1] = a[ 1] + b[ 1];
		r[ 0] = a[ 0] + b[ 0];
	}
}


/*--------------------------------------
 * full-radix add-digit, no carry propagation
 * caller must track above-radix bits' state
 */
static inline void bn_add_20nc(uint64_t r[20],
                         const uint64_t a[20],
                         const uint64_t b[20])
{
	if LIKELY(a && r && b) {
		r[19] = a[19] + b[19];
		r[18] = a[18] + b[18];
		r[17] = a[17] + b[17];
		r[16] = a[16] + b[16];
		r[15] = a[15] + b[15];
		r[14] = a[14] + b[14];
		r[13] = a[13] + b[13];
		r[12] = a[12] + b[12];
		r[11] = a[11] + b[11];
		r[10] = a[10] + b[10];
		r[ 9] = a[ 9] + b[ 9];
		r[ 8] = a[ 8] + b[ 8];
		r[ 7] = a[ 7] + b[ 7];
		r[ 6] = a[ 6] + b[ 6];
		r[ 5] = a[ 5] + b[ 5];
		r[ 4] = a[ 4] + b[ 4];
		r[ 3] = a[ 3] + b[ 3];
		r[ 2] = a[ 2] + b[ 2];
		r[ 1] = a[ 1] + b[ 1];
		r[ 0] = a[ 0] + b[ 0];
	}
}


/*--------------------------------------
 * full-radix add-digit, no carry propagation
 * caller must track above-radix bits' state
 */
static inline void bn_add_27nc(uint64_t r[27],
                         const uint64_t a[27],
                         const uint64_t b[27])
{
	if LIKELY(a && r && b) {
		r[26] = a[26] + b[26];
		r[25] = a[25] + b[25];
		r[24] = a[24] + b[24];
		r[23] = a[23] + b[23];
		r[22] = a[22] + b[22];
		r[21] = a[21] + b[21];
		r[20] = a[20] + b[20];
		r[19] = a[19] + b[19];
		r[18] = a[18] + b[18];
		r[17] = a[17] + b[17];
		r[16] = a[16] + b[16];
		r[15] = a[15] + b[15];
		r[14] = a[14] + b[14];
		r[13] = a[13] + b[13];
		r[12] = a[12] + b[12];
		r[11] = a[11] + b[11];
		r[10] = a[10] + b[10];
		r[ 9] = a[ 9] + b[ 9];
		r[ 8] = a[ 8] + b[ 8];
		r[ 7] = a[ 7] + b[ 7];
		r[ 6] = a[ 6] + b[ 6];
		r[ 5] = a[ 5] + b[ 5];
		r[ 4] = a[ 4] + b[ 4];
		r[ 3] = a[ 3] + b[ 3];
		r[ 2] = a[ 2] + b[ 2];
		r[ 1] = a[ 1] + b[ 1];
		r[ 0] = a[ 0] + b[ 0];
	}
}


/*--------------------------------------
 * full-radix add-digit, no carry propagation
 * caller must track above-radix bits' state
 */
static inline void bn_add_30nc(uint64_t r[30],
                         const uint64_t a[30],
                         const uint64_t b[30])
{
	if LIKELY(a && r && b) {
		r[29] = a[29] + b[29];
		r[28] = a[28] + b[28];
		r[27] = a[27] + b[27];
		r[26] = a[26] + b[26];
		r[25] = a[25] + b[25];
		r[24] = a[24] + b[24];
		r[23] = a[23] + b[23];
		r[22] = a[22] + b[22];
		r[21] = a[21] + b[21];
		r[20] = a[20] + b[20];
		r[19] = a[19] + b[19];
		r[18] = a[18] + b[18];
		r[17] = a[17] + b[17];
		r[16] = a[16] + b[16];
		r[15] = a[15] + b[15];
		r[14] = a[14] + b[14];
		r[13] = a[13] + b[13];
		r[12] = a[12] + b[12];
		r[11] = a[11] + b[11];
		r[10] = a[10] + b[10];
		r[ 9] = a[ 9] + b[ 9];
		r[ 8] = a[ 8] + b[ 8];
		r[ 7] = a[ 7] + b[ 7];
		r[ 6] = a[ 6] + b[ 6];
		r[ 5] = a[ 5] + b[ 5];
		r[ 4] = a[ 4] + b[ 4];
		r[ 3] = a[ 3] + b[ 3];
		r[ 2] = a[ 2] + b[ 2];
		r[ 1] = a[ 1] + b[ 1];
		r[ 0] = a[ 0] + b[ 0];
	}
}


/*--------------------------------------
 * full-radix add-digit, no carry propagation
 * caller must track above-radix bits' state
 */
static inline void bn_add_40nc(uint64_t r[40],
                         const uint64_t a[40],
                         const uint64_t b[40])
{
	if LIKELY(a && r && b) {
		r[39] = a[39] + b[39];
		r[38] = a[38] + b[38];
		r[37] = a[37] + b[37];
		r[36] = a[36] + b[36];
		r[35] = a[35] + b[35];
		r[34] = a[34] + b[34];
		r[33] = a[33] + b[33];
		r[32] = a[32] + b[32];
		r[31] = a[31] + b[31];
		r[30] = a[30] + b[30];
		r[29] = a[29] + b[29];
		r[28] = a[28] + b[28];
		r[27] = a[27] + b[27];
		r[26] = a[26] + b[26];
		r[25] = a[25] + b[25];
		r[24] = a[24] + b[24];
		r[23] = a[23] + b[23];
		r[22] = a[22] + b[22];
		r[21] = a[21] + b[21];
		r[20] = a[20] + b[20];
		r[19] = a[19] + b[19];
		r[18] = a[18] + b[18];
		r[17] = a[17] + b[17];
		r[16] = a[16] + b[16];
		r[15] = a[15] + b[15];
		r[14] = a[14] + b[14];
		r[13] = a[13] + b[13];
		r[12] = a[12] + b[12];
		r[11] = a[11] + b[11];
		r[10] = a[10] + b[10];
		r[ 9] = a[ 9] + b[ 9];
		r[ 8] = a[ 8] + b[ 8];
		r[ 7] = a[ 7] + b[ 7];
		r[ 6] = a[ 6] + b[ 6];
		r[ 5] = a[ 5] + b[ 5];
		r[ 4] = a[ 4] + b[ 4];
		r[ 3] = a[ 3] + b[ 3];
		r[ 2] = a[ 2] + b[ 2];
		r[ 1] = a[ 1] + b[ 1];
		r[ 0] = a[ 0] + b[ 0];
	}
}


/*--------------------------------------
 * full-radix add-digit, no carry propagation
 * caller must track above-radix bits' state
 */
static inline void bn_add_60nc(uint64_t r[60],
                         const uint64_t a[60],
                         const uint64_t b[60])
{
	if LIKELY(a && b && r) {
		r[59] =  a[59] + b[59];
		r[58] =  a[58] + b[58];
		r[57] =  a[57] + b[57];
		r[56] =  a[56] + b[56];
		r[55] =  a[55] + b[55];
		r[54] =  a[54] + b[54];
		r[53] =  a[53] + b[53];
		r[52] =  a[52] + b[52];
		r[51] =  a[51] + b[51];
		r[50] =  a[50] + b[50];
		r[49] =  a[49] + b[49];
		r[48] =  a[48] + b[48];
		r[47] =  a[47] + b[47];
		r[46] =  a[46] + b[46];
		r[45] =  a[45] + b[45];
		r[44] =  a[44] + b[44];
		r[43] =  a[43] + b[43];
		r[42] =  a[42] + b[42];
		r[41] =  a[41] + b[41];
		r[40] =  a[40] + b[40];
		r[39] =  a[39] + b[39];
		r[38] =  a[38] + b[38];
		r[37] =  a[37] + b[37];
		r[36] =  a[36] + b[36];
		r[35] =  a[35] + b[35];
		r[34] =  a[34] + b[34];
		r[33] =  a[33] + b[33];
		r[32] =  a[32] + b[32];
		r[31] =  a[31] + b[31];
		r[30] =  a[30] + b[30];
		r[29] =  a[29] + b[29];
		r[28] =  a[28] + b[28];
		r[27] =  a[27] + b[27];
		r[26] =  a[26] + b[26];
		r[25] =  a[25] + b[25];
		r[24] =  a[24] + b[24];
		r[23] =  a[23] + b[23];
		r[22] =  a[22] + b[22];
		r[21] =  a[21] + b[21];
		r[20] =  a[20] + b[20];
		r[19] =  a[19] + b[19];
		r[18] =  a[18] + b[18];
		r[17] =  a[17] + b[17];
		r[16] =  a[16] + b[16];
		r[15] =  a[15] + b[15];
		r[14] =  a[14] + b[14];
		r[13] =  a[13] + b[13];
		r[12] =  a[12] + b[12];
		r[11] =  a[11] + b[11];
		r[10] =  a[10] + b[10];
		r[ 9] =  a[ 9] + b[ 9];
		r[ 8] =  a[ 8] + b[ 8];
		r[ 7] =  a[ 7] + b[ 7];
		r[ 6] =  a[ 6] + b[ 6];
		r[ 5] =  a[ 5] + b[ 5];
		r[ 4] =  a[ 4] + b[ 4];
		r[ 3] =  a[ 3] + b[ 3];
		r[ 2] =  a[ 2] + b[ 2];
		r[ 1] =  a[ 1] + b[ 1];
		r[ 0] =  a[ 0] + b[ 0];
	}
}


//--------------------------------------
static inline
uint64_t bn_modn1_32m1(const uint64_t a[32], uint64_t modn, uint64_t shf)
{
	uint128_t tmp[32];

	tmp[ 0] = (uint128_t) a[ 0];
	tmp[ 1] = (uint128_t) a[ 1];
	tmp[ 2] = (uint128_t) a[ 2];
	tmp[ 3] = (uint128_t) a[ 3];
	tmp[ 4] = (uint128_t) a[ 4];
	tmp[ 5] = (uint128_t) a[ 5];
	tmp[ 6] = (uint128_t) a[ 6];
	tmp[ 7] = (uint128_t) a[ 7];
	tmp[ 8] = (uint128_t) a[ 8];
	tmp[ 9] = (uint128_t) a[ 9];
	tmp[10] = (uint128_t) a[10];
	tmp[11] = (uint128_t) a[11];
	tmp[12] = (uint128_t) a[12];
	tmp[13] = (uint128_t) a[13];
	tmp[14] = (uint128_t) a[14];
	tmp[15] = (uint128_t) a[15];
	tmp[16] = (uint128_t) a[16];
	tmp[17] = (uint128_t) a[17];
	tmp[18] = (uint128_t) a[18];
	tmp[19] = (uint128_t) a[19];
	tmp[20] = (uint128_t) a[20];
	tmp[21] = (uint128_t) a[21];
	tmp[22] = (uint128_t) a[22];
	tmp[23] = (uint128_t) a[23];
	tmp[24] = (uint128_t) a[24];
	tmp[25] = (uint128_t) a[25];
	tmp[26] = (uint128_t) a[26];
	tmp[27] = (uint128_t) a[27];
	tmp[28] = (uint128_t) a[28];
	tmp[29] = (uint128_t) a[29];
	tmp[30] = (uint128_t) a[30];
		/**/
	tmp[31] = a[31] % modn;

	tmp[ 1] += (tmp[ 0]  * shf) % modn;
	tmp[ 1] %=  modn;
	tmp[ 2] += (tmp[ 1]  * shf) % modn;
	tmp[ 2] %=  modn;
	tmp[ 3] += (tmp[ 2]  * shf) % modn;
	tmp[ 3] %=  modn;
	tmp[ 4] += (tmp[ 3]  * shf) % modn;
	tmp[ 4] %=  modn;
	tmp[ 5] += (tmp[ 4]  * shf) % modn;
	tmp[ 5] %=  modn;
	tmp[ 6] += (tmp[ 5]  * shf) % modn;
	tmp[ 6] %=  modn;
	tmp[ 7] += (tmp[ 6]  * shf) % modn;
	tmp[ 7] %=  modn;
	tmp[ 8] += (tmp[ 7]  * shf) % modn;
	tmp[ 8] %=  modn;
	tmp[ 9] += (tmp[ 8]  * shf) % modn;
	tmp[ 9] %=  modn;
	tmp[10] += (tmp[ 9]  * shf) % modn;
	tmp[10] %=  modn;
	tmp[11] += (tmp[10]  * shf) % modn;
	tmp[11] %=  modn;
	tmp[12] += (tmp[11]  * shf) % modn;
	tmp[12] %=  modn;
	tmp[13] += (tmp[12]  * shf) % modn;
	tmp[13] %=  modn;
	tmp[14] += (tmp[13]  * shf) % modn;
	tmp[14] %=  modn;
	tmp[15] += (tmp[14]  * shf) % modn;
	tmp[15] %=  modn;
	tmp[16] += (tmp[15]  * shf) % modn;
	tmp[16] %=  modn;
	tmp[17] += (tmp[16]  * shf) % modn;
	tmp[17] %=  modn;
	tmp[18] += (tmp[17]  * shf) % modn;
	tmp[18] %=  modn;
	tmp[19] += (tmp[18]  * shf) % modn;
	tmp[19] %=  modn;
	tmp[20] += (tmp[19]  * shf) % modn;
	tmp[20] %=  modn;
	tmp[21] += (tmp[20]  * shf) % modn;
	tmp[21] %=  modn;
	tmp[22] += (tmp[21]  * shf) % modn;
	tmp[22] %=  modn;
	tmp[23] += (tmp[22]  * shf) % modn;
	tmp[23] %=  modn;
	tmp[24] += (tmp[23]  * shf) % modn;
	tmp[24] %=  modn;
	tmp[25] += (tmp[24]  * shf) % modn;
	tmp[25] %=  modn;
	tmp[26] += (tmp[25]  * shf) % modn;
	tmp[26] %=  modn;
	tmp[27] += (tmp[26]  * shf) % modn;
	tmp[27] %=  modn;
	tmp[28] += (tmp[27]  * shf) % modn;
	tmp[28] %=  modn;
	tmp[29] += (tmp[28]  * shf) % modn;
	tmp[29] %=  modn;
	tmp[30] += (tmp[29]  * shf) % modn;
	tmp[30] %=  modn;
	tmp[31] += (tmp[30]  * shf) % modn;
	tmp[31] %=  modn;

	return (uint64_t) tmp[31];
}


//--------------------------------------
static inline
uint64_t bn_modn1_48m1(const uint64_t a[48], uint64_t modn, uint64_t shf)
{
	uint128_t tmp[48];

	tmp[ 0] = (uint128_t) a[ 0];
	tmp[ 1] = (uint128_t) a[ 1];
	tmp[ 2] = (uint128_t) a[ 2];
	tmp[ 3] = (uint128_t) a[ 3];
	tmp[ 4] = (uint128_t) a[ 4];
	tmp[ 5] = (uint128_t) a[ 5];
	tmp[ 6] = (uint128_t) a[ 6];
	tmp[ 7] = (uint128_t) a[ 7];
	tmp[ 8] = (uint128_t) a[ 8];
	tmp[ 9] = (uint128_t) a[ 9];
	tmp[10] = (uint128_t) a[10];
	tmp[11] = (uint128_t) a[11];
	tmp[12] = (uint128_t) a[12];
	tmp[13] = (uint128_t) a[13];
	tmp[14] = (uint128_t) a[14];
	tmp[15] = (uint128_t) a[15];
	tmp[16] = (uint128_t) a[16];
	tmp[17] = (uint128_t) a[17];
	tmp[18] = (uint128_t) a[18];
	tmp[19] = (uint128_t) a[19];
	tmp[20] = (uint128_t) a[20];
	tmp[21] = (uint128_t) a[21];
	tmp[22] = (uint128_t) a[22];
	tmp[23] = (uint128_t) a[23];
	tmp[24] = (uint128_t) a[24];
	tmp[25] = (uint128_t) a[25];
	tmp[26] = (uint128_t) a[26];
	tmp[27] = (uint128_t) a[27];
	tmp[28] = (uint128_t) a[28];
	tmp[29] = (uint128_t) a[29];
	tmp[30] = (uint128_t) a[30];
	tmp[31] = (uint128_t) a[31];
	tmp[32] = (uint128_t) a[32];
	tmp[33] = (uint128_t) a[33];
	tmp[34] = (uint128_t) a[34];
	tmp[35] = (uint128_t) a[35];
	tmp[36] = (uint128_t) a[36];
	tmp[37] = (uint128_t) a[37];
	tmp[38] = (uint128_t) a[38];
	tmp[39] = (uint128_t) a[39];
	tmp[40] = (uint128_t) a[40];
	tmp[41] = (uint128_t) a[41];
	tmp[42] = (uint128_t) a[42];
	tmp[43] = (uint128_t) a[43];
	tmp[44] = (uint128_t) a[44];
	tmp[45] = (uint128_t) a[45];
	tmp[46] = (uint128_t) a[46];
		/**/
	tmp[47] = a[47] % modn;

	tmp[ 1] += (tmp[ 0]  * shf) % modn;
	tmp[ 1] %=  modn;
	tmp[ 2] += (tmp[ 1]  * shf) % modn;
	tmp[ 2] %=  modn;
	tmp[ 3] += (tmp[ 2]  * shf) % modn;
	tmp[ 3] %=  modn;
	tmp[ 4] += (tmp[ 3]  * shf) % modn;
	tmp[ 4] %=  modn;
	tmp[ 5] += (tmp[ 4]  * shf) % modn;
	tmp[ 5] %=  modn;
	tmp[ 6] += (tmp[ 5]  * shf) % modn;
	tmp[ 6] %=  modn;
	tmp[ 7] += (tmp[ 6]  * shf) % modn;
	tmp[ 7] %=  modn;
	tmp[ 8] += (tmp[ 7]  * shf) % modn;
	tmp[ 8] %=  modn;
	tmp[ 9] += (tmp[ 8]  * shf) % modn;
	tmp[ 9] %=  modn;
	tmp[10] += (tmp[ 9]  * shf) % modn;
	tmp[10] %=  modn;
	tmp[11] += (tmp[10]  * shf) % modn;
	tmp[11] %=  modn;
	tmp[12] += (tmp[11]  * shf) % modn;
	tmp[12] %=  modn;
	tmp[13] += (tmp[12]  * shf) % modn;
	tmp[13] %=  modn;
	tmp[14] += (tmp[13]  * shf) % modn;
	tmp[14] %=  modn;
	tmp[15] += (tmp[14]  * shf) % modn;
	tmp[15] %=  modn;
	tmp[16] += (tmp[15]  * shf) % modn;
	tmp[16] %=  modn;
	tmp[17] += (tmp[16]  * shf) % modn;
	tmp[17] %=  modn;
	tmp[18] += (tmp[17]  * shf) % modn;
	tmp[18] %=  modn;
	tmp[19] += (tmp[18]  * shf) % modn;
	tmp[19] %=  modn;
	tmp[20] += (tmp[19]  * shf) % modn;
	tmp[20] %=  modn;
	tmp[21] += (tmp[20]  * shf) % modn;
	tmp[21] %=  modn;
	tmp[22] += (tmp[21]  * shf) % modn;
	tmp[22] %=  modn;
	tmp[23] += (tmp[22]  * shf) % modn;
	tmp[23] %=  modn;
	tmp[24] += (tmp[23]  * shf) % modn;
	tmp[24] %=  modn;
	tmp[25] += (tmp[24]  * shf) % modn;
	tmp[25] %=  modn;
	tmp[26] += (tmp[25]  * shf) % modn;
	tmp[26] %=  modn;
	tmp[27] += (tmp[26]  * shf) % modn;
	tmp[27] %=  modn;
	tmp[28] += (tmp[27]  * shf) % modn;
	tmp[28] %=  modn;
	tmp[29] += (tmp[28]  * shf) % modn;
	tmp[29] %=  modn;
	tmp[30] += (tmp[29]  * shf) % modn;
	tmp[30] %=  modn;
	tmp[31] += (tmp[30]  * shf) % modn;
	tmp[31] %=  modn;
	tmp[32] += (tmp[31]  * shf) % modn;
	tmp[32] %=  modn;
	tmp[33] += (tmp[32]  * shf) % modn;
	tmp[33] %=  modn;
	tmp[34] += (tmp[33]  * shf) % modn;
	tmp[34] %=  modn;
	tmp[35] += (tmp[34]  * shf) % modn;
	tmp[35] %=  modn;
	tmp[36] += (tmp[35]  * shf) % modn;
	tmp[36] %=  modn;
	tmp[37] += (tmp[36]  * shf) % modn;
	tmp[37] %=  modn;
	tmp[38] += (tmp[37]  * shf) % modn;
	tmp[38] %=  modn;
	tmp[39] += (tmp[38]  * shf) % modn;
	tmp[39] %=  modn;
	tmp[40] += (tmp[39]  * shf) % modn;
	tmp[40] %=  modn;
	tmp[41] += (tmp[40]  * shf) % modn;
	tmp[41] %=  modn;
	tmp[42] += (tmp[41]  * shf) % modn;
	tmp[42] %=  modn;
	tmp[43] += (tmp[42]  * shf) % modn;
	tmp[43] %=  modn;
	tmp[44] += (tmp[43]  * shf) % modn;
	tmp[44] %=  modn;
	tmp[45] += (tmp[44]  * shf) % modn;
	tmp[45] %=  modn;
	tmp[46] += (tmp[45]  * shf) % modn;
	tmp[46] %=  modn;
	tmp[47] += (tmp[46]  * shf) % modn;
	tmp[47] %=  modn;

	return (uint64_t) tmp[47];
}


//--------------------------------------
static inline
uint64_t bn_modn1_64m1(const uint64_t a[64], uint64_t modn, uint64_t shf)
{
	uint128_t tmp[64];

	tmp[ 0] = (uint128_t) a[ 0];
	tmp[ 1] = (uint128_t) a[ 1];
	tmp[ 2] = (uint128_t) a[ 2];
	tmp[ 3] = (uint128_t) a[ 3];
	tmp[ 4] = (uint128_t) a[ 4];
	tmp[ 5] = (uint128_t) a[ 5];
	tmp[ 6] = (uint128_t) a[ 6];
	tmp[ 7] = (uint128_t) a[ 7];
	tmp[ 8] = (uint128_t) a[ 8];
	tmp[ 9] = (uint128_t) a[ 9];
	tmp[10] = (uint128_t) a[10];
	tmp[11] = (uint128_t) a[11];
	tmp[12] = (uint128_t) a[12];
	tmp[13] = (uint128_t) a[13];
	tmp[14] = (uint128_t) a[14];
	tmp[15] = (uint128_t) a[15];
	tmp[16] = (uint128_t) a[16];
	tmp[17] = (uint128_t) a[17];
	tmp[18] = (uint128_t) a[18];
	tmp[19] = (uint128_t) a[19];
	tmp[20] = (uint128_t) a[20];
	tmp[21] = (uint128_t) a[21];
	tmp[22] = (uint128_t) a[22];
	tmp[23] = (uint128_t) a[23];
	tmp[24] = (uint128_t) a[24];
	tmp[25] = (uint128_t) a[25];
	tmp[26] = (uint128_t) a[26];
	tmp[27] = (uint128_t) a[27];
	tmp[28] = (uint128_t) a[28];
	tmp[29] = (uint128_t) a[29];
	tmp[30] = (uint128_t) a[30];
	tmp[31] = (uint128_t) a[31];
	tmp[32] = (uint128_t) a[32];
	tmp[33] = (uint128_t) a[33];
	tmp[34] = (uint128_t) a[34];
	tmp[35] = (uint128_t) a[35];
	tmp[36] = (uint128_t) a[36];
	tmp[37] = (uint128_t) a[37];
	tmp[38] = (uint128_t) a[38];
	tmp[39] = (uint128_t) a[39];
	tmp[40] = (uint128_t) a[40];
	tmp[41] = (uint128_t) a[41];
	tmp[42] = (uint128_t) a[42];
	tmp[43] = (uint128_t) a[43];
	tmp[44] = (uint128_t) a[44];
	tmp[45] = (uint128_t) a[45];
	tmp[46] = (uint128_t) a[46];
	tmp[47] = (uint128_t) a[47];
	tmp[48] = (uint128_t) a[48];
	tmp[49] = (uint128_t) a[49];
	tmp[50] = (uint128_t) a[50];
	tmp[51] = (uint128_t) a[51];
	tmp[52] = (uint128_t) a[52];
	tmp[53] = (uint128_t) a[53];
	tmp[54] = (uint128_t) a[54];
	tmp[55] = (uint128_t) a[55];
	tmp[56] = (uint128_t) a[56];
	tmp[57] = (uint128_t) a[57];
	tmp[58] = (uint128_t) a[58];
	tmp[59] = (uint128_t) a[59];
	tmp[60] = (uint128_t) a[60];
	tmp[61] = (uint128_t) a[61];
	tmp[62] = (uint128_t) a[62];
		/**/
	tmp[63] = a[63] % modn;

	tmp[ 1] += (tmp[ 0]  * shf) % modn;
	tmp[ 1] %=  modn;
	tmp[ 2] += (tmp[ 1]  * shf) % modn;
	tmp[ 2] %=  modn;
	tmp[ 3] += (tmp[ 2]  * shf) % modn;
	tmp[ 3] %=  modn;
	tmp[ 4] += (tmp[ 3]  * shf) % modn;
	tmp[ 4] %=  modn;
	tmp[ 5] += (tmp[ 4]  * shf) % modn;
	tmp[ 5] %=  modn;
	tmp[ 6] += (tmp[ 5]  * shf) % modn;
	tmp[ 6] %=  modn;
	tmp[ 7] += (tmp[ 6]  * shf) % modn;
	tmp[ 7] %=  modn;
	tmp[ 8] += (tmp[ 7]  * shf) % modn;
	tmp[ 8] %=  modn;
	tmp[ 9] += (tmp[ 8]  * shf) % modn;
	tmp[ 9] %=  modn;
	tmp[10] += (tmp[ 9]  * shf) % modn;
	tmp[10] %=  modn;
	tmp[11] += (tmp[10]  * shf) % modn;
	tmp[11] %=  modn;
	tmp[12] += (tmp[11]  * shf) % modn;
	tmp[12] %=  modn;
	tmp[13] += (tmp[12]  * shf) % modn;
	tmp[13] %=  modn;
	tmp[14] += (tmp[13]  * shf) % modn;
	tmp[14] %=  modn;
	tmp[15] += (tmp[14]  * shf) % modn;
	tmp[15] %=  modn;
	tmp[16] += (tmp[15]  * shf) % modn;
	tmp[16] %=  modn;
	tmp[17] += (tmp[16]  * shf) % modn;
	tmp[17] %=  modn;
	tmp[18] += (tmp[17]  * shf) % modn;
	tmp[18] %=  modn;
	tmp[19] += (tmp[18]  * shf) % modn;
	tmp[19] %=  modn;
	tmp[20] += (tmp[19]  * shf) % modn;
	tmp[20] %=  modn;
	tmp[21] += (tmp[20]  * shf) % modn;
	tmp[21] %=  modn;
	tmp[22] += (tmp[21]  * shf) % modn;
	tmp[22] %=  modn;
	tmp[23] += (tmp[22]  * shf) % modn;
	tmp[23] %=  modn;
	tmp[24] += (tmp[23]  * shf) % modn;
	tmp[24] %=  modn;
	tmp[25] += (tmp[24]  * shf) % modn;
	tmp[25] %=  modn;
	tmp[26] += (tmp[25]  * shf) % modn;
	tmp[26] %=  modn;
	tmp[27] += (tmp[26]  * shf) % modn;
	tmp[27] %=  modn;
	tmp[28] += (tmp[27]  * shf) % modn;
	tmp[28] %=  modn;
	tmp[29] += (tmp[28]  * shf) % modn;
	tmp[29] %=  modn;
	tmp[30] += (tmp[29]  * shf) % modn;
	tmp[30] %=  modn;
	tmp[31] += (tmp[30]  * shf) % modn;
	tmp[31] %=  modn;
	tmp[32] += (tmp[31]  * shf) % modn;
	tmp[32] %=  modn;
	tmp[33] += (tmp[32]  * shf) % modn;
	tmp[33] %=  modn;
	tmp[34] += (tmp[33]  * shf) % modn;
	tmp[34] %=  modn;
	tmp[35] += (tmp[34]  * shf) % modn;
	tmp[35] %=  modn;
	tmp[36] += (tmp[35]  * shf) % modn;
	tmp[36] %=  modn;
	tmp[37] += (tmp[36]  * shf) % modn;
	tmp[37] %=  modn;
	tmp[38] += (tmp[37]  * shf) % modn;
	tmp[38] %=  modn;
	tmp[39] += (tmp[38]  * shf) % modn;
	tmp[39] %=  modn;
	tmp[40] += (tmp[39]  * shf) % modn;
	tmp[40] %=  modn;
	tmp[41] += (tmp[40]  * shf) % modn;
	tmp[41] %=  modn;
	tmp[42] += (tmp[41]  * shf) % modn;
	tmp[42] %=  modn;
	tmp[43] += (tmp[42]  * shf) % modn;
	tmp[43] %=  modn;
	tmp[44] += (tmp[43]  * shf) % modn;
	tmp[44] %=  modn;
	tmp[45] += (tmp[44]  * shf) % modn;
	tmp[45] %=  modn;
	tmp[46] += (tmp[45]  * shf) % modn;
	tmp[46] %=  modn;
	tmp[47] += (tmp[46]  * shf) % modn;
	tmp[47] %=  modn;
	tmp[48] += (tmp[47]  * shf) % modn;
	tmp[48] %=  modn;
	tmp[49] += (tmp[48]  * shf) % modn;
	tmp[49] %=  modn;
	tmp[50] += (tmp[49]  * shf) % modn;
	tmp[50] %=  modn;
	tmp[51] += (tmp[50]  * shf) % modn;
	tmp[51] %=  modn;
	tmp[52] += (tmp[51]  * shf) % modn;
	tmp[52] %=  modn;
	tmp[53] += (tmp[52]  * shf) % modn;
	tmp[53] %=  modn;
	tmp[54] += (tmp[53]  * shf) % modn;
	tmp[54] %=  modn;
	tmp[55] += (tmp[54]  * shf) % modn;
	tmp[55] %=  modn;
	tmp[56] += (tmp[55]  * shf) % modn;
	tmp[56] %=  modn;
	tmp[57] += (tmp[56]  * shf) % modn;
	tmp[57] %=  modn;
	tmp[58] += (tmp[57]  * shf) % modn;
	tmp[58] %=  modn;
	tmp[59] += (tmp[58]  * shf) % modn;
	tmp[59] %=  modn;
	tmp[60] += (tmp[59]  * shf) % modn;
	tmp[60] %=  modn;
	tmp[61] += (tmp[60]  * shf) % modn;
	tmp[61] %=  modn;
	tmp[62] += (tmp[61]  * shf) % modn;
	tmp[62] %=  modn;
	tmp[63] += (tmp[62]  * shf) % modn;
	tmp[63] %=  modn;

	return (uint64_t) tmp[63];
}


//--------------------------------
static inline void bn_add1_32(uint64_t r[32],
                        const uint64_t a[32], uint64_t b)
{
	if (a && r) {
		r[31] =  a[31] +b; b = (r[31] < b);
		r[30] =  a[30] +b; b = (r[30] < b);
		r[29] =  a[29] +b; b = (r[29] < b);
		r[28] =  a[28] +b; b = (r[28] < b);
		r[27] =  a[27] +b; b = (r[27] < b);
		r[26] =  a[26] +b; b = (r[26] < b);
		r[25] =  a[25] +b; b = (r[25] < b);
		r[24] =  a[24] +b; b = (r[24] < b);
		r[23] =  a[23] +b; b = (r[23] < b);
		r[22] =  a[22] +b; b = (r[22] < b);
		r[21] =  a[21] +b; b = (r[21] < b);
		r[20] =  a[20] +b; b = (r[20] < b);
		r[19] =  a[19] +b; b = (r[19] < b);
		r[18] =  a[18] +b; b = (r[18] < b);
		r[17] =  a[17] +b; b = (r[17] < b);
		r[16] =  a[16] +b; b = (r[16] < b);
		r[15] =  a[15] +b; b = (r[15] < b);
		r[14] =  a[14] +b; b = (r[14] < b);
		r[13] =  a[13] +b; b = (r[13] < b);
		r[12] =  a[12] +b; b = (r[12] < b);
		r[11] =  a[11] +b; b = (r[11] < b);
		r[10] =  a[10] +b; b = (r[10] < b);
		r[ 9] =  a[ 9] +b; b = (r[ 9] < b);
		r[ 8] =  a[ 8] +b; b = (r[ 8] < b);
		r[ 7] =  a[ 7] +b; b = (r[ 7] < b);
		r[ 6] =  a[ 6] +b; b = (r[ 6] < b);
		r[ 5] =  a[ 5] +b; b = (r[ 5] < b);
		r[ 4] =  a[ 4] +b; b = (r[ 4] < b);
		r[ 3] =  a[ 3] +b; b = (r[ 3] < b);
		r[ 2] =  a[ 2] +b; b = (r[ 2] < b);
		r[ 1] =  a[ 1] +b; b = (r[ 1] < b);
		r[ 0] =  a[ 0] +b; b = (r[ 0] < b);
	}
}


//--------------------------------------
static inline int bn_has_any_msbit1_8(const uint64_t a[8])
{
	return a &&
	       ((a[0]| a[1]| a[2]| a[3]| a[4]| a[5]| a[6]| a[7]) >> 63);
}


//--------------------------------------
static inline int bn_has_any_msbit1_9(const uint64_t a[9])
{
	return a &&
	       ((a[0]| a[1]| a[2]| a[3]| a[4]| a[5]| a[6]| a[7]| a[8]) >> 63);
}


//--------------------------------------
static inline int bn_has_any_msbit1_10(const uint64_t a[10])
{
	return a &&
	       ((a[0]| a[1]| a[2]| a[3]| a[4]| a[5]| a[6]| a[7]|
	         a[8]| a[9]) >> 63);
}


//--------------------------------------
static inline int bn_has_any_msbit1_14(const uint64_t a[14])
{
	return a &&
	       ((a[0]| a[1]| a[ 2]| a[ 3]| a[ 4]| a[ 5]| a[6]| a[7]|
	         a[8]| a[9]| a[10]| a[11]| a[12]| a[13]) >> 63);
}


//--------------------------------------
static inline int bn_has_any_msbit1_20(const uint64_t a[20])
{
	return a &&
	       ((a[ 0]| a[ 1]| a[ 2]| a[ 3]| a[ 4]| a[ 5]| a[ 6]| a[ 7]|
	         a[ 8]| a[ 9]| a[10]| a[11]| a[12]| a[13]| a[14]| a[15]|
	         a[16]| a[17]| a[18]| a[19]) >> 63);
}


//--------------------------------------
static inline int bn_has_any_msbit1_22(const uint64_t a[22])
{
	return a &&
	       ((a[ 0]| a[ 1]| a[ 2]| a[ 3]| a[ 4]| a[ 5]| a[ 6]| a[ 7]|
	         a[ 8]| a[ 9]| a[10]| a[11]| a[12]| a[13]| a[14]| a[15]|
	         a[16]| a[17]| a[18]| a[19]| a[20]| a[21]) >> 63);
}


//--------------------------------------
static inline int bn_has_any_msbit1_27(const uint64_t a[27])
{
	return a &&
	       ((a[ 0]| a[ 1]| a[ 2]| a[ 3]| a[ 4]| a[ 5]| a[ 6]| a[ 7]|
	         a[ 8]| a[ 9]| a[10]| a[11]| a[12]| a[13]| a[14]| a[15]|
	         a[16]| a[17]| a[18]| a[19]| a[20]| a[21]| a[22]| a[23]|
	         a[24]| a[25]| a[26]) >> 63);
}


//--------------------------------------
static inline int bn_has_any_msbit1_30(const uint64_t a[30])
{
	return a &&
	       ((a[ 0]| a[ 1]| a[ 2]| a[ 3]| a[ 4]| a[ 5]| a[ 6]| a[ 7]|
	         a[ 8]| a[ 9]| a[10]| a[11]| a[12]| a[13]| a[14]| a[15]|
	         a[16]| a[17]| a[18]| a[19]| a[20]| a[21]| a[22]| a[23]|
	         a[24]| a[25]| a[26]| a[27]| a[28]| a[29]) >> 63);
}


//--------------------------------------
static inline int bn_has_any_msbit1_40(const uint64_t a[40])
{
	return a &&
	       ((a[ 0]| a[ 1]| a[ 2]| a[ 3]| a[ 4]| a[ 5]| a[ 6]| a[ 7]|
	         a[ 8]| a[ 9]| a[10]| a[11]| a[12]| a[13]| a[14]| a[15]|
	         a[16]| a[17]| a[18]| a[19]| a[20]| a[21]| a[22]| a[23]|
	         a[24]| a[25]| a[26]| a[27]| a[28]| a[29]| a[30]| a[31]|
	         a[32]| a[33]| a[34]| a[35]| a[36]| a[37]| a[38]| a[39]) >> 63);
}


//--------------------------------------
static inline int bn_has_any_msbit1_60(const uint64_t a[60])
{
	return a &&
	       ((a[ 0]| a[ 1]| a[ 2]| a[ 3]| a[ 4]| a[ 5]| a[ 6]| a[ 7]|
	         a[ 8]| a[ 9]| a[10]| a[11]| a[12]| a[13]| a[14]| a[15]|
	         a[16]| a[17]| a[18]| a[19]| a[20]| a[21]| a[22]| a[23]|
	         a[24]| a[25]| a[26]| a[27]| a[28]| a[29]| a[30]| a[31]|
	         a[32]| a[33]| a[34]| a[35]| a[36]| a[37]| a[38]| a[39]|
	         a[40]| a[41]| a[42]| a[43]| a[44]| a[45]| a[46]| a[47]|
	         a[48]| a[49]| a[50]| a[51]| a[52]| a[53]| a[54]| a[55]|
	         a[56]| a[57]| a[58]| a[59]) >> 63);

}


/*--------------------------------------
 * full-radix add-digit
 */
static inline uint64_t bn_add1_11(uint64_t r[11],
                            const uint64_t a[11], uint64_t b)
{
	uint64_t c = 0;

	if (a && r) {
		r[10] =  a[10] +b;
		c     = (r[10] < b);
		r[ 9] =  a[ 9] +c;
		c     = (r[ 9] < c);
		r[ 8] =  a[ 8] +c;
		c     = (r[ 8] < c);
		r[ 7] =  a[ 7] +c;
		c     = (r[ 7] < c);
		r[ 6] =  a[ 6] +c;
		c     = (r[ 6] < c);
		r[ 5] =  a[ 5] +c;
		c     = (r[ 5] < c);
		r[ 4] =  a[ 4] +c;
		c     = (r[ 4] < c);
		r[ 3] =  a[ 3] +c;
		c     = (r[ 3] < c);
		r[ 2] =  a[ 2] +b;
		c     = (r[ 2] < b);
		r[ 1] =  a[ 1] +c;
		c     = (r[ 1] < c);
		r[ 0] =  a[ 0] +c;
		c     = (r[ 0] < c);
	}

	return c;
}


/*--------------------------------------
 * full-radix add-digit
 */
static inline uint64_t bn_add1_16(uint64_t r[16],
                            const uint64_t a[16], uint64_t b)
{
	uint64_t c = 0;

	if (a && r) {
		r[15] =  a[15] +b; c = (r[15] < b);
		r[14] =  a[14] +c; c = (r[14] < c);
		r[13] =  a[13] +c; c = (r[13] < c);
		r[12] =  a[12] +c; c = (r[12] < c);
		r[11] =  a[11] +c; c = (r[11] < c);
		r[10] =  a[10] +c; c = (r[10] < c);
		r[ 9] =  a[ 9] +c; c = (r[ 9] < c);
		r[ 8] =  a[ 8] +c; c = (r[ 8] < c);
		r[ 7] =  a[ 7] +c; c = (r[ 7] < c);
		r[ 6] =  a[ 6] +c; c = (r[ 6] < c);
		r[ 5] =  a[ 5] +c; c = (r[ 5] < c);
		r[ 4] =  a[ 4] +c; c = (r[ 4] < c);
		r[ 3] =  a[ 3] +c; c = (r[ 3] < c);
		r[ 2] =  a[ 2] +c; c = (r[ 2] < c);
		r[ 1] =  a[ 1] +c; c = (r[ 1] < c);
		r[ 0] =  a[ 0] +c; c = (r[ 0] < c);
	}
	return c;
}


//--------------------------------------
static inline uint64_t bn_add1_22(uint64_t r[22],
                            const uint64_t a[22], uint64_t b)
{
	if (a && r) {
		r[21] = a[21] +b; b = (r[21] < b);
		r[20] = a[20] +b; b = (r[20] < b);
		r[19] = a[19] +b; b = (r[19] < b);
		r[18] = a[18] +b; b = (r[18] < b);
		r[17] = a[17] +b; b = (r[17] < b);
		r[16] = a[16] +b; b = (r[16] < b);
		r[15] = a[15] +b; b = (r[15] < b);
		r[14] = a[14] +b; b = (r[14] < b);
		r[13] = a[13] +b; b = (r[13] < b);
		r[12] = a[12] +b; b = (r[12] < b);
		r[11] = a[11] +b; b = (r[11] < b);
		r[10] = a[10] +b; b = (r[10] < b);
		r[ 9] = a[ 9] +b; b = (r[ 9] < b);
		r[ 8] = a[ 8] +b; b = (r[ 8] < b);
		r[ 7] = a[ 7] +b; b = (r[ 7] < b);
		r[ 6] = a[ 6] +b; b = (r[ 6] < b);
		r[ 5] = a[ 5] +b; b = (r[ 5] < b);
		r[ 4] = a[ 4] +b; b = (r[ 4] < b);
		r[ 3] = a[ 3] +b; b = (r[ 3] < b);
		r[ 2] = a[ 2] +b; b = (r[ 2] < b);
		r[ 1] = a[ 1] +b; b = (r[ 1] < b);
		r[ 0] = a[ 0] +b; b = (r[ 0] < b);
	}

	return b;
}


//--------------------------------------
static inline
uint64_t bn_add1_24(uint64_t r[24], const uint64_t a[24], uint64_t b)
{
	if LIKELY(a && r) {
		r[23] = a[23] +b; b =  (r[23] < b);
		r[22] = a[22] +b; b &= !r[22];
		r[21] = a[21] +b; b &= !r[21];
		r[20] = a[20] +b; b &= !r[20];
		r[19] = a[19] +b; b &= !r[19];
		r[18] = a[18] +b; b &= !r[18];
		r[17] = a[17] +b; b &= !r[17];
		r[16] = a[16] +b; b &= !r[16];
		r[15] = a[15] +b; b &= !r[15];
		r[14] = a[14] +b; b &= !r[14];
		r[13] = a[13] +b; b &= !r[13];
		r[12] = a[12] +b; b &= !r[12];
		r[11] = a[11] +b; b &= !r[11];
		r[10] = a[10] +b; b &= !r[10];
		r[ 9] = a[ 9] +b; b &= !r[ 9];
		r[ 8] = a[ 8] +b; b &= !r[ 8];
		r[ 7] = a[ 7] +b; b &= !r[ 7];
		r[ 6] = a[ 6] +b; b &= !r[ 6];
		r[ 5] = a[ 5] +b; b &= !r[ 5];
		r[ 4] = a[ 4] +b; b &= !r[ 4];
		r[ 3] = a[ 3] +b; b &= !r[ 3];
		r[ 2] = a[ 2] +b; b &= !r[ 2];
		r[ 1] = a[ 1] +b; b &= !r[ 1];
		r[ 0] = a[ 0] +b; b &= !r[ 0];
	}

	return b;
}


//--------------------------------------
static inline
uint64_t bn_add1_48(uint64_t r[48], const uint64_t a[48], uint64_t b)
{
	if LIKELY(a && r) {
		r[47] = a[47] +b; b =  (r[47] < b);
		r[46] = a[46] +b; b &= !r[46];
		r[45] = a[45] +b; b &= !r[45];
		r[44] = a[44] +b; b &= !r[44];
		r[43] = a[43] +b; b &= !r[43];
		r[42] = a[42] +b; b &= !r[42];
		r[41] = a[41] +b; b &= !r[41];
		r[40] = a[40] +b; b &= !r[40];
		r[39] = a[39] +b; b &= !r[39];
		r[38] = a[38] +b; b &= !r[38];
		r[37] = a[37] +b; b &= !r[37];
		r[36] = a[36] +b; b &= !r[36];
		r[35] = a[35] +b; b &= !r[35];
		r[34] = a[34] +b; b &= !r[34];
		r[33] = a[33] +b; b &= !r[33];
		r[32] = a[32] +b; b &= !r[32];
		r[31] = a[31] +b; b &= !r[31];
		r[30] = a[30] +b; b &= !r[30];
		r[29] = a[29] +b; b &= !r[29];
		r[28] = a[28] +b; b &= !r[28];
		r[27] = a[27] +b; b &= !r[27];
		r[26] = a[26] +b; b &= !r[26];
		r[25] = a[25] +b; b &= !r[25];
		r[24] = a[24] +b; b &= !r[24];
		r[23] = a[23] +b; b &= !r[23];
		r[22] = a[22] +b; b &= !r[22];
		r[21] = a[21] +b; b &= !r[21];
		r[20] = a[20] +b; b &= !r[20];
		r[19] = a[19] +b; b &= !r[19];
		r[18] = a[18] +b; b &= !r[18];
		r[17] = a[17] +b; b &= !r[17];
		r[16] = a[16] +b; b &= !r[16];
		r[15] = a[15] +b; b &= !r[15];
		r[14] = a[14] +b; b &= !r[14];
		r[13] = a[13] +b; b &= !r[13];
		r[12] = a[12] +b; b &= !r[12];
		r[11] = a[11] +b; b &= !r[11];
		r[10] = a[10] +b; b &= !r[10];
		r[ 9] = a[ 9] +b; b &= !r[ 9];
		r[ 8] = a[ 8] +b; b &= !r[ 8];
		r[ 7] = a[ 7] +b; b &= !r[ 7];
		r[ 6] = a[ 6] +b; b &= !r[ 6];
		r[ 5] = a[ 5] +b; b &= !r[ 5];
		r[ 4] = a[ 4] +b; b &= !r[ 4];
		r[ 3] = a[ 3] +b; b &= !r[ 3];
		r[ 2] = a[ 2] +b; b &= !r[ 2];
		r[ 1] = a[ 1] +b; b &= !r[ 1];
		r[ 0] = a[ 0] +b; b &= !r[ 0];
	}

	return b;
}


//--------------------------------------
static inline
uint64_t bn_add1_64(uint64_t r[64], const uint64_t a[64], uint64_t b)
{
	if LIKELY(a && r) {
		r[63] = a[63] +b; b =  (r[63] < b);
		r[62] = a[62] +b; b &= !r[62];
		r[61] = a[61] +b; b &= !r[61];
		r[60] = a[60] +b; b &= !r[60];
		r[59] = a[59] +b; b &= !r[59];
		r[58] = a[58] +b; b &= !r[58];
		r[57] = a[57] +b; b &= !r[57];
		r[56] = a[56] +b; b &= !r[56];
		r[55] = a[55] +b; b &= !r[55];
		r[54] = a[54] +b; b &= !r[54];
		r[53] = a[53] +b; b &= !r[53];
		r[52] = a[52] +b; b &= !r[52];
		r[51] = a[51] +b; b &= !r[51];
		r[50] = a[50] +b; b &= !r[50];
		r[49] = a[49] +b; b &= !r[49];
		r[48] = a[48] +b; b &= !r[48];
		r[47] = a[47] +b; b &= !r[47];
		r[46] = a[46] +b; b &= !r[46];
		r[45] = a[45] +b; b &= !r[45];
		r[44] = a[44] +b; b &= !r[44];
		r[43] = a[43] +b; b &= !r[43];
		r[42] = a[42] +b; b &= !r[42];
		r[41] = a[41] +b; b &= !r[41];
		r[40] = a[40] +b; b &= !r[40];
		r[39] = a[39] +b; b &= !r[39];
		r[38] = a[38] +b; b &= !r[38];
		r[37] = a[37] +b; b &= !r[37];
		r[36] = a[36] +b; b &= !r[36];
		r[35] = a[35] +b; b &= !r[35];
		r[34] = a[34] +b; b &= !r[34];
		r[33] = a[33] +b; b &= !r[33];
		r[32] = a[32] +b; b &= !r[32];
		r[31] = a[31] +b; b &= !r[31];
		r[30] = a[30] +b; b &= !r[30];
		r[29] = a[29] +b; b &= !r[29];
		r[28] = a[28] +b; b &= !r[28];
		r[27] = a[27] +b; b &= !r[27];
		r[26] = a[26] +b; b &= !r[26];
		r[25] = a[25] +b; b &= !r[25];
		r[24] = a[24] +b; b &= !r[24];
		r[23] = a[23] +b; b &= !r[23];
		r[22] = a[22] +b; b &= !r[22];
		r[21] = a[21] +b; b &= !r[21];
		r[20] = a[20] +b; b &= !r[20];
		r[19] = a[19] +b; b &= !r[19];
		r[18] = a[18] +b; b &= !r[18];
		r[17] = a[17] +b; b &= !r[17];
		r[16] = a[16] +b; b &= !r[16];
		r[15] = a[15] +b; b &= !r[15];
		r[14] = a[14] +b; b &= !r[14];
		r[13] = a[13] +b; b &= !r[13];
		r[12] = a[12] +b; b &= !r[12];
		r[11] = a[11] +b; b &= !r[11];
		r[10] = a[10] +b; b &= !r[10];
		r[ 9] = a[ 9] +b; b &= !r[ 9];
		r[ 8] = a[ 8] +b; b &= !r[ 8];
		r[ 7] = a[ 7] +b; b &= !r[ 7];
		r[ 6] = a[ 6] +b; b &= !r[ 6];
		r[ 5] = a[ 5] +b; b &= !r[ 5];
		r[ 4] = a[ 4] +b; b &= !r[ 4];
		r[ 3] = a[ 3] +b; b &= !r[ 3];
		r[ 2] = a[ 2] +b; b &= !r[ 2];
		r[ 1] = a[ 1] +b; b &= !r[ 1];
		r[ 0] = a[ 0] +b; b &= !r[ 0];
	}

	return b;
}


//--------------------------------------
static inline uint64_t bn_modn1_3m1(const uint64_t a[3],
                                          uint64_t modn, uint64_t shf)
{
	uint128_t t[3];

	t[0] = (uint128_t) a[0];
	t[1] = (uint128_t) a[1];
	t[2] = a[2] % modn;

	t[1] += (t[0] * shf) % modn;
	t[1] %= modn;

	t[2] += (t[1] * shf) % modn;
	t[2] %= modn;

	return t[2];
}


//--------------------------------------
static inline uint64_t bn_modn1_6m1(const uint64_t a[6],
                                          uint64_t modn, uint64_t shf)
{
	uint128_t t[6];

	t[0] = (uint128_t) a[0];
	t[1] = (uint128_t) a[1];
	t[2] = (uint128_t) a[2];
	t[3] = (uint128_t) a[3];
	t[4] = (uint128_t) a[4];
	t[5] = a[5] % modn;

	t[1] += (t[0] * shf) % modn;
	t[1] %= modn;

	t[2] += (t[1] * shf) % modn;
	t[2] %= modn;

	t[3] += (t[2] * shf) % modn;
	t[3] %= modn;

	t[4] += (t[3] * shf) % modn;
	t[4] %= modn;

	t[5] += (t[4] * shf) % modn;
	t[5] %= modn;

	return t[5];
}


// in case we ever allow 2*p1*p2 direct import, which is 384+1 bits,
// 7 64-bit digits, this would be used:

//--------------------------------------
static inline
uint64_t bn_modn1_7m1(const uint64_t a[7], uint64_t modn, uint64_t shf)
{
	uint128_t tmp[7];

	tmp[0] = (uint128_t) a[0];
	tmp[1] = (uint128_t) a[1];
	tmp[2] = (uint128_t) a[2];
	tmp[3] = (uint128_t) a[3];
	tmp[4] = (uint128_t) a[4];
	tmp[5] = (uint128_t) a[5];
		/**/
	tmp[6] = a[6] % modn;

	tmp[1] += (tmp[0] * shf) % modn;
	tmp[1] %=  modn;
	tmp[2] += (tmp[1] * shf) % modn;
	tmp[2] %=  modn;
	tmp[3] += (tmp[2] * shf) % modn;
	tmp[3] %=  modn;
	tmp[4] += (tmp[3] * shf) % modn;
	tmp[4] %=  modn;
	tmp[5] += (tmp[4] * shf) % modn;
	tmp[5] %=  modn;
	tmp[6] += (tmp[5] * shf) % modn;
	tmp[6] %=  modn;

	return (uint64_t) tmp[6];
}


//--------------------------------------
static inline
uint64_t bn_modn1_8m1(const uint64_t a[8], uint64_t modn, uint64_t shf)
{
	uint128_t t[8];

	t[0] = (uint128_t) a[0];
	t[1] = (uint128_t) a[1];
	t[2] = (uint128_t) a[2];
	t[3] = (uint128_t) a[3];
	t[4] = (uint128_t) a[4];
	t[5] = (uint128_t) a[5];
	t[6] = (uint128_t) a[6];
	t[7] = a[7] % modn;

	t[1] += (t[0] * shf) % modn;
	t[1] %= modn;

	t[2] += (t[1] * shf) % modn;
	t[2] %= modn;

	t[3] += (t[2] * shf) % modn;
	t[3] %= modn;

	t[4] += (t[3] * shf) % modn;
	t[4] %= modn;

	t[5] += (t[4] * shf) % modn;
	t[5] %= modn;

	t[6] += (t[5] * shf) % modn;
	t[6] %= modn;

	t[7] += (t[6] * shf) % modn;
	t[7] %= modn;

	return t[7];
}


//--------------------------------------
static inline
uint64_t bn_modn1_10m1(const uint64_t a[10], uint64_t modn, uint64_t shf)
{
	uint128_t t[10];

	t[0] = (uint128_t) a[0];
	t[1] = (uint128_t) a[1];
	t[2] = (uint128_t) a[2];
	t[3] = (uint128_t) a[3];
	t[4] = (uint128_t) a[4];
	t[5] = (uint128_t) a[5];
	t[6] = (uint128_t) a[6];
	t[7] = (uint128_t) a[7];
	t[8] = (uint128_t) a[8];
	t[9] = a[9] % modn;

	t[1] += (t[0] * shf) % modn;
	t[1] %= modn;

	t[2] += (t[1] * shf) % modn;
	t[2] %= modn;

	t[3] += (t[2] * shf) % modn;
	t[3] %= modn;

	t[4] += (t[3] * shf) % modn;
	t[4] %= modn;

	t[5] += (t[4] * shf) % modn;
	t[5] %= modn;

	t[6] += (t[5] * shf) % modn;
	t[6] %= modn;

	t[7] += (t[6] * shf) % modn;
	t[7] %= modn;

	t[8] += (t[7] * shf) % modn;
	t[8] %= modn;

	t[9] += (t[8] * shf) % modn;
	t[9] %= modn;

	return t[9];
}


//--------------------------------------
static inline
uint64_t bn_modn1_11m1(const uint64_t a[11], uint64_t modn, uint64_t shf)
{
	uint128_t t[11];

	t[ 0] = (uint128_t) a[0];
	t[ 1] = (uint128_t) a[1];
	t[ 2] = (uint128_t) a[2];
	t[ 3] = (uint128_t) a[3];
	t[ 4] = (uint128_t) a[4];
	t[ 5] = (uint128_t) a[5];
	t[ 6] = (uint128_t) a[6];
	t[ 7] = (uint128_t) a[7];
	t[ 8] = (uint128_t) a[8];
	t[ 9] = (uint128_t) a[9];
	t[10] = a[10] % modn;

	t[1] += (t[0] * shf) % modn;
	t[1] %= modn;

	t[2] += (t[1] * shf) % modn;
	t[2] %= modn;

	t[3] += (t[2] * shf) % modn;
	t[3] %= modn;

	t[4] += (t[3] * shf) % modn;
	t[4] %= modn;

	t[5] += (t[4] * shf) % modn;
	t[5] %= modn;

	t[6] += (t[5] * shf) % modn;
	t[6] %= modn;

	t[7] += (t[6] * shf) % modn;
	t[7] %= modn;

	t[8] += (t[7] * shf) % modn;
	t[8] %= modn;

	t[9] += (t[8] * shf) % modn;
	t[9] %= modn;

	t[10] += (t[9] * shf) % modn;
	t[10] %= modn;

	return t[10];
}


//--------------------------------------
static inline
uint64_t bn_modn1_12m1(const uint64_t a[12], uint64_t modn, uint64_t shf)
{
	uint128_t t[12];

	t[ 0] = (uint128_t) a[ 0];
	t[ 1] = (uint128_t) a[ 1];
	t[ 2] = (uint128_t) a[ 2];
	t[ 3] = (uint128_t) a[ 3];
	t[ 4] = (uint128_t) a[ 4];
	t[ 5] = (uint128_t) a[ 5];
	t[ 6] = (uint128_t) a[ 6];
	t[ 7] = (uint128_t) a[ 7];
	t[ 8] = (uint128_t) a[ 8];
	t[ 9] = (uint128_t) a[ 9];
	t[10] = (uint128_t) a[10];
	t[11] = a[11] % modn;

	t[ 1] += (t[ 0] * shf) % modn;
	t[ 1] %= modn;

	t[ 2] += (t[ 1] * shf) % modn;
	t[ 2] %= modn;

	t[ 3] += (t[ 2] * shf) % modn;
	t[ 3] %= modn;

	t[ 4] += (t[ 3] * shf) % modn;
	t[ 4] %= modn;

	t[ 5] += (t[ 4] * shf) % modn;
	t[ 5] %= modn;

	t[ 6] += (t[ 5] * shf) % modn;
	t[ 6] %= modn;

	t[ 7] += (t[ 6] * shf) % modn;
	t[ 7] %= modn;

	t[ 8] += (t[ 7] * shf) % modn;
	t[ 8] %= modn;

	t[ 9] += (t[ 8] * shf) % modn;
	t[ 9] %= modn;

	t[10] += (t[ 9] * shf) % modn;
	t[10] %= modn;

	t[11] += (t[10] * shf) % modn;
	t[11] %= modn;

	return t[11];
}


//--------------------------------------
static inline
uint64_t bn_modn1_16m1(const uint64_t a[16], uint64_t modn, uint64_t shf)
{
	uint128_t t[16];

	t[ 0] = (uint128_t) a[ 0];
	t[ 1] = (uint128_t) a[ 1];
	t[ 2] = (uint128_t) a[ 2];
	t[ 3] = (uint128_t) a[ 3];
	t[ 4] = (uint128_t) a[ 4];
	t[ 5] = (uint128_t) a[ 5];
	t[ 6] = (uint128_t) a[ 6];
	t[ 7] = (uint128_t) a[ 7];
	t[ 8] = (uint128_t) a[ 8];
	t[ 9] = (uint128_t) a[ 9];
	t[10] = (uint128_t) a[10];
	t[11] = (uint128_t) a[11];
	t[12] = (uint128_t) a[12];
	t[13] = (uint128_t) a[13];
	t[14] = (uint128_t) a[14];
	t[15] = a[15] % modn;

	t[ 1] += (t[ 0] * shf) % modn;
	t[ 1] %= modn;

	t[ 2] += (t[ 1] * shf) % modn;
	t[ 2] %= modn;

	t[ 3] += (t[ 2] * shf) % modn;
	t[ 3] %= modn;

	t[ 4] += (t[ 3] * shf) % modn;
	t[ 4] %= modn;

	t[ 5] += (t[ 4] * shf) % modn;
	t[ 5] %= modn;

	t[ 6] += (t[ 5] * shf) % modn;
	t[ 6] %= modn;

	t[ 7] += (t[ 6] * shf) % modn;
	t[ 7] %= modn;

	t[ 8] += (t[ 7] * shf) % modn;
	t[ 8] %= modn;

	t[ 9] += (t[ 8] * shf) % modn;
	t[ 9] %= modn;

	t[10] += (t[ 9] * shf) % modn;
	t[10] %= modn;

	t[11] += (t[10] * shf) % modn;
	t[11] %= modn;

	t[12] += (t[11] * shf) % modn;
	t[12] %= modn;

	t[13] += (t[12] * shf) % modn;
	t[13] %= modn;

	t[14] += (t[13] * shf) % modn;
	t[14] %= modn;

	t[15] += (t[14] * shf) % modn;
	t[15] %= modn;

	return t[15];
}


//--------------------------------------
static inline
uint64_t bn_modn1_17m1(const uint64_t a[17], uint64_t modn, uint64_t shf)
{
	uint128_t tmp[17];

	tmp[ 0] = (uint128_t) a[ 0];
	tmp[ 1] = (uint128_t) a[ 1];
	tmp[ 2] = (uint128_t) a[ 2];
	tmp[ 3] = (uint128_t) a[ 3];
	tmp[ 4] = (uint128_t) a[ 4];
	tmp[ 5] = (uint128_t) a[ 5];
	tmp[ 6] = (uint128_t) a[ 6];
	tmp[ 7] = (uint128_t) a[ 7];
	tmp[ 8] = (uint128_t) a[ 8];
	tmp[ 9] = (uint128_t) a[ 9];
	tmp[10] = (uint128_t) a[10];
	tmp[11] = (uint128_t) a[11];
	tmp[12] = (uint128_t) a[12];
	tmp[13] = (uint128_t) a[13];
	tmp[14] = (uint128_t) a[14];
	tmp[15] = (uint128_t) a[15];
		/**/
	tmp[16] = a[16] % modn;

	tmp[ 1] += (tmp[ 0] * shf) % modn;
	tmp[ 1] %=  modn;
	tmp[ 2] += (tmp[ 1] * shf) % modn;
	tmp[ 2] %=  modn;
	tmp[ 3] += (tmp[ 2] * shf) % modn;
	tmp[ 3] %=  modn;
	tmp[ 4] += (tmp[ 3] * shf) % modn;
	tmp[ 4] %=  modn;
	tmp[ 5] += (tmp[ 4] * shf) % modn;
	tmp[ 5] %=  modn;
	tmp[ 6] += (tmp[ 5] * shf) % modn;
	tmp[ 6] %=  modn;
	tmp[ 7] += (tmp[ 6] * shf) % modn;
	tmp[ 7] %=  modn;
	tmp[ 8] += (tmp[ 7] * shf) % modn;
	tmp[ 8] %=  modn;
	tmp[ 9] += (tmp[ 8] * shf) % modn;
	tmp[ 9] %=  modn;
	tmp[10] += (tmp[ 9] * shf) % modn;
	tmp[10] %=  modn;
	tmp[11] += (tmp[10] * shf) % modn;
	tmp[11] %=  modn;
	tmp[12] += (tmp[11] * shf) % modn;
	tmp[12] %=  modn;
	tmp[13] += (tmp[12] * shf) % modn;
	tmp[13] %=  modn;
	tmp[14] += (tmp[13] * shf) % modn;
	tmp[14] %=  modn;
	tmp[15] += (tmp[14] * shf) % modn;
	tmp[15] %=  modn;
	tmp[16] += (tmp[15] * shf) % modn;
	tmp[16] %=  modn;

	return (uint64_t) tmp[16];
}


//--------------------------------------
static inline
uint64_t bn_modn1_21m1(const uint64_t a[21], uint64_t modn, uint64_t shf)
{
	uint128_t tmp[21];

	tmp[ 0] = (uint128_t) a[ 0];
	tmp[ 1] = (uint128_t) a[ 1];
	tmp[ 2] = (uint128_t) a[ 2];
	tmp[ 3] = (uint128_t) a[ 3];
	tmp[ 4] = (uint128_t) a[ 4];
	tmp[ 5] = (uint128_t) a[ 5];
	tmp[ 6] = (uint128_t) a[ 6];
	tmp[ 7] = (uint128_t) a[ 7];
	tmp[ 8] = (uint128_t) a[ 8];
	tmp[ 9] = (uint128_t) a[ 9];
	tmp[10] = (uint128_t) a[10];
	tmp[11] = (uint128_t) a[11];
	tmp[12] = (uint128_t) a[12];
	tmp[13] = (uint128_t) a[13];
	tmp[14] = (uint128_t) a[14];
	tmp[15] = (uint128_t) a[15];
	tmp[16] = (uint128_t) a[16];
	tmp[17] = (uint128_t) a[17];
	tmp[18] = (uint128_t) a[18];
	tmp[19] = (uint128_t) a[19];
		/**/
	tmp[20] = a[20] % modn;

	tmp[ 1] += (tmp[ 0] * shf) % modn;
	tmp[ 1] %=  modn;
	tmp[ 2] += (tmp[ 1] * shf) % modn;
	tmp[ 2] %=  modn;
	tmp[ 3] += (tmp[ 2] * shf) % modn;
	tmp[ 3] %=  modn;
	tmp[ 4] += (tmp[ 3] * shf) % modn;
	tmp[ 4] %=  modn;
	tmp[ 5] += (tmp[ 4] * shf) % modn;
	tmp[ 5] %=  modn;
	tmp[ 6] += (tmp[ 5] * shf) % modn;
	tmp[ 6] %=  modn;
	tmp[ 7] += (tmp[ 6] * shf) % modn;
	tmp[ 7] %=  modn;
	tmp[ 8] += (tmp[ 7] * shf) % modn;
	tmp[ 8] %=  modn;
	tmp[ 9] += (tmp[ 8] * shf) % modn;
	tmp[ 9] %=  modn;
	tmp[10] += (tmp[ 9] * shf) % modn;
	tmp[10] %=  modn;
	tmp[11] += (tmp[10] * shf) % modn;
	tmp[11] %=  modn;
	tmp[12] += (tmp[11] * shf) % modn;
	tmp[12] %=  modn;
	tmp[13] += (tmp[12] * shf) % modn;
	tmp[13] %=  modn;
	tmp[14] += (tmp[13] * shf) % modn;
	tmp[14] %=  modn;
	tmp[15] += (tmp[14] * shf) % modn;
	tmp[15] %=  modn;
	tmp[16] += (tmp[15] * shf) % modn;
	tmp[16] %=  modn;
	tmp[17] += (tmp[16] * shf) % modn;
	tmp[17] %=  modn;
	tmp[18] += (tmp[17] * shf) % modn;
	tmp[18] %=  modn;
	tmp[19] += (tmp[18] * shf) % modn;
	tmp[19] %=  modn;
	tmp[20] += (tmp[19] * shf) % modn;
	tmp[20] %=  modn;

	return (uint64_t) tmp[20];
}


//--------------------------------------
static inline
uint64_t bn_modn1_22m1(const uint64_t a[22], uint64_t modn, uint64_t shf)
{
	uint128_t tmp[22];

	tmp[ 0] = (uint128_t) a[ 0];
	tmp[ 1] = (uint128_t) a[ 1];
	tmp[ 2] = (uint128_t) a[ 2];
	tmp[ 3] = (uint128_t) a[ 3];
	tmp[ 4] = (uint128_t) a[ 4];
	tmp[ 5] = (uint128_t) a[ 5];
	tmp[ 6] = (uint128_t) a[ 6];
	tmp[ 7] = (uint128_t) a[ 7];
	tmp[ 8] = (uint128_t) a[ 8];
	tmp[ 9] = (uint128_t) a[ 9];
	tmp[10] = (uint128_t) a[10];
	tmp[11] = (uint128_t) a[11];
	tmp[12] = (uint128_t) a[12];
	tmp[13] = (uint128_t) a[13];
	tmp[14] = (uint128_t) a[14];
	tmp[15] = (uint128_t) a[15];
	tmp[16] = (uint128_t) a[16];
	tmp[17] = (uint128_t) a[17];
	tmp[18] = (uint128_t) a[18];
	tmp[19] = (uint128_t) a[19];
	tmp[20] = (uint128_t) a[20];
		/**/
	tmp[21] = a[21] % modn;

	tmp[ 1] += (tmp[ 0] * shf) % modn;
	tmp[ 1] %=  modn;
	tmp[ 2] += (tmp[ 1] * shf) % modn;
	tmp[ 2] %=  modn;
	tmp[ 3] += (tmp[ 2] * shf) % modn;
	tmp[ 3] %=  modn;
	tmp[ 4] += (tmp[ 3] * shf) % modn;
	tmp[ 4] %=  modn;
	tmp[ 5] += (tmp[ 4] * shf) % modn;
	tmp[ 5] %=  modn;
	tmp[ 6] += (tmp[ 5] * shf) % modn;
	tmp[ 6] %=  modn;
	tmp[ 7] += (tmp[ 6] * shf) % modn;
	tmp[ 7] %=  modn;
	tmp[ 8] += (tmp[ 7] * shf) % modn;
	tmp[ 8] %=  modn;
	tmp[ 9] += (tmp[ 8] * shf) % modn;
	tmp[ 9] %=  modn;
	tmp[10] += (tmp[ 9] * shf) % modn;
	tmp[10] %=  modn;
	tmp[11] += (tmp[10] * shf) % modn;
	tmp[11] %=  modn;
	tmp[12] += (tmp[11] * shf) % modn;
	tmp[12] %=  modn;
	tmp[13] += (tmp[12] * shf) % modn;
	tmp[13] %=  modn;
	tmp[14] += (tmp[13] * shf) % modn;
	tmp[14] %=  modn;
	tmp[15] += (tmp[14] * shf) % modn;
	tmp[15] %=  modn;
	tmp[16] += (tmp[15] * shf) % modn;
	tmp[16] %=  modn;
	tmp[17] += (tmp[16] * shf) % modn;
	tmp[17] %=  modn;
	tmp[18] += (tmp[17] * shf) % modn;
	tmp[18] %=  modn;
	tmp[19] += (tmp[18] * shf) % modn;
	tmp[19] %=  modn;
	tmp[20] += (tmp[19] * shf) % modn;
	tmp[20] %=  modn;
	tmp[21] += (tmp[20] * shf) % modn;
	tmp[21] %=  modn;

	return (uint64_t) tmp[21];
}


//--------------------------------------
static inline
uint64_t bn_modn1_24m1(const uint64_t a[24], uint64_t modn, uint64_t shf)
{
	uint128_t tmp[24];

	tmp[ 0] = (uint128_t) a[ 0];
	tmp[ 1] = (uint128_t) a[ 1];
	tmp[ 2] = (uint128_t) a[ 2];
	tmp[ 3] = (uint128_t) a[ 3];
	tmp[ 4] = (uint128_t) a[ 4];
	tmp[ 5] = (uint128_t) a[ 5];
	tmp[ 6] = (uint128_t) a[ 6];
	tmp[ 7] = (uint128_t) a[ 7];
	tmp[ 8] = (uint128_t) a[ 8];
	tmp[ 9] = (uint128_t) a[ 9];
	tmp[10] = (uint128_t) a[10];
	tmp[11] = (uint128_t) a[11];
	tmp[12] = (uint128_t) a[12];
	tmp[13] = (uint128_t) a[13];
	tmp[14] = (uint128_t) a[14];
	tmp[15] = (uint128_t) a[15];
	tmp[16] = (uint128_t) a[16];
	tmp[17] = (uint128_t) a[17];
	tmp[18] = (uint128_t) a[18];
	tmp[19] = (uint128_t) a[19];
	tmp[20] = (uint128_t) a[20];
	tmp[21] = (uint128_t) a[21];
	tmp[22] = (uint128_t) a[22];
		/**/
	tmp[23] = a[23] % modn;

	tmp[ 1] += (tmp[ 0] * shf) % modn;
	tmp[ 1] %=  modn;
	tmp[ 2] += (tmp[ 1] * shf) % modn;
	tmp[ 2] %=  modn;
	tmp[ 3] += (tmp[ 2] * shf) % modn;
	tmp[ 3] %=  modn;
	tmp[ 4] += (tmp[ 3] * shf) % modn;
	tmp[ 4] %=  modn;
	tmp[ 5] += (tmp[ 4] * shf) % modn;
	tmp[ 5] %=  modn;
	tmp[ 6] += (tmp[ 5] * shf) % modn;
	tmp[ 6] %=  modn;
	tmp[ 7] += (tmp[ 6] * shf) % modn;
	tmp[ 7] %=  modn;
	tmp[ 8] += (tmp[ 7] * shf) % modn;
	tmp[ 8] %=  modn;
	tmp[ 9] += (tmp[ 8] * shf) % modn;
	tmp[ 9] %=  modn;
	tmp[10] += (tmp[ 9] * shf) % modn;
	tmp[10] %=  modn;
	tmp[11] += (tmp[10] * shf) % modn;
	tmp[11] %=  modn;
	tmp[12] += (tmp[11] * shf) % modn;
	tmp[12] %=  modn;
	tmp[13] += (tmp[12] * shf) % modn;
	tmp[13] %=  modn;
	tmp[14] += (tmp[13] * shf) % modn;
	tmp[14] %=  modn;
	tmp[15] += (tmp[14] * shf) % modn;
	tmp[15] %=  modn;
	tmp[16] += (tmp[15] * shf) % modn;
	tmp[16] %=  modn;
	tmp[17] += (tmp[16] * shf) % modn;
	tmp[17] %=  modn;
	tmp[18] += (tmp[17] * shf) % modn;
	tmp[18] %=  modn;
	tmp[19] += (tmp[18] * shf) % modn;
	tmp[19] %=  modn;
	tmp[20] += (tmp[19] * shf) % modn;
	tmp[20] %=  modn;
	tmp[21] += (tmp[20] * shf) % modn;
	tmp[21] %=  modn;
	tmp[22] += (tmp[21] * shf) % modn;
	tmp[22] %=  modn;
	tmp[23] += (tmp[22] * shf) % modn;
	tmp[23] %=  modn;

	return (uint64_t) tmp[23];
}


//--------------------------------------
static inline
uint64_t bn_modn1_27m1(const uint64_t a[27], uint64_t modn, uint64_t shf)
{
	uint128_t tmp[27];

	tmp[ 0] = (uint128_t) a[ 0];
	tmp[ 1] = (uint128_t) a[ 1];
	tmp[ 2] = (uint128_t) a[ 2];
	tmp[ 3] = (uint128_t) a[ 3];
	tmp[ 4] = (uint128_t) a[ 4];
	tmp[ 5] = (uint128_t) a[ 5];
	tmp[ 6] = (uint128_t) a[ 6];
	tmp[ 7] = (uint128_t) a[ 7];
	tmp[ 8] = (uint128_t) a[ 8];
	tmp[ 9] = (uint128_t) a[ 9];
	tmp[10] = (uint128_t) a[10];
	tmp[11] = (uint128_t) a[11];
	tmp[12] = (uint128_t) a[12];
	tmp[13] = (uint128_t) a[13];
	tmp[14] = (uint128_t) a[14];
	tmp[15] = (uint128_t) a[15];
	tmp[16] = (uint128_t) a[16];
	tmp[17] = (uint128_t) a[17];
	tmp[18] = (uint128_t) a[18];
	tmp[19] = (uint128_t) a[19];
	tmp[20] = (uint128_t) a[20];
	tmp[21] = (uint128_t) a[21];
	tmp[22] = (uint128_t) a[22];
	tmp[23] = (uint128_t) a[23];
	tmp[24] = (uint128_t) a[24];
	tmp[25] = (uint128_t) a[25];
		/**/
	tmp[26] = a[26] % modn;

	tmp[ 1] += (tmp[ 0] * shf) % modn;
	tmp[ 1] %=  modn;
	tmp[ 2] += (tmp[ 1] * shf) % modn;
	tmp[ 2] %=  modn;
	tmp[ 3] += (tmp[ 2] * shf) % modn;
	tmp[ 3] %=  modn;
	tmp[ 4] += (tmp[ 3] * shf) % modn;
	tmp[ 4] %=  modn;
	tmp[ 5] += (tmp[ 4] * shf) % modn;
	tmp[ 5] %=  modn;
	tmp[ 6] += (tmp[ 5] * shf) % modn;
	tmp[ 6] %=  modn;
	tmp[ 7] += (tmp[ 6] * shf) % modn;
	tmp[ 7] %=  modn;
	tmp[ 8] += (tmp[ 7] * shf) % modn;
	tmp[ 8] %=  modn;
	tmp[ 9] += (tmp[ 8] * shf) % modn;
	tmp[ 9] %=  modn;
	tmp[10] += (tmp[ 9] * shf) % modn;
	tmp[10] %=  modn;
	tmp[11] += (tmp[10] * shf) % modn;
	tmp[11] %=  modn;
	tmp[12] += (tmp[11] * shf) % modn;
	tmp[12] %=  modn;
	tmp[13] += (tmp[12] * shf) % modn;
	tmp[13] %=  modn;
	tmp[14] += (tmp[13] * shf) % modn;
	tmp[14] %=  modn;
	tmp[15] += (tmp[14] * shf) % modn;
	tmp[15] %=  modn;
	tmp[16] += (tmp[15] * shf) % modn;
	tmp[16] %=  modn;
	tmp[17] += (tmp[16] * shf) % modn;
	tmp[17] %=  modn;
	tmp[18] += (tmp[17] * shf) % modn;
	tmp[18] %=  modn;
	tmp[19] += (tmp[18] * shf) % modn;
	tmp[19] %=  modn;
	tmp[20] += (tmp[19] * shf) % modn;
	tmp[20] %=  modn;
	tmp[21] += (tmp[20] * shf) % modn;
	tmp[21] %=  modn;
	tmp[22] += (tmp[21] * shf) % modn;
	tmp[22] %=  modn;
	tmp[23] += (tmp[22] * shf) % modn;
	tmp[23] %=  modn;
	tmp[24] += (tmp[23] * shf) % modn;
	tmp[24] %=  modn;
	tmp[25] += (tmp[24] * shf) % modn;
	tmp[25] %=  modn;
	tmp[26] += (tmp[25] * shf) % modn;
	tmp[26] %=  modn;

	return (uint64_t) tmp[26];
}


#if 1   /*---  delimiter: mod-X dedicated functions  -----------------------*/
static inline unsigned char bn_modn12_16(const uint64_t p[16])
{
	uint32_t mod12 = 0;

	if (p) {
		mod12 = (p[ 0] % 12) +
		        (p[ 1] % 12) +
		        (p[ 2] % 12) +
		        (p[ 3] % 12) +
		        (p[ 4] % 12) +
		        (p[ 5] % 12) +
		        (p[ 6] % 12) +
		        (p[ 7] % 12) +
		        (p[ 8] % 12) +
		        (p[ 9] % 12) +
		        (p[10] % 12) +
		        (p[11] % 12) +
		        (p[12] % 12) +
		        (p[13] % 12) +
		        (p[14] % 12) ;

		mod12 *= 4;
		mod12 += p[15] % 12;
		mod12 %= 12;
	}

	return mod12;
}


//--------------------------------------
static inline unsigned char bn_modn12_20rx52(const uint64_t p[20])
{
	uint32_t mod12 = 0;

	if (p) {
		mod12 = (((p[ 0] + p[ 1] + p[ 2] + p[ 3] + p[ 4] + p[ 5] + p[ 6] + p[ 7] + p[ 8] + p[ 9] + p[10] + p[11] + p[12] + p[13] + p[14] + p[15] + p[16] + p[17] + p[18]) % 12) * 4) + (p[19] % 12) ;
	}

	return mod12;
}


//--------------------------------------
static inline unsigned char bn_modn12_32(const uint64_t p[32])
{
	uint32_t mod12 = 0;

	if (p) {
		mod12 = (p[ 0] % 12) +
		        (p[ 1] % 12) +
		        (p[ 2] % 12) +
		        (p[ 3] % 12) +
		        (p[ 4] % 12) +
		        (p[ 5] % 12) +
		        (p[ 6] % 12) +
		        (p[ 7] % 12) +
		        (p[ 8] % 12) +
		        (p[ 9] % 12) +
		        (p[10] % 12) +
		        (p[11] % 12) +
		        (p[12] % 12) +
		        (p[13] % 12) +
		        (p[14] % 12) +
		        (p[15] % 12) +
		        (p[16] % 12) +
		        (p[17] % 12) +
		        (p[18] % 12) +
		        (p[19] % 12) +
		        (p[20] % 12) +
		        (p[21] % 12) +
		        (p[22] % 12) +
		        (p[23] % 12) +
		        (p[24] % 12) +
		        (p[25] % 12) +
		        (p[26] % 12) +
		        (p[27] % 12) +
		        (p[28] % 12) +
		        (p[29] % 12) +
		        (p[30] % 12) ;

		mod12 *= 4;
		mod12 += p[31] % 12;
		mod12 %= 12;
	}

	return mod12;
}


//--------------------------------------
static inline unsigned char bn_modn12_40rx52(const uint64_t p[40])
{
	uint32_t mod12 = 0;

	if (p) {
		mod12 = (((p[ 0] + p[ 1] + p[ 2] + p[ 3] + p[ 4] + p[ 5] + p[ 6] + p[ 7] + p[ 8] + p[ 9] + p[10] + p[11] + p[12] + p[13] + p[14] + p[15] + p[16] + p[17] + p[18] + p[19] + p[20] + p[21] + p[22] + p[23] + p[24] + p[25] + p[26] + p[27] + p[28] + p[29] + p[30] + p[31] + p[32] + p[33] + p[34] + p[35] + p[36] + p[37] + p[38]) % 12) * 4) + (p[39] % 12) ;
	}

	return mod12;
}


//--------------------------------------
static inline unsigned char bn_modn12(const uint64_t *p, unsigned int pn)
{
	switch (p ? pn : 0) {
	case 16:
		return bn_modn12_16(p);
	case 32:
		return bn_modn12_32(p);

	default:
		return ~((unsigned char) 0);
	}
}
#endif   /* 1 */  /*---  /delimiter  ------------------*/


#endif   /* 1 */  /*---  /delimiter  ------------------*/

#if defined(__cplusplus)
}
#endif

/* /generated code */
