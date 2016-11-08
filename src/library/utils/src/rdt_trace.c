#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <rdtrace.h>

#include "rdt_trace.h"

static FILE *output = NULL;
static uint64_t last = 0;
static uint64_t delta = 0;
static unsigned int depth = 0;

static inline void print(const char *type, const char *loc, const char *name) {
	fprintf(output, 
            "%"PRId64",%d,%s,%s,%s\n", 
            delta,
            depth,
            type, 
            CHKSTR(loc),
            CHKSTR(name));
}

static inline void compute_delta() {
    delta = (timestamp() - last) / 1000;
}

void trace_begin() {
    fprintf(output, "DELTA,DEPTH,TYPE,LOCATION,NAME\n");
    fflush(output);

    last = timestamp();
}

void trace_function_entry(const SEXP call, const SEXP op, const SEXP rho) {
    compute_delta();

    const char *type = is_byte_compiled(call) ? "bc-function-entry" : "function-entry";
    const char *name = get_name(call);
    const char *ns = get_ns_name(op);
    char *loc = get_location(op);

    print(type, loc, name);

    if (loc) free(loc);
	
    depth++;
    last = timestamp();
}

void trace_function_exit(const SEXP call, const SEXP op, const SEXP rho, const SEXP retval) {
    compute_delta();
    depth -= depth > 0 ? 1 : 0;
    
    const char *type = is_byte_compiled(call) ? "bc-function-exit" : "function-exit";
    const char *name = get_name(call);
    const char *ns = get_ns_name(op);
    char *loc = get_location(op);
    
    print(type, loc, name);

    if (loc) free(loc);

    last = timestamp();
}

void trace_builtin_entry(const SEXP call) {
    compute_delta();

    const char *name = get_name(call);

    print("builtin-entry", NULL, name);

	depth++;
    last = timestamp();
}

void trace_builtin_exit(const SEXP call, const SEXP retval) {
    compute_delta();
    depth -= depth > 0 ? 1 : 0;

    const char *name = get_name(call);
    
    print("builtin-exit", NULL, name);

    last = timestamp();
}

void trace_force_promise_entry(const SEXP symbol) {
    compute_delta();

    const char *name = get_name(symbol);
    
    print("promise-entry", NULL, name);

	depth++;
    last = timestamp();
}

void trace_force_promise_exit(const SEXP symbol, const SEXP val) {
    compute_delta();
    depth -= depth > 0 ? 1 : 0;

    const char *name = get_name(symbol);

    print("promise-exit", NULL, name);

    last = timestamp();
}

void trace_promise_lookup(const SEXP symbol, const SEXP val) {
    compute_delta();

    const char *name = get_name(symbol);

    print("promise-lookup", NULL, name);
    
    last = timestamp();
}

void trace_error(const SEXP call, const char* message) {
    compute_delta();

    const char *name = get_call(call);
    char *loc = get_location(call);
    
    print("error", NULL, name);

    if (loc) free(loc);
    
    depth = 0;
    last = timestamp();
}

void trace_vector_alloc(int sexptype, long length, long bytes, const char* srcref) {
    compute_delta();
    print("vector-alloc", NULL, NULL);
    last = timestamp();
}

static const rdt_handler trace_rdt_handler = {
    &trace_begin,
    NULL,
    &trace_function_entry,
    &trace_function_exit,
    &trace_builtin_entry,
    &trace_builtin_exit,
    &trace_force_promise_entry,
    &trace_force_promise_exit,
    &trace_promise_lookup,
    &trace_error,
    &trace_vector_alloc,
    NULL, // probe_eval_entry
    NULL  // probe_eval_exit        
};

static int setup_tracing(const char *filename) {
    output = fopen(filename, "wt");
    if (!output) {
        error("Unable to open %s: %s\n", filename, strerror(errno));
        return 0;
    }

    return 1; 
}

static int running = 0;

SEXP RdtTrace(SEXP s_filename) {
    const char *filename = CHAR(STRING_ELT(s_filename, 0));

    if (running) {
        error("RDT is already running");
        return R_TrueValue;
    } else {
        if (setup_tracing(filename)) { 
            rdt_start(&trace_rdt_handler);
            running = 1;
            return R_TrueValue;
        } else {
            error("Unable to initialize dynamic tracing");
            return R_FalseValue;
        }
    }
}

SEXP RdtStop() {
    if (!running) {
        warning("RDT is not running\n");
    } else {
        fclose(output);

        rdt_stop(&trace_rdt_handler);
        running = 0;
    }
    
    return R_FalseValue;
}