GNU = gcc
INTEL = icc
CSTRIP = -s
CFLAGS = -lpng -lm -Iinclude
CPARALLEL = -fopenmp
CSECURE = -O1 -D_GLIBCXX_ASSERTIONS -fasynchronous-unwind-tables -fexceptions -fpie -fpic -Wl,-z,relro -Wl,-z,now -z defs -fcf-protection
FILENAME = main
TEXT = text

all: gcc run verify

icc:
		$(INTEL) -o $(FILENAME) $(FILENAME).c $(CFLAGS) $(CPARALLEL)

strip-icc:
		$(INTEL) -o $(FILENAME) $(FILENAME).c $(CSTRIP) $(CFLAGS) $(CPARALLEL)

gcc:
		$(GNU) -o $(FILENAME) $(FILENAME).c $(CFLAGS)

parallel-gcc:
		$(GNU) -o $(FILENAME) $(FILENAME).c $(CFLAGS) $(CPARALLEL)

secure-gcc:
		$(GNU) -o $(FILENAME) $(FILENAME).c $(CFLAGS) $(CPARALLEL) $(CSECURE)

strip-gcc:
		$(GNU) -o $(FILENAME) $(FILENAME).c $(CSTRIP) $(CFLAGS) $(CPARALLEL)

all-gcc:
		$(GNU) -o $(FILENAME) $(FILENAME).c $(CSTRIP) $(CFLAGS) $(CSECURE) $(CPARALLEL)

run:
		./$(FILENAME) $(TEXT)

open:
		xdg-open embedded.png; xdg-open output.png

verify:
		python3 extract.py embedded.png text
		cat text

clean: 
		rm $(FILENAME) embedded.png output.png embed.log
