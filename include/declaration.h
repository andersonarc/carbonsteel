/**
 * @file declaration.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-08-09
 * 
 *  Lexical type declarations moved into
 *  a different file to avoid conflicts
 */
    /* header guard */
#ifndef CARBONSTEEL_DECLARATION_H
#define CARBONSTEEL_DECLARATION_H

    /* includes */
#include "list.h" /* list utilities */

    /* defines */
/**
 * Structure declaration macro
 * 
 * @param[in] name Name of the structure
 */
#define d_struct(name)   typedef struct name name
#define da_struct(name)  d_struct(name); arraylist_declare(name)
#define dl_struct(name)  d_struct(name); list_declare(name)
#define dla_struct(name) d_struct(name); list_declare(name); arraylist_declare(name);

/**
 * Enum declaration macro
 * 
 * @param[in] name Name of the enum
 */
#define d_enum(name)  typedef enum name name
#define da_enum(name) d_enum(name); arraylist_declare(name)
#define dl_enum(name) d_enum(name); list_declare(name)

/**
 * Binary expression declaration macro
 * 
 * @param[in] name Binary expression name, without prefix
 */
#define d_biex(name) da_struct(name##_binode_t); d_enum(op_##name); da_struct(ex_##name)

        /* type declarations */

    /* lexical types */
dl_struct(ast_primitive);
da_struct(ast_alias);

da_struct(ast_enum);
dl_struct(ast_enum_member);

da_struct(ast_type);
da_enum(ast_type_level);

da_struct(ast_structure);
dl_struct(ast_structure_member);

da_struct(ast_function);
dl_struct(ast_function_parameter);

typedef char* ast_import_level;
list_declare(ast_import_level);
typedef list(ast_import_level) ast_import;
arraylist_declare(ast_import);

    /* expressions */
d_struct(ex_boolean);

d_enum(ex_basic_kind);
d_struct(ex_basic);

d_struct(ex_number);
d_enum(ex_number_kind);

d_struct(ex_postfix);
d_enum(ex_postfix_value_kind);
d_enum(ex_postfix_kind);
da_struct(ex_postfix_level);
da_enum(ex_postfix_level_kind);

d_struct(ex_unary);
d_enum(ex_unary_kind);
da_enum(op_unary);

d_struct(ex_cast);
typedef ex_cast* ex_cast_ptr;
arraylist_declare(ex_cast_ptr);

d_biex(multiply);
d_biex(addition);
d_biex(shift);
d_biex(compare);
d_biex(equal);
d_biex(bit_and);
d_biex(bit_xor);
d_biex(bit_or);
d_biex(and);
d_biex(or);

d_struct(ex_condition);

dl_struct(expression);
d_enum(op_assign);

    /* statements */
typedef expression* st_expression;

dl_struct(st_compound_item);
typedef list(st_compound_item) st_compound;

d_struct(st_jump);
d_enum(st_jump_kind);

d_enum(statement_kind);
da_struct(statement);

da_struct(st_variable);
d_struct(st_if);
d_struct(st_while);

#endif /* CARBONSTEEL_DECLARATION_H */