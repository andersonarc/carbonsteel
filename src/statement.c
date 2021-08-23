/**
 * @file statement.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-08-05
 * 
 *  Statement stack function
 *  implementations
 */
    /* includes */
#include "statement.h" /* this */

#include "stack.h" /* lexical stack */
#include "ast.h" /* syntax tree */
#include "copy.h" /* copying */

    /* generic implementations */
list_define(st_compound_item);
arraylist_define(st_variable);
arraylist_define(statement);

        /* functions */

    /** PROPERTY ADD **/

void add_st_variable() {
    /* create new statement */
    st_variable value;

    /* init value, name and type */
    expression ex = stack_find_pop(STACK_EXPRESSION).u_expression;
    value.value = copy_structure(ex);
    value.name = stack_find_pop(STACK_NAME).u_name;
    value.type = stack_find_pop(STACK_TYPE).u_type;
    logd("property '%s'", value.name);
    
    /* check context */
    stack_node* function = stack_find_or_null(STACK_FUNCTION);
    if (function != NULL) {
        assert_arraylist_add(st_variable, function->u_function._variable_stack, value);
        stack_push_new(ST_VARIABLE, st_variable);
    } else {
        assert_arraylist_add(st_variable, ast.st_variable_list, value);
    }
}


    /** JUMP ADD **/

void add_st_jump(st_jump_kind kind) {
    /* create new node */
    stack_node node;
    node.kind = STACK_ST_JUMP;

    /* init kind and optional value */
    node.u_st_jump.kind = kind;
    if (kind == ST_J_RETURN) {
        expression ex = stack_find_pop(STACK_EXPRESSION).u_expression;
        node.u_st_jump.u_return_value = copy_structure(ex);
        logd("return value; statement");
    } else {
        logd("continue/break/return; statement")
    }

    /* push the node */
    stack_push(node);
}


    /** IF ADD **/

void add_st_if() {
    /* create new statement */
    stack_node node;
    node.kind = STACK_ST_IF;

    /* init body */
    node.u_st_if.has_else = false;
    statement st = stack_find_pop(STACK_STATEMENT).u_statement;
    node.u_st_if.body = copy_structure(st);
    expression ex = stack_find_pop(STACK_EXPRESSION).u_expression;;
    node.u_st_if.condition = copy_structure(ex);
    logd("if statement")

    /* push the statement */
    stack_push(node);
}


    /** ELSE ADD **/

void add_st_if_else() {
    /* pop the else statement */
    statement st = stack_find_pop(STACK_STATEMENT).u_statement;

    /* initialize previous if statement */
    stack_check_kind(stack_top, STACK_ST_IF);
    stack_top.u_st_if.has_else = true;
    stack_top.u_st_if.u_else = copy_structure(st);
    logd("else statement");
}


    /** EXPRESSION ADD **/

void add_st_expression() {
    /* change the stack top kind from EXRPESSION to ST_EXPRESSION */
    stack_check_kind(stack_top, STACK_EXPRESSION);
    stack_top.kind = STACK_ST_EXPRESSION;
    logd("expression statement");

    /* wrap the expression */
    expression value = stack_top.u_expression;
    stack_top.u_st_expression = copy_structure(value);
}


    /** WHILE ADD **/

void add_st_while() {
    /* create new statement */
    stack_node node;
    node.kind = STACK_ST_WHILE;

    /* init body and condition */
    statement st = stack_find_pop(STACK_STATEMENT).u_statement;
    node.u_st_while.body = copy_structure(st);
    expression ex = stack_find_pop(STACK_EXPRESSION).u_expression;
    node.u_st_while.condition = copy_structure(ex);
    logd("while statement");

    /* push new node */
    stack_push(node);
}


    /** STATEMENT ADD **/

void add_statement(statement_kind kind) {
    /* create new node */
    stack_node node; 
    //todo node constructor
    node.kind = STACK_STATEMENT;
    node.u_statement.kind = kind;

    /* declarations */
    st_compound st;

    /* check kind */
    switch (kind) {
        case ST_JUMP:
            node.u_statement.u_st_jump = stack_find_pop(STACK_ST_JUMP).u_st_jump;
            break;
        
        case ST_IF:
            node.u_statement.u_st_if = stack_find_pop(STACK_ST_IF).u_st_if;
            break;

        case ST_EXPRESSION:
            node.u_statement.u_st_expression = stack_find_pop(STACK_ST_EXPRESSION).u_st_expression;
            break;

        case ST_COMPOUND:
            st = stack_find_pop(STACK_ST_COMPOUND).u_st_compound;
            node.u_statement.u_st_compound = copy_structure(st);
            break;

        case ST_WHILE:
            node.u_statement.u_st_while = stack_find_pop(STACK_ST_WHILE).u_st_while;
            break;

        otherwise_error
    }

    /* push new node */
    stack_push(node);
}


    /** COMPOUND START **/

void start_st_compound() {
    /* create new node */
    stack_node node;
    node.kind = STACK_ST_COMPOUND;
    logd("{ start")

    /* push new node */
    stack_push(node);
}

void end_st_compound() {
    stack_walk_prefix(st_compound_item, ST_COMPOUND, st_compound, );

    /* add statement children */
    stack_walk {
        st_compound_item value;

        switch (stack_current.kind) {
            case STACK_ST_VARIABLE:
                value.kind = ST_C_PROPERTY;
                value.u_st_variable = stack_current.u_st_variable;
                break;

            case STACK_STATEMENT:
                value.kind = ST_C_STATEMENT;
                value.u_statement = stack_current.u_statement;
                break;

            default:
                error_internal("unexpected stack node kind: expected STACK_ST_VARIABLE || STACK_STATEMENT"
                " got %s at stack_top", stack_node_kind_to_string(stack_current.kind));
                break;
        }

        node->u_st_compound.data[i] = value;
    }
    stack_walk_end(st_compound_item, node->u_st_compound);

    /* revert and trim lists */
    logd("} end")
}