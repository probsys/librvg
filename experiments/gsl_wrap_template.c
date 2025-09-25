/*
  Name:     gsl_wrap_template.c
  Purpose:  Generate wrapped version of GSL rng objects with counter.
  Author:   F. Saad
  Copyright (C) 2025 CMU Probabilistic Computing Systems Lab
*/

#include <stdio.h>
#include <string.h>
#include <gsl/gsl_rng.h>

char * GSL_WRAP_TEMPLATE = "\
GSL_VAR const gsl_rng_type *%1$s_wrap;\n\
\n\
static inline unsigned long int %1$s_wrap_get (void *vstate);\n\
static double %1$s_wrap_get_double (void *vstate);\n\
static void %1$s_wrap_set (void *state, unsigned long int s);\n\
\n\
typedef struct {\n\
    gsl_rng * rng;\n\
    unsigned long n;\n\
} %1$s_wrap_state_t;\n\
\n\
static inline unsigned long int\n\
%1$s_wrap_get (void *vstate) {\n\
    %1$s_wrap_state_t *state = (%1$s_wrap_state_t *) vstate;\n\
    state->n += 1;\n\
    return gsl_rng_get(state->rng);\n\
}\n\
\n\
static double\n\
%1$s_wrap_get_double (void *vstate) {\n\
  %1$s_wrap_state_t *state = (%1$s_wrap_state_t *) vstate;\n\
  state->n += 1;\n\
  return gsl_rng_uniform(state->rng);\n\
}\n\
\n\
static void\n\
%1$s_wrap_set (void *vstate, unsigned long int s) {\n\
  %1$s_wrap_state_t *state = (%1$s_wrap_state_t *) vstate;\n\
\n\
  gsl_rng_free(state->rng);\n\
  state->rng = gsl_rng_alloc(gsl_rng_%1$s);\n\
  gsl_rng_set(state->rng, s);\n\
}\n\
\n\
unsigned long\n\
gsl_rng_%1$s_wrap_get_calls (gsl_rng * rng) {\n\
  %1$s_wrap_state_t *state = (%1$s_wrap_state_t *) (rng->state);\n\
  return state->n;\n\
}\n\
\n\
static const gsl_rng_type %1$s_wrap_type = {\n\
    \"%1$s_wrap\",               /* name */\n\
    0x%2$llX,                  /* RAND_MAX  */\n\
    %3$llx,                  /* RAND_MIN  */\n\
    sizeof (%1$s_wrap_state_t),\n\
    &%1$s_wrap_set,\n\
    &%1$s_wrap_get,\n\
    &%1$s_wrap_get_double,\n\
};\n\
\n\
const gsl_rng_type * gsl_rng_%1$s_wrap = &%1$s_wrap_type;\n\
";

char * replace_char(char* str, char find, char replace){
    char * current_pos = strchr(str,find);
    while (current_pos) {
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}

int main (int argc, char *argv[]) {

    FILE *f;

    // WRITE THE C FILE.
    f = fopen("gsl_wrap.c", "w");
    fprintf(f, "#include <gsl/gsl_rng.h>\n");
    fprintf(f, "#include \"rvg/urandom.h\"\n");
    fprintf(f, "#include \"rvg/deterministic.h\"\n\n");
    // GSL types.
    const gsl_rng_type **t, **t0 = gsl_rng_types_setup();
    for (t = t0; *t != 0; t++) {
        char m[256];
        strcpy(m, (*t)->name);
        char * name = replace_char(m, '-', '_');
        fprintf(f, GSL_WRAP_TEMPLATE, m, (*t)->max, (*t)->min);
    }
    // Custom types.
    fprintf(f, GSL_WRAP_TEMPLATE, "urandom", 0xffffffffffffffffUL, 0);
    fprintf(f, GSL_WRAP_TEMPLATE, "deterministic", 0xffffffffU, 0);
    // fprintf(f, "\n");
    // fprintf(f, "#define gsl_rng_free_wrap(gsl_rng_type, rng) \\\n\
    // gsl_rng_free(((gsl_rng_type##_state_t *)(rng->state))->rng); \\\n\
    // gsl_rng_free(rng);\n\
    // #define gsl_rng_num_calls(gsl_rng_type, rng) ((gsl_rng_type##_state_t *)(rng->state))->n\n");
    fclose(f);

    // WRITE THE .h file
    f = fopen("gsl_wrap.h", "w");
    fprintf(f, "#ifndef GSL_RNG_WRAP_H\n");
    fprintf(f, "#define GSL_RNG_WRAP_H\n");
    fprintf(f, "#include <gsl/gsl_rng.h>\n\n");
    // GSL types.
    for (t = t0; *t != 0; t++) {
        char m[256];
        strcpy(m, (*t)->name);
        char * name = replace_char(m, '-', '_');
        fprintf(f, "GSL_VAR const gsl_rng_type * gsl_rng_%s_wrap;\n", name);
        fprintf(f, "unsigned long gsl_rng_%s_wrap_get_calls (void *vstate);\n", name);
    }
    // Custom types.
    fprintf(f, "GSL_VAR const gsl_rng_type * gsl_rng_%s_wrap;\n", "urandom");
    fprintf(f, "unsigned long gsl_rng_%s_wrap_get_calls (void *vstate);\n", "urandom");
    fprintf(f, "GSL_VAR const gsl_rng_type * gsl_rng_%s_wrap;\n", "deterministic");
    fprintf(f, "unsigned long gsl_rng_%s_wrap_get_calls (void *vstate);\n", "deterministic");

    fprintf(f, "\n");
    fprintf(f, "#define gsl_rng_calls(gsl_rng_type, rng) gsl_rng_type##_get_calls(rng)\n");
    fprintf(f, "#endif\n");
    fclose(f);
}
