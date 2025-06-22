## Architecture control
##
## we force architecture to match one of the following predefined
## configurations, by expecting one of the following env. variables:
##     AVX=256    -- amd64, AVX2 (256-bit)
##     AVX=512
##     S390       -- SIMD extensions (256-bit)
##     ARM        -- ARM64, Neon (128-bit)
##     ARMSVE     -- ARM64, Scalable Vector Extensions
##                -- =256 or =512 to force specific SVE bitwidth
##     NOSIMD     -- prohibit use of SIMD extensions (while still inferring
##                   architecture, to know the exact invocation to disable)
##
## please define exactly one of these (except NOSIMD); results when
## multiple are defined are unpredictable.
## we use amd64 with AVX2 as default.
##
## set CC to gcc or clang to force specific compiler.
## results are marked 'architecture-subvariant-compiler'


## the interaction of AVX2/512/10 switches, and their prohibition,
## is compiler etc. dependent; we show a best-effort attempt below.
##
## see inbox.sourceware.org/gcc-patches/ZNT%2Fz8wE%2F4TW0F6Z@tucnak/T/
## for a representative example
##
## switches to disable *every* feature even remotely related to AVX:
NO_AVX_ALL := -mno-avx512f -mno-avx10.2 -mno-avx2 -mno-avx \
              -mno-avx512vl -mno-sse2 -mno-sse3 -mno-sse4
##
## not on clang: -mno-avx10.1-256 -mno-avx10.1-512
##
## please do not comment on how nice this is
##
## we are aware that mixing AVX-512 and AVX-10 options are *strongly*
## discouraged; we assume any target would resolve this (and we picked
## these options only to demonstrate our primitives' compilation)
##
## since our AVX primitives update memory-resident vectors, but there are no
## AVX return values, we do not expect AVX512/AVX10 interference:
##
## from clang.llvm.org/docs/UsersManual.html:
## > Current binaries built with AVX512 features can run on Intel
## > AVX10/512 capable processors without re-compile, but cannot run on
## > AVX10/256 capable processors. Users need to re-compile their code
## > with -mavx10.N, and maybe update some code that calling to 512-bit
## > X86 specific intrinsics and passing or returning 512-bit vector
## > types in function call, if they want to run on AVX10/256 capable
## > processors. Binaries built with -mavx10.N can run on both AVX10/256
## > and AVX10/512 capable processors.


##-----  pick up BUILD_ARCH, TUNE_ARCH and DESCR for processor/instr.set
## note: do not leave spaces in DESCR
##
ifeq ($(AVX),256)
BUILD_ARCH := -march=x86-64-v2 -mno-avx512f -mno-avx10.2 -mavx2
TUNE_ARCH  :=
DESCR      := amd64-avx2
		## -mno-avx512f is redundant; it prohibits AVX-512 by ignoring
		## the ..512f 'fundamental' instructions, which every other
		## AVX-512 feature depends on 
		##
		## arch=x86-64-v2 applies generic tuning
ifneq ($(NOSIMD),)
BUILD_ARCH := -march=x86-64-v2 $(NO_AVX_ALL)
endif

else ifeq ($(AVX),512)  ##----------------------------------------------------
BUILD_ARCH := -march=x86-64-v4 -mavx10.2
TUNE_ARCH  :=
DESCR      := amd64-avx512
		## note: -mavx10.1-512 is deprecated by gcc15
ifneq ($(NOSIMD),)
BUILD_ARCH := -march=x86-64-v2 $(NO_AVX_ALL)
endif


else ifeq ($(ARM),1)  ##-----  ARM/neon  -------------------------------------
BUILD_ARCH := -march=armv8-a+simd
TUNE_ARCH  :=
DESCR      := arm64-neon
##
ifneq ($(NOSIMD),)
BUILD_ARCH := -march=armv8-a+nosimd
endif


else ifeq ($(ARMSVE),1)   ##-----  ARM/SVE  ----------------------------------
$(error "ARM/SVE things come here")
BUILD_ARCH := -march=armv8-a+simd
TUNE_ARCH  :=
DESCR      := arm64-sve
##
ifneq ($(NOSIMD),)
BUILD_ARCH := -march=armv8-a+nosimd
endif

else
$(error "environment not defined")

endif


##---  mark SIMD-prohibited build
ifneq ($(NOSIMD),)
DESCR := $(DESCR)-nosimd
endif


## special cases:
##   -march=native is x86/riscv-only for clang
## BUILD_ARCH := $(ifeq $(CC)-$(PF),clang-arm64,,$(BUILD_ARCH))


## ID=
##
## TODO: force AVX512 (arch=sapphirerapids)
##
## ARM:
##   use Neon:
##     ... -march=armv8-a+neon ...
##   force SVE:
##     ... -march=armv8-a+sve ...  (also: armv9-a+sve2)
##   SVE of specific bit width:
##     ... -msve-vector-bits=256 ...
##
##   autovectorization is enabled with -O3.
##   -fopt-info-vec returns which loops were vectorized.
##   -fopt-info-vec-missed reports loops which failed to vectorize.
##
##   see 'Port Code to Arm Scalable Vector Extension (SVE)' and
##   'Neon Programmer Guide for Armv8-A Coding for Neon' from Arm.
##   note that our loops are explicitly unrolled, so few real loops remain.
##
##   TODO: check 'aarch64-autovec-preference', which allows manual
##   control of Neon/SVE preferences.

## global marker; DESCR is append in the end
MARK     :=

## Tier 1 compilers to test for
CCTIER1 := $(if $(filter gcc clang,$(CC)),$(CC),)
##
## compiler marker if its a known one (-gcc/-gclang...) or empty
## the ID we include in generated files' names
CCMARK  := $(if $(filter gcc clang tcc,$(CC)),-$(CC),)

ifeq ($(CC),cc)
$(error "specify CC=gcc/clang/tcc...")
endif

## ## platform mnemonic, if known; map to amd64/arm64
## ##
## PLATFORM := $(filter x86_64 aarch64,$(shell uname -m))
## ##
## PF.x86_64  := amd64
## PF.aarch64 := arm64
## PF := -$(PF.$(PLATFORM))
## ##
## ## TODO: stop here on unknown platforms (if PF==- by now)


OPTLEVEL := -O3
PROF     := -ggdb3


## ## pick up ARCH; we do not support all AVX/env combinations
## BUILD_ARCH_AVX512 := -march=graniterapids
## TUNE_ARCH_AVX512  := -mtune=graniterapids
## ##
## BUILD_ARCH_AVX512 := \
##     $(ifeq $(CC),clang,-march=x86-64-v4,-march=x86-64-v4) \
##     -mavx10.1-512
## TUNE_ARCH_AVX512  :=
## ##
## BUILD_ARCH := -march=native
## ##
## ## TODO:
## BUILD_ARCH := $(BUILD_ARCH_AVX512)
## TUNE_ARCH  := $(TUNE_ARCH_AVX512)
## MARKPLUS   += -avx512
## ##
## ## special cases:
## ##   -march=native is x86/riscv-only for clang
## ## BUILD_ARCH := $(ifeq $(CC)-$(PF),clang-arm64,,$(BUILD_ARCH))


## warnings supported by all our targeted gcc/clang versions
##
CWARN := -Wall -Wextra -Wshadow -Wformat=2 -Wredundant-decls \
         -Wno-packed -Wnonnull -Winit-self -Wwrite-strings   \
         -Werror=implicit-int -Werror=implicit-function-declaration \
         -Werror=int-conversion -Werror=strict-prototypes \
         -Werror=old-style-definition

## sanitizers and related diags
CSAN := -fstack-usage

COPT := $(OPTLEVEL) $(TUNE_ARCH) $(BUILD_ARCH) $(PROF)

## verbose disassembly
DISASM := objdump -d -C -g -S -r -l -t

## remove interleaved source-file markers
UNSRC := grep -v -e ^/ -e '^[a-z].*[^a-z0-9]:$$'

## drop all empty lines
NWSPACE := grep .


##--------------------------------------
## some of the settings are not tcc-friendly
## we only use tcc for baseline comparison; safe to skip features/switches
##
## mark such -...or.empty... as '...OR0'
##
## TODO: which non-x86 clang lacks -Q?
Q_OR0 := $(if $(CCTIER1),-Q,)
##
## note: set this after -mtune...
NOAVX512_OR0 := $(if $(NOAVX512),-mno-avx512f)
##
## -mno-avx512f  -- 'AVX-512 Foundation' which disables the rest of AVX-512
## see gcc.gnu.org/onlinedocs/gcc/x86-Options.html
##
## force AVX512 (examples):
##   -march=graniterapids -mtune=graniterapids
##   -march=emeraldrapids -mtune=emeraldrapids
##
## apply these only after setting anything system-specific
ALL_OR0 := $(Q_OR0) $(NOAVX512_OR0)


##--------------------------------------
## marker for this setup
MARK := $(PF)-$(DESCR)$(CCMARK)


##--------------------------------------
simdprime$(MARK).o: simdprime.c $(wildcard *.h)
	$(CC) $(CWARN) $(COPT) $(CSAN) $(ALL_OR0) -v -o $@ -c $< | \
		tee simdprime$(MARK)-build.log

##--------------------------------------
## full disassembled asm
##
simdprime$(MARK).s0: simdprime$(MARK).o
	$(DISASM) $< > $@

##--------------------------------------
## minimized asm, removing some source references (-ggdb),
## relocation-only lines, and lines with instruction-trailing 00/01...
##
## note: make quoting interferes with some of grep constructs ('()..')
##
simdprime$(MARK).s: simdprime$(MARK).s0
	grep -v -e ^/ -e '^[a-z].*[^a-z0-9]:$$' $< | \
		grep -v -E '[0-9a-f]:.*R_(X86|AARCH)' | \
	        expand | \
		grep -v -E '  (00 )*[0-9a-f][0-9a-f] $$' > $@
##
## (1) file/function context, repated for most source lines:
##       simd_no_spfactor64x16():
##       /home/.../simdprime/simdprime.c:315
##     note that plain-grep '()' interferes with makefile quoting,
##     we just pick a 'close enough' pattern to remove
##
## (3) typical instruction-terminating straggler bytes:
##       ...
##       1ab5c:   c4 e2 65 00 05 00 00    vpshufb 0x0(%rip),%ymm3,%ymm0
##       1ab63:   00 00  
##       ...
##       1ae48:   62 72 f5 49 66 35 00    vpblendmw 0x0(%rip),%zmm1,%zmm14{%k1}
##       1ae4f:   00 00 00
##       ...
##         res[  0 ] = v[  0 ] * coeff[  0 ];
##       2225f:   41 0f b7 87 80 07 00    movzwl 0x780(%r15),%eax
##       22266:   00
##       22267:   c5 fd 6f 05 00 00 00    vmovdqa 0x0(%rip),%ymm0
##       2226e:   00
##       ...
##     note: with trailing whitespace:
##       ...
##       ^Ires[  0 ] = v[  0 ] * coeff[  0 ];$ 
##         2225f:^I41 0f b7 87 80 07 00 ^Imovzwl 0x780(%r15),%eax$
##         22266:^I00 $ 
##         22267:^Ic5 fd 6f 05 00 00 00 ^Ivmovdqa 0x0(%rip),%ymm0
##         2226e:^I00 $
##       ...


## entire disassembled file
asm: simdprime$(MARK).s


## representative disassembled functions
## generates simdprime$(MARK)-fns.s
##:
simdprime$(MARK)-fns.s: simdprime$(MARK).o
	$(CC) --version >  $@
	echo            >> $@
	echo OPTIONS: $(OPTLEVEL) $(TUNE_ARCH) $(BUILD_ARCH) $(PROF)      >> $@
	echo            >> $@
	gdb -batch -ex "disassemble/rs sfsieve_advance_l" \
		-ex "disassemble/rs twin_advance_l" $^ \
			| $(NWSPACE) >> $@


asmfns: simdprime$(MARK)-fns.s


##--------------------------------------
GEN   += simdprime*.o *.su
CLEAN += simd*.s simd*.s0 simd*log
##
## assume we wish to retain disassembly etc. as 'final output'


##--------------------------------------
tidy:
	$(if $(wildcard $(GEN)),$(RM) $(wildcard $(GEN)))

clean: tidy
	$(if $(wildcard $(CLEAN)),$(RM) $(wildcard $(CLEAN))) 


.PHONY: clean  tidy  asm  asmfns

