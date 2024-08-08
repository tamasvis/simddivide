/* misc. primitives shared by primality-search loops
 *
 * uint16_t modn16(const uint64_t *n, unsigned int count, uint16_t modn) ;
 *      -- big-endian (n, count) % modn, 16-bit modn
 *
 * if __SIZEOF_INT128__ is defined, AND uint128_t has been defined/typedef'd:
 * uint64_t modn64(const uint64_t *n, unsigned int count, uint64_t modn) ;
 *      -- big-endian (n, count) % modn, 64-bit modn
 *
 * size_t hex2u64(uint64_t *r, unsigned int rn, const char *hxs, size_t hbytes)
 *      -- read bignumber into big-endian u64[] array
 *      -- limit on bignumber size: PP_MAX_NR_BITS; caller may predefine
 *
 * unsigned long prm2ulong(const char *spec) ;
 *      -- read [...any single letter...=] ...value...
 *      -- assume values are >=0; ~0 is returned if anything fails
 *
 * unsigned int u64_lead0(const uint64_t *a, unsigned int an) ;
 *      -- number of leading all-zero digits of big-endian u64[] array 'a'
 *      -- returns 0 if nothing to remove, incl. NULL/empty input
 *      --         >0 otherwise (< an)
 *
 * wrapped Clic functionality:
 * prohibit by defining NO_USE_CLIC when including
 *
 * int is_fermat_probprime(const unsigned char *prime, size_t pbytes,
 *                         const unsigned char *rbase, size_t rbytes,
 *                                    uint64_t base) ;
 *      -- check for base^(prime-1) for 1; Fermat primality test
 *      -- note that Carmichael numbers fail this test, even if composite
 *      -- relies on 'void *dhpriv' in the including scope, which must
 *      -- be set to NULL for first call.
 *
 * note: caller MUST dispose global 'clic_global_ctx' after use
 */

#if !defined(COMMON_PRIME_TOOLS_H__)  //--------------------------------------
#define  COMMON_PRIME_TOOLS_H__  1

#if !defined(PP_MAX_NR_BITS)
#define  PP_MAX_NR_BITS  ((unsigned int) 8192)
#endif

#include <stdlib.h>


#if defined(CLiC_VERSION)  //-------------------------------------------------
#if !defined(NO_USE_CLIC)
// this section uses 'clic_global_ctx' for objects
// caller MUST dispose upon use
//
#if defined(COMMON_UTIL_H__)
#if !defined(USE_ERR_ANNOTATE) && !defined(USE_UTIL_ALL)
#error "using cu_reportrc(); include common-util.h with USE_ERR_ANNOTATE"
#endif

#else        /* COMMON_UTIL_H__ */
#define  USE_ERR_ANNOTATE
#include "common-util.h"
#endif


static void *clic_global_ctx;


/*--------------------------------------
 * returns NULL if can not initialize
 * make sure only called once
 */
static inline void *clic_ctx(void)
{
	if (clic_global_ctx == NULL) {
		long rc = CLiC_context_new(&clic_global_ctx, CLiC_T_SW_CONTEXT,
		                           CLiC_CONTEXT_NO_RNG_SEED);
		if (rc < 0)
			cu_reportrc("RNG setup failed\n", -1);
	}

	return clic_global_ctx;
}


#if 0
// modexp
{
	void *ctx = clic_ctx();

	if ((ctx == NULL) && (rc < 0))
		break;

	rc = CLiC_pk_new(&dhpriv, ctx, CLiC_T_DH_PRIVATE_KEY,
	                 pbytes*8, NULL, 0);
	if (rc < 0)
		break;

			/* create DH equivalent of (base)^(P-1) mod P */

	rc = CLiC_pk_setMaterial(dhpriv, CLiC_DH_G, basebin, bbytes);
	if (rc < 0)
		break;

	rc = CLiC_pk_setMaterial(dhpriv, CLiC_DH_P, prime, pbytes);
	if (rc < 0)
		break;

	rc = CLiC_pk_setMaterial(dhpriv, CLiC_DH_X, pminus1, pbytes);
	if (rc < 0)
		break;

	rc = CLiC_pk_validate(dhpriv);
	if (rc < 0)
		break;

	rbytes = pbytes;
	rc = CLiC_pk_getMaterial(dhpriv, CLiC_DH_Y, pminus1, &rbytes);
	if (rc <= 0)
		break;                   // expect result, so 0 is also failure

	return ;
}
#endif


/*-----------------------------------------------
 * check for base^(P-1) == 1 mod P; necessary but not sufficient for P prime
 * can utilize inherent windowing in DH-private mod.exponentiation.
 *
 * REQUIRES A VISIBLE 'void *dhpriv' which is initially set to NULL
 * caller must dispose() after any calls
 *
 * import GEN=2, X=P-1, PRIME=P as DH priv.key, which recomputes Y=2^X mod P
 *
 * (prime, pbytes) is alleged prime; MUST be already 00-trimmed
 * base default is 2
 *
 * returns >0  (prime, pbytes) passes base-X Fermat primality test
 *         0   appears to be composite
 *         <0  if anything failed
 *
 * may leave non-NULL objects in ctx and dhpriv
 */
static int is_fermat_probprime(const unsigned char *prime, size_t pbytes,
                               const unsigned char *rbase, size_t rbytes,
                                          uint64_t base)
{
	unsigned char pminus1[ PP_MAX_NR_BITS /8 ] = { 0, },
	              basebin[ 8 ] = { 0, };
	size_t bbytes = 0;
//	void *key = NULL;
	long rc = 0;

(void) rbase;
	if UNLIKELY(!prime || !pbytes || (pbytes > sizeof(pminus1)))
		return -1;

	if (!base)
		base = 2;

	MSBF8_WRITE(basebin, base);
	rbytes = 0;
	while ((rbytes < sizeof(basebin)) && (basebin[ rbytes ] == 0))
		++rbytes;

	if (rbytes >= sizeof(basebin))
		return -1;

	bbytes = sizeof(basebin) -rbytes;
	if (rbytes)
		memmove(basebin, &(basebin[ rbytes ]), bbytes);
	// (basebin, bbytes) is raw (u)int of base to use

	memcpy(pminus1, prime, pbytes);

	if ((pminus1[ pbytes-1 ] & 1) == 0)
		return -1;                 // even, not an RSA-reasonable prime
	pminus1[ pbytes -1 ]--;            // prime-1 for exponent

	do {
if (1) {
	void *ctx = clic_ctx();

	if ((ctx == NULL) && (rc < 0))
		break;

	rc = CLiC_pk_new(&dhpriv, ctx, CLiC_T_DH_PRIVATE_KEY,
	                 pbytes*8, NULL, 0);
	if (rc < 0)
		break;

			/* create DH equivalent of (base)^(P-1) mod P */

	rc = CLiC_pk_setMaterial(dhpriv, CLiC_DH_G, basebin, bbytes);
	if (rc < 0)
		break;

	rc = CLiC_pk_setMaterial(dhpriv, CLiC_DH_P, prime, pbytes);
	if (rc < 0)
		break;

	rc = CLiC_pk_setMaterial(dhpriv, CLiC_DH_X, pminus1, pbytes);
	if (rc < 0)
		break;

	rc = CLiC_pk_validate(dhpriv);
	if (rc < 0)
		break;

	rbytes = pbytes;
	rc = CLiC_pk_getMaterial(dhpriv, CLiC_DH_Y, pminus1, &rbytes);
	if (rc <= 0)
		break;                   // expect result, so 0 is also failure
}

	rc = (rc == 1) && (pminus1[ rbytes ] == 1);
					// result is 00-trimmed
	} while (0);

//	CLiC_dispose(&dhpriv);

	return (rc < 0) ? -1 : !!rc;
}
#endif    //-----  !NO_USE_CLIC  ---------------------------------------------
#endif    //-----  !CLiC_VERSION  --------------------------------------------


#if defined(__SIZEOF_INT128__)
#if defined(UINT128_T_DEFINED__)
/*--------------------------------------
 */
static inline uint64_t modn64(const uint64_t *n, unsigned int count,
                                    uint64_t modn)
{
	uint64_t r64 = ((uint64_t) -((int64_t) modn)) % modn;       // 2^64 % m
	uint128_t acc = 0;
	unsigned int i;

	if (!n || !count || (modn < 2))
		return 0;

	for (i=0; i<count; ++i) {
		acc *= r64;
		acc += (n[i] % modn);
		acc %= modn;
	}

	return (uint64_t) acc;
}
#endif   // UINT128_T_DEFINED__
#endif   // __SIZEOF_INT128__


// TODO: size-specialized version: unrolling simplifies all aux. variables

/*--------------------------------------
 * return (n, count) % (16-bit) modn
 */
static inline uint16_t modn16(const uint64_t *n, unsigned int count,
                                    uint16_t modn)
{
	uint64_t acc = 0;             // no realistic carry problems with our N

	if ((modn >= 2) && n && count) {
		uint16_t mod264 = ((uint64_t) -modn) % modn;
							// floor(2^64 % modn)
		unsigned int i;

		for (i=0; i<count; ++i) {
			uint16_t red = (uint16_t) (n[i] % modn);

			acc *= mod264;
			acc += red;
			acc %= modn;
		}

		acc %= modn;
	}

	return (uint16_t) acc;
}


/*--------------------------------------
 * check parameters which are expected to fit an u64
 *
 * returns ~0 if value is set, but invalid
 */
static inline uint64_t env2u64(const char *varname, uint64_t defvalue)
{
	char *value = getenv(varname);

	if (value) {
		defvalue = cu_readuint(value, 0);

		if (defvalue == CU_INVD_UINT64)
			cu_reportrc("invalid parameter/int", -1);
	}

	return defvalue;
}


/*--------------------------------------
 * convert big-endian raw (n, nbytes) to big-endian u64 (r, rn)
 *
 * returns >> (any reasonable size) if does not fit
 * number of digits set at start of (r, rn) otherwise
 *
 * note: no leading-zero elimination
 */
static inline unsigned int bin2u64(uint64_t *r, unsigned int rn,
                        const unsigned char *n,   size_t nbytes)
{
	unsigned int i, nrd = 0;

	if (!r || (rn < ((nbytes +7) /8)))
		return ~((unsigned int) 0);

	if (nbytes % 8) {
		unsigned char msd[ 8 ] = { 0, };

		memmove(&(msd[ 8 - (nbytes % 8) ]), n, nbytes % 8);

		r[0]   =  MSBF8_READ(msd);
		nrd    =  1;
		n      += nbytes % 8;
		nbytes -= nbytes % 8;
	}

	for (i=0; i<nbytes/8; ++i)
		r[ nrd+i ] = MSBF8_READ(&(n[ i*8 ]));

	nrd += nbytes/8;                // n[nrd] are u64 units of (nr, nbytes)

	return nrd;
}


/*--------------------------------------
 * read hex-encoded raw bignumber into u64[]
 * returns digit count >0 if accepting string
 * value >> any reasonable size upon failure
 **/
static inline size_t hex2u64(uint64_t *r, unsigned int rn,
                           const char *hexstr,  size_t hbytes)
{
	unsigned char raw[ (PP_MAX_NR_BITS +7) /8 ] = { 0, };
	unsigned int wr;
	size_t rc;

	if (!hexstr)
		return 0;

	rc = cu_hex2bin(raw, sizeof(raw), hexstr, hbytes);
	if (rc > sizeof(raw))
		return rc;

	wr = bin2u64(r, rn, raw, rc);

	return (wr > rn) ? ~((size_t) 0) : wr;
}


/*--------------------------------------
 * tolerate and ignore '...any letter...=' prefix
 * returns ~0 if anything untoward happened, incl. NULL 'spec'
 *         parameter value read, otherwise (assume within range)
 */
static inline unsigned long prm2ulong(const char *spec)
{
	uint64_t ird;

	if (!spec)
		return ~((unsigned long) 0);

	if (isalpha(spec[0]) && (spec[1]  == '='))
		spec += 2;

	ird = cu_readuint(spec, 0);
	if (ird == CU_INVD_UINT64) {
		cu_reportrc("iteration count invalid", -1);
		return ~((unsigned long) 0);
	}

// NOP on most reasonable platforms
// do not comment on repeated casts, the structure is valid

	if (((uint64_t) ((unsigned long) ird)) != ird) {
		cu_reportrc("iteration count out of range", -1);
		return ~((unsigned long) 0);
	}

	return (unsigned long) ird;
}


//-------------------------------------
static inline unsigned int u64_lead0(const uint64_t *a, unsigned int an)
{
	unsigned int n = 0;

	if (a && (an > 1)) {
		--an;

		while ((n < an) && !(a[n]))
			++n;
	}

	return n;
}


/*-------------------------------------
 * nr. of bytes to skip common hex prefixes (0x..., x... etc.)
 *
 * tolerates NULL or not recognized encodings as 0
 */
static inline size_t hexprefix_advance(const char *hexstr)
{
	switch (hexstr ? hexstr[0] : 0) {
	case '0':
		if ((hexstr[1] == 'x') || (hexstr[1] == 'X'))
			return 2;
		return 0;

	case 'x':
	case 'X':
		return 1;

	default:
		return 0;
	}
}

#endif     //-----  COMMON_PRIME_TOOLS_H__  ----------------------------------
