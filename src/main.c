#include "../include/evaluator.h"
#include "../include/val.h"
#include <editline/readline.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __linux__
#include <editline/history.h>
#endif

int main(void) {
    puts("Misp Version 0.0.1");
    puts("Press ctrl-c to exit");
    evaluator *ev = new_evaluator();

    while (true) {
        char *input = readline("\nmisp> ");
        add_history(input);

        Value *res = read_evaluate(ev, input);
        print_val(res);

        deallocate_value(res);
        free(input);
    }
    delete_evaluator(ev);
    return 0;
}
