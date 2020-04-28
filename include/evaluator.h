#include "environment.h"
#include "khash.h"
#include "mpc.h"
#include "parser.h"
#include "val.h"

#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_

typedef struct evaluator {
    parser *parser;
    environment *global_env;
    khash_t(sym_table) * sym_table;
} evaluator;

evaluator *new_evaluator();
void delete_evaluator(evaluator *);
Value *ast_to_val(khash_t(sym_table) * st, const mpc_ast_t *t);
Value *evaluate_val(environment *env, Value *);
Value *read_evaluate(evaluator *ev, const char *);

#endif
