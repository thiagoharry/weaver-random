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
	${CC} ${FLAGS} -DW_RNG_MERSENNE_TWISTER -pthread tests/test.c src/random.c -o test -lbsd
	./test
#web-test:
#	emcc  tests/test.c src/memory.c -s WASM=1 -o doc/test/test.html
#web-benchmark:
#	emcc src/memory.c benchmark/benchmark.c  -s WASM=1 -o doc/benchmark/bench.html
#benchmark: src benchmark/benchmark.c src/memory.c
#	${CC} ${FLAGS} src/memory.c benchmark/benchmark.c -o bench -lm 
#	./bench
clean:
	rm -f *~ *.core *.scn *.dvi *.idx *.log tests/*~ test bench benchmark/*~
distclean: clean
	rm -f test weaver-memory-manager.pdf src/*
