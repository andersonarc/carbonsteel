/**
 * @file context.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-01
 * 
 *  Parser context type definition
 * 
 *  Parser context tracks stores the current scope
 *  as a stack list and grants access to local declarations 
 *  (such as function parameters and local variables) 
 *  by storing them in a list.
 */
    /* header guard */
#ifndef CARBONSTEEL_PARSER_CONTEXT_H
#define CARBONSTEEL_PARSER_CONTEXT_H

    /* includes */
#include "ast/declaration.h" /* ast declaration type */
#include "ast/root.h" /* ast root type */

#include "ctool/type/bitset.h" /* bitset type */

    /* global variables */
/**
 * Parser context level kind string values
 */
extern const char* se_context_level_kind_strings[];


    /* typedefs */
/**
 * Expression block structure
 * that tracks expression-specific content
 */
struct ex_block {
    arraylist(ex_constructor_ptr) constructors;
    expression* value;
};

/**
 * Enum context structure
 * that tracks the element index and enum value
 * 
 * If an enum has implicit values defined,
 * than it cannot have explicit values and vice versa,
 * because it may lead to value conflicts.
 */
typedef enum enum_context_kind {
    ENUM_KIND_UNKNOWN,
    ENUM_KIND_IMPLICIT,
    ENUM_KIND_EXPLICIT
} enum_context_kind;

struct enum_context {
    dc_enum* value;
    size_t member_index;
    enum_context_kind kind;
};

/**
 * Context level which is a bitset structure
 * with the following flags:
 * 
 * [] is the default value
 * 
 *  0) [signed]/unsigned (only for integers) 
 * 
 * @param[in] this The flag context level
 * 
 * @note What is important about the flag context is the fact that
 *        it is not mandatory to be set and is exited from by the called function
 *        (not the caller function as you would expect with other kinds of contexts)
 */
bitset_declare(1);
typedef uint8_t_bitset_t flag_context;
#define flag_context_init(this)         bitset_init(this)
#define flag_context_set_signed(this)   bitset_clear(this, 0)
#define flag_context_set_unsigned(this) bitset_set(this, 0)
#define flag_context_if_signed(this)    bitset_if_not_set(this, 0)
#define flag_context_if_unsigned(this)  bitset_if_set(this, 0)

/**
 * Set the context to skip mode
 * if it is currently on pass n
 */
#define context_skip(context, n)                    \
    if (context->pass == SCTX_PASS_##n) {           \
        context->expect_skip_from = true;           \
    }


/**
 * Parser context level kind
 */
typedef enum se_context_level_kind {
    SCTX_GLOBAL,
    SCTX_IMPORT, SCTX_FUNCTION, SCTX_EXPRESSION,
    SCTX_ENUM, SCTX_FLAG
} se_context_level_kind;

/**
 * Parser context level structure
 */
typedef struct se_context_level {
    se_context_level_kind kind;
    union {
        arraylist(ast_local_declaration) u_locals; /* in SCTX_FUNCTION   */
        ex_block u_ex_block;                       /* in SCTX_EXPRESSION */
        enum_context u_enum_context;               /* in SCTX_ENUM       */
        flag_context u_flag_context;               /* in SCTX_FLAG       */
    };
} se_context_level;
arraylist_declare(se_context_level);

/**
 * Import pass of the parser
 * 
 * Pass 1: Only add declared symbols (type names, function names, variable names, etc.)
 * Pass 2: Add type bodies and function declarations
 * Pass 3: Only done for the origin file, adds all data including function bodies
 */
typedef enum se_context_pass {
    SCTX_PASS_1,
    SCTX_PASS_2,
    SCTX_PASS_3
} se_context_pass;

/**
 * Imported file structure
 */
typedef struct se_context_import_file {
    char* filename;
    se_context_pass last; /* last pass done on the file */
} se_context_import_file;

/**
 * Parser context structure
 */
typedef struct se_context {
    arraylist(se_context_level) stack;
    ast_root ast;
    se_context_pass pass;
    arraylist(se_context_import_file_ptr) file_list; /* the list of imported files */

    /* assistant fields for skipping tokens */
    int skip_pair_count;
    char skip_until;
    bool expect_skip_from;
    /* todo refactor it? doesn't look very well */
} se_context;


    /* functions */
/**
 * Allocates and initializes a new parser context
 * 
 * @return Pointer to the new parser context
 */
se_context* context_new();

/**
 * Add a new context level
 * on top of the context stack
 * 
 * @param context Pointer to the parser context
 * @param kind    Kind of the context level
 */
void context_enter(se_context* context, se_context_level_kind kind);

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
se_context_level* context_get(se_context* context, se_context_level_kind kind);

/**
 * Returns the current context level
 * 
 * @param[in] context Pointer to the parser context
 */
static inline se_context_level* context_current(se_context* context) {
    return &arraylist_last(context->stack);
}

/**
 * Checks if the current context
 * is of specified kind
 * 
 * @param[in] context Pointer to the parser context
 * @param[in] kind    The expected kind
 */
static inline bool context_is(se_context* context, se_context_level_kind kind)  {
    return context_current(context)->kind == kind;
}

/**
 * Finds a last occurence of a specified
 * level kind in the context stack and
 * returns it or NULL
 * 
 * @param[in] context Pointer to the parser context
 * @param[in] kind    The kind to search for
 */
se_context_level* context_find(se_context* context, se_context_level_kind kind);

/**
 * Removes the last context level and
 * releases its contents if needed
 * 
 * @param context Pointer to the parser context
 */
void context_exit(se_context* context);

/**
 * Parses the given file and adds data from it (depending on the pass)
 * to the context's abstract syntax tree
 * 
 * @param context Pointer to the parser context
 * @param filename Path to the file
 */
void context_parse(se_context* context, char* filename);

/**
 * Parses the given file, treating it as the origin
 * This means that function bodies and other non-header data
 * from this file will be stored in the abstact syntax tree
 * 
 * @param context Pointer to the parser context
 * @param filename Path to the file
 */
void context_parse_origin(se_context* context, char* filename);

/**
 * Imports the given file, importing only the header data
 * such as type and function declarations
 * 
 * @param context Pointer to the parser context
 * @param import The import declaration
 */
void context_import(se_context* context, dc_import* import);

#endif /* CARBONSTEEL_PARSER_CONTEXT_H */