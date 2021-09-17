/**
 * @file context.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-08
 * 
 *  Parser context function implementations
 */
    /* includes */
#include "parser/context.h" /* this */

#include "syntax/declaration/declaration.h" /* declarations */
#include "misc/memory.h" /* memory allocation */

    /* global variables */
/**
 * Parser context level kind string values
 */
const char* se_context_level_kind_strings[] = {
    "GLOBAL", "IMPORT", "FUNCTION", "EXPRESSION", "ENUM"
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

    /* add a global level */
    se_context_level global = { .kind = SCTX_GLOBAL };
    arl_add(se_context_level, context->stack, global);

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
            arl_init_empty(ex_constructor_ptr, level.u_expression_block.constructors);
            break;

        case SCTX_FUNCTION:
            arl_init(ast_local_declaration, level.u_locals);
            break;

        case SCTX_ENUM:
            level.u_enum_context.value = allocate(dc_enum);
            level.u_enum_context.member_index = 0;
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