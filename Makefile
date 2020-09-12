CC=gcc
FLAGS=-Wall -O2

report:
	magitex-cweb weaver-random.tex
	dvipdf weaver-random.dvi
#doc_en:
#	tex weaver-memory-manager_en.tex
#	dvipdf weaver-memory-manager_en.dvi
src: weaver-random.tex
	ctangle weaver-random.tex
	rm weaver-random.c
test: src tests/test.c src/random.c
	${CC} ${FLAGS} -DW_RNG_PCG -pthread tests/test.c src/random.c -o test_pcg -lbsd
	${CC} ${FLAGS} -DW_RNG_MERSENNE_TWISTER -pthread tests/test.c src/random.c -o test_sfmt -lbsd
	${CC} ${FLAGS} -DW_RNG_XORSHIRO -pthread tests/test.c src/random.c -o test_xorshiro -lbsd
	./test_sfmt
	./test_xorshiro
	./test_pcg
benchmark: src tests/test.c src/random.c
	${CC} ${FLAGS} -DW_RNG_PCG -pthread benchmark/benchmark.c src/random.c -o bench_pcg -lbsd -lm
	${CC} ${FLAGS} -DW_RNG_MERSENNE_TWISTER -pthread benchmark/benchmark.c src/random.c -o bench_sfmt -lbsd -lm
	${CC} ${FLAGS} -DW_RNG_XORSHIRO -pthread benchmark/benchmark.c src/random.c -o bench_xorshiro -lbsd -lm
	./bench_sfmt
	./bench_xorshiro
	./bench_pcg
clean:
	rm -f *~ *.core *.scn *.dvi *.idx *.log tests/*~ test bench benchmark/*~
distclean: clean
	rm -f test_* weaver-memory-manager.pdf src/*
