#include "environment.h"
#include "mpc.h"
#include "parser.h"
#include "val.h"

#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_

typedef struct evaluator {
    parser *parser;
    environment *global_env;
} evaluator;

evaluator *new_evaluator();
void delete_evaluator(evaluator *);
Value *ast_to_val(const mpc_ast_t *t);
Value *evaluate_val(environment *env, Value *);
Value *read_evaluate(evaluator *ev, const char *);

#endif
