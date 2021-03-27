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
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_LCG -pthread tests/test.c src/random.c -o test_lcg -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_CHACHA20 -pthread tests/test.c src/random.c -o test_chacha -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_SPLITMIX -pthread tests/test.c src/random.c -o test_splitmix -lm
	./test_chacha
	./test_sfmt
	./test_xorshiro
	./test_pcg
	./test_lcg
	./test_spitmix
wasm:
	emcc $(CFLAGS) -Wall -O2 -DW_RNG_PCG tests/test.c src/random.c -o docs/pcg.html -lm
	emcc $(CFLAGS) -Wall -O2 -DW_RNG_LCG tests/test.c src/random.c -o docs/lcg.html -lm
	emcc $(CFLAGS) -Wall -O2 -DW_RNG_CHACHA20 tests/test.c src/random.c -o docs/chacha.html -lm
	emcc $(CFLAGS) -Wall -O2 -DW_RNG_MERSENNE_TWISTER tests/test.c src/random.c -o docs/sfmt.html -lm
	emcc $(CFLAGS) -Wall -O2 -DW_RNG_XORSHIRO tests/test.c src/random.c -o docs/xorshiro.html -lm
clean:
	rm -f *~ *.core *.scn *.dvi *.idx *.log tests/*~ test bench benchmark/*~
distclean: clean
	rm -f test_* weaver-memory-manager.pdf src/*
