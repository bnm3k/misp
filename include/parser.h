#include "../include/mpc.h"
#include <stdbool.h>

#ifndef PARSER_H_
#define PARSER_H_
/*
 * 
*/
typedef struct parse_result {
    bool no_err_occurred;
    mpc_result_t *r;
} parse_result;

/*
 * Quick note on usage. The grammar is defined in parser.c, any
 * modifications on it should go there.
 * 
 * 'parser_init' and 'parser_cleanup' should only be called once 
 * throughout the entirety of the program, each at the start and at 
 * the end respectively
 * 
 * To parse the actual input, such as in a REPL pass the input 
 * (as a string) into 'parse' which returns a pointer to a 
 * `parse_result_t` as defined above. The pointer might be NULL
 * if malloc (which is used internally) fails, therefore the caller 
 * should check for it.
 * 
 * Once done with a non-null 'parse_result_t *' value, the caller 
 * should pass it to `parse_res_cleanup` for, you know, cleanup, 
 * it's safe to pass
 * 
*/

typedef struct parser {
    mpc_parser_t *Int;
    mpc_parser_t *Symbol;
    mpc_parser_t *S_Expr;
    mpc_parser_t *Q_Expr;
    mpc_parser_t *Expr;
    mpc_parser_t *Misp;
} parser;

parser *new_parser();
parse_result *parse_str_to_ast(parser *p, const char *input);
void parse_res_cleanup(parse_result *p_res);
void delete_parser(parser *p);

#endif
