/*----------------------------------------------------------------------
 *  Author: Visegrady, Tamas (tamas@visegrady.ch)
 *----------------------------------------------------------------------*/

/* shared macros for public distribution; no typedefs
 *
 * this file may define static-inline functions, if environment permits
 * (these operate effectively as macros, under recent gcc/xlc/clang)
 *
 * note that this file uses identifiers containing double-underscores
 * (but not starting with them). Such identifiers are reserved in C++
 * (ISO/IEC 14882:2022, 5.10, Identifiers, 3.1), but not in C (ISO/IEC
 * 9899:2023, 7.1.3, Reserved identifiers). You MAY need to convince
 * your compiler to prevent compilation as C++, not included below.
 */

#if !defined(COMMON_BASE_H__)
#define COMMON_BASE_H__ 1

/*-------------------------------------
 * conditional control: define the following symbols when including to enable
 *                      conditionally-supplied features:
 *
 *   NO_ARCH_DEPS     prevent auto-detection of CPU/endianness etc. by compiler
 *                    see also: interaction with USE_ENV_DEFS
 *   USE_ENV_DEFS     add environment-specific SYS_... identifiers (list below)
 *                    (centralized macro set for OS/compiler/env detection)
 *
 * details:
 *   NO_ARCH_DEPS     prevent auto-detection of CPU/endianness etc. by compiler
 *                    supplies conservative defaults for all operations
 *
 * COMMON_BASE_H__  is defined after inclusion
 */


/*-------------------------------------
 * definitions marked with '##' are not redefined if already present
 *                     -- function/type modifiers:
 *   ATTR_PURE__       -- pure func (stateless; evaluated without side effects)
 *   ATTR_CONST__      -- const func (pure + not reading any global variable)
 *   ATTR_UNUSED__     -- unused-function attribute (no warnings if not ref'd)
 *   ATTR_REALLY_INLINE__
 *                     -- the strongest 'force function to be inlined'
 *                     -- attribute definition.
 *   ATTR_PACKED__     -- packed (struct), no inter-field padding
 *
 *                     -- function/prototype annotations:
 *   ATTR_NONNULL__(x) -- pointer argument/s 'x' are non-NULL
 *   ATTR_NONNULLS__   -- none of the pointer arguments are NULL pointers
 *
 *     search for the following gcc attributes for clarification:
 *         __attribute__((NNN)), where NNN is: pure, const, unused, packed,
 *                               'nonnull x', nonnull, warn_unused_result
 *
 *   LIKELY(x)         -- likely path (expect to be taken)
 *   UNLIKELY(x)       -- unlikely......
 *   MARK_UNUSED       ## mark value as ignored (such as: unused fn arguments)
 *
 *   SYS_64BIT         -- defined if 64-bit support is implied (autodetected)
 *   SYS_BIG_ENDIAN    ## defined if autodetected by environment (4321)
 *   SYS_LITTLE_ENDIAN ## defined if autodetected by environment (1234)
 *   SYS_FOUND_ENDIAN  -- if inferred endianness, i.e., defined one of the above
 *                       note: endian.h does not exist on all IBM server OSes,
 *                             gcc/BSD builtins are unreliable/non-portable
 *                       no capabilities for PDP/mixed-endian environments
 *
 * endianness conversion: 16, 32, 64-bit variables with fixed endianness
 *   MSBF<n>_WRITE(p, v)   -- write v as <n>-byte big-endian
 *
 * miscellaneous conversion
 *   ARRAY_ELEMS()     ## array element count
 *   BUILD_ASSERT      ## stops compilation if condition is false
 */


/* Implementation note: target is mainly autodetected from compiler/env
 * settings. Since many compilers define (theoretically) GCC-only
 * variables such as GCC version, GCC-specific sections are better left
 * as the last detected configuration.
 *
 * Example compilers pretending to be GCC: Intel icc, tcc (Tiny
 * C), LLVM/clang, (basically gcc-compatible), IBM C/xlc (partial
 * gcc-compatibility support). In most cases, GCC additions are supplied
 * by them by default; see SYS__CC_... defines for details.
 *
 *-----  compiler special cases  ---------------------------------------------
 * static+inline 'function macros' differ under gcc and clang, even
 * if clang reports __GNUC__:
 *
 *   to better approximate macros, static+inline functions are
 *   recommended instead of #define, when over a given complexity (cf. gcc
 *   ref below). gcc silently ignores these functions, without
 *   reporting as unreferenced if so---i.e., treat as a macro, not as a
 *   [non-inline] static function.
 *
 *   unfortunately, while clang supports static+inline in a
 *   gcc-compatible way, it does report unreferenced static+inline
 *   functions with default settings.
 *
 * see also:
 *   '[patch] compiler, clang: move inline definition to compiler-gcc.h'
 *   www.mail-archive.com/linux-kernel@vger.kernel.org/msg1414783.html
 *   rationale for Linux kernel commit abb2ea7dfd82 [2017-06-06]
 *   accessed [2017-06-10]
 *
 *   'An Inline Function is As Fast As a Macro', accessed 2017-06-10
 *   gcc.gnu.org/onlinedocs/gcc/Inline.html
 *
 *   'Inline Functions In C', esp. section on static inline +gcc:
 *   www.greenend.org.uk/rjk/tech/inline.html
 *   accessed 2017-06-10
 */

#include <stdio.h>        /* need size_t */


#if 0  /*=====  /compiler autodetection  ===================================*/
/* these definitions are used only as needed, so we do not break if
 * we could not infer exact setup. check for SYS__CC_FOUND if needed:
 * it is defined only when we have settled on a candidate.
 *
 * this section has been extracted since many compilers mimic each
 * other, so answering 'is this really gcc, or clang pretending to be
 * gcc?' is sometimes context-dependent. we do not wish to replicate
 * such code if not necessary.
 *
 * since compilers may supply IDs of others, do not reject 'conflicting'
 * definitions during auto-detection. we attempt more specific to
 * generic guesses instead, and stop at the first reasonable-looking
 * one. To make sure our guessing is consistent, some of the checks
 * below are redundant, such as verifying that only the section of
 * compiler NNN sets its own specific NNN options.
 *
 * sections may define:
 *   SYS__CC_GCC_COMPAT  gcc-compatible, including LLVM/clang and Intel icc
 *   SYS__CC_GCC         gcc (specifically, excluding ~compatible ones)
 *   SYS__CC_CLANG       LLVM/clang, gcc-compatible
 *   SYS__CC_TCC         tiny C (tcc), generally gcc-compatible
 *   SYS__CC_FOUND       defined only when we found compiler setup
 */
#undef  SYS__CC_GCC_COMPAT
#undef  SYS__CC_GCC
#undef  SYS__CC_CLANG
#undef  SYS__CC_TCC
#undef  SYS__CC_FOUND


#if defined(__clang__)  /*--------------------------------------------------*/
#if !defined(SYS__CC_GCC_COMPAT)
#define  SYS__CC_GCC_COMPAT  "gcc-compatible: LLVM/clang"
#endif

#if defined(SYS__CC_CLANG)
#error  "someone already claims to have recognized clang: unexpected"
#endif
/**/
#define  SYS__CC_CLANG  "gcc-compatible: LLVM/clang"

#if !defined(SYS__CC_FOUND)
#define  SYS__CC_FOUND  "gcc-compatible: LLVM/clang"
#endif
#endif  /*-----  LLVM/clang  -----------------------------------------------*/


#if defined(__TINYC__)  /*--------------------------------------------------*/
#if !defined(SYS__CC_GCC_COMPAT)
#define  SYS__CC_GCC_COMPAT  "gcc-compatible: tcc"
#endif

#if defined(SYS__CC_TCC)
#error  "someone already claims to have recognized clang: unexpected"
#endif
/**/
#define  SYS__CC_TCC  "gcc-compatible: tcc"

#if !defined(SYS__CC_FOUND)
#define  SYS__CC_FOUND  "gcc-compatible: tcc"
#endif
#endif  /*-----  tcc  ------------------------------------------------------*/


#if defined(__GNUC__) && !defined(SYS__CC_FOUND)  /*------------------------*/

/* assume it is gcc, if no other compiler has been detected before */

#if !defined(SYS__CC_GCC_COMPAT)
#define  SYS__CC_GCC_COMPAT  "gcc; absolutely gcc-compatible"
#endif

#if defined(SYS__CC_GCC)
#error  "someone already claims to have recognized gcc (exactly): unexpected"
#endif
/**/
#define  SYS__CC_GCC  "gcc"

#if !defined(SYS__CC_FOUND)
#define  SYS__CC_FOUND  "gcc (the real thing)"
#endif
#endif  /*-----  gcc  ------------------------------------------------------*/

#endif  /*=====  /compiler autodetection  ==================================*/


#if 1   /*===  misc. attributes etc.  ======================================*/
/* LLVM/clang implements GCC-compatible equivalents and reports __GNUC__ too */

/* attributes in this list are empty-defined unless otherwise set */


/*-----  gcc or compatible  ------------------------------------------------*/
#if (__GNUC__)       /* note: pre-gcc3 support is basically irrelevant */
#define ATTR_PURE__     __attribute__ ((pure))
#define ATTR_CONST__    __attribute__ ((const))
#define ATTR_UNUSED__   __attribute__ ((unused))
#define ATTR_PACKED__   __attribute__ ((packed))
#define ATTR_REALLY_INLINE__  __attribute__((always_inline))
/**/
#define ATTR_NONNULL__(x)      __attribute__ ((nonnull x))  /* specific prms */
#define ATTR_NONNULLS__        __attribute__ ((nonnull))    /* no NULLs */
#endif     /* __GNUC__ */

/* non-gcc equivalents of markers/attributes come here */


#if 1    /*-----  delimiter: empty-define any yet-undefined marker/attr  ----*/
#if !defined(ATTR_PURE__)
#define ATTR_PURE__           /* missing pure attr */
#endif

#if !defined(ATTR_CONST__)
#define ATTR_CONST__          /* missing const attr */
#endif

#if !defined(ATTR_UNUSED__)
#define ATTR_UNUSED__         /* missing unused attr */
#endif

#if !defined(ATTR_PACKED__)
#define ATTR_PACKED__         /* missing packed attr */
#endif

#if !defined(ATTR_REALLY_INLINE__)
#define ATTR_REALLY_INLINE__  /* missing force-really-inline attr */
#endif

#if !defined (ATTR_NONNULL__)
#define ATTR_NONNULL__(x)     /* missing non-NULL attr */
#endif

#if !defined (ATTR_NONNULLS__)
#define ATTR_NONNULLS__        /* missing non-NULLs attr */
#endif
#endif                            /* delimiter only */


#if !defined(MARK_UNUSED)
#define MARK_UNUSED(prm)   (void) (prm)      /* silence 'unused' warning */
#endif

#if !defined(ARRAY_ELEMS)
#define  ARRAY_ELEMS(arr)  (sizeof(arr) / sizeof((arr)[0]))
#endif

/* breaks compilation if condition is false
 * leaves no runtime code (_Static_assert for older C compilers)
 */
#if !defined(BUILD_ASSERT)
#define  BUILD_ASSERT(condition) ((void) sizeof(char[ 1- 2*!(condition) ]))
#endif
/**/
/* C11 equivalent; not all our targets support C11 */
#if !defined(_Static_assert)
#define  _Static_assert(condition, str)  \
         BUILD_ASSERT((condition), "build-assert")
#endif

#endif   /*===  /misc. attributes  =========================================*/


#if 1   /*===  endianness conversion macros  ===============================*/
/* MSBF<n>_READ(), _WRITE(): big-endian write of N bytes
 * LSBF<n>_READ(), _WRITE(): little-endian read...
 * 2, 4 bytes always, 64-bit only if system supports it (derived from stdint.h)
 *
 * let setup+compiler specify any of these
 * fall back to generic code otherwise
 */
#undef  LSBF2_WRITE
#undef  LSBF4_WRITE
#undef  LSBF8_WRITE
#undef  MSBF2_WRITE
#undef  MSBF4_WRITE
#undef  MSBF8_WRITE
/**/
#undef  LSBF2_READ
#undef  LSBF4_READ
#undef  LSBF8_READ
#undef  MSBF2_READ
#undef  MSBF4_READ
#undef  MSBF8_READ


/*---  auto-detected infrastructure versions  --------------------------------
 * defining NO_ARCH_DEPS falls back to system-provided generic implementations
 *
 * this section defines SYS_64BIT if assuming/implied 64-bit support
 */
#if !defined(NO_ARCH_DEPS)

/* we replicate much of <endian.h> or <byteorder.h> here, since their contents
 * are not universally available (which differ between
 * BSD/SysV/glibc-heritage systems, f.ex. betoh32() vs. be32toh(),
 * location <machine/endian.h> vs. <endian.h>, etc).
 */
#undef  SYS__ARCH         /* set this to detected arch name, when found */
                          /* error if multiple architectures found      */
#undef  SYS__BIG_ENDIAN     /* private macros storing context */
#undef  SYS__LITTLE_ENDIAN

#undef  SYS_BIG_ENDIAN      /* public macros for export (1234/4321) */
#undef  SYS_LITTLE_ENDIAN
#undef  SYS_FOUND_ENDIAN
/**/
#undef  SYS__64BIT        /* if platform implies 64-bit capability        */
                          /* our own internal var; define SYS_64BIT for   */
                          /* global consumption                           */
#undef  SYS__NO64_BIT     /* if explicitly forbidden, such as 32-bit mode */
/**/
#define SYS__UNALIGNED 1  /* set if platform does not need aligned/u8 access */
                          /* note that undefined-behaviour sanitizer reports */
                          /* unaligned access even on supporting platforms   */
/* for comparison, see the HAVE_EFFICIENT_UNALIGNED_ACCESS Linux macro,
 * defined on the following architectures [2016-11]:
 *    s390 arm arm64 powerpc x86 x86_64
 * warning: UBsanitizer still reports unaligned access on these platforms
 *
 * ARM provides __ARM_FEATURE_UNALIGNED when compiling on alignment-obivious
 * variants
 *
 * we do not provide get_unaligned_be32() or equivalent non-aligned-read fns
 */

/* shortcuts to detected combinations will be defined when:
 *
 * SYS__X86_BITS    >= 32   on x86 (any type); 64 for x32 mode
 *                          (32-bit pointers, 64-bit data and registers)
 * SYS__S390_BITS   >= 32   on mainframes (including z/Linux, MVS and z/VM)
 * SYS__POWER_BITS  >= 32   on Power and PPC
 * SYS__ARM_BITS    >= 32   on ARM variants (only certain architectures)
 *
 * note that these options are not mutually exclusive (x86/Windows etc.)
 */


/*---  pick up compiler-specified architecture  ------------------------------
 * sections expected to be unique, multiple matching ones stop with error
 * protect sections with not-defined-SYS__ARCH to implement precedence
 *
 * USE_HIGHRES_CLK depends on some of these values, so do not change
 * or undefine them. (If that happens, the clock-specific section below
 * will probably break.)
 */

/* x86 supported by multiple compilers & word sizes
 * precedence is 64-bit, then 32-bit fallback
 * S390 OSes may further specialize, but they all use S390_BITS
 **/
#undef  SYS__X86_BITS
#undef  SYS__S390_BITS
#undef  SYS__POWER_BITS
#undef  SYS__ARM_BITS

/* note that usage of __x86_64 vs. __x86_64__ is not consistent
 * some special cases check for both (tcc, as an example, switched defines once)
 * we do not currently differentiate 64-bit and x32 mode [ILP32 extension,
 * 32-bit pointers with 64-bit data/registers]
 *
 * AMD64 generally defines __x86_64__ as well
 * Intel icc generally defines gcc-compatible values
 * tcc versions are inconsistently defining __x86_64__ or __x86_64
 * _M_IX86 is Microsoft equivalent with further sublevels
 */

#if 1     /*-----  delimiter: expect unique matches in this section  -------*/

#if (defined(__x86_64) || defined(__amd64__)) && !defined(SYS__X86_BITS)
#define  SYS__X86_BITS  64   /* Intel/AMD x86/64, Intel/gcc */
                             /* MAY include non-Windows tcc, see below */
#endif

#if defined(SYS__CC_TCC) && !defined(SYS__X86_BITS)
#if defined(__x86_64__)
#define  SYS__X86_BITS  64   /* tcc, Intel x86/64, alternate define */
#endif
#endif

#if defined(_M_X64) && !defined(SYS__X86_BITS)
#define  SYS__X86_BITS    64   /* Microsoft or icc, 64-bit x86 */
#define  SYS__WIN32_BITS  64
#endif

/* 32-bit x86, no 64-bit support: assume non-64-bit x86 (gcc, clang, icc)
 * only x86 defines __i386__; 64-bit versions were caught above
 **/
#if defined(__i386__) && !defined(SYS__X86_BITS)
#define  SYS__X86_BITS    32  /* gcc,clang,icc/x86, no 64-bit */
#endif

/* Windows: assume 64-bit types are available, even on 32-bit targets */
/**/
#if defined(__x86__) && !defined(SYS__X86_BITS)
#define  SYS__X86_BITS    32  /* Intel/AMD x86, Intel/gcc, no 64-bit support */
#define  SYS__WIN32_BITS  32
#define  SYS__64BIT    "assumed 64-bit support on (Windows) x86"
#endif

#if defined(_M_IX86) && !defined(SYS__X86_BITS)
#define  SYS__X86_BITS    32   /* Microsoft or icc, 32-bit only x86 */
#define  SYS__WIN32_BITS  32
#define  SYS__64BIT    "assumed 64-bit support on (Windows) x86"
#endif

/* ARM: different configs; see 'ARM C language extensions' (IHI 0053B,
 * IHI 0053D)
 * __ARM_ARCH  defined regardless of Thumb etc. support
 * break on systems lacking __ARM_ARCH_ISA_ARM/A64 (Thumb only)
 * __ARM_ARCH_ISA_ARM alone implies pre-AArch64 toolchain
 */
#if defined(__ARM_ARCH)
#if (__ARM_ARCH_ISA_ARM != 1) && (__ARM_ARCH_ISA_A64 != 1)
#error "your ARM seems to be Thumb-only, or otherwise restricted. contact us."
#endif

#if (__ARM_64BIT_STATE == 1)
#define  SYS__64BIT    "64-bit support, AArch64"        /* [ACLE2016, 6.4.1] */
#define  SYS__ARM_BITS  64

#elif (__ARM_ARCH >= 7) && (__ARM_32BIT_STATE != 1)
#define  SYS__64BIT    "assumed 64-bit support on v7+ ARM"
                       /* __ARM_32BIT_STATE is [ACLE2016, 6.4.1] */
#define  SYS__ARM_BITS  64

#else
#define  SYS__ARM_BITS  32
#endif
#endif        /* __ARM_ARCH */

#endif    /*-----  /delimiter: expect unique matches in this section  ------*/


/* simple comparison of undefined should work as 0
 * clang analyzer reports them as invalid
 */
#if defined(SYS__X86_BITS)
#if (64 <= SYS__X86_BITS)  /*-----------------------------------------------*/
#if defined(SYS__ARCH)
#error "conflicting architecture found (x86/64)"
#endif
#define  SYS__ARCH           "x86/64; implied: 64-bit types"
#define  SYS__LITTLE_ENDIAN  "x86/64"

#if !defined(SYS__64BIT)
#define  SYS__64BIT     "x86/64, inferred native 64-bit support"
#endif
#endif     /*----  x86, 64-bit  --------------------------------------------*/
#endif     /* x86, any */


#if defined(SYS__X86_BITS)
#if (32 <= SYS__X86_BITS) && (SYS__X86_BITS < 64) /*------------------------*/
#if defined(SYS__ARCH)
#error "conflicting architecture found (32-bit x86)"
#endif
#define  SYS__ARCH           "x86 without 64-bit support"
#define  SYS__LITTLE_ENDIAN  "x86"

#if !defined(SYS__NO64_BIT)
#define  SYS__NO64_BIT       "x86 without 64-bit support"
#endif
#endif     /*----  x86, 32-bit  --------------------------------------------*/
#endif     /* x86, any */


#if defined(__s390__)  /*---------------------------------------------------*/
#if defined(SYS__ARCH)
#error "conflicting architecture found (S390, gcc+derivatives)"
#endif
#define  SYS__ARCH         "S390, gcc or derivatives"
#define  SYS__BIG_ENDIAN   "S390, native big-endian"

#if defined(__s390x__)
#define  SYS__64BIT        "S390, 64-bit"
#define  SYS__S390_BITS    64
#else
/* S390 supports 64-bit ops even with -q32, so do not prohibit */
#define  SYS__S390_BITS    32
#endif         /* s390x */
#endif   /*----  s390  -----------------------------------------------------*/

/*---  end of env setups  ------------*/


/* assume 64-bit if (a) saw 64-bit stdint.h and (b) no other information */
/**/
#if defined(UINT64_MAX) && !defined(SYS__64BIT) && !defined(SYS__NO64_BIT)
#define  SYS__64BIT  "assumed 64-bit, inferred from stdint.h"
#endif
#endif         /*---  !NO_ARCH_DEPS  ---------------------------------------*/


/* collapse <stdint.h> plus architecture settings to single switch */
/**/
#if defined(UINT64_MAX) || (defined(SYS__64BIT) || !defined(SYS__NO64_BIT))
#undef  SYS__64BIT
#define SYS__64BIT  "need 64-bit versions"
#endif


/*---  endian.h equivalents  -----------------------------*/
#if defined(SYS__BIG_ENDIAN) && defined(SYS__LITTLE_ENDIAN)
#error "conflicting endianness inferred from system"
#endif

#if defined(SYS__BIG_ENDIAN) && !defined(SYS_BIG_ENDIAN)
#define  SYS_BIG_ENDIAN  4321
/**/
#if !defined(SYS_FOUND_ENDIAN)
#define SYS_FOUND_ENDIAN  SYS_BIG_ENDIAN
#endif
#endif

#if defined(SYS__LITTLE_ENDIAN) && !defined(SYS_LITTLE_ENDIAN)
#define  SYS_LITTLE_ENDIAN  1234
/**/
#if !defined(SYS_FOUND_ENDIAN)
#define SYS_FOUND_ENDIAN  SYS_BIG_ENDIAN
#endif
#endif


/*---  defaults, if no other previous definition  ----------------------------
 * notes:
 *   - gcc/clang/compatibles all turn static-inline fn into macro
 *     note: you may need to suppress clang pedantic warnings if a
 *     static-inline fn does not get used [which does not happen with macros]
 *     as a benefit, static-inlines add typing
 *     note: check if macro-scoped local var achieves the same on ARM
 *   - byte-granular reads, should work anywhere
 *   - these inline fns, as macro replacements, only eval their inputs once
 *   - keep all versions fully expanded: gcc/xlc do a decent job recognizing
 *     and optimizing to bswap() etc. calls, if bytes are all accessed in
 *     sequence.  Equivalent formulations, such as 8 bytes == 2x4 bytes +shift
 *     may not get recognized (status as of 2014-02-26).
 */
#if !defined(MSBF4_READ)
#if defined(SYS__CC_GCC_COMPAT)
static inline uint32_t MSBF4_READ(const void *p)
{
	const unsigned char *pb = (const unsigned char *) p;

	return ((((uint32_t)  (pb)[3])       ) |
		(((uint32_t) ((pb)[2])) <<  8) |
		(((uint32_t) ((pb)[1])) << 16) |
		(((uint32_t) ((pb)[0])) << 24)) ;
}
#else
#define MSBF4_READ(p) (((uint32_t) (((const unsigned char *) (p))[3])     ) | \
                       ((uint32_t) (((const unsigned char *) (p))[2]) << 8) | \
                       ((uint32_t) (((const unsigned char *) (p))[1]) <<16) | \
                       ((uint32_t) (((const unsigned char *) (p))[0]) <<24) )
#endif       /* gcc-like */
#endif

#if !defined(LSBF4_READ)
#if defined(SYS__CC_GCC_COMPAT)
static inline uint32_t LSBF4_READ(const void *p)
{
	const unsigned char *pb = (const unsigned char *) p;

	return ((((uint32_t)  (pb)[0])       ) |
		(((uint32_t) ((pb)[1])) <<  8) |
		(((uint32_t) ((pb)[2])) << 16) |
		(((uint32_t) ((pb)[3])) << 24)) ;
}
#else
#define LSBF4_READ(p) (((uint32_t) (((const unsigned char *) (p))[0])     ) | \
                       ((uint32_t) (((const unsigned char *) (p))[1]) << 8) | \
                       ((uint32_t) (((const unsigned char *) (p))[2]) <<16) | \
                       ((uint32_t) (((const unsigned char *) (p))[3]) <<24) )
#endif       /* gcc-like */
#endif


#if !defined(MSBF4_WRITE)
#if defined(SYS__CC_GCC_COMPAT)
static inline void MSBF4_WRITE(void *p, uint32_t v)
{
	unsigned char *pb = (unsigned char *) p;

	pb[3] = (unsigned char)  v;
	pb[2] = (unsigned char) (v >> 8);
	pb[1] = (unsigned char) (v >>16);
	pb[0] = (unsigned char) (v >>24);
}
#else
#define MSBF4_WRITE(p,v)  do { \
            unsigned char *plocal = (unsigned char *) (p);  \
                                                            \
            plocal[3] = (unsigned char)  (v);               \
            plocal[2] = (unsigned char) ((v) >>  8);        \
            plocal[1] = (unsigned char) ((v) >> 16);        \
            plocal[0] = (unsigned char) ((v) >> 24);        \
        } while (0)
#endif         /* gcc-like */
#endif


#if !defined(LSBF4_WRITE)
#if defined(SYS__CC_GCC_COMPAT)
static inline void LSBF4_WRITE(void *p, uint32_t v)
{
	unsigned char *pb = (unsigned char *) p;

	pb[3] = (unsigned char)  v;
	pb[2] = (unsigned char) (v >> 8);
	pb[1] = (unsigned char) (v >>16);
	pb[0] = (unsigned char) (v >>24);
}
#else
#define LSBF4_WRITE(p,v)  do { \
            unsigned char *plocal = (unsigned char *) (p);  \
                                                            \
            plocal[0] = (unsigned char)  (v);               \
            plocal[1] = (unsigned char) ((v) >>  8);        \
            plocal[2] = (unsigned char) ((v) >> 16);        \
            plocal[3] = (unsigned char) ((v) >> 24);        \
        } while (0)
#endif         /* gcc-like */
#endif


#if defined(SYS__CC_GCC_COMPAT)
static inline uint64_t MSBF8_READ(const void *p)
{
	const unsigned char *pb = (const unsigned char *) p;

	return ((( uint32_t) pb[7]       ) |
		(((uint32_t) pb[6]) <<  8) |
		(((uint32_t) pb[5]) << 16) |
		(((uint32_t) pb[4]) << 24) |
		(((uint64_t) pb[3]) << 32) |
		(((uint64_t) pb[2]) << 40) |
		(((uint64_t) pb[1]) << 48) |
		(((uint64_t) pb[0]) << 56) ) ;
}
#else
#define MSBF8_READ(p) ((((uint32_t)  ((const unsigned char *)(p))[7])      ) |\
                       (((uint32_t) (((const unsigned char *)(p))[6])) << 8) | \
                       (((uint32_t) (((const unsigned char *)(p))[5])) <<16) | \
                       (((uint32_t) (((const unsigned char *)(p))[4])) <<24) | \
                       (((uint64_t) (((const unsigned char *)(p))[3])) <<32) | \
                       (((uint64_t) (((const unsigned char *)(p))[2])) <<40) | \
                       (((uint64_t) (((const unsigned char *)(p))[1])) <<48) | \
                       (((uint64_t) (((const unsigned char *)(p))[0])) <<56) )
#endif     /* gcc-like */


#if defined(SYS__CC_GCC_COMPAT)
static inline uint64_t LSBF8_READ(const void *p)
{
	const unsigned char *pb = (const unsigned char *) p;

	return ((( uint32_t) pb[0]       ) |
		(((uint32_t) pb[1]) <<  8) |
		(((uint32_t) pb[2]) << 16) |
		(((uint32_t) pb[3]) << 24) |
		(((uint64_t) pb[4]) << 32) |
		(((uint64_t) pb[5]) << 40) |
		(((uint64_t) pb[6]) << 48) |
		(((uint64_t) pb[7]) << 56) ) ;
}
#else
#define LSBF8_READ(p) (((uint32_t) (((const unsigned char *) (p))[0])     ) | \
                       ((uint32_t) (((const unsigned char *) (p))[1]) << 8) | \
                       ((uint32_t) (((const unsigned char *) (p))[2]) <<16) | \
                       ((uint32_t) (((const unsigned char *) (p))[3]) <<24) | \
                       ((uint64_t) (((const unsigned char *) (p))[4]) <<32) | \
                       ((uint64_t) (((const unsigned char *) (p))[5]) <<40) | \
                       ((uint64_t) (((const unsigned char *) (p))[6]) <<48) | \
                       ((uint64_t) (((const unsigned char *) (p))[7]) <<56) )
#endif     /* gcc-like */


#if defined(SYS__CC_GCC_COMPAT)
static inline void MSBF8_WRITE(void *p, uint64_t v)
{
	unsigned char *pb = (unsigned char *) p;

	pb[7] = (unsigned char)  v;
	pb[6] = (unsigned char) (v >> 8);
	pb[5] = (unsigned char) (v >>16);
	pb[4] = (unsigned char) (v >>24);
	pb[3] = (unsigned char) (v >>32);
	pb[2] = (unsigned char) (v >>40);
	pb[1] = (unsigned char) (v >>48);
	pb[0] = (unsigned char) (v >>56);
}
#else
#define MSBF8_WRITE(p,v)  do { \
           unsigned char *plocal = (unsigned char *) (p);      \
                                                               \
           plocal[7] = (unsigned char)  (v);                   \
           plocal[6] = (unsigned char) (((uint16_t) v) >>  8); \
           plocal[5] = (unsigned char) (((uint32_t) v) >> 16); \
           plocal[4] = (unsigned char) (((uint32_t) v) >> 24); \
           plocal[3] = (unsigned char) (((uint64_t) v) >> 32); \
           plocal[2] = (unsigned char) (((uint64_t) v) >> 40); \
           plocal[1] = (unsigned char) (((uint64_t) v) >> 48); \
           plocal[0] = (unsigned char) (((uint64_t) v) >> 56); \
        } while (0)
#endif     /* gcc-like */


#if defined(SYS__CC_GCC_COMPAT)
static inline void LSBF8_WRITE(void *p, uint64_t v)
{
	unsigned char *pb = (unsigned char *) p;

	pb[0] = (unsigned char)  v;
	pb[1] = (unsigned char) (v >>  8);
	pb[2] = (unsigned char) (v >> 16);
	pb[3] = (unsigned char) (v >> 24);
	pb[4] = (unsigned char) (v >> 32);
	pb[5] = (unsigned char) (v >> 40);
	pb[6] = (unsigned char) (v >> 48);
	pb[7] = (unsigned char) (v >> 56);
}
#else
#define LSBF8_WRITE(p,v)  do { \
            unsigned char *plocal = (unsigned char *) (p);      \
                                                                \
            plocal[0] = (unsigned char)  (v);                   \
            plocal[1] = (unsigned char) (((uint16_t) v) >>  8); \
            plocal[2] = (unsigned char) (((uint32_t) v) >> 16); \
            plocal[3] = (unsigned char) (((uint32_t) v) >> 24); \
            plocal[4] = (unsigned char) (((uint64_t) v) >> 32); \
            plocal[5] = (unsigned char) (((uint64_t) v) >> 40); \
            plocal[6] = (unsigned char) (((uint64_t) v) >> 48); \
            plocal[7] = (unsigned char) (((uint64_t) v) >> 56); \
        } while (0)
#endif     /* gcc-like */

#define  SYS_64BIT  64
#endif   /*===  endianness conversion  =====================================*/


#endif      /* !defined(COMMON_BASE_H__) */
