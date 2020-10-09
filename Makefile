GNU = gcc
INTEL = icc
CSTRIP = -s
CFLAGS = -lpng -lm
CSECURE = -O1 -D_FORTIFY_SOURCE=3 -D_GLIBCXX_ASSERTIONS -fasynchronous-unwind-tables -fexceptions -fpie -fpic -Wl,-z,relro -Wl,-z,now -z defs -fcf-protection
FILENAME = embed

all: gcc run open

icc:
		$(INTEL) -o $(FILENAME) $(FILENAME).c $(CFLAGS)

secure-icc:
		$(INTEL) -o $(FILENAME) $(FILENAME).c $(CFLAGS) $(CSECURE)

strip-icc:
		$(INTEL) -o $(FILENAME) $(FILENAME).c $(CSTRIP) $(CFLAGS)

all-icc:
		$(INTEL) -o $(FILENAME) $(FILENAME).c $(CSTRIP) $(CFLAGS) $(CSECURE)

gcc:
		$(GNU) -o $(FILENAME) $(FILENAME).c $(CFLAGS)

secure-gcc:
		$(GNU) -o $(FILENAME) $(FILENAME).c $(CFLAGS) $(CSECURE)

strip-gcc:
		$(GNU) -o $(FILENAME) $(FILENAME).c $(CSTRIP) $(CFLAGS)

all-gcc:
		$(GNU) -o $(FILENAME) $(FILENAME).c $(CSTRIP) $(CFLAGS) $(CSECURE)

run:
		./$(FILENAME)

open:
		xdg-open embedded.png; xdg-open output.png
