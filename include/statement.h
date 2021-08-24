/**
 * @file statement.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-07-30
 * 
 *  Lexical statement types
 */
    /* header guard */
#ifndef CARBONSTEEL_STATEMENT_H
#define CARBONSTEEL_STATEMENT_H

    /* includes */
#include <stdbool.h> /* bool */
#include "expression.h" /* expressions */
#include "type.h" /* lexical types */

        /* typedefs */

    /** PROPERTY **/

struct st_variable {
    dc_type type;
    char* name;
    expression* value;
};


    /** JUMP **/

enum st_jump_kind {
    ST_J_CONTINUE, ST_J_BREAK,
    ST_J_RETURN_VOID, ST_J_RETURN
};

struct st_jump {
    st_jump_kind kind;
    expression* u_return_value;
};


    /** CONDITION **/

struct st_if {
    bool has_else;
    expression* condition;
    statement* body;
    statement* u_else;
};

    /** WHILE **/

struct st_while {
    expression* condition;
    statement* body;
};


    /** STATEMENT **/

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
        st_compound* u_st_compound;
    };
};

    /** COMPOUND **/

struct st_compound_item {
    enum {
        ST_C_PROPERTY, ST_C_STATEMENT
    } kind;
    union {
        st_variable u_st_variable;
        statement u_statement;
    };
};

    /* functions */

/**
 * @pop EXPRESSION
 * @pop NAME
 * @pop TYPE
 * @init st_variable (.name, .type, .value) full
 * @if stack has FUNCTION
 *  @init dc_function (._variable_stack)
 *  @push ST_VARIABLE
 * @else
 *  @declare st_variable
 */
void add_st_variable();

/**
 * @if kind == ST_J_RETURN
 *  @pop EXPRESSION
 *  @init st_jump (.u_return_value)
 * 
 * @init st_jump (.kind) full
 * @push ST_JUMP
 */
void add_st_jump(st_jump_kind kind);

/**
 * @pop STATEMENT
 * @init st_if (.has_else, .condition, .body) full
 * @push ST_IF
 */
void add_st_if();

/**
 * @pop STATEMENT
 * @init st_if (.has_else, .u_else) full
 */
void add_st_if_else();

/**
 * @pop EXPRESSION
 * @init st_expression full
 * @push ST_EXPRESSION
 */
void add_st_expression();

/**
 * @pop STATEMENT
 * @pop EXPRESSION
 * @init st_while (.name, .type, .value) full
 * @push ST_WHILE
 */
void add_st_while();

/**
 * @pop ST_JUMP | ST_IF | ST_EXPRESSION | ST_COMPOUND | ST_WHILE
 * @init statement (.kind, .u_*) full
 * @push STATEMENT
 */
void add_statement(statement_kind kind);

/**
 * @init st_compound
 * @push ST_COMPOUND
 */
void start_st_compound();

/**
 * @walk from ST_COMPOUND
 *  @get ST_VARIABLE | STATEMENT
 * @init st_compound full
 */
void end_st_compound();

#endif /* CARBONSTEEL_STATEMENT_H */