#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

#ifdef _WIN32
// Compiling on Windows.
#include <string.h>

// Buffer for user input size 2048.
static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

// Fake add_history function.
void add_history(char* unused) {}

#else
// Compiling on Linux or Mac.
#include <editline/history.h>
#include <editline/readline.h>
#endif

int main(int argc, char** argv) {
  // Create Parsers.
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");

  // Define parser with language.
  mpca_lang(MPCA_LANG_DEFAULT,
            "number : /-?[0-9]+/ ; \
             operator : '+' | '-' | '*' | '/' ; \
             expr : <number> | '(' <operator> <expr>+ ')' ; \
             lispy : /^/ <operator> <expr>+ /$/ ;",
            Number, Operator, Expr, Lispy);

  // Version and Exit information.
  puts("Lispy Version 0.0.0.0.2");
  puts("Press Ctrl+c to Exit\n");

  while (1) {
    char* input = readline("lispy> ");
    add_history(input);

    // Parse user input.
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      // Success -> Print AST.
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      // Print Error.
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    free(input);
  }

  // Undefine and Delete the Parser.
  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  return 0;
}