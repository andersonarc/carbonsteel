/**
 * @file stack.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-26
 * 
 *  Stack function implementations
 *  and primitive stack types
 */
    /* includes */
#include "stack.h" /* this */

#include "lexer.h" /* lexer */
#include "copy.h" /* duplication */

    /* generic implementations */
arraylist_define(stack_node);

    /* global variables */
arraylist(stack_node) stack;

const char* stack_node_kind_strings[] = {
    "NAME", "TYPE",
    "NUMBER", "STRING",
    "STRUCTURE", "ENUM", "ALIAS", "FUNCTION",
    "FUNCTION_REFERENCE", "ST_VARIABLE_REFERENCE",
    "EXPRESSION", "EX_BASIC", "EX_POSTFIX", 
    "EX_UNARY", "OP_UNARY", "EX_CAST",
    "EX_MULTIPLY", "EX_ADDITION", "EX_SHIFT",
    "EX_COMPARE", "EX_EQUAL", "EX_BIT_AND", "EX_BIT_XOR",
    "EX_BIT_OR", "EX_AND", "EX_OR", "OP_ASSIGN", "EX_CONDITION",
    "ST_VARIABLE", "ST_EXPRESSION",
    "ST_WHILE", "ST_JUMP", "ST_IF",
    "STATEMENT", "ST_COMPOUND", "BOOLEAN", "IMPORT",
    "FUNCTION_PARAMETER", "EX_CONSTRUCTOR"
};

    /* functions */
/**
 * Initializes the lexical stack
 * and allocates memory for it
 */
void stack_init() {
    assert_arraylist_init(stack_node, stack);
    assert_arraylist_init(ex_constructor_ptr, expression_stack);
}

/**
 * Pushes a new stack node
 * 
 * @param[in] node The node
 */
void stack_push(stack_node node) {
    assert_arraylist_add(stack_node, stack, node);
}

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
stack_node stack_get(index_t index, stack_node_kind kind) {
    if (index >= stack.size) {
        error_internal("invalid get index: %zu for stack of size %zu", index, stack.size);
    }
    stack_check_kind(stack.data[index], kind);
    return stack.data[index];
}

/**
 * Finds the last stack node of specified kind 
 * and removes it, or throws an internal error
 * 
 * @param[in] kind Kind of the node
 * 
 * @return The found node
 */
stack_node stack_find_remove(stack_node_kind kind) {
    int index = stack_find_index(kind);
    stack_node node = stack.data[index];
    assert_arraylist_remove(stack_node, stack, index);
    return node;
}

/**
 * Removes the last stack node
 */
void stack_pop() {
    assert_arraylist_not_empty(stack);
    assert_arraylist_pop(stack_node, stack);
}

/**
 * Checks the kind of the last stack node,
 * pops and returns it, or throws an internal error
 * 
 * @param[in] kind Kind of the node
 * 
 * @return The last stack node
 */
stack_node stack_find_pop(stack_node_kind kind) {
    assert_arraylist_not_empty(stack);
    stack_check_kind(stack_top, kind);
    stack_node node = stack_top;
    assert_arraylist_pop(stack_node, stack);
    return node;
}

/**
 * Finds the last stack node of
 * specified kind, or returns NULL
 * 
 * @param[in] kind Kind of the node
 * 
 * @return Pointer to the found node or NULL
 */
stack_node* stack_find_or_null(stack_node_kind kind) {
    if (arraylist_is_empty(stack)) {
        return NULL;
    }
    for (int i = stack.size - 1; i >= 0; i--) {
        if (stack.data[i].kind == kind) {
            return &stack.data[i];
        }
    }
    return NULL;
}

/**
 * Finds the last stack node of
 * specified kind, or throws an internal error
 * 
 * @param[in] kind Kind of the node
 * 
 * @return Pointer to the found node
 */
stack_node* stack_find(stack_node_kind kind) {
    stack_node* node = stack_find_or_null(kind);
    if (node == NULL) {
        error_internal("unable to find a node of kind %s in the (stack) " arraylist_to_string(stack_node, stack), stack_node_kind_to_string(kind), arraylist_to_string_format(stack));
    }
    return node;
}

/**
 * Finds the last index of a stack node of
 * specified kind, or returns -1
 * 
 * @param[in] kind Kind of the node
 * 
 * @return Index of the found node or -1
 */
int stack_find_index_or_null(stack_node_kind kind) {
    if (arraylist_is_empty(stack)) {
        return -1;
    }
    for (int i = stack.size - 1; i >= 0; i--) {
        if (stack.data[i].kind == kind) {
            return i;
        }
    }
    return -1;
}

/**
 * Finds the last index of a stack node of
 * specified kind, or throws an internal error
 * 
 * @param[in] kind Kind of the node
 * 
 * @return Index of the found node
 */
int stack_find_index(stack_node_kind kind) {
    int index = stack_find_index_or_null(kind);
    if (index < 0) {
        error_internal("unable to find a node of kind %s in the (stack) " arraylist_to_string(stack_node, stack), stack_node_kind_to_string(kind), arraylist_to_string_format(stack));
    }
    return index;
}

    /* type-specific functions */
/**
 * Pushes a copy of a new name (identifier)
 * into the lexical stack
 * 
 * @param[in] name The identifier
 */
void stack_push_name(const char* name) {
    stack_node node;
    node.kind = STACK_NAME;
    node.u_name = copy_string(name);
    stack_push(node);
}

/**
 * Pushes a copy of a string literal
 * into the lexical stack
 * 
 * @param[in] token The string literal token
 */
void stack_push_string(char* token) {
    stack_node node;
    node.kind = STACK_STRING;
    node.u_string = copy_string(token);
    stack_push(node);
}

/**
 * Pushes a copy of a new numerical literal
 * into the lexical stack
 * 
 * @param[in] number The number token
 * @param[in] kind Kind of the number token
 */
void stack_push_number(char* number, ex_number_kind kind) {
    stack_node node;
    node.kind = STACK_NUMBER;
    node.u_number.kind = kind;
    node.u_number.value = copy_string(number);
    stack_push(node);
}


/**
 * Pushes a new boolean literal
 * into the lexical stack
 * 
 * @param[in] boolean Boolean token value
 */
void stack_push_boolean(char* boolean) {
    stack_node node;
    node.kind = STACK_BOOLEAN;
    node.u_boolean.value = copy_string(boolean);
    stack_push(node);
}