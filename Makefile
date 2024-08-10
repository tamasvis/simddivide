## Architecture control
##
## set S390_256, AVX512, AVX256, ARM, ARM_SVE to force-select
## S390, AVX-512, AVX2, ARM Neon or ARM SVE, respectively.  The
## default is to use the current architecture (-mtune=native).
##
## we mark generated disassembly with gcc/clang if those
## are set in $(CC).

## AVX512
## -march=native -mtune=native
##
## ID=

## compiler marker if its a known one
CCMARK := $(if $(filter gcc clang,$(CC)),-$(CC),)

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
simdprime$(CCMARK).o: simdprime.c $(wildcard *.h)
	$(CC) $(CWARN) $(COPT) $(CSAN) -Q -v -o $@ -c $< |& \
		tee simdprime$(CCMARK)-build.log

##--------------------------------------
## full disassembled asm
##
simdprime$(CCMARK).s0: simdprime$(CCMARK).o
	objdump -d -C -g -S -r -l -t $< > $@

##--------------------------------------
## minimized asm, removing some source references (-ggdb),
## relocation-only lines, and lines with instruction-trailing 00/01...
##
## note: make quoting interferes with some of grep constructs ('()..')
##
simdprime$(CCMARK).s: simdprime$(CCMARK).s0
	grep -v -e ^/ -e '^[a-z].():$$' $< | \
		grep -v -E '[0-9a-f]:.*R_(X86|AARCH)' | \
	        expand | \
		grep -v -E '  (00 )*[0-9a-f][0-9a-f] $$' > $@
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

