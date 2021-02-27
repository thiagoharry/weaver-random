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
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_LCG -pthread tests/test.c src/random.c -o test_lcg -lm
	$(CC) $(CFLAGS) -Wall -O2 -DW_RNG_CHACHA20 -pthread tests/test.c src/random.c -o test_chacha -lm
	./test_chacha
	./test_sfmt
	./test_xorshiro
	./test_pcg
	./test_lcg
clean:
	rm -f *~ *.core *.scn *.dvi *.idx *.log tests/*~ test bench benchmark/*~
distclean: clean
	rm -f test_* weaver-memory-manager.pdf src/*
