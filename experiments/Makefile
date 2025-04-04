LIBS = -lrvg -lgsl -lgmp -lm
INCLUDES = -I ../build/include -L ../build/lib/
CFLAGS = -O3 -DNDEBUG

gsl_wrap_template.out: gsl_wrap_template.c
	gcc $(CFLAGS) -o $@ $(INCLUDES) $^ $(LIBS)

gsl_wrap.c gsl_wrap.h: gsl_wrap_template.out
	./gsl_wrap_template.out

%.o: %.c %.h
	gcc -c -o $@ $(CFLAGS) $(INCLUDES) $< $(LIBS)

%.out: %.c gsl_wrap.o
	gcc -o $@ $(CFLAGS) -Wl,-z,execstack $(INCLUDES) $^ $(LIBS)

results.bounds: bounds.out
	mkdir -p $@
	./bounds.out 0 | tee $@/bounds.ran
	./bounds.out 1 | tee $@/bounds.cdf

results.rate: rate.out
	mkdir -p $@
	distributions="\
		beta@5@5 \
		binomial@.2@100 \
		cauchy@7 \
		chisq@13 \
		exponential@15 \
		exppow@1@.5 \
		fdist@5@2 \
		flat@-7@3 \
		gamma@.5@1 \
		gaussian@15 \
		geometric@.4 \
		gumbel1@1@1 \
		gumbel2@1@5 \
		hypergeometric@5@20@7 \
		laplace@2 \
		logistic@.5 \
		lognormal@1@1 \
		negative_binomial@.71@18 \
		pareto@3@2 \
		pascal@1@5 \
		poisson@71 \
		rayleigh@11 \
		tdist@5 \
		weibull@2@3"; \
	seed=10;\
	n=100000;\
	for d in $${distributions}; do \
	    f=$$(echo $$d | tr '@' '_').dist; \
	    ./$< $${seed} $${n} $$(echo $${d} | tr '@' ' ') | tee $@/$${d}; \
	done

.PHONY: table1
table1: results.rate
	cd results.rate && ./table_1.py > table_1.txt

.PHONY: table2
table2: results.bounds
	cd results.bounds && ./table_2.py

.PHONY: figure8
figure8: results.rate
	cd results.rate && ./figure_8.py

.PHONY: clean
clean:
	rm -rf *.out *.o gsl_wrap.c gsl_wrap.h
