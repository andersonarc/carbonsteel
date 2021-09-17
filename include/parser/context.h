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
struct expression_block {
    arraylist(ex_constructor_ptr) constructors;
    expression* value;
};

/**
 * Enum context structure
 * that tracks the element index and enum value
 */
struct enum_context {
    dc_enum* value;
    int member_index;
};

/**
 * Parser context level kind
 */
typedef enum se_context_level_kind {
    SCTX_GLOBAL,
    SCTX_IMPORT, SCTX_FUNCTION, SCTX_EXPRESSION,
    SCTX_ENUM
} se_context_level_kind;


/**
 * Parser context level structure
 */
typedef struct se_context_level {
    se_context_level_kind kind;
    union {
        arraylist(ast_local_declaration) u_locals; /* in SCTX_FUNCTION   */
        expression_block u_expression_block;       /* in SCTX_EXPRESSION */
        enum_context u_enum_context;               /* in SCTX_ENUM       */
    };
} se_context_level;
arraylist_declare(se_context_level);


/**
 * Parser context structure
 */
typedef struct se_context {
    arraylist(se_context_level) stack;
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


#endif /* CARBONSTEEL_PARSER_CONTEXT_H */