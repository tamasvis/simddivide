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
 *   USE_PREARCH_DEPS do not undefine SYS_IS_... macros [see below]
 *   USE_ATOMIC_OPS   define system+compiler-specific atomic-operation macros
 *   USE_HIGHRES_CLK  add macro (inline fn) returning high-resolution timestamp
 *   USE_ARCH_STRENC  supply macros to autodetect native strings (ASCII/EBCDIC)
 *   USE_ENV_DEFS     add environment-specific SYS_... identifiers (list below)
 *                    (centralized macro set for OS/compiler/env detection)
 *   USE_128BIT_TYPES   define uint128_t or fail with build-time error
 *                      defines SYS_HAS_128BIT_TYPE if successful
 *
 * details:
 *   NO_ARCH_DEPS     prevent auto-detection of CPU/endianness etc. by compiler
 *                    supplies conservative defaults for all operations
 *
 *   USE_PREARCH_DEPS do not undefine SYS_IS_... macros, which are otherwise
 *                    cleared during the first inclusion. this option allows
 *                    us to pass predefined SYS_IS_... definitions through,
 *                    such as when cross-building [emulating SYS_IS_... values]
 *                    inconsistent SYS_IS_... settings may conflict, so use
 *                    this option at your own risk.
 *
 *   USE_ATOMIC_OPS  define system+compiler-specific atomic-op macros:
 *                   - fetch-and-add     16, 32 and 64-bit types
 *                   - substract-and-add 16, 32 and 64-bit types
 *                   - i.e., U<...>_ATOMIC_ADD, U<...>_ATOMIC_SUB,
 *                     U<...>_ATOMIC_READ and U<...>_ATOMIC_INIT
 *
 *         in environments where atomics are approximated through aligned
 *         primitive types synchronized access, we define:
 *             USE_ATOMIC_OPS_ALIGNED
 *         which callers SHOULD use to ensure their atomic vars are aligned.
 *         Callers can reject inclusion if they expect their platform to
 *         provide proper atomics (gcc, xlc platforms do).
 *
 *         Platforms MUST expect that repeated evaluations of 'val' are
 *         possible, and wrap ATOMIC_... invocations in [inline] functions
 *         where this may be a problem.
 *
 *         Atomic types may require modifiers; in such cases, we define
 *             USE_ATOMIC_TYPES
 *         In any case, ATOMIC_TYPE(...) is a macro supplying the atomic
 *         version of ...; it MAY be used to detect presence of atomic defs.
 *
 *         All platforms MUST provide 32-bit atomics; U32_ATOMIC_READ() MAY
 *         be used to detect support.  If 16-bit versions are not available,
 *             U16_ATOMIC_NOT_AVAILABLE
 *         is defined to >0, allowing callers to stop compilation.
 *
 *         atomic variables MAY be initialized to 0; for other
 *         initial values, use U<...>_ATOMIC_INIT macros. [if your
 *         implementation does not allow 0-initialization, please report
 *         as a bug: see initialization rules mandating explicit 0-init
 *         as legal: 7.17.2.1, 'Initialization: the ATOMIC_VAR_INIT
 *         macro', of C+11 9899:2011]
 *
 *         When we recognize a compiler without native atomic support, but
 *         may provided atomic updates (through updates to aligned variables),
 *         we define
 *             USE_ATOMIC_OPS_APPROXIMATE after inclusion. These
 *         approximate-atomics update variables safely---so they
 *         may, for example, control locked execution---but do not
 *         appropriately return proper previous values. Since we do not
 *         use these environments in production, only in certain tests,
 *         we do not intend to fix this. Callers who require atomic
 *         services may choose to reject the affected environments based
 *         on the presence of this macro.
 *
 *         Compilation stops if platform/compiler lacks atomic capabilities
 *         and they are requested.
 *
 *   USE_ARCH_STRENC supply auto-detection macros to detect architecture-native
 *                   string encoding. When requested, exactly one of the
 *                   following feature macros will be defined as non-zero
 *                   after including this header:
 *             SYS_IS_ASCII
 *             SYS_IS_EBCDIC
 *                   ...with exactly one non-zero, and one zero.
 *         The resulting fixed conditions are compile-time constants,
 *         and may be used to select with or without the preprocessor.
 *         With decent dead code elimination, their use is expected to
 *         produce no code for inactive conditional branches.
 *
 *         Note that the detection is assumed stable, regardless
 *         of locale etc. settings, but it assumes the native string encoding
 *         is 'sufficiently' ASCII/EBCDIC-like to detect reliably.
 *
 *         Specifically, we require 'a', 'Z', and '9' to appear in their
 *         expected positions. This holds for even codepage-adjusted ASCII
 *         and EBCDIC setups, to the best of our current knowledge.
 *
 *   USE_HIGHRES_CLK  add macro (inline fn) returning high-resolution timestamp
 *         defines the SYS_HRCLOCK, possibly an alias to a system-specific
 *         (inline) function, returning an unsigned long:
 *             unsigned long SYS_HRCLOCK(void) ;
 *
 *         Sets SYS_HRCLOCK_BITS to expected width of returned value;
 *         this value is always defined when a clock setup is found.
 *
 *         Do not depend on SYS_HRCLOCK being a macro: it may be a
 *         static (inline) function instead. All current targets are
 *         expected to transparently discard unreferenced static
 *         functions from header files; if you get an error, please
 *         contact us (or supress the compiler warning).
 *
 *         SYS_HRCLOCK_BITS is defined as 0 if ambiguous. This is always
 *         a macro, you may use it to conditionalize clock-using code.
 *         The value is cast to 'unsigned int' to break comparisons with
 *         negative values; unfortunately, this cast prevents its use
 *         in preprocessor conditionals (since they will not interpret
 *         typed constants).
 *
 *         Presence of USE_HIGHRES_CLK implies lack of NO_ARCH_DEPS;
 *         conflicting setup breaks compilation.
 *
 *   USE_ENV_DEFS     define environment-specific SYS_... identifiers
 *         SYS_IS_LINUX      defined >0 on Linux
 *         SYS_IS_S390       S390 or z/Architecture[=64] or 32-bit[=32]
 *         SYS_IS_I390       i390, mainframe firmware (between hypervisor
 *                           and millicode) control software; will also
 *                           set SYS_IS_S390
 *         SYS_IS_ZVM        z/VM, 31/64 bits, depending on autodetected bitsize
 *         SYS_IS_AIX        32/64, depending on autodetected bitsize
 *         SYS_IS_POWER      32/64, both Power and PowerPC
 *         SYS_IS_ARM        32/64
 *         SYS_IS_KERNEL     somewhat vaguely defined; assume OS/driver/etc.
 *                           level functionality. currently relevant for Linux
 *                           and AIX only
 *         SYS_IS_MACOS      Mac, OS X
 *         SYS_IS_WINDOWS    set if built under Win32[=32] or Win64[=64]
 *
 *                           capabilities (to centralize ifdef-jungles):
 *         SYS_HAS_MMAP      assume POSIX mmap is available
 *         SYS_WAS_DETECTED  defined in all cases
 *
 *         Presence of USE_ENV_DEFS implies lack of NO_ARCH_DEPS;
 *         conflicting setup breaks compilation.
 *         See also: USE_PREARCH_DEPS
 *
 * COMMON_BASE_H__  is defined after inclusion
 */


/*-------------------------------------
 * definitions marked with '##' are not redefined if already present
 *                     -- function/type modifiers:
 *   ATTR_PURE__       -- pure func (stateless; evaluated without side effects)
 *   ATTR_CONST__      -- const func (pure + not reading any global variable)
 *   ATTR_UNUSED__     -- unused-function attribute (no warnings if not ref'd)
 *   ATTR_STATIC_INLINE__
 *                     -- attribute silencing warnings about static
 *                     -- AND inline 'function macros' not being referenced.
 *                     -- use this attribute only for such functions!
 *                     --   see background under 'compiler special cases' below
 *   ATTR_REALLY_INLINE__
 *                     -- the strongest 'force function to be inlined'
 *                     -- attribute definition.
 *   ATTR_PACKED__     -- packed (struct), no inter-field padding
 *   ATTR_FALLTHROUGH__
 *                     -- marker to insert after switch case to annotate
 *                     -- fall-through explicitly, preventing compiler/static
 *                     -- analyzer warnings.
 *                     -- this macro MUST be added where the missing break
 *                     -- would appear.
 *                     --   see under 'compiler special cases' below
 *
 *                     -- function/prototype annotations:
 *   ATTR_NONNULL__(x) -- pointer argument/s 'x' are non-NULL
 *   ATTR_NONNULLS__   -- none of the pointer arguments are NULL pointers
 *   ATTR_WARN_IF_UNUSED__
 *                     -- warning if return value is ignored
 *   ATTR_FORMAT__     -- mark function parameters as printf/scanf format
 *                     -- string +arguments
 *   ATTR_FORMAT_ARG__ -- mark argument as transformed printf/scanf fmt string
 *
 *     search for the following gcc attributes for clarification:
 *         __attribute__((NNN)), where NNN is: pure, const, unused, packed,
 *                               'nonnull x', nonnull, warn_unused_result,
 *                               'format(type, fmt, arg)', format_arg(arg)
 *
 *   LIKELY(x)         -- likely path (expect to be taken)
 *   UNLIKELY(x)       -- unlikely......
 *   INLINE            ## compiler-specific setup for inline/__inline__/...
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
 *   MSBF<n>_READ(p)       -- read <n> bytes as big-endian
 *   MSBF<n>_WRITE(p, v)   -- write v as <n>-byte big-endian
 *   LSBF<n>_READ(p)       -- read <n> bytes as little-endian
 *   LSBF<n>_WRITE(p, v)   -- write v as <n>-byte little-endian
 *   MSBF_READ(p, n)       -- read <n> bytes as big-endian to uint64 [up to 8]
 *   MSBF_WRITE(p, n, v)   -- write <n> bytes as big-endian [up to 8]
 *
 * 64-bit types are always defined, even if system is implied to be 32-bit.
 * Contact us if this creates a problem for your platform; all our current
 * targets supply 64-bit definitions.
 * note that these are either defined as macros, or static-inline functions,
 * depending on autodetected system config. [In other words, do not depend
 * on them being #defined(...) etc.]
 *
 * NONE OF THESE MACROS CHECK FOR NULL POINTER
 *
 * on platforms which do not penalize non-native alignment, these macros
 * MAY access multibyte units in non-aligned operations. These may trigger
 * undefined-behaviour sanitizer [ubsan] warnings.
 *
 * Note that the unit of these macros [bytes] differs from the unit
 * of atomic-access macros [bits] due to historical reasons.
 *
 * miscellaneous conversion
 *   STRINGIFY(s)      -- insert 's' as verbatim string (NOT ON WINDOWS)
 *   IS_POWER_OF_TWO   -- for any integer type; note: accepts 0
 *   BIT2BYTE
 *   ARRAY_ELEMS()     ## array element count
 *   ROUND_UP_UNIT(n, unit)   -- bytecount of minimum number of units >= N
 *   BUILD_ASSERT      ## stops compilation if condition is false
 *
 * atomic operations (see also: USE_ATOMIC_OPS)
 *    U<n>_ATOMIC_ADD (uint<n>_t *ptr, uint<n>_t value) ;  -- fetch-and-add
 *    U<n>_ATOMIC_SUB (uint<n>_t *ptr, uint<n>_t value) ;  -- fetch-and-subtract
 *                                     -- both return the _previous_ value
 *    U<n>_ATOMIC_READ(uint<n>_t *ptr)
 *      n is 16, 32 or 64 [bits]; latter conditional on SYS_64BIT
 *      all atomic-access ops return corresponding uint<n>_t type
 * value being added/subtracted MUST NOT trigger side effects when evaluated
 * (i.e., make sure it may be subtracted/added back safely, just in case
 * the atomic-access operation is invoked through a macro)
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
 *   functions with default settings (as of clang 3/4). at the same
 *   time, clang also reports gcc-like behaviour through defining
 *   __GNUC__, so we define ATTR_STATIC_INLINE__ to avoid
 *   clang-specific #ifdefs.
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
 *
 *--------------------------------------
 * fallthrough (ATTR_FALLTHROUGH__):
 *
 * static analyzers, and gcc7.0 [as of 2016-10] warn about fall-through
 * in switch statements. we provide the ATTR_FALLTHROUGH__ marker to
 * insert where the missing break statement would be. see the documentation
 * of -Wimplicit-fallthrough for further details; this usage as of
 * gcc7 matches static-analyzer equivalents.
 *
 * clang5 provides a compatible attribute. we expect to update these
 * definitions once the forms are standardized (expect gcc and clang to
 * converge with C+17, P0188R1)
 *
 * note that solutions/workarounds before the C+17-standardized
 * forms, which depend on structured comments, are only best-effort
 * approximations.
 *
 *   '-Wimplicit-fallthrough in GCC 7'
 *     developers.redhat.com/blog/2017/03/10/wimplicit-fallthrough-in-gcc-7/
 *     accessed 2017-06-10
 *     discussion of gcc/standard support of non-comment based annotations
 *
 *   'Bug 77817 - -Wimplicit-fallthrough: cpp directive renders FALLTHRU
 *   comment ineffective'
 *     gcc.gnu.org/bugzilla/show_bug.cgi?id=77817
 *     accessed 2017-06-10
 *     even if the bug resolution is effectively WONTFIX, discussion
 *     is informative. it provides some background on gcc implementation,
 *     shows some corner cases of related heuristics (and cases known to fail)
 */

#include <stdio.h>        /* need size_t */

#if defined(SYS_WAS_DETECTED)  /* will mark !NO_ARCH_DEPS */
#undef  SYS_WAS_DETECTED
#endif


#if 1  /*=====  stdint.h and equivalents  ===================================*/
/* systems without stdint.h: Solaris 9 or before, Windows
 * note that gcc/xlc provide stdint.h even in pre-C99 setups
 */
/* note: Solaris 9- detection would come here, if ever needed: */
/*       Solaris before 10 lacks C99 stdint.h                  */

#if !defined(_WIN32) && !defined(_WIN64)  /*================================*/
/* non-Windows: assume C99 POSIX stdint.h and inttypes.h are available */

/* search for "S390, z/VM, xlc" for z/VM sys-definition logic
 * __VM__ is defined for all IBM z/VM builds [as of 2017-01]
 */
#if !defined(UINT32_MAX) || !defined(PRIx32)
                          /* were stdint.h and inttypes.h already included? */
#if defined(__VM__) && defined(__IBMC__)
#if !defined(__C99)
#error "VM: ensure -qlanglvl=extc99 is used, or langlvl(extc99) pragma supplied"
#error "[we intend to include stdint.h in C99 mode below]"
#endif
#endif
/**/
/* note on definitions: pre-C11, C++ (but not C) implementations MAY have
 * supplied definitions only if __STDC_LIMIT_MACROS, __STDC_CONSTANT_MACROS,
 * __STDC_FORMAT_MACROS etc. were defined before stdint.h inclusion.
 *
 * since this recommendation was not adopted even before C11, and
 * we already expect to be built in C, not C++ mode, we do not
 * supply any of these constants.
 */
#include <stdint.h>         /* C99 uint<NNN>_t and relatives */
#include <inttypes.h>       /* PRIx32 etc. */
#endif      /* !UINT32_MAX !PRIx32 -> stdint/inttypes.h not seen previously */

#else   /*===  WIN32/64  ===================================================*/

#if !defined(uint16_t)
#define uint16_t  unsigned __int16
#endif
#if !defined(uint32_t)
#define uint32_t  unsigned __int32
#endif
#if !defined(uint64_t)
#define uint64_t  unsigned __int64
#endif

	/* type-specific printf format macros from <inttypes.h> */
	/* TODO: check Win32 vs. Win64 setup */
#if !defined(PRIx16)
#define PRIx16 "x"
#endif
#if !defined(PRIu16)
#define PRIu16 "u"
#endif

#if !defined(PRIx32)
#define PRIx32 "lx"
#endif
#if !defined(PRIu32)
#define PRIu32 "lu"
#endif

#if !defined(PRIx64)
#define PRIx64 "llx"
#endif
#if !defined(PRIu64)
#define PRIu64 "llu"
#endif

#if !defined(UINT16_C)
#define UINT16_C(v)  (v)
#endif
#if !defined(UINT32_C)
#define UINT32_C(v)  (v##UL)
#endif
#if !defined(UINT64_C)
#define UINT64_C(v)  (v##ULL)
#endif
#endif    /*  WIN32/64  */
#endif    /*=====  stdint.h and equivalents  ===============================*/


#if 1  /*=====  /compiler autodetection  ===================================*/
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
 *   SYS__CC_XLC         IBM xlc
 *   SYS__CC_CLANG       LLVM/clang, gcc-compatible
 *   SYS__CC_TCC         tiny C (tcc), generally gcc-compatible
 *   SYS__CC_INTELC      icc, gcc-compatible
 *   SYS__CC_MSVC        Microsoft Visual C
 *   SYS__CC_FOUND       defined only when we found compiler setup
 */
#undef  SYS__CC_GCC_COMPAT
#undef  SYS__CC_GCC
#undef  SYS__CC_XLC
#undef  SYS__CC_CLANG
#undef  SYS__CC_TCC
#undef  SYS__CC_INTELC
#undef  SYS__CC_MSVC
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


#if 1    /* higher precedence: compiler-specific defs before gcc versions */
/* for compilers which report gcc compatibility, but are not, allow
 * definitions here. the gcc[-compatible] section does not override
 * already-defined versions.
 *
 * currently, this selective redefinition is restricted to:
 *     ATTR_FALLTHROUGH__
 */

	/* clang 3.8+ supports [[clang::fallthrough]]
	 *       3.9.1 adds support for C+17 [[fallthrough]]
	 *
	 * note that the clang warning is more selective than the gcc7+ one,
	 * fall-throughs are reported only when the C++/level/etc. setup
	 * implies the developer expects the warning. we do not attempt to
	 * replicate that logic here (beyond the standard __has... check)
	 *
	 * the cpp attribute MAY interfere with pure-C settings
	 */
#if defined(SYS__CC_CLANG)
#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(clang::fallthrough)
#define  ATTR_FALLTHROUGH__  [[clang::fallthrough]]
#endif
#endif

#if !defined(ATTR_FALLTHROUGH__)                       /* skip gcc def below */
#define  ATTR_FALLTHROUGH__  /* clang, without fall-through attribute */
#endif
#endif   /* CC_CLANG */

#endif   /* delimiter: pre-gcc-compatible defs */


/*-----  gcc or compatible  ------------------------------------------------*/
#if (__GNUC__ >= 3)       /* note: pre-gcc3 support is basically irrelevant */
#define ATTR_PURE__     __attribute__ ((pure))
#define ATTR_CONST__    __attribute__ ((const))
#define ATTR_UNUSED__   __attribute__ ((unused))
#define ATTR_PACKED__   __attribute__ ((packed))
#define ATTR_STATIC_INLINE__  __attribute__((unused))
                                   /* not a typo, identical to ATTR_UNUSED__ */
#define ATTR_REALLY_INLINE__  __attribute__((always_inline))
#define LIKELY(x)       (__builtin_expect (!!(x), 1))
#define UNLIKELY(x)     (__builtin_expect (!!(x), 0))
/**/
#if (__GNUC__ > 3) || (__GNUC_MINOR__ >= 3)
#define ATTR_NONNULL__(x)      __attribute__ ((nonnull x))  /* specific prms */
#define ATTR_NONNULLS__        __attribute__ ((nonnull))    /* no NULLs */
#define ATTR_WARN_IF_UNUSED__  __attribute__ ((warn_unused_result))
#else
/* unsupported by gcc<3.3; of historic interest only: gcc3.3.x release: 2005 */
#define ATTR_NONNULL__(x)
#define ATTR_NONNULLS__
#define ATTR_WARN_IF_UNUSED__
#endif

#if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6))
#define ATTR_FORMAT__(type, fmt, arg) __attribute__ ((format (type, fmt, arg)))
#define ATTR_FORMAT_ARG__(arg)        __attribute__ ((format_arg (arg)))
#endif


/* fall-through annotations are in flux, as of 2017-06, with the following
 * alternatives:
 *   - [[gnu::fallthrough]]  -- with gcc7.0 or higher, C+11 and C+14
 *   - [[fallthrough]]       -- C+17, P0188R1, draft 2016-02-29
 *   - __attribute__ ((fallthrough))   -- pre-C+11, gcc7.0 or above
 *
 * we use the gcc-portable form; revisit when C+17 compatibility picks up
 * see references in compiler-special rationale above
 *
 * keep conditional: pre-gcc check may have defined above
 */
#if !defined(ATTR_FALLTHROUGH__)
#if (__GNUC__ >= 7)
#define  ATTR_FALLTHROUGH__  __attribute__ ((fallthrough))
#else
#define  ATTR_FALLTHROUGH__  /* gcc[compatible] missing fall-through marker */
                             /* define empty to prevent any subsequent check */
#endif        /* gcc >= 7 */
#endif        /* !ATTR_FALLTHROUGH__ */


/* gcc<=4.8 is not std conform in c11 mode -> does not support c11-atomics */
/* but does also not set __STDC_NO_ATOMICS__                               */
/* double check definition of __STDC_NO_ATOMICS__ because of clang compat  */
/* mode and private fixes to older compilers                               */
#if defined(__GNUC__) && ((__GNUC__ == 4 ) && (__GNUC_MINOR__ <= 8))
#if !defined(__STDC_NO_ATOMICS__)
#define __STDC_NO_ATOMICS__
#endif
#endif
#endif          /* (gcc >= 3) */


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

#if !defined(ATTR_FALLTHROUGH__)
#define ATTR_FALLTHROUGH__    /* missing fallthrough-marker attr   */
#endif

#if !defined(ATTR_STATIC_INLINE__)
#define ATTR_STATIC_INLINE__  /* missing unused-static-inline attr */
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

#if !defined(ATTR_FORMAT__)
#define ATTR_FORMAT__(type, fmt, arg)  /* format-string attr missing */
#endif

#if !defined(ATTR_FORMAT_ARG__)
#define ATTR_FORMAT_ARG__(arg)         /* format-string attr missing */
#endif

#if !defined (ATTR_WARN_IF_UNUSED__)
#define ATTR_WARN_IF_UNUSED__  /* missing warn-unused attr */
#endif

#if !defined(LIKELY)
#define LIKELY(x)       (x)
#endif

#if !defined(UNLIKELY)
#define UNLIKELY(x)     (x)
#endif
#endif                            /* delimiter only */


#if !defined(INLINE)  /*----------------------------------------------------*/

#if defined(SYS__CC_GCC_COMPAT)
#define  INLINE  inline      /* basic gcc, C89 plus lots of extensions */
                             /* LLVM/clang support is compatible (for  */
                             /* static-inline purposes)                */
#else
#define  INLINE              /* unknown inline setup, ignoring */
#endif
#endif        /*---  !defined(INLINE)  -------------------------------------*/


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
 * SYS__WIN32_BITS  >= 32   Windows or 64-bit Windows[=64]
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
#undef  SYS__I390       /* i390, special-cased within non-VM S390 builds */
#undef  SYS__POWER_BITS
#undef  SYS__WIN32_BITS
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

#if defined(SYS__CC_TCC) && defined(_WIN64) && !defined(SYS__X86_BITS)
#define  SYS__X86_BITS    64   /* Intel/Windows 64-bit, tcc */
#define  SYS__WIN32_BITS  64
#endif

#if defined(SYS__CC_TCC) && defined(_WIN32) && !defined(SYS__X86_BITS)
#define  SYS__X86_BITS    32   /* Intel/Windows non-64-bit, tcc */
#define  SYS__WIN32_BITS  32
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


#if defined(SYS__CC_XLC) && defined(__PPC__)  /*----------------------------*/
#if defined(SYS__ARCH)
#error "conflicting architecture found (Power, xlc)"
#endif
/**/
#define  SYS__ARCH        "Power, xlc"
#define  SYS__BIG_ENDIAN  "Power, xlc, native big-endian"
#undef   SYS__UNALIGNED   // Power, prevent mis-optimization of mixed ptr types

#if defined(__PPC64__) || defined(__64BIT__)
#define  SYS__64BIT       "PPC/xlc, native 64-bit"
#define  SYS__POWER_BITS  64
#else
/* Power supports 64-bit ops even with -q32, so do not prohibit _64BIT */
/* however, mark target/system as 32-bit                               */
#define  SYS__POWER_BITS  32
#endif         /* PPC64    */
#endif   /*-----  IBMC/PPC  ------------------------------------------------*/


#if !defined(SYS__CC_XLC) && defined(__PPC__)  /*---------------------------*/
#if defined(SYS__CC_GCC_COMPAT)
#if defined(SYS__ARCH)
#error "conflicting architecture found (Power, gcc)"
#endif
/**/
#define  SYS__ARCH        "Power, gcc"
#define  SYS__BIG_ENDIAN  "Power, gcc, native big-endian"
#undef   SYS__UNALIGNED   // Power, prevent mis-optimization of mixed ptr types

#if defined(__PPC64__) || defined(__64BIT__)
#define  SYS__64BIT       "PPC/gcc, native 64-bit"
#define  SYS__POWER_BITS  64
#else
/* Power supports 64-bit ops even with -q32, so do not prohibit _64BIT */
/* however, mark target/system as 32-bit                               */
#define  SYS__POWER_BITS  32
#endif         /* PPC64    */
#endif         /* gcc */

#endif   /*-----  IBMC/PPC  ------------------------------------------------*/


#if defined(SYS__ARM_BITS)  /*----------------------------------------------*/
#if (defined(__GNUC__) || defined(__EDG__))
/* EDG SDKs feature gcc-compatible macros
 * we detect both ARM C lang.extn. (ACLE) and gcc(compatible) macros
 * ACLE refs below are to "ARM C language extensions v2.1" (2016)
 */

#if defined(SYS__ARCH)
#error "conflicting architecture found (ARM/gcc/EDG)"
#endif

#define  SYS__ARCH        "ARM, gcc/EDG"
#define  SYS_IS_ARM       SYS__ARM_BITS

/* [ACLE] 6.3, active indication of big-endian; default is little-endian */
#if (__ARM_BIG_ENDIAN == 1)
#define  SYS__BIG_ENDIAN     "ARM, gcc/EDG (ACLE define)"

#elif defined(__BYTE_ORDER__) && (__ORDER_BIG_ENDIAN__ == __BYTE_ORDER__)
#define  SYS__BIG_ENDIAN     "ARM, gcc/EDG (gcc-defined)"

#else
#define  SYS__LITTLE_ENDIAN  "ARM, gcc/EDG, not marked as big-endian"
#endif

#else
#error "unknown ARM environment, please contact us"

#endif   /*-----  GNUC/ACLE/EDG  -------------------------------------------*/
#endif   /*-----  arm  -----------------------------------------------------*/

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


/*---  generic/system-specific versions  -------------------------------------
 * these depend on autodetected endianness etc., therefore,
 * section is inactive on generic builds
 */
#if defined(SYS__UNALIGNED)
#if defined(SYS__BIG_ENDIAN)
#define MSBF2_READ(p)    *((const uint16_t *) (p))
#define MSBF4_READ(p)    *((const uint32_t *) (p))

#if defined(SYS__64BIT)
#define MSBF8_READ(p)    *((const uint64_t *) (p))
#endif
/**/
#define MSBF2_WRITE(p, v)    do { *((uint16_t *) (p)) = (v); } while (0)
#define MSBF4_WRITE(p, v)    do { *((uint32_t *) (p)) = (v); } while (0)

#if defined(SYS__64BIT)
#define MSBF8_WRITE(p, v)    do { *((uint64_t *) (p)) = (v); } while (0)
#endif
#endif     /* SYS__BIG_ENDIAN */

#if defined(SYS__LITTLE_ENDIAN)
#define LSBF2_READ(p)    *((const uint16_t *) (p))
#define LSBF4_READ(p)    *((const uint32_t *) (p))

#if defined(SYS__64BIT)
#define LSBF8_READ(p)    *((const uint64_t *) (p))
#endif
/**/
#define LSBF2_WRITE(p, v)    do { *((uint16_t *) (p)) = (v); } while (0)
#define LSBF4_WRITE(p, v)    do { *((uint32_t *) (p)) = (v); } while (0)

#if defined(SYS__64BIT)
#define LSBF8_WRITE(p, v)    do { *((uint64_t *) (p)) = (v); } while (0)
#endif
#endif     /* SYS__LITTLE_ENDIAN */
#endif     /* SYS__UNALIGNED */


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
static INLINE uint32_t MSBF4_READ(const void *p)
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
static INLINE uint32_t LSBF4_READ(const void *p)
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
static INLINE void MSBF4_WRITE(void *p, uint32_t v)
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
static INLINE void LSBF4_WRITE(void *p, uint32_t v)
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


/*-----  16-bit primitives  ------------------------------*/
/* redundant casts of entire compound, already uint16_t, needed by
 * HSM cross-compiler [gcc4.x], reporting conversions otherwise
 */
#if !defined(MSBF2_READ)
#define MSBF2_READ(p) (  (uint16_t)                                           \
                      ((((uint16_t)  ((const unsigned char *)(p))[1])      ) |\
                       (((uint16_t) (((const unsigned char *)(p))[0])) << 8) ))
#endif

#if !defined(LSBF2_READ)
#define LSBF2_READ(p) (  (uint16_t)                                           \
                      ((((uint16_t)  ((const unsigned char *)(p))[0])      ) |\
                       (((uint16_t) (((const unsigned char *)(p))[1])) << 8) ))
#endif

#if !defined(MSBF2_WRITE)
#define MSBF2_WRITE(p, v)  do { \
            unsigned char *plocal = (unsigned char *) (p);    \
                                                              \
            plocal[0] = (unsigned char) (((uint16_t) v) >>8); \
            plocal[1] = (unsigned char)  (v);                 \
        } while (0)
#endif

#if !defined(LSBF2_WRITE)
#define LSBF2_WRITE(p, v)  do { \
            unsigned char *plocal = (unsigned char *) (p);    \
                                                              \
            plocal[1] = (unsigned char) (((uint16_t) v) >>8); \
            plocal[0] = (unsigned char)  (v);                 \
        } while (0)
#endif


/*------------------------------------*/
static INLINE uint64_t MSBF_READ(const void *p, size_t n)
{
	const unsigned char *pb = (const unsigned char *) p;
	uint64_t v = 0;

	while (0 < n--) {
		v <<= 8;
		v +=  *(pb++);
	}

	return v;
}


/*------------------------------------*/
static INLINE void MSBF_WRITE(void *p, size_t n, uint64_t val)
{
	unsigned char *pb = (unsigned char *) p;

	pb += n;

	while (0 < n--) {
		*(--pb) = (unsigned char) val;
		val >>= 8;
	}
}


/*------------------------------------*/
static INLINE uint64_t LSBF_READ(const void *p, size_t n)
{
	const unsigned char *pb = (const unsigned char *) p;
	uint64_t v = 0;

	pb += n;

	while (0 < n--) {
		v <<= 8;
		v +=  *(--pb);
	}

	return v;
}


/*------------------------------------*/
static INLINE void LSBF_WRITE(void *p, size_t n, uint64_t val)
{
	unsigned char *pb = (unsigned char *) p;

	while (0 < n--) {
		*(pb++) = (unsigned char) val;
		val >>= 8;
	}
}


#if defined(SYS__64BIT)  /*-----  64-bit only  -----------------------------*/
#if !defined(MSBF8_READ)
#if defined(SYS__CC_GCC_COMPAT)
static INLINE uint64_t MSBF8_READ(const void *p)
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
#endif


#if !defined(LSBF8_READ)
#if defined(SYS__CC_GCC_COMPAT)
static INLINE uint64_t LSBF8_READ(const void *p)
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
#endif


#if !defined(MSBF8_WRITE)
#if defined(SYS__CC_GCC_COMPAT)
static INLINE void MSBF8_WRITE(void *p, uint64_t v)
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
#endif


#if !defined(LSBF8_WRITE)
#if defined(SYS__CC_GCC_COMPAT)
static INLINE void LSBF8_WRITE(void *p, uint64_t v)
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
#endif

#define  SYS_64BIT  64
#endif   /*---  64-bit versions  -------------------------------------------*/
#endif   /*===  endianness conversion  =====================================*/


#endif      /* !defined(COMMON_BASE_H__) */
