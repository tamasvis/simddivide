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
	$(CC) $(CWARN) $(COPT) $(CSAN) -o $@ -c $<

##--------------------------------------
## full disassembled asm
##
simdprime$(CCMARK).s0: simdprime$(CCMARK).o
	objdump -d -C -g -S -r -l -t $< > $@

##--------------------------------------
## minimized asm, removing some source references (-ggdb),
## relocation-only lines, and lines with instruction-trailing 00/01...
##
simdprime$(CCMARK).s: simdprime$(CCMARK).s0
	grep -v -e ^/ -e '^[a-z].():$$' $< | \
		grep -v -E '[0-9a-f]:.*R_(X|AARCH)' | \
		grep -v $'\t[0-9a-f][0-9a-f] $'     > $@

##--------------------------------------
GEN   += simdprime*.o *.su
CLEAN += simd*.s simd*.s0

## $(CC) $CWARN0 $(MTUNE) -O3 -ggdb3 -Q -c simdprime$(ID).c |& \
##   tee build$(ID).log && 
##
## dis simdprime$(ID).o | tee simdprime$(ID).s0 &&
##   unsrc simdprime$(ID).s0 | nosimd > simdprime$(ID).s


##--------------------------------------
tidy:
	$(if $(wildcard $(GEN)),$(RM) $(wildcard $(GEN)))

clean: tidy
	$(if $(wildcard $(CLEAN)),$(RM) $(wildcard $(CLEAN))) 


.PHONY: clean  tidy

