all: main.out readme.out

LIBS = -lrvg $(shell gsl-config --libs-without-cblas) -lgmp
INCLUDES = -I ../build/include -L ../build/lib/
CFLAGS = -O3 -DNDEBUG -Wl,-z,execstack $(shell gsl-config --cflags)

%.out: %.c
	gcc -o $@ $(CFLAGS) $(INCLUDES) $^ $(LIBS)

.PHONY: clean
clean:
	rm -rf *.out
