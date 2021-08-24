/**
 * @file stack.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-26
 * 
 *  Lexical stack type definition
 *  and modification functions
 */
    /* header guard */
#ifndef CARBONSTEEL_STACK_H
#define CARBONSTEEL_STACK_H

    /* includes */
#include "statement.h" /* statements */
#include "expression.h" /* expressions */
#include "type.h" /* lexical types */
#include "error.h" /* error throw */

    /* defines */
/**
 * Throws an internal error if a node
 * has unexpected kind
 * 
 * @param[in] node The node
 * @param[in] node_kind Kind of the node
 */
#define stack_check_kind(node, node_kind) \
if (node.kind != node_kind) {             \
    error_internal("unexpected stack node kind: expected %s, got %s at " #node, stack_node_kind_to_string(node_kind), stack_node_kind_to_string(node.kind)); \
}

/**
 * Finds a start node of specified kind
 * for a stack walking routine
 * 
 * @param[in] kind       Start node kind, uppercase and without prefix
 */
#define stack_walk_start(kind)              \
int index = stack_find_index(STACK_##kind); \
stack_node* node = &stack.data[index];      \
index_t i = 0

/**
 * Initializes a list
 * for a stack walking routine
 * 
 * @param[in] type       Type of the list
 * @param[in] kind_lower Start node kind, lowercase and without prefix
 * @param[in] list       List property of the node
 */
#define stack_walk_list_init(type, kind_lower, list) \
assert_list_init(type, node->u_##kind_lower list, stack.size - index - 1)

/**
 * Prefix for a stack walking routine
 * 
 * @param[in] type       Type of the list
 * @param[in] kind       Start node kind, uppercase and without prefix
 * @param[in] kind_lower Start node kind, lowercase and without prefix
 * @param[in] list       List property of the node
 */
#define stack_walk_prefix(type, kind, kind_lower, list) \
stack_walk_start(kind);                                 \
stack_walk_list_init(type, kind_lower, list)

/**
 * The stack walking routine
 */
#define stack_walk for (index_t ex_i = index + 1; ex_i < stack.size; i++, ex_i++)

/**
 * Cleanup routine after walking the stack
 * 
 * @param[in] type Type of the initialized list
 * @param[in] list The initialized list
 */
#define stack_walk_end(type, list)                  \
assert_list_resize(type, list, i);                  \
while (stack.size > index + 1) { stack_pop(); }

/**
 * Alias for the current node 
 * in a stack walking routine
 */
#define stack_current stack.data[ex_i]

/**
 * Checks the kind of a current node
 * in a stack walking routine and
 * returns it or throws a internal error
 * 
 * @param[in] kind Expected kind of the current node
 */
#define stack_get_current(kind) stack_get(ex_i, kind)

/**
 * Checks if the stack has a next node,
 * and if it is true, increments the expression
 * counter and returns the next node, otherwise
 * returns null
 * 
 * @param[in] node_kind Expected kind of the next node
 */
#define stack_get_next_or_null(node_kind) ((ex_i + 1 == stack.size) ? NULL : ((stack.data[ex_i + 1].kind == node_kind) ? &stack.data[++ex_i] : NULL))

/**
 * Checks if the stack has a next node,
 * and if it is true, increments the expression
 * counter and returns the next node, otherwise
 * throws an internal error
 * 
 * @param[in] kind Expected kind of the next node
 */
#define stack_get_next(kind) stack_get(++ex_i, kind)

/**
 * Converts a stack node kind to a string
 * 
 * @param[in] kind Kind of the node
 * 
 * @return The string
 */
#define stack_node_kind_to_string(kind) stack_node_kind_strings[kind]

/**
 * Top of the stack
 * 
 * @note Don't forget to check whether the stack is empty before usage
 */
#define stack_top arraylist_last(stack)

/**
 * Creates a new stack node from the
 * specified value of specified type
 * 
 * @param[in] value           The node value
 * @param[in] node_kind_upper Kind of the node, upper-case and without prefix
 * @param[in] node_kind       Kind of the node, lower-case and without prefix
 */
#define stack_push_new_value(value, node_kind_upper, node_kind) \
stack_node node_new;                                            \
stack_push_new_value_predeclared(value, node_kind_upper, node_kind)

/**
 * Creates a new stack node from the
 * specified value of specified type
 * with a predeclared stack_node node_new
 * 
 * @param[in] value           The node value
 * @param[in] node_kind_upper Kind of the node, upper-case and without prefix
 * @param[in] node_kind       Kind of the node, lower-case and without prefix
 */
#define stack_push_new_value_predeclared(value, node_kind_upper, node_kind) \
node_new.kind = STACK_##node_kind_upper;           \
node_new.u_##node_kind = value;                    \
stack_push(node_new)

/**
 * Creates a new stack node from the
 * "value" local variable of specified type
 * 
 * @param[in] node_kind_upper Kind of the node, upper-case and without prefix
 * @param[in] node_kind       Kind of the node, lower-case and without prefix
 */
#define stack_push_new(node_kind_upper, node_kind) stack_push_new_value(value, node_kind_upper, node_kind)

/**
 * Pops a node of specified type from
 * stack and returns its value
 * 
 * @param[in] kind_upper Kind of the node, upper-case and without prefix
 * @param[in] kind       Kind of the node, lower-case and without prefix
 */
#define stack_pop_get(kind_upper, kind) stack_find_pop(STACK_##kind_upper).u_##kind

/**
 * Alias for stack_find_remove which
 * returns node value
 * 
 * @param[in] kind_upper Kind of the node, upper-case and without prefix
 * @param[in] kind       Kind of the node, lower-case and without prefix
 */
#define stack_find_remove_value(kind_upper, kind) stack_find_remove(STACK_##kind_upper).u_##kind

    /* typedefs */
typedef enum stack_node_kind {
    STACK_NAME, STACK_TYPE, 
    STACK_NUMBER, STACK_STRING,
    STACK_STRUCTURE, STACK_ENUM, STACK_ALIAS, STACK_FUNCTION,
    STACK_FUNCTION_REFERENCE, STACK_ST_VARIABLE_REFERENCE,
    STACK_EXPRESSION, STACK_EX_BASIC, STACK_EX_POSTFIX, 
    STACK_EX_UNARY, STACK_OP_UNARY, STACK_EX_CAST,
    STACK_EX_MULTIPLY, STACK_EX_ADDITION, STACK_EX_SHIFT,
    STACK_EX_COMPARE, STACK_EX_EQUAL, STACK_EX_BIT_AND, STACK_EX_BIT_XOR,
    STACK_EX_BIT_OR, STACK_EX_AND, STACK_EX_OR, STACK_OP_ASSIGN,
    STACK_EX_CONDITION, STACK_ST_VARIABLE, STACK_ST_EXPRESSION,
    STACK_ST_WHILE, STACK_ST_JUMP, STACK_ST_IF,
    STACK_STATEMENT, STACK_ST_COMPOUND, STACK_BOOLEAN, STACK_IMPORT,
    STACK_FUNCTION_PARAMETER_REFERENCE, STACK_EX_CONSTRUCTOR
} stack_node_kind;

typedef struct stack_node {
    stack_node_kind kind;
    union {
        char* u_name;
        dc_type u_type;
        ex_number u_number;
        char* u_string;
        dc_structure u_structure;
        dc_enum u_enum;
        dc_alias u_alias;
        dc_function u_function;
        expression u_expression;
        ex_basic u_ex_basic;
        ex_postfix u_ex_postfix;
        ex_unary u_ex_unary;
        op_unary u_op_unary;
        ex_cast u_ex_cast;
        ex_multiply u_ex_multiply;
        ex_addition u_ex_addition;
        ex_shift u_ex_shift;
        ex_compare u_ex_compare;
        ex_equal u_ex_equal;
        ex_bit_and u_ex_bit_and;
        ex_bit_xor u_ex_bit_xor;
        ex_bit_or u_ex_bit_or;
        ex_and u_ex_and;
        ex_or u_ex_or;
        op_assign u_op_assign;
        ex_condition u_ex_condition;
        st_variable u_st_variable;
        st_expression u_st_expression;
        st_while u_st_while;
        st_jump u_st_jump;
        st_if u_st_if;
        statement u_statement;
        st_compound u_st_compound;
        ex_boolean u_boolean;
        dc_import u_import;
        dc_function_parameter u_function_parameter;
        ex_constructor u_ex_constructor;
    };
} stack_node;
arraylist_declare(stack_node);

    /* global variables */
extern arraylist(stack_node) stack;

extern const char* stack_node_kind_strings[];

    /* functions */
/**
 * Initializes the lexical stack
 * and allocates memory for it
 */
void stack_init();

/**
 * Pushes a new stack node
 * 
 * @param[in] node The node
 */
void stack_push(stack_node node);

/**
 * Removes the last stack node
 */
void stack_pop();

/**
 * Checks the kind of a node
 * at a specified index and if it
 * matches, returns the node, otherwise
 * throws an internal error
 * 
 * @param index Index of the node
 * 
 * @return The node
 */
stack_node stack_get(index_t index, stack_node_kind kind);

/**
 * Finds the last stack node of specified kind 
 * and removes it, or throws an internal error
 * 
 * @param[in] kind Kind of the node
 * 
 * @return The found node
 */
stack_node stack_find_remove(stack_node_kind kind);

/**
 * Checks the kind of the last stack node,
 * pops and returns it, or throws an internal error
 * 
 * @param[in] kind Kind of the node
 * 
 * @return The last stack node
 */
stack_node stack_find_pop(stack_node_kind kind);

/**
 * Finds the last stack node of
 * specified kind, or returns NULL
 * 
 * @param[in] kind Kind of the node
 * 
 * @return Pointer to the found node or NULL
 */
stack_node* stack_find_or_null(stack_node_kind kind);

/**
 * Finds the last stack node of
 * specified kind, or throws an internal error
 * 
 * @param[in] kind Kind of the node
 * 
 * @return Pointer to the found node
 */
stack_node* stack_find(stack_node_kind kind);

/**
 * Finds the last index of a stack node of
 * specified kind, or returns -1
 * 
 * @param[in] kind Kind of the node
 * 
 * @return Index of the found node or -1
 */
int stack_find_index_or_null(stack_node_kind kind);

/**
 * Finds the last index of a stack node of
 * specified kind, or throws an internal error
 * 
 * @param[in] kind Kind of the node
 * 
 * @return Index of the found node
 */
int stack_find_index(stack_node_kind kind);

    /* type-specific functions */
/**
 * Pushes a copy of a new name (identifier)
 * into the lexical stack
 * 
 * @param[in] name The identifier
 */
void stack_push_name(const char* name);

/**
 * Pushes a copy of a string literal
 * into the lexical stack
 * 
 * @param[in] token The string literal token
 */
void stack_push_string(char* token);

/**
 * Pushes a copy of a new numerical literal
 * into the lexical stack
 * 
 * @param[in] number The number token
 * @param[in] kind Kind of the number token
 */
void stack_push_number(char* number, ex_number_kind kind);

/**
 * Pushes a new boolean literal
 * into the lexical stack
 * 
 * @param[in] boolean Boolean token value
 */
void stack_push_boolean(char* boolean);

#endif /* CARBONSTEEL_STACK_H */