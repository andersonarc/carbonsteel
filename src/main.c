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
#include "ast/type/primitive.h" /* primitives */
#include "codegen/codegen.h" /* code generation */
#include "misc/memory.h" /* memory allocation */
#include "language/parser.h" /* parser */
#include "language/native/parser.h"
#include "language/lexer.h" /* lexer */
#include <stdlib.h>

    /* functions */
/**
 * Compiler entrypoint
 * 
 * @param argc Argument count (at least 2)
 * @param argv Arguments (compiler action and files)
 */
int main(int argc, char* argv[]) {
    //cyydebug = 1;
    arraylist(char_ptr) output_files;
    arl_init(char_ptr, output_files);

    arraylist(char_ptr) input_files;
    arl_init(char_ptr, input_files);


    /* check the arguments */
    if (strncmp(argv[1], "forge", 5) != 0) {
        loge("Unknown compiler option: %s", argv[1]);
        logfe("Please specify an action for the compiler: [forge]");
    }
    if (argc < 3) {
        logfe("Please specify which files to compile after the compiler action");
    }

    primitive_list_init();

    /* add input files */
    bool output_specified = false;
    iterate_range_single(i, 2, argc) {
        if (!output_specified) {
            if (strncmp(argv[i], "-o", sizeof("-o")) == 0) {
                output_specified = true;
                continue;
            }
            char* filename =  realpath(argv[i], NULL);
            arl_add(char_ptr, input_files, filename);
            if (filename == NULL) {
                logfe("failed to determine the absolute path to %s", argv[i]);
            }
        } else {
            arl_add(char_ptr, output_files, argv[i]);
        }
    }
    if (!output_specified) {
        iterate_array(i, input_files.size) {
            size_t length = strlen(input_files.data[i]);
            char* output_filename = allocate_array(char, length + sizeof(".c"));
            strncpy(output_filename, input_files.data[i], length);
            strcpy(output_filename + length, ".c");
            output_filename[length + sizeof(".c") - 1] = 0;
            arl_add(char_ptr, output_files, output_filename);
        }
    } else {
        if (input_files.size != output_files.size) {
            logfe("There must be an equal number of input and output filenames provided,"
                "but got %zu input files and %zu output files", input_files.size, output_files.size);
        }
    }

    /* compile each file */
    iterate_array(i, input_files.size) {
        /* parse */
        se_context* context = context_new();
        context_parse_origin(context, input_files.data[i]);

        /* set output */
        FILE* output = fopen(output_files.data[i], "w");
        if (output == NULL) {
            loge("Unable to open file %s for output", output_files.data[i]);
            continue;
        }

        /* do code generation */
        codegen(&context->ast, output);

        /* close the files */
        fclose(output);
    }

    /* success */
    return 0;
}