#CC=gcc
FLAGS=-Wall -O2

report:
	magitex-cweb weaver-random.tex
	dvipdf weaver-random.dvi
#doc_en:
#	tex weaver-memory-manager_en.tex
#	dvipdf weaver-memory-manager_en.dvi
src/random.c: weaver-random.tex
	ctangle weaver-random.tex
	rm weaver-random.c
test: tests/test.c src/random.c
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_PCG -pthread tests/test.c src/random.c -o test_pcg -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_MERSENNE_TWISTER -pthread tests/test.c src/random.c -o test_sfmt -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_XORSHIRO -pthread tests/test.c src/random.c -o test_xorshiro -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_ISO_C -pthread tests/test.c src/random.c -o test_iso -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_CRYPTO -pthread tests/test.c src/random.c -o test_crypto -lm
	./test_sfmt
	./test_xorshiro
	./test_pcg
	./test_iso
	./test_crypto
test_performance: tests/test.c src/random.c
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_PCG -pthread benchmark/benchmark.c src/random.c -o bench_pcg  -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_MERSENNE_TWISTER  -pthread benchmark/benchmark.c src/random.c -o bench_sfmt -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_XORSHIRO -pthread benchmark/benchmark.c src/random.c -o bench_xorshiro  -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_ISO_C  -pthread benchmark/benchmark.c src/random.c -o bench_iso  -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_CRYPTO  -pthread benchmark/benchmark.c src/random.c -o bench_crypto  -lm
	./bench_sfmt
	./bench_xorshiro
	./bench_pcg
	./bench_iso
	./bench_crypto
clean:
	rm -f *~ *.core *.scn *.dvi *.idx *.log tests/*~ test bench benchmark/*~
distclean: clean
	rm -f test_* weaver-memory-manager.pdf src/*
