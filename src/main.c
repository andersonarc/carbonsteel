/**
 * @file main.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-08-13
 * 
 *  Compiler main function and interface
 */
#include "ast.h" /* ast */
#include "parser.h" /* parser */
#include "lexer.h" /* lexer */
#include "stack.h" /* stack */
#include "codegen.h" /* code generation */

/**
 * Compiler entrypoint
 * 
 * @param argc Argument count (at least 2)
 * @param argv Arguments (compiler action and files)
 */
int main(int argc, const char* argv[]) {
    /* check the arguments */
    if (argc < 2) {
        logfe("Please specify an action for the compiler: [forge]");
    }
    if (strncmp(argv[1], "forge", 5) != 0) {
        loge("Unknown compiler option: %s", argv[1]);
        logfe("Please specify an action for the compiler: [forge]");
    }
    if (argc < 3) {
        logfe("Please specify which files to compile after the compiler action");
    }

    /* initialize the compiler */
    ast_init();
    stack_init();

    /* compile each file */
    iterate_range_single(i, 2, argc) {
        /* set input */
        FILE* input = fopen(argv[i], "r");
        if (input == NULL) {
            loge("Unable to open file %s", argv[i]);
            continue;
        }
        yyset_in(input);

        /* set output */
        size_t input_name_length = strlen(argv[i]);
        char* output_name = malloc(input_name_length + sizeof(".c"));
        strncpy(output_name, argv[i], input_name_length);
        strncpy(&output_name[input_name_length], ".c", 3);
        FILE* output = fopen(output_name, "w");
        if (output == NULL) {
            loge("Unable to open file %s for output", output_name);
            continue;
        }
        free(output_name);

        /* parse */
        if (yyparse() != 0) {
            printf("Parsing file %s failed", argv[i]);
            continue;
        }

        /* do code generation */
        codegen(output);

        /* close the files */
        fclose(input);
        fclose(output);
    }

    /* success */
    return 0;
}