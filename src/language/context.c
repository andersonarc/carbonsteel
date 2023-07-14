/**
 * @file context.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-08
 * 
 *  Parser context function implementations
 */
    /* includes */
#include "language/context.h" /* this */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include "syntax/declaration/declaration.h" /* declarations */
#include "misc/memory.h" /* memory allocation */
#include "language/parser.h" /* parser */
#include "language/lexer.h" /* lexer */

    /* global variables */
/**
 * Parser context level kind string values
 */
const char* se_context_level_kind_strings[] = {
    "GLOBAL", "IMPORT", "FUNCTION", "EXPRESSION", "ENUM", "FLAG"
};

    /* functions */
/**
 * Allocates and initializes a new parser context
 * 
 * @return Pointer to the new parser context
 */
se_context* context_new() {
    se_context* context = allocate(se_context);
    arl_init(se_context_level, context->stack);
    context->skip_pair_count = 0;
    context->skip_until = 0;
    context->expect_skip_from = false;
    context->pass = SCTX_PASS_3;

    /* initialize the file list */
    arl_init(se_context_import_file_ptr, context->file_list);

    /* add a global level */
    se_context_level global;
    global.kind = SCTX_GLOBAL;
    arl_add(se_context_level, context->stack, global);

    /* initialize the AST */
    ast_init(&context->ast);

    return context;
}


/**
 * Add a new context level
 * on top of the context stack
 * 
 * @param context Pointer to the parser context
 * @param kind    Kind of the context level
 */
void context_enter(se_context* context, se_context_level_kind kind) {
    se_context_level level = { .kind = kind };

    /* initialize the level value */
    switch (kind) {
        case SCTX_EXPRESSION:
            arraylist_init_empty(ex_constructor_ptr)(&level.u_ex_block.constructors);
            break;

        case SCTX_FUNCTION:
            arl_init(ast_local_declaration, level.u_locals);
            break;

        case SCTX_ENUM:
            level.u_enum_context.value = allocate(dc_enum);
            level.u_enum_context.member_index = 0;
            level.u_enum_context.kind = ENUM_KIND_UNKNOWN;
            break;

        case SCTX_FLAG:
            flag_context_init(level.u_flag_context);
            break;

        case SCTX_IMPORT:
        case SCTX_GLOBAL:
            break;

        otherwise_error
    }

    arl_add(se_context_level, context->stack, level);
}


/**
 * Finds a last occurence of a specified
 * level kind in the context stack and
 * returns it or NULL
 * 
 * @param[in] context Pointer to the parser context
 * @param[in] kind    The kind to search for
 */
se_context_level* context_find(se_context* context, se_context_level_kind kind) {
    for (int i = context->stack.size - 1; i >= 0; i--) {
        se_context_level* current = &context->stack.data[i];
        if (current->kind == kind) {
            return current;
        }
    }
    return NULL;
}


/**
 * Returns the last context stack level
 * if its kind matches expected, otherwise
 * throws an internal error
 * 
 * @param context Pointer to the parser context
 * @param kind    Expected kind of the last context level
 * 
 * @return Pointer to the last context level 
 */
se_context_level* context_get(se_context* context, se_context_level_kind kind) {
    se_context_level* level = &arraylist_last(context->stack);

    if (level->kind != kind) {
        error_internal("unexpected context level kind: expected \"%s\", got \"%s\"",
                        se_context_level_kind_strings[kind],
                        se_context_level_kind_strings[level->kind]);
    }

    return level;
}


/**
 * Removes the last context level and
 * releases its contents if needed
 * 
 * @param context Pointer to the parser context
 */
void context_exit(se_context* context) {
    if (arraylist_last(context->stack).kind == SCTX_GLOBAL) {
        error_internal("attempted to exit from the global context");
    }
    
    arl_pop(se_context_level, context->stack);
}


/**
 * Translates an import statement into a relative filename
 * 
 * @param import The import statement
 * 
 * @return Relative path to the file, allocated with malloc
 */
char* import_to_filename(dc_import* import) {
    char* extension;
    if (import->is_native) {
        extension = ".h";
    } else {
        extension = ".cst";
    }
    size_t extension_length = strlen(extension);

    /* firstly, resolve the relative filename from path segments */
    size_t total_length = 0;
    for (int i = 0; i < import->path.size; i++) {
        total_length += strlen(import->path.data[i]) + 1;
        /* +1 for the '/' path separator after each element */
        /* the trailing '/' is used for the null terminator */
    }
    /* account for the extension */
    total_length += extension_length;

    /* start merging the segments into an allocated buffer */
    char* filename = malloc(sizeof(char) * total_length);
    size_t offset = 0;
    for (int i = 0; i < import->path.size; i++) {
        /* append the path element*/
        size_t current_length = strlen(import->path.data[i]);
        strncpy(filename + offset, import->path.data[i], current_length);
        offset += current_length;

        /* append the path separator */
        filename[offset] = '/';
        offset += 1;
    }
    /* append the extension */
    strncpy(&filename[offset - 1], extension, extension_length);
    filename[offset - 1 + extension_length] = 0;

    /* return */
    return filename;
}


/**
 * Parses the given file and adds data from it (depending on the pass)
 * to the context's abstract syntax tree
 * 
 * @param context Pointer to the parser context
 * @param filename Path to the file
 */
void context_parse_native(se_context* context, char* filename) {
    logd("native parsing %s on pass %d", filename, context->pass + 1);

    /* create the pipes */
    int pd_in[2];
    int pd_out[2];
    if (pipe(pd_in) != 0 || pipe(pd_out) != 0) {
        logfe("import: unable to create a pipe");
    }
    
    /* start the gcc preprocessor */
    pid_t child = fork();
    if (child < 0) {
        logfe("import: unable to create a child process");
    }
    if (child == 0) {
        logd("forked successfully");
        close(pd_in[1]);
        dup2(pd_in[0], 0);

        close(pd_out[0]);
        dup2(pd_out[1], 1);
        
        if (execlp("gcc", "gcc", "-E", "-", NULL) != 0) {
            logfe("failed to start gcc preprocessor")
        }

        exit(0);
    }
    close(pd_in[0]);
    close(pd_out[1]);

    /* write to gcc input */
    write(pd_in[1], "#include <", strlen("#include <"));
    write(pd_in[1], filename, strlen(filename));
    write(pd_in[1], ">", strlen(">"));
    close(pd_in[1]);
    
    /* read the preprocessed output */
    FILE* input = fdopen(pd_out[0], "r");
    if (input == NULL) {
        error_internal("import: unable to open the preprocessor output");
    }

    /* initialize the scanner */
    yyscan_t scanner;
    if (cyylex_init(&scanner) != 0) {
        error_internal("import: unable to initizize the yacc scanner");
    }
    cyyset_in(input, scanner);

    /* parse */
    if (cyyparse(scanner, context) != 0) {
        error_internal("import: parsing file %s failed", filename);
    }

    /* free */
    cyylex_destroy(scanner);
    fclose(input);

    logd("successful");
}


/**
 * Parses the given file and adds data from it (depending on the pass)
 * to the context's abstract syntax tree
 * 
 * @param context Pointer to the parser context
 * @param filename Path to the file
 */
void context_parse(se_context* context, char* filename) {
    logd("parsing %s on pass %d", filename, context->pass + 1);

    /* open the file */
    FILE* input = fopen(filename, "r");
    if (input == NULL) {
        error_internal("import: unable to open file %s", filename);
    }

    /* initialize the scanner */
    yyscan_t scanner;
    if (myylex_init(&scanner) != 0) {
        error_internal("import: unable to initizize the yacc scanner");
    }
    myyset_in(input, scanner);

    /* parse */
    if (myyparse(scanner, context) != 0) {
        error_internal("import: parsing file %s failed", filename);
    }

    /* free */
    myylex_destroy(scanner);
    fclose(input);

    logd("successful");
}


/**
 * Parses the given file, treating it as the origin
 * This means that function bodies and other non-header data
 * from this file will be stored in the abstact syntax tree
 * 
 * @param context Pointer to the parser context
 * @param filename Path to the file
 */
void context_parse_origin(se_context* context, char* filename_) {
    if (context->pass != SCTX_PASS_3) {
        logfe("this function should only be called with newly created contexts");
    }

    /* ensure the path is absolute */
    char* filename = realpath(filename_, NULL);
    if (filename == NULL) {
        logfe("failed to determine the absolute path to %s", filename_);
    }
    logd("starting the parser at %s", filename);

    /* mark the file as imported to prevent self-imports */
    se_context_import_file* import_file = allocate(se_context_import_file);
    import_file->filename = filename;
    arl_add(se_context_import_file_ptr, context->file_list, import_file);

    /* do three passes on the file */
    context->pass = SCTX_PASS_1;
    import_file->last = context->pass;
    context_parse(context, filename);
    context->pass = SCTX_PASS_2;
    import_file->last = context->pass;
    context_parse(context, filename);
    context->pass = SCTX_PASS_3;
    import_file->last = context->pass;
    context_parse(context, filename);

    /* free */
    free(filename);

    logd("successful");
}


/**
 * Imports the given file, importing only the header data
 * such as type and function declarations
 * 
 * @param context Pointer to the parser context
 * @param import The import declaration
 */
void context_import(se_context* context, dc_import* import) {
    /* resolve the path */
    char* relative_name = import_to_filename(import);
    size_t relative_length = strlen(relative_name);

    char* filename;
    size_t filename_length;
    
    if (import->is_native) {
        filename = relative_name;
        filename_length = relative_length;
    } else {
        char* parent_name = arraylist_last(context->file_list)->filename;
        size_t parent_length = strlen(parent_name);

        /* a workaround to find the parent directory of the origin file */
        while (parent_length >= 0 && 
            parent_name[parent_length] != '/' &&
            parent_name[parent_length] != '\\'
        ) {
            parent_length--;
        }
        parent_length++;

        /* merge the file and directory paths */
        filename_length = parent_length + relative_length + 1;
        filename = allocate_array(char, filename_length);
        strncpy(filename, parent_name, parent_length);
        strncpy(filename + parent_length, relative_name, relative_length);
    }

    /* handle repeating (circular/self) imports */
    for (int i = 0; i < context->file_list.size; i++) {
        se_context_import_file* current_file = context->file_list.data[i];
        if (strncmp(filename, current_file->filename, filename_length) == 0) {
            if (current_file->last == context->pass) {
                logd("rejected repeat import of %s", filename);
                free(filename);
                return;
            }
        }
    }
    se_context_import_file* import_file = allocate(se_context_import_file);
    import_file->filename = filename;
    import_file->last = context->pass;
    arl_add(se_context_import_file_ptr, context->file_list, import_file);
    
    /* parse the file */
    
    if (import->is_native) {
        if (context->pass == SCTX_PASS_1) {
            ast_add_declaration(&context->ast, DC_IMPORT, import, false, NULL);
            context_parse_native(context, filename);
        }
    } else {
        if (context->pass != SCTX_PASS_3) {
            context_parse(context, filename);
        }
    }
}