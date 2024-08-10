## Architecture control
##
## set S390_256, AVX512, AVX256, ARM, ARM_SVE to force-select
## S390, AVX-512, AVX2, ARM Neon or ARM SVE, respectively.  The
## default is to use the current architecture (-mtune=native).

## AVX512
## -march=native -mtune=native
##
## ID=

OPTLEVEL := -O3
TUNE     := -march=native -mtune=native

CWARN := -Wall -Wextra -Wshadow -Wformat=2 -Wredundant-decls \
         -Wno-packed -Wnonnull -Winit-self -Wwrite-strings   \
         -Werror=implicit-int -Werror=implicit-function-declaration \
         -Werror=int-conversion -Werror=strict-prototypes \
         -Werror=old-style-definition

## sanitizers and related diags
CSAN := -fstack-usage

COPT := $(OPTLEVEL) $(TUNE)

simdprime.o: simdprime.c $(wildcard *.h)
	$(CC) $(CWARN) $(COPT) $(CSAN) -o $@ $<

## $(CC) $CWARN0 $(MTUNE) -O3 -ggdb3 -Q -c simdprime$(ID).c |& \
##   tee build$(ID).log && 
## dis simdprime$(ID).o | tee simdprime$(ID).s0 &&
##   unsrc simdprime$(ID).s0 | nosimd > simdprime$(ID).s

