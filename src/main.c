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

    primitive_list_init();

    /* compile each file */
    iterate_range_single(i, 2, argc) {
        /* set input */
        FILE* input = fopen(argv[i], "r");
        if (input == NULL) {
            loge("Unable to open file %s", argv[i]);
            continue;
        }
        yyscan_t scanner;
        if (myylex_init(&scanner) != 0) {
            logfe("unable to initizize the yacc scanner");
        }
        myyset_in(input, scanner);

        /* set output */
        size_t input_name_length = strlen(argv[i]);

        char* source_name = allocate_array(char, input_name_length + sizeof(".c"));
        strncpy(source_name, argv[i], input_name_length);
        strncpy(&source_name[input_name_length], ".c", 3);
        FILE* source = fopen(source_name, "w");
        if (source == NULL) {
            loge("Unable to open file %s for output", source_name);
            continue;
        }
        free(source_name);

        char* header_name = allocate_array(char, input_name_length + sizeof(".c"));
        strncpy(header_name, argv[i], input_name_length);
        strncpy(&header_name[input_name_length], ".h", 3);
        FILE* header = fopen(header_name, "w");
        if (header == NULL) {
            loge("Unable to open file %s for output", header_name);
            continue;
        }

        /* parse */
        se_context* context = context_new();
        if (myyparse(scanner, context) != 0) {
            printf("Parsing file %s failed", argv[i]);
            continue;
        }

        /* do code generation */
        codegen(&context->ast, source, header, header_name);

        /* close the files */
        fclose(input);
        fclose(source);
        fclose(header);
        free(header_name);
    }

    /* success */
    return 0;
}