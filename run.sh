#!/bin/sh

## export AVX in env
## we supply the rest below

## collect all generated .asm into this directory
## note: res.asm is marked to git-ignore; recommended for local results
##
GENDIR=res.asm
BUILD_LOG=$GENDIR/build.log

make clean

for compiler in gcc clang ; do
	for simd in {0,1} ; do
		export CC=$compiler
		[ $simd -le 0 ] && export NOSIMD=1

		make asm asmfns |& tee -a $BUILD_LOG && \
			cp *.s $GENDIR && make clean
	done
done

