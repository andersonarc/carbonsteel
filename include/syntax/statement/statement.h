/**
 * @file statement.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-30
 * 
 *  Lexical statement type definitions
 * 
 *  Statements include:
 *   - dc_st_variable, variable declaration statement
 *   - st_jump, break/continue/return statement
 *   - 
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_STATEMENT_H
#define CARBONSTEEL_SYNTAX_STATEMENT_H

    /* includes */
#include "syntax/expression/complex.h" /* complex expression */
#include "parser/context.h" /* parser context */
#include "ast/type.h" /* lexical type */

    /* typedefs */
/**
 * Variable declaration statement
 * in either global or local scope
 * 
 * Uninitialized variables are not allowed.
 */
struct dc_st_variable {
    ast_type type;
    char* name;
    expression_block value;
};


/**
 * Jump statement which changes the
 * code execution order by jumping to
 * the next loop iteration, out of the loop
 * or out of the current function
 */
enum st_jump_kind {
    ST_J_CONTINUE, ST_J_BREAK,
    ST_J_RETURN_VOID, ST_J_RETURN
};
struct st_jump {
    st_jump_kind kind;
    expression_block u_return_value;
};


/**
 * Conditional "if" statement with
 * optional "else" block
 */
struct st_if {
    bool has_else;
    expression_block condition;
    statement* body;
    statement* u_else;
};


/**
 * Simple iteration statement
 * which continues execution of the body
 * while the condition is true
 */
struct st_while {
    expression_block condition;
    statement* body;
};


/**
 * Generic statement of any kind
 * except variable declaration
 */
enum statement_kind {
    ST_JUMP, ST_IF,
    ST_EXPRESSION, ST_WHILE,
    ST_COMPOUND
};
struct statement {
    statement_kind kind;
    union {
        st_jump u_st_jump;
        st_if u_st_if;
        st_expression u_st_expression;
        st_while u_st_while;
        st_compound u_st_compound;
    };
};


/**
 * Compound statement block
 * in curly braces
 */
struct st_compound_item {
    enum {
        ST_C_ST_VARIABLE, ST_C_STATEMENT
    } kind;
    union {
        dc_st_variable* u_variable;
        statement* u_statement;
    };
};


#endif /* CARBONSTEEL_SYNTAX_STATEMENT_H */