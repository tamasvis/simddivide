#!/bin/sh

## export AVX in env
## we supply the rest below

## collect all generated .asm into this directory
## note: res.asm is marked to git-ignore; recommended for local results
##
GENDIR=res.asm
BUILD_LOG=$GENDIR/build.log
ITERATE=21

make clean

for compiler in gcc clang ; do
	for simd in {0,1} ; do
		export CC=$compiler NOSIMD=
		[ $simd -le 0 ] && export NOSIMD=1

		BINARY=$( echo $( make measure.name ) | sed 's/.*NAME=//' )
		echo $BINARY

		time make -j asm asmfns measure |& tee -a $BUILD_LOG && \
			cp *.s $GENDIR

		rm -f $GENDIR/$BINARY-perf.log

		for i in $( seq -w $ITERATE ) ; do
			time ./$BINARY |& tee -a $GENDIR/$BINARY-perf.log
		done

		make clean
	done
done

