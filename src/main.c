/**
 * @file main.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-08-13
 * 
 *  Compiler main function and interface
 */
    /* includes */
#include "ast/root.h" /* ast */
#include "codegen/codegen.h" /* code generation */
#include "misc/memory.h" /* memory allocation */
#include "language/parser.h" /* parser */
#include "language/lexer.h" /* lexer */

    /* functions */
/**
 * Compiler entrypoint
 * 
 * @param argc Argument count (at least 2)
 * @param argv Arguments (compiler action and files)
 */
int main(int argc, char* argv[]) {
    //myydebug = 1;

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

    primitive_list_init();

    /* compile each file */
    iterate_range_single(i, 2, argc) {
        /* parse */
        se_context* context = context_new();
        context_parse_origin(context, argv[i]);

        /* set output */
        size_t input_name_length = strlen(argv[i]);
        char* output_name = allocate_array(char, input_name_length + sizeof(".c"));
        strncpy(output_name, argv[i], input_name_length);
        strncpy(&output_name[input_name_length], ".c", 3);
        FILE* output = fopen(output_name, "w");
        if (output == NULL) {
            loge("Unable to open file %s for output", output_name);
            continue;
        }
        free(output_name);

        /* do code generation */
        codegen(&context->ast, output);

        /* close the files */
        fclose(output);
    }

    /* success */
    return 0;
}