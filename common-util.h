/*----------------------------------------------------------------------
 *  Author: Visegrady, Tamas (tamas@visegrady.ch)
 *----------------------------------------------------------------------*/

/* shared static-inline 'macros' for public distribution
 * frequently used functions small enough, selfcontained, to include
 * (static inlines are resolved and discarded, if not referenced, by gcc)
 *
 * one must select included elements explicitly; by default, no code
 * is added to the including scope
 *
 * note that this file uses identifiers containing double-underscores
 * (but not starting with them). Such identifiers are reserved in C++
 * (ISO/IEC 14882:2022, 5.10, Identifiers, 3.1), but not in C (ISO/IEC
 * 9899:2023, 7.1.3, Reserved identifiers). You MAY need to convince
 * your compiler to prevent compilation as C++, not included below.
 */

/* select the following functionality when including, or USE_UTIL_ALL:
 *   USE_ERR_ANNOTATE -- wrapper functions error-logging strings to stderr,
 *                    -- errno-to-string lookup table
 *   USE_HEXDUMP      -- add bare-bones hexdump routine
 *   USE_U64DUMP      -- add bare-bones uint64-t-dump routine
 *   USE_HEX2BIN      -- add hexstring-to-binary converter
 *   USE_TIMEDIFF     -- add elapsed-time calculator
 *   USE_READINT      -- minimal read-an-unsigned-integer function
 *   USE_WRITEINT     -- code to write decimals to memory without printf()
 *   USE_READALL      -- read or mmap a given file
 *   USE_STRINGFNS    -- portable UTF-8/native/etc. string functions
 *   USE_MIXER        -- non-cryptographic pseudorandom permutation for
 *                       _fast_ random-ish streams
 *                       suitable for fast sanity checks on PRF fields
 *   USE_MEMMGMT      -- error-reporting macros around malloc(3) and free(3)
 *   USE_UTIL_ALL     -- enable all of the above
 *
 * additional details
 *   USE_ERR_ANNOTATE -- define as <0 to prohibit any output
 *                    -- prototype remains, passing through ret.value only
 *   USE_HEXDUMP      -- define number of bytes per line, 0 for 32-byte default
 *                    -- [which fits 80-column output], <0 to prevent wrapping
 *   USE_READINT      -- accept non-negative integers up to 64 bits, decimal
 *                    -- or 0x<...hex...>. No whitespace etc. is removed,
 *                    -- unlike scanf().
 *   USE_READALL      -- uses mmap(2) on POSIX systems, file reads otherwise
 *                    -- Defined value of USE_READALL is used file-read blocking
 *                    -- unit [bytecount] if defined to >=1024.  Used only on
 *                    -- platforms lacking mmap() or equivalent; sane default
 *                    -- is supplied otherwise.
 *
 *   USE_STRINGFNS    -- functions similar to ctype.h ones]; symbolic constants
 *                    -- related to UTF-8 etc. processing
 *   USE_MEMMGMT      -- set to 0 to report failures +exit [NOT FOR LIBRARIES!]
 *                    -- set >0   to report, but not terminate, on failure
 *
 * some of the functionality is too large to inline; we still supply inline
 * (equivalent) just in case static+inline is special-cased, see f.ex.
 * www.greenend.org.uk/rjk/tech/inline.html  OR
 * gcc.gnu.org/onlinedocs/gcc/Inline.html
 * [both accessed 2022-11-03]
 *
 * prototypes
 *   long cu_reportrc(const char *msg, long rc) ;
 *                    -- stderr-logs, +syncs 'msg' if non-NULL and rc<0
 *                    -- passes through rc
 *                    -- NOP if USE_ERR_ANNOTATE<0 is defined when importing
 *
 *   void cu_hexprint(const char *prefix, const void *data, size_t dbytes) ;
 *                    -- does not produce output without data; callers MUST
 *                    -- special-case their situations for missing/empty data,
 *                    -- use cu_hexprint_all() otherwise.
 *                    --
 *
 *   under USE_HEX2BIN:
 *   size_t cu_hex2bin(void *bin, size_t bbytes,
 *               const void *hex, size_t hbytes) ;
 *                    -- accepts hex string---even with odd number of digits,
 *                    -- i.e., not byte-granular---optionally with
 *                    -- zero-termination. (TODO: control flag to require
 *                    -- byte-granular strings, which was the previous default)
 *                    --
 *                    -- returns number of bytes used by binary form
 *                    -- ~((size_t) 1) if hex string is malformed
 *                    -- ~((size_t) 2) if non-NULL bin has insufficient size
 *                    -- [all these SHOULD be >> sizeof(any real buffer)]
 *                    -- size query with NULL 'bin'
 *                    --
 *                    -- auto-detects string size with 0 'hbytes'.
 *                    -- bin/hex may be the same buffer. Other overlapping
 *                    -- configurations are undefined.
 *                    --
 *                    -- MAY end up writing partial result in case of failure.
 *
 *   under USE_TIMEDIFF:
 *   float cu_msdelta(const struct timeval *start, const struct timeval *end) ;
 *                    -- elapsed time, in milliseconds, between timeval's
 *                    -- start/end filled by "gettimeofday(&..., NULL);"
 *                    -- tolerates start>end [and reorders to start<end]
 *                    -- see timercmp() for detecting start/end order
 *   float cu_msdelta2(const struct timespec *start,
 *                     const struct timespec *end) ;
 *   void cu_currtime(struct timespec *p) ;
 *   int cu__time1ge2(const struct timespec *time1,
 *                    const struct timespec *time2) ;
 *
 *   under USE_READINT:
 *   uint64_t cu_readuint(const char *val, size_t vbytes) ;
 *                    -- accepts pure-numeric (no whitespace etc.) strings
 *                    -- decimal or hexadecimal [if starting with "0x"]
 *                    -- string length is autodetected if vbytes is 0
 *                    --
 *                    -- returns CU_INVD_UINT64 if string invalid, NULL etc.
 *                    -- [basically, strtol() without accepting whitespace etc.]
 *                    --
 *                    -- NOTE: IF VBYTES >0, REJECTS STRING IF ITS FIRST VBYTES
 *                    -- CHARACTERS ARE ACCEPTED, BUT DEC/HEX DIGITS WOULD
 *                    -- CONTINUE AT NEXT CHAR [i.e., find exact length first,
 *                    -- if processing slices of an all-numeric string]
 *                    --
 *                    -- length-checking SHOULD be able to compensate, for
 *                    -- checking, if vbytes is known
 *
 * definitions, some optional if corresponding section has been enabled
 *   COMMON_UTIL_H__   -- unconditionally defined
 *   CU_INVD_UINT64    -- uint64_t ret.value: parse failure from cu_readuint()
 *
 * note that the defined values MAY be typed, therefore are not suitable for
 * preprocessor (value) comparisons
 */

#if !defined(COMMON_UTIL_H__)
#define COMMON_UTIL_H__ 1

/* env.dependencies: readall (for mmap)
 * common-base.h MAY have been included already, without detecting environment
 * break if this is the case
 */
#undef  SYS__NEEDS_ENV

/* may have already included common-base.h, without setting SYS__NEEDS_ENV */
#if defined(SYS__NEEDS_ENV)
#if defined(COMMON_BASE_H__) && !defined(SYS_WAS_DETECTED)
#error "common-base.h already included, w/o env autodetect [fix NO_ARCH_DEPS]"
#endif
#if !defined(USE_ENV_DEFS)
#define  USE_ENV_DEFS  1            /* make sure environment is autodetected */
#endif
#endif           /* SYS__NEEDS_ENV */

#include "common-base.h"


/*-----  undefine everything conditionally-exported later  -----------------*/
#undef  CU_INVD_UINT64


/*=====  implementations  ==================================================*/

#if defined(USE_ERR_ANNOTATE) || defined(USE_UTIL_ALL)  /*==================*/
#include <errno.h>

#if defined(USE_ERR_ANNOTATE)
static inline long cu_reportrc(const char *msg, long rc)
{
	if ((NULL != msg) && (rc < 0)) {
		fflush(stdout);      /* squeeze preceding non-error logs out */

		fprintf(stderr, "ERROR: %s (%ld/x%08x)\n", msg, rc,
		        (unsigned int) rc);
		fflush(stderr);
	}

	return rc;
}
#else      /* USE_ERR_ANNOTATE < 0 */

static inline long cu_reportrc(const char *msg, long rc)
{
	(void) msg;
	return rc;
}
#endif        /* USE_ERR_ANNOTATE <=> 0 */
#endif  /*=====   USE_ERR_ANNOTATE || USE_UTIL_ALL  ========================*/


#if defined(USE_HEXDUMP) || defined(USE_UTIL_ALL)  /*=======================*/
/* reminder: value is number of bytes to wrap at [0: default(32)] */
#include <string.h>
#include <stdio.h>

/* pick default if not specified */
#if !defined(USE_HEXDUMP)
#define USE_HEXDUMP  32
#else
#if !(USE_HEXDUMP + 0)
#undef  USE_HEXDUMP
#define USE_HEXDUMP  32
#endif
#endif


static inline
void cu_hexprint(const char *prefix, const void *data, size_t dbytes)
{
	const unsigned char *org, *b = (const unsigned char *) data;
	org = b;

	if ((NULL == b) || (0 == dbytes) || (b+dbytes < b))
		return;
	if (NULL != prefix)
		printf("%s", prefix);

	while (0 < dbytes--) {
		printf("%02x", *(b++));
#if (USE_HEXDUMP >= 0)
		if ((0 < dbytes) && (0 == (b-org) % USE_HEXDUMP))
			printf("\n");
#endif
	}

	if (NULL != prefix)
		printf("\n");

	MARK_UNUSED(org);
}
#endif  /*=====  USE_HEXDUMP || USE_UTIL_ALL  ==============================*/


#if defined(USE_HEX2BIN) || defined(USE_TESTFNS) || defined(USE_UTIL_ALL)
/* returns 0 if not a hex digit, 1..16 otherwise [i.e., isxdigit which
 * also returns value read]
 *
 * a..f and A..F consecutive ranges, safe to compare on EBCDIC
 * [all of a..z, A..Z consecutive on ASCII]
 */
static inline unsigned char cu__is_hexdigit(unsigned char c)
{
	if (('0' <= c) && (c <= '9')) {
		return c - '0' +1;

	} else if (('a' <= c) && (c <= 'f')) {
		return c - 'a' +10 +1;

	} else if (('A' <= c) && (c <= 'F')) {
		return c - 'A' +10 +1;

	} else {
		return 0;
	}
}


/*------------------------------------*/
static inline
size_t cu_hex2bin(void *bin, size_t bbytes, const void *hex, size_t hbytes)
{
	const unsigned char *ph = (const unsigned char *) hex;
	      unsigned char *pb =       (unsigned char *) bin;
	unsigned char x = 0;
	size_t wr = 0;

	if (NULL == hex)
		return 0;

	if (0 == hbytes) {
		hbytes = strlen(hex);
		if (0 == hbytes)
			return 0;

	} else if ('\0' == ph[ hbytes -1 ]) {      /* tolerate trailing '\0' */
		--hbytes;
	}
		/* hbytes is net length, trimmed any preexisting '\0' tail */

	if (hbytes % 2) {                 /* 2N+1 -> 0 + 1 leading hex digit */
		unsigned char c = cu__is_hexdigit(ph[0]);

		if (!c)
			return ~((size_t) 1);               /* invalid digit */
		if (!bbytes)
			return ~((size_t) 2);                     /* overrun */
		pb[0] = c-1;

		--hbytes;
		--bbytes;
		++ph;
		++pb;
		++wr;
	}

			/* (hex, hbytes) must be 2N-digit raw string by now */

	while (0 < hbytes--) {
		unsigned char c = cu__is_hexdigit(*(ph++));

		if (!c)
			return ~((size_t) 1);

			/* w/o (unsigned char) cast, clang reports */
			/* int auto-promotion, then truncation     */

		x = (unsigned char) (x << 4) | (c -1);

		if (0 == (1 & hbytes)) {
			++wr;
			if (NULL != pb) {
				if (wr > bbytes)
					return ~((size_t) 2);     /* overrun */
				*(pb++) = x;
			}
		}
	}

	return wr;
}
#endif  /*=====  USE_HEX2BIN || USE_TESTFNS || USE_UTIL_ALL  ===============*/


#if defined(USE_TIMEDIFF) || defined(USE_UTIL_ALL)  /*======================*/
#include <time.h>

/*------------------------------------
 * is time(1) >= time(2)?
 * tolerates NULL entries, returing impl.defined result
 *
 * timercmp() is, unfortunately, not defined for timespec (only timeval)
 */
static inline
int cu__time1ge2(const struct timespec *time1, const struct timespec *time2)
{
	return ((time1 && time2)                    &&
	        ( (time1->tv_sec  >  time2->tv_sec) ||
		 ((time1->tv_sec  == time2->tv_sec) &&
		  (time1->tv_nsec >= time2->tv_nsec))));

		/* nanosecond partial seconds only; seconds decide <=> */
}


/*------------------------------------
 * cu_msdelta() equivalent, with 'timespec' and not 'timeval'
 * note: POSIX.2008 deprecates gettimeofday()
 *
 * fill start/end with clock_gettime (CLOCK_THREAD_CPUTIME_ID, &...) ;
 *    CLOCK_THREAD_CPUTIME_ID:  Linux only, total thread time
 *    CLOCK_MONOTONIC:          recommended, POSIX-portable
 */
static inline
float cu_msdelta2(const struct timespec *start, const struct timespec *end)
{
	float d = 0.0;

	if (start && end) {
		if (cu__time1ge2(start, end)) {
			const struct timespec *t = start;

			end   = start;
			start = t;
		}
			/* force 'start <= end' */

		d = (1000* (end->tv_sec - start->tv_sec)
		     + (end->tv_nsec - start->tv_nsec)
		             /(float) 1000000.0);
	}

	return d;
}

#endif  /*=====  USE_TIMEDIFF || USE_UTIL_ALL  =============================*/


#if defined(USE_READINT) || defined(USE_UTIL_ALL)  /*=======================*/
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

/* not really inline; compiler may complain about failing inlining */

#define  CU_INVD_UINT64  (~((uint64_t) 0))

/* manually detecting '0x' or starting digit, to prevent strtol() from
 * accepting whitespace etc.
 * Basically, we are checking enough to skip non-number digits which
 * strtol(3) would accept at the start.
 *
 * tolerates NULL val
 */
static inline
uint64_t cu_readuint(const char *val, size_t vbytes)
{
	uint64_t v = CU_INVD_UINT64;

	if (val) {
		int base = 10;
		char *end;
		long rc;

		if ((!vbytes || (2 < vbytes))       &&
		    ('0' == val[0]) && ('x' == val[1]))
		{
			base = 16;
			val += 2;
			if (vbytes)
				vbytes -= 2;
			if (!isxdigit(val[0]))
				return v;

		} else if (!isdigit(val[0])) {
			return v;
		}

		while ((vbytes>1) && ('0' == val[0])) {
			++val;
			--vbytes;
		}

		rc = strtoull(val, &end, base);
		if (LONG_MAX != rc) {           /* overflow returns LONG_MAX */
			if (vbytes) {
				if (vbytes == (size_t)((const char *) end -val))
					v = (uint64_t) rc;

			} else if ('\0' == *end) {
				v = (uint64_t) rc;
			}
		}
	}

	return v;
}
#endif  /*=====  USE_READINT || USE_UTIL_ALL  ==============================*/

#endif  /* !COMMON_UTIL_H__ */
