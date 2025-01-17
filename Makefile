## Architecture control
##
## set S390_256, AVX512, AVX256, ARM, ARM_SVE to force-select
## S390, AVX-512, AVX2, ARM Neon or ARM SVE, respectively.  The
## default is to use the current architecture (-mtune=native).
##
## we mark generated disassembly with gcc/clang if those
## are set in $(CC).
##
## set 'NOAVX512' to prevent use of AVX-512 (for AVX2 platforms)
##
## generated files are marked by -..arch..-..compiler.. by default.
## TODO: add NOMARK

## AVX512
## -march=native -mtune=native
##
## ID=
##
## TODO: force AVX512 (arch=sapphirerapids)
## TODO: ARM: switch between Neon, SVE/256 and SVE/512


## Tier 1 compilers to test for
CCTIER1 := $(if $(filter gcc clang,$(CC)),$(CC),)
##
## compiler marker if its a known one (-gcc/-gclang...) or empty
## the ID we include in generated files' names
CCMARK  := $(if $(filter gcc clang tcc,$(CC)),-$(CC),)

ifeq ($(CC),cc)
$(error "specify CC=gcc/clang/tcc...")
endif

## platform mnemonic, if known
PLATFORM := $(filter x86_64 aarch64,$(shell uname -m))
##
PF.x86_64  := amd64
PF.aarch64 := arm64
PF := -$(PF.$(PLATFORM))
##
## TODO: stop here on unknown platforms (if PF==- by now)


OPTLEVEL := -O3
TUNE     := -march=native -mtune=native
PROF     := -ggdb3


CWARN := -Wall -Wextra -Wshadow -Wformat=2 -Wredundant-decls \
         -Wno-packed -Wnonnull -Winit-self -Wwrite-strings   \
         -Werror=implicit-int -Werror=implicit-function-declaration \
         -Werror=int-conversion -Werror=strict-prototypes \
         -Werror=old-style-definition

## sanitizers and related diags
CSAN := -fstack-usage

COPT := $(OPTLEVEL) $(TUNE) $(PROF)


##--------------------------------------
## some of the settings are not tcc-friendly
## we only use tcc for baseline comparison; safe to skip features/switches
##
## mark such -...or.empty... as '...OR0'
##
Q_OR0        := $(if $(CCTIER1),-Q,)
##
## note: set this after -mtune...
NOAVX512_OR0 := $(if $(NOAVX512),-mno-avx512f)
##
## -mno-avx512f  -- 'AVX-512 Foundation' which disables the rest of AVX-512
## see gcc.gnu.org/onlinedocs/gcc/x86-Options.html
##
## force AVX512:
##   -march=graniterapids -mtune=graniterapids
##   -march=emeraldrapids -mtune=emeraldrapids
##
## apply these only after setting anything system-specific
ALL_OR0 := $(Q_OR0) $(NOAVX512_OR0)


##--------------------------------------
## marker for this setup
MARK := $(PF)$(CCMARK)


##--------------------------------------
simdprime$(MARK).o: simdprime.c $(wildcard *.h)
	$(CC) $(CWARN) $(COPT) $(CSAN) $(ALL_OR0) -v -o $@ -c $< |& \
		tee simdprime$(MARK)-build.log

##--------------------------------------
## full disassembled asm
##
simdprime$(MARK).s0: simdprime$(MARK).o
	objdump -d -C -g -S -r -l -t $< > $@

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


.PHONY: clean  tidy

