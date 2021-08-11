/**
 * @file expression.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-27
 * 
 *  Expression stack and code
 *  generation function implementations
 */
    /* includes */
#include "expression.h" /* this */

#include "stack.h" /* lexical stack */
#include "copy.h" /* duplication */

    /* global variables */
const char* op_assign_strings[] = {
    "=", "*=", "/=", "%=",
    "+=", "-=", "<<=", ">>=",
    "&=", "|=", "^="
};

const char* op_unary_strings[] = {
    "&", "*", "~", "!"
};

    /* defines */
/**
 * Defines a binary expression 
 * implementation of specified type
 *
 * @note The definition should be placed in a source file
 * 
 * @param[in] name Name of the expression (without prefix)
 * @param[in] name_upper Upper-cased name of the expression (without prefix)
 * @param[in] op Operand name (without prefix)
 * @param[in] op_upper Upper-cased operand name (without prefix)
 * @param[in] __VA_ARGS__ Operator string values
 */
#define biex_define(name, name_upper, op, op_upper, ...)    \
                                                            \
arraylist_define(binode(name));                             \
                                                            \
const char* biex_op_strings(name)[] = {                     \
    __VA_ARGS__                                             \
};                                                          \
                                                            \
                                                            \
    /** START BIEX **/                                      \
                                                            \
void biex_start(name)() {                                   \
    /* create a new binary expression */                    \
    biex(name) value;                                       \
                                                            \
    /* initialize first operand */                          \
    value.has_operation = false;                            \
    value.value = stack_pop_get(EX_##op_upper, ex_##op);    \
                                                            \
    /* push new node */                                     \
    stack_push_new(EX_##name_upper, ex_##name);             \
}                                                           \
                                                            \
                                                            \
    /** ADD OP BIEX **/                                     \
                                                            \
void biex_add_op(name)(bioperator(name) op_kind) {          \
    /* find the start of the expression */                  \
    biex(name)* value = &stack_find(STACK_EX_##name_upper)->u_ex_##name;  \
                                                                 \
    /* initialize operation list if needed */                    \
    if (!value->has_operation) {                                 \
        assert_arraylist_init(binode(name), value->u_operation); \
        value->has_operation = true;                             \
    }                                                            \
    logd("operation " macro_stringify(name) " (%s)", biex_op_strings(name)[op_kind]);  \
                                                                \
    /* add an operator */                                       \
    binode(name) operator;                                      \
    operator.kind = BNK_OPERATOR;                               \
    operator.u_operator = op_kind;                              \
    assert_arraylist_add(binode(name), value->u_operation, operator); \
                                                                \
    /* add an operand */                                        \
    binode(name) operand;                                       \
    operand.kind = BNK_OPERAND;                                 \
    operand.u_operand = stack_pop_get(EX_##op_upper, ex_##op);  \
    assert_arraylist_add(binode(name), value->u_operation, operand); \
}                                                               \
                                                                \
                                                                \
    /** END BIEX **/                                            \
                                                                \
void biex_end(name)() {                                         \
    /* check last node kind */                                  \
    stack_check_kind(stack_top, STACK_EX_##name_upper);         \
                                                                \
    /* trim operation list if required */                       \
    if (stack_top.u_ex_##name.has_operation) {                  \
        assert_arraylist_trim(binode(name), stack_top.u_ex_##name.u_operation); \
    }                                                           \
}                                                               \
                                                                \
                                                                \
    /** CODEGEN BIEX **/                                        \
                                                                \
cg_ex(name) {                                                   \
    /* generate code for the first operand */                   \
    cg(ex_##op)(&ex->value);                                    \
    if (ex->has_operation) {                                    \
                                                                \
        /* generate operation code */                           \
        iterate_array(i, ex->u_operation.size) {                \
            switch (ex->u_operation.data[i].kind) {             \
                case BNK_OPERAND:                               \
                    /* generate operand */                      \
                    cg(ex_##op)(&ex->u_operation.data[i].u_operand); \
                    break;                                      \
                                                                \
                case BNK_OPERATOR:                              \
                    /* generate operator */                     \
                    out(char)(' ');                             \
                    out(string)(biex_op_strings(name)[ex->u_operation.data[i].u_operator]); \
                    out(char)(' ');                                     \
                    break;                                              \
                                                                        \
                otherwise_error                                         \
            }                                                           \
        }                                                               \
    }                                                                   \
}                                                                       \
                                                                        \
/**                                                                     \
 * Binary expression constructor check                                  \
 */                                                                     \
arraylist(ex_cast_ptr) biex_check_constructor(name)(biex(name)* ex) {       \
    arraylist(ex_cast_ptr) base = biex_check_constructor(op)(&ex->value);   \
    if (ex->has_operation) {                                            \
        iterate_array(i, ex->u_operation.size) {                        \
            if (ex->u_operation.data[i].kind == BNK_OPERAND) {          \
                arraylist(ex_cast_ptr) new = biex_check_constructor(op)(&ex->u_operation.data[i].u_operand); \
                iterate_array(j, new.size) {                            \
                    assert_arraylist_add(ex_cast_ptr, base, new.data[j])    \
                }                                                       \
            }                                                           \
        }                                                               \
    }                                                                   \
    return base;                                                        \
}

    /* generic implementations*/
arraylist_define(ex_postfix_level);
arraylist_define(ex_cast_ptr);
arraylist_define(op_unary);
list_define(expression);

/**
 * @todo
 * Expression type checking (X has Property Y? Invocation? Indexation?)
 */

        /* stack functions */

    /** BASIC ADD **/

void add_ex_basic(ex_basic_kind kind) {
    ex_basic value;
    value.kind = kind;
    
    /* declarations */
    st_variable variable;
    ast_function function;
    ast_function_parameter function_parameter;
    expression ex;

    /* check kind */
    switch (kind) {
        case EX_B_NUMBER:
            value.u_number = stack_find_pop(STACK_NUMBER).u_number;
            logd("ex_basic number: %s", value.u_number.value);
            break;

        case EX_B_FUNCTION:
            function = stack_find_pop(STACK_FUNCTION_REFERENCE).u_function;
            value.u_function = copy_structure(function);
            logd("ex_basic function: %s", function.name);
            break;

        case EX_B_VARIABLE:
            variable = stack_find_pop(STACK_ST_VARIABLE_REFERENCE).u_st_variable;
            value.u_variable = copy_structure(variable);
            logd("ex_basic variable: %s", variable.name);
            break;

        case EX_B_EXPRESSION:
            ex = stack_find_pop(STACK_EXPRESSION).u_expression;
            value.u_expression = copy_structure(ex);
            logd("ex_basic expression");
            break;

        case EX_B_BOOLEAN:
            value.u_boolean = stack_find_pop(STACK_BOOLEAN).u_boolean;
            logd("ex_basic boolean");
            break;

        case EX_B_STRING:
            value.u_string = stack_find_pop(STACK_STRING).u_string;
            logd("ex_basic string");
            break;

        case EX_B_FUNCTION_PARAMETER:
            function_parameter = stack_pop_get(FUNCTION_PARAMETER_REFERENCE, function_parameter);
            value.u_function_parameter = copy_structure(function_parameter);
            logd("ex_basic function parameter");
            break;

        otherwise_error
    }

    /* push basic expression node */
    stack_push_new(EX_BASIC, ex_basic);
}


    /** POSTFIX START **/

void start_ex_postfix() {
    /* create a new stack node */
    stack_node node;
    node.kind = STACK_EX_POSTFIX;

    /* find a basic expression node */
    node.u_ex_postfix.value = stack_find_pop(STACK_EX_BASIC).u_ex_basic;

    /* init lists and push new node */
    assert_arraylist_init(ex_postfix_level, node.u_ex_postfix.level_list);
    stack_push(node);
}


    /** TYPE POSTFIX START **/

void start_ex_type_postfix() {
    /* create a new postfix expression */
    ex_postfix value;
    assert_arraylist_init(ex_postfix_level, value.level_list);

    /* create a new postfix level */
    ex_postfix_level level;
    level.kind = EX_PL_INVOCATION;

    /* initialize the invocation level with stack walk */
    index_t i = 0;
    int index = stack_find_index(STACK_TYPE);
    assert_list_init(expression, level.u_arguments, stack.size - index - 1);
    stack_walk {
        level.u_arguments.data[i] = stack_get_current(STACK_EXPRESSION).u_expression;
    }
    stack_walk_end(expression, level.u_arguments);

    /* initialize the basic expression */
    ast_type type = stack_find_pop(STACK_TYPE).u_type;
    value.value.u_type = copy_structure(type);
    value.value.kind = EX_B_TYPE;
    
    /* add the level and push a new node */
    assert_arraylist_add(ex_postfix_level, value.level_list, level);
    stack_push_new(EX_POSTFIX, ex_postfix);
}


    /** POSTFIX ADD LEVEL **/

void add_ex_postfix_level(ex_postfix_level_kind kind) {
    stack_walk_start(EX_POSTFIX);
    ex_postfix_level level;
    level.kind = kind;

    /* check level kind */
    switch (kind) {
        case EX_PL_INDEX:
            /* [index] */
            stack_check_kind(stack_top, STACK_EXPRESSION);

            /* initialize level and remove from stack */
            level.u_index = copy_structure(stack_top.u_expression);
            stack_pop();
            logd("ex_postfix [index]")
            break;

        case EX_PL_PROPERTY:
            /* .property */
            stack_check_kind(stack_top, STACK_NAME);
            
            /* initialize level and remove from stack */
            level.u_property = stack_top.u_name;
            logd("ex_postfix .property: %s", level.u_property)
            stack_pop();
            break;
        
        case EX_PL_INVOCATION:
            /* invocation() */
            logd("ex_postfix invocation()");

            /* initialize level and remove all arguments from stack */
            assert_list_init(expression, level.u_arguments, stack.size - index - 1);
            stack_walk {
                level.u_arguments.data[i] = stack_get_current(STACK_EXPRESSION).u_expression;
            }
            stack_walk_end(expression, level.u_arguments);
            break;
        
        default:
            error_internal("unexpected postfix level kind: expected EX_PL_INDEX, EX_PL_PROPERTY or\
                EX_PL_INVOCATION, got %d", kind);
    }

    /* add the level to the postfix expression */
    assert_arraylist_add(ex_postfix_level, node->u_ex_postfix.level_list, level);
}


    /** POSTFIX END **/

void end_ex_postfix(ex_postfix_kind kind) {
    /* check if stack top is a postfix expression */
    stack_check_kind(stack_top, STACK_EX_POSTFIX);

    /* set the postfix expression kind */
    stack_top.u_ex_postfix.kind = kind;
    if (kind != EX_P_PLAIN) {
        if (kind == EX_P_INCREMENT) {
            logd("postfix increment");
        } else if (kind == EX_P_DECREMENT) {
            logd("postfix decrement");
        }
    }
}


    /** UNARY START **/

void start_ex_unary() {
    /* create a new unary expression node */
    stack_node node;
    node.kind = STACK_EX_UNARY;

    /* init with postfix expression */
    node.u_ex_unary.value = stack_find_pop(STACK_EX_POSTFIX).u_ex_postfix;

    /* pop all levels */
    assert_arraylist_init(op_unary, node.u_ex_unary.op_list);
    while (stack_top.kind == STACK_OP_UNARY) {
        assert_arraylist_add(op_unary, node.u_ex_unary.op_list, stack_top.u_op_unary);
        stack_pop();
    }
    arraylist_revert(op_unary)(&node.u_ex_unary.op_list);

    /* push new node */
    stack_push(node);
}


    /** UNARY OPERATOR ADD **/

void add_op_unary(op_unary level) {
    /* create a new level stack node */
    stack_node node;
    node.kind = STACK_OP_UNARY;
    node.u_op_unary = level;

    /* push the level node */
    stack_push(node);
    logd("ex_unary level %d", level);
}


    /** UNARY END **/

void end_ex_unary(ex_unary_kind kind) {
    /* check if stack top is an unary expression */
    stack_check_kind(stack_top, STACK_EX_UNARY);

    /* set the unary expression kind */
    stack_top.u_ex_unary.kind = kind;
    if (kind != EX_U_PLAIN) {
        logd("unary %s", op_unary_strings[kind]);
    }
}


    /** CAST START **/

void start_ex_cast() {
    /* create a new stack node */
    stack_node node;
    node.kind = STACK_EX_CAST;

    /* find an unary expression node */
    node.u_ex_cast.value = stack_find_pop(STACK_EX_UNARY).u_ex_unary;

    /* init lists and push new node */
    assert_arraylist_init(ast_type, node.u_ex_cast.cast_list);
    stack_push(node);
}


    /** CAST LEVEL ADD **/

void add_ex_cast_level() {
    /* find the last cast expression */
    stack_node* node = stack_find(STACK_EX_CAST);

    /* add a new cast level */
    assert_arraylist_add(ast_type, node->u_ex_cast.cast_list, stack_find_pop(STACK_TYPE).u_type);
    logd("casted");
}


    /** CAST END **/

void end_ex_cast() {
    /* check stack top kind */
    stack_check_kind(stack_top, STACK_EX_CAST);

    /* trim lists */
    assert_arraylist_trim(ast_type, stack_top.u_ex_cast.cast_list);
}

    /** MULTIPLICATION **/
biex_define(multiply, MULTIPLY, cast, CAST, "*", "/", "%");

    /** ADDITION **/
biex_define(addition, ADDITION, multiply, MULTIPLY, "+", "-");

    /* SHIFT **/
biex_define(shift, SHIFT, addition, ADDITION, "<<", ">>");

    /** COMPARISON **/
biex_define(compare, COMPARE, shift, SHIFT, ">", ">=", "<", "<=");

    /** EQUATION **/
biex_define(equal, EQUAL, compare, COMPARE, "==", "!=");

    /** BITWISE AND **/
biex_define(bit_and, BIT_AND, equal, EQUAL, "&");

    /** BITWISE XOR **/
biex_define(bit_xor, BIT_XOR, bit_and, BIT_AND, "^");

    /** BITWISE OR **/
biex_define(bit_or, BIT_OR, bit_xor, BIT_XOR, "|");

    /** LOGICAL AND **/
biex_define(and, AND, bit_or, BIT_OR, "&&");

    /** LOGICAL OR **/
biex_define(or, OR, and, AND, "||");


    /** ADD ASSIGNMENT OPERATOR **/

void add_op_assign(op_assign value) {
    stack_node node;
    node.kind = STACK_OP_ASSIGN;
    node.u_op_assign = value;
    stack_push(node);
}


    /** ADD CONDITION **/

void add_ex_condition(bool has_condition) {
    ex_condition value;
    value.has_condition = has_condition;

    if (has_condition) {
        logd("conditional expression");
        ex_condition u_else = stack_find_pop(STACK_EX_CONDITION).u_ex_condition;
        value.u_else = copy_structure(u_else);
        expression u_if = stack_find_pop(STACK_EXPRESSION).u_expression;
        value.u_if = copy_structure(u_if);
    }
    
    value.a = stack_find_pop(STACK_EX_OR).u_ex_or;

    stack_node node;
    node.kind = STACK_EX_CONDITION;
    node.u_ex_condition = value;
    stack_push(node);
}


    /** ADD EXPRESSION **/

void add_expression(bool has_assignment) {
    expression value;
    value.has_assignment = has_assignment;

    if (has_assignment) {
        logd("assignment expression");
        expression b = stack_find_pop(STACK_EXPRESSION).u_expression;
        value.u_assignment.b = copy_structure(b);
        value.u_assignment.op = stack_find_pop(STACK_OP_ASSIGN).u_op_assign;
        value.u_assignment.a = stack_find_pop(STACK_EX_UNARY).u_ex_unary;
    } else {
        value.u_plain = stack_find_pop(STACK_EX_CONDITION).u_ex_condition;
    }

    stack_node node;
    node.kind = STACK_EXPRESSION;
    node.u_expression = value;
    stack_push(node);
}