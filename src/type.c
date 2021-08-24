/**
 * @file type.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-26
 * 
 *  Stack manipulation function for
 *  structures, enums, aliases, imports,
 *  types and other lexical types
 */
    /* includes */
#include "type.h" /* this */
#include "ctool/iteration.h" /* iteration */

#include "ast.h" /* syntax tree */
#include "stack.h" /* stack */
#include "lexer.h" /* lexer */
#include "copy.h" /* duplication */
#include "convert.h" /* conversion utilities */
    
/**
 * @todo
 * No duplicates ---
 * Global lookup for Structure, Enum, Variable, Alias declarations
 * Local (member) lookup for Structure, Enum
 * Local (function) lookup for Variable
 * (arraylist_add_unique)
 */

    /* generic implementations */
list_define(dc_structure_member);
arraylist_define(string);
list_define(dc_enum_member);
arraylist_define(dc_type_level);
arraylist_define(dc_type);
list_define(dc_function_parameter);
list_define(dc_type_primitive);
arraylist_define(declaration);
list_define(dc_import_level);

        /* stack functions */

    /** STRUCTURE START */

void start_structure() {
    /* create new stack node */
    stack_node node;
    node.kind = STACK_STRUCTURE;

    /* initialize name */
    node.u_structure.name = stack_find_pop(STACK_NAME).u_name;
    logd("structure '%s'", node.u_structure.name);

    /* push new node */
    stack_push(node);
}

//todo stack_pop_get(UPPER, lower)

    /** STRUCTURE END **/

void end_structure() {
    stack_walk_prefix(dc_structure_member, STRUCTURE, structure, .member_list);

    /* initialize members */
    stack_walk {
        dc_structure_member value;
        value.type = stack_get_current(STACK_TYPE).u_type;
        value.name = stack_get_next   (STACK_NAME).u_name;
        logd("member '%s'", value.name);

        node->u_structure.member_list.data[i] = value;
    }
    stack_walk_end(dc_structure_member, node->u_structure.member_list);
}


    /** TYPE LEVEL ADD **/

void add_type_level(dc_type_level_kind kind) {
    /* find the type node */
    dc_type* type = &stack_find(STACK_TYPE)->u_type;

    /* initialize the level */
    dc_type_level level;
    level.kind = kind;

    /* add new level */
    assert_arraylist_add(dc_type_level, type->level_list, level);
}


    /* ENUM START **/

void start_enum() {
    /* create new stack node */
    stack_node node;
    node.kind = STACK_ENUM;

    /* initialize name */
    node.u_enum.name = stack_find_pop(STACK_NAME).u_name;
    logd("enum '%s'", node.u_enum.name);

    /* push new node */
    stack_push(node);
}

    /** ENUM END **/

void end_enum() {
    stack_walk_prefix(dc_enum_member, ENUM, enum, .member_list);

    /* initialize members */
    stack_walk {
        /* create an enum member with a name */
        dc_enum_member value;
        value.name = stack_get_current(STACK_NAME).u_name;
        logd("member '%s'", value.name);

        /* get optional member value */
        stack_node* next = stack_get_next_or_null(STACK_NAME);
        if (next != NULL) {
            value.value = copy_structure(stack_top.u_number);
        } else {
            ex_number number;
            number.kind = EX_N_INT;
            number.value = size2string(i);
            value.value = copy_structure(number);
        }
        
        /* add the member */
        node->u_enum.member_list.data[i] = value;
    }
    stack_walk_end(dc_enum_member, node->u_enum.member_list);
}


    /** FUNCTION START **/

void start_function() {
    /* create new stack node */
    stack_node node;
    node.kind = STACK_FUNCTION;

    /* initialize name and return type */
    node.u_function.name = stack_find_pop(STACK_NAME).u_name;
    logd("function '%s'", node.u_function.name);

    node.u_function.return_type = stack_find_pop(STACK_TYPE).u_type;

    /* init lists and push new node */
    node.u_function.parameter_list.data = NULL;
    assert_arraylist_init(st_variable, node.u_function._variable_stack);
    stack_push(node);
}


    /** FUNCTION PARAMETERS ADD **/

void add_function_parameters() {
    /* initialize the function parameters */
    stack_walk_prefix(dc_function_parameter, FUNCTION, function, .parameter_list);
    stack_walk {
        dc_function_parameter value;
        value.type = stack_get_current(STACK_TYPE).u_type;
        value.name = stack_get_next   (STACK_NAME).u_name;
        logd("parameter '%s'", value.name);

        node->u_function.parameter_list.data[i] = value;
    }
    stack_walk_end(dc_function_parameter, node->u_function.parameter_list);
}


    /** FUNCTION END **/

void end_function() {
    /* find the function */
    dc_function* function = &stack_find(STACK_FUNCTION)->u_function;

    /* initialize the function body */
    function->body = stack_find_pop(STACK_ST_COMPOUND).u_st_compound;
}


    /** IMPORT START **/

void start_import() {
    /* create and push new node */
    stack_node node;
    node.kind = STACK_IMPORT;
    stack_push(node);
    logd("start import");
}


    /** IMPORT END **/

void end_import() {
    /* add import nodes */
    stack_walk_prefix(dc_import_level, IMPORT, import, );
    stack_walk {
        node->u_import.data[i] = stack_current.u_name;
        logd("import %s", stack_current.u_name);
    }
    stack_walk_end(dc_import_level, node->u_import);

    /* check for empty import */
    if (node->u_import.size == 0) {
        error_syntax("empty import statement");
    }
}


    /** ALIAS ADD **/

void add_alias() {
    /* create new alias */
    dc_alias value;
    dc_type type = stack_find_pop(STACK_TYPE).u_type;
    value.target = copy_structure(type);
    value.name = stack_find_pop(STACK_NAME).u_name;
    logd("alias %s", value.name);
}


    /** DECLARATION ADD **/

void add_declaration(declaration_kind kind) {
    /* create a new declaration */
    declaration dc;
    dc.kind = kind;

    /* predeclarations */
    st_variable st;

    /* initialize it */
    switch (kind) {
        case DC_IMPORT:
            dc.u_import = stack_pop_get(IMPORT, import);
            dc.name = NULL;
            break;

        case DC_STRUCTURE:
            dc.u_structure = stack_pop_get(STRUCTURE, structure);
            dc.name = dc.u_structure.name;
            break;

        case DC_ALIAS:
            dc.u_alias = stack_pop_get(ALIAS, alias);
            dc.name = dc.u_alias.name;
            break;

        case DC_ENUM:
            dc.u_enum = stack_pop_get(ENUM, enum);
            dc.name = dc.u_enum.name;
            break;

        case DC_FUNCTION:
            dc.u_function = stack_pop_get(FUNCTION, function);
            dc.name = dc.u_function.name;
            break;
        
        case DC_ST_VARIABLE:
            st = stack_pop_get(ST_VARIABLE, st_variable);
            dc.u_st_variable = copy_structure(st);
            dc.name = dc.u_st_variable->name;
            break;

        otherwise_error
    }

    /* add to the ast */
    assert_arraylist_add(declaration, ast.declaration_list, dc);
}