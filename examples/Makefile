all: main.out readme.out

LIBS = -lrvg -lgsl -lm
INCLUDES = -I ../build/include -L ../build/lib/
CFLAGS = -O3 -DNDEBUG -Wl,-z,execstack

%.out: %.c
	gcc -o $@ $(CFLAGS) $(INCLUDES) $^ $(LIBS)

.PHONY: clean
clean:
	rm -rf *.out
