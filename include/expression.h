/**
 * @file expression.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-07-26
 * 
 *  Expression type definitions and
 *  function declarations
 */
    /* header guard */
#ifndef CARBONSTEEL_EXPRESSION_H
#define CARBONSTEEL_EXPRESSION_H

    /* includes */
#include <stdbool.h> /* boolean */

#include "declaration.h" /* declarations */
#include "codegen.h" /* code generation */
#include "list.h" /* list utilities */

    /* defines */
/**
 * Generates a generic name for
 * an binary expression of specified type
 * 
 * @param[in] name Name of the expression (without prefix)
 * @param[in] name_upper Upper-cased name of the expression (without prefix)
 */
#define binode(name) name##_binode_t
#define bioperator(name) op_##name
#define biex(name) ex_##name
#define biex_check_constructor(name) check_constructor_ex_##name
#define biex_start(name) start_ex_##name
#define biex_add_op(name) add_ex_##name##_op
#define biex_end(name) end_ex_##name
#define biex_cg(name) codegen_ex_##name
#define biex_op_strings(name) biex_op_##name##_strings

/**
 * Declares a binary expression 
 * of specified type
 *
 * @note The declaration should be placed in a header file
 * 
 * @param[in] name Name of the expression (without prefix)
 * @param[in] op   Operand name (without prefix)
 * @param[in] __VA_ARGS__ Operator enum entries 
 */
#define biex_declare(name, op, ...)     \
                                        \
/**                                     \
 * Binary operator string values        \
 */                                     \
extern const char* biex_op_strings(name)[]; \
                                        \
/**                                     \
 * Binary operator enum                 \
 */                                     \
enum bioperator(name) {                 \
    __VA_ARGS__                         \
};                                      \
                                        \
/**                                     \
 * Binary expression node, either       \
 * an operand or an operator            \
 */                                     \
struct binode(name) {                   \
    binode_kind kind;                   \
    union {                             \
        biex(op) u_operand;             \
        bioperator(name) u_operator;    \
    };                                  \
};                                      \
                                        \
/**                                     \
 * The binary expression                \
 */                                     \
struct biex(name) {                     \
    bool has_operation;                 \
    biex(op) value;                     \
    name##_binode_t_arraylist_t u_operation; \
};                                      \
                                        \
/**                                     \
 * @pop OPERAND                         \
 * @init biex (:first_operand)          \
 * @push BIEX                           \
 */                                     \
void biex_start(name)();                \
                                        \
/**                                     \
 * @pop OPERAND                         \
 * @find BIEX                           \
 * @init biex (:operator, :operand)     \
 */                                     \
void biex_add_op(name)(bioperator(name) operator); \
                                        \
/**                                     \
 * @peek BIEX                           \
 * @trim biex                           \
 */                                     \
void biex_end(name)();                  \
                                        \
/**                                     \
 * Binary expression code generation    \
 */                                     \
cg_ex(name);

    /* global variables */
/**
 * Assignment operator string values
 */
extern const char* op_assign_strings[];

/**
 * Unary operator string values
 */
extern const char* op_unary_strings[];

/**
 * Expression stack for constructors
 */
extern arraylist(ex_constructor_ptr) expression_stack;

    /* declarations */
/**
 * Cast expression code generation
 */
cg_ex(cast);

/**
 * Binary expression node kind
 */
typedef enum binode_kind {
    BNK_OPERATOR, BNK_OPERAND
} binode_kind;

        /* typedefs */

    /** BOOLEAN **/

struct ex_boolean {
    char* value;
};


    /** NUMBER **/

enum ex_number_kind {
    EX_N_INT, EX_N_FLOAT
};

struct ex_number {
    ex_number_kind kind;
    char* value;
};


    /** CONSTRUCTOR **/

struct ex_constructor {
    bool is_new;
    bool is_array;
    char* u_variable_name;
    expression* u_array_size;
    dc_type* type;
    list(expression) arguments;
};


    /** BASIC **/

enum ex_basic_kind {
    EX_B_VARIABLE, EX_B_FUNCTION,
    EX_B_NUMBER, EX_B_EXPRESSION,
    EX_B_BOOLEAN, EX_B_STRING,
    EX_B_CONSTRUCTOR, EX_B_FUNCTION_PARAMETER
};
    
struct ex_basic {
    ex_basic_kind kind;
    union {
        st_variable* u_variable;
        dc_function* u_function;
        ex_number u_number;
        expression* u_expression;
        ex_boolean u_boolean;
        char* u_string;
        ex_constructor* u_constructor;
        dc_function_parameter* u_function_parameter;
    };
};


    /** POSTFIX **/

enum ex_postfix_level_kind {
    EX_PL_INDEX, EX_PL_INVOCATION,
    EX_PL_PROPERTY
};

struct ex_postfix_level {
    ex_postfix_level_kind kind;
    union {
        expression* u_index;
        list(expression) u_arguments;
        char* u_property;
    };
};

enum ex_postfix_kind {
    EX_P_PLAIN, EX_P_INCREMENT,
    EX_P_DECREMENT
};

struct ex_postfix {
    ex_postfix_kind kind;
    arraylist(ex_postfix_level) level_list;
    ex_basic value;
};


    /** UNARY **/

enum op_unary {
    OP_UN_REFERENCE, OP_UN_DEREFERENCE,
    OP_UN_BITWISE_NOT, OP_UN_LOGIC_NOT
};

enum ex_unary_kind {
    EX_U_PLAIN, EX_U_INCREMENT, EX_U_DECREMENT,
    EX_U_PLUS, EX_U_MINUS, EX_U_NEW
};

struct ex_unary {
    ex_unary_kind kind;
    arraylist(op_unary) op_list;
    ex_postfix value;
};


        /** CAST **/

struct ex_cast {
    ex_unary value;
    arraylist(dc_type) cast_list;
};


    /** MULTIPLICATION **/
biex_declare(multiply, cast, OP_M_MULTIPLY, OP_M_DIVIDE, OP_M_MODULO)

    /** ADDITION **/
biex_declare(addition, multiply, OP_A_PLUS, OP_A_MINUS)

    /* SHIFT **/
biex_declare(shift, addition, OP_S_LEFT, OP_S_RIGHT)

    /** COMPARISON **/
biex_declare(compare, shift, OP_C_GT, OP_C_GE, OP_C_LT, OP_C_LE)

    /** EQUATION **/
biex_declare(equal, compare, OP_E_EQ, OP_E_NEQ)

    /** BITWISE AND **/
biex_declare(bit_and, equal, OP_BIT_AND);

    /** BITWISE XOR **/
biex_declare(bit_xor, bit_and, OP_BIT_XOR);

    /** BITWISE OR **/
biex_declare(bit_or, bit_xor, OP_BIT_OR);

    /** LOGICAL AND **/
biex_declare(and, bit_or, OP_AND);

    /** LOGICAL OR **/
biex_declare(or, and, OP_OR);


    /** CONDITIONAL **/

struct ex_condition {
    bool has_condition;
    ex_or a;
    expression* u_if;
    struct ex_condition* u_else;
};


    /** ASSIGNMENT **/

enum op_assign {
    OP_AS_PLAIN, OP_AS_MUL, OP_AS_DIV, OP_AS_MOD,
    OP_AS_ADD, OP_AS_SUB, OP_AS_SHL, OP_AS_SHR,
    OP_AS_BIT_AND, OP_AS_BIT_OR, OP_AS_BIT_XOR
};

struct expression {
    bool has_assignment;
    arraylist(ex_constructor_ptr) constructors;
    union {
        ex_condition u_plain;
        struct {
            ex_unary a;
            op_assign op;
            struct expression* b;
        } u_assignment;
    };
};

    /* stack functions */
/**
 * @pop NUMBER | FUNCTION_REFERENCE | VARIABLE_REFERENCE | EXPRESSION
 * @init ex_basic (.kind, .u_*) full
 * @push EX_BASIC
 */
void add_ex_basic(ex_basic_kind kind);

/**
 * @walk from TYPE || TYPE + 1 if is_array
 *  @get EXPRESSION
 * @if is_array
 *  @pop EXPRESSION
 * @pop TYPE
 * @init ex_constructor (.is_new = false, .is_array, .arguments, .u_array_size, .type)
 * @push EX_CONSTRUCTOR
 */
void add_ex_constructor(bool is_array);

/**
 * @peek EX_CONSTRUCTOR
 * @init ex_constructor (.is_new = true)
 */
void add_ex_constructor_new();

/**
 * @pop EX_BASIC
 * @init ex_postfix (.level_list, .value)
 * @push EX_POSTFIX
 */
void start_ex_postfix();

/**
 * @pop list(EXPRESSION)
 * @pop TYPE
 * @init ex_postfix_level (.kind, .u_arguments) full
 * @init ex_postfix (.level_list)
 * @push EX_POSTFIX
 */
void start_ex_type_postfix();

/**
 * @find EX_POSTFIX
 * @pop EXPRESSION | NAME | list(EXPRESSION)
 * @init ex_postfix_level (.kind, .u_*) full
 * @init ex_postfix (.level_list)
 */
void add_ex_postfix_level(ex_postfix_level_kind kind);

/**
 * @peek EX_POSTFIX
 * @init ex_postfix (.kind) full
 */
void end_ex_postfix(ex_postfix_kind kind);

/**
 * @loop
 *  @pop OP_UNARY
 * @revert
 * @pop EX_POSTFIX
 * @init ex_unary (.level_list, .value)
 * @push EX_UNARY
 */
void start_ex_unary();

/**
 * @push OP_UNARY
 */
void add_op_unary(op_unary level);

/**
 * @peek EX_UNARY
 * @init ex_unary (.kind) full
 */
void end_ex_unary(ex_unary_kind kind);

/**
 * @pop EX_UNARY
 * @init ex_cast (.value, .cast_list)
 * @push EX_CAST
 */
void start_ex_cast();

/**
 * @find EX_CAST
 * @init ex_cast (.cast_list)
 */
void add_ex_cast_level();

/**
 * @trim ex_cast (.cast_list)
 */
void end_ex_cast();

/**
 * @push EX_OP_ASSIGN
 */
void add_op_assign(op_assign value);

/**
 * @init ex_condition (.has_condition)
 * @if has_condition
 *  @pop EX_CONDITION
 *  @pop EXPRESSION
 *  @init ex_condition (.u_else, .u_if)
 * @pop EX_OR
 * @init ex_condition (.a) full
 * @push EX_CONDITION
 */
void add_ex_condition(bool has_condition);

/**
 * @init expression (.has_assignment)
 * @if has_assignment
 *  @pop EX_CONDITION
 *  @pop OP_ASSIGN
 *  @pop EX_UNARY
 *  @init (.b, .op, .a) full
 * @else
 *  @pop EX_CONDITION
 *  @init (.a) full
 */
void add_expression(bool has_assignment);

/**
 * @peek EXPRESSION
 * @init expression (.constructors)
 */
void add_expression_block();

#endif /* CARBONSTEEL_EXPRESSION_H */