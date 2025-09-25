all: build

FILES.h = $(wildcard *.h)
FILES.c = ${FILES.h:.h=.c}
FILES.o = ${FILES.h:.h=.o}

LIBS = -lgsl -lgmp -lm

CFLAGS ?= -O3 -DNDEBUG -flto -march=native

%.o: %.c %.h
	gcc -c $(CFLAGS) -o $@ $<

%.out: %.c ${FILES.o}
	gcc $(CFLAGS) -Wl,-z,execstack -o $@ $^ $(LIBS)

%.valgrind: %.out
	valgrind --leak-check=full \
			 --show-leak-kinds=all \
			 --track-origins=yes \
			 --verbose \
			 --log-file=$@ \
			 ./$^

librvg.a: ${FILES.o}
	ar rcs $@ $^

build: librvg.a ${FILES.h}
	rm -rf build/
	mkdir -p build/include/rvg/
	cp ${FILES.h} build/include/rvg/
	mkdir -p build/lib
	cp librvg.a build/lib

librvg.tar.gz: \
		Makefile Dockerfile LICENSE \
		readme.md api.md api.md.in \
		readme.html api.html tools/ \
		${FILES.h} ${FILES.c} \
		experiments/Makefile experiments/*.c \
		experiments/results.bounds experiments/results.rate \
		examples/Makefile examples/*.c
	tar -c -f $@ -z -- $^

.PHONY: clean
clean:
	rm -rf \
		*.a *.o *.s *.out *.debug __pycache__ \
		*.gcno *.gcov *.gcda *.valgrind \
		build \
		librvg.tar.gz \
		api.md readme.html
