/**
 * @file codegen.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-08-06
 * 
 *  Code generation functions
 */
    /* includes */
#include "codegen/codegen.h" /* this */

#include <string.h> /* string functions */

#include "ast/root.h" /* abstract syntax tree */
#include "syntax/statement/statement.h" /* statements */
#include "syntax/declaration/declaration.h" /* declarations */
#include "misc/memory.h" /* memory allocation */

    /* defines */
/**
 * Code generation task (for structures, functions, etc.)
 * declaration and invocation macro
 */
#define cgtask_declare_ast(name) void macro_concatenate(codegen_task_, name)(FILE* file, ast_root* ast)
#define cgtask_declare(name) void macro_concatenate(codegen_task_, name)(FILE* file)
#define cgtask_ast(name) macro_concatenate(codegen_task_, name)(file, ast)
#define cgtask(name) macro_concatenate(codegen_task_, name)(file)

    /* forward declarations */
cgd_type();
cgd_statement();
cgd_st(compound);
cgd_ex(condition_data);
cgd_ex(expression_data);
cgd_ex(block);
cgd_ex(constant);

    /* internal functions */
/**
 * Generates a new temporary variable name
 * 
 * @param[in] tmp Pointer to the temporary variable index
 * 
 * @return The new name, heap-allocated
 */
char* get_tmp_name_from(int* tmp) {
    int name_size = snprintf(NULL, 0, "_cst_tmpvar_%d_", *tmp) + 1;
    char* name = allocate_array(char, name_size);
    snprintf(name, name_size, "_cst_tmpvar_%d_", *tmp);
    (*tmp)++;
    return name;
}
#define get_tmp_name() get_tmp_name_from(tmp)

        /* code generation functions */

    /** CONSTRUCTOR EXPRESSION **/

cgd_ex(constructor) {
    out(tabs)();
    ex->u_variable_name = get_tmp_name();
    
    cg(type)(ex->type);
    if (ex->is_new) {
        out(char)('*');
    }
    out(char)(' ');
    out(string)(ex->u_variable_name);

    if (ex->is_array) {
        if (ex->is_new) {
            out(string)(" = malloc(sizeof(");
            cg(type)(ex->type);
            out(string)(") * ");
            cg(ex_expression_data)(ex->u_array_size);
            out(string)(");\n");

            iterate_array(i, ex->argument_list.size) {
                out(tabs)();
                out(format)("%s[%zu] = ", ex->u_variable_name, i);
                cg(ex_expression_data)(&ex->argument_list.data[i]->data);
                out(string)(";\n");
            }
        } else {
            out(char)('[');
            cg(ex_expression_data)(ex->u_array_size);

            out(string)("] = { ");

            iterate_array(i, ex->argument_list.size) {
                cg(ex_expression_data)(&ex->argument_list.data[i]->data);
                if (i != ex->argument_list.size - 1) {
                    out(char)(',');
                }
                out(char)(' ');
            }

            out(string)("};\n");
        }
    } else {
        switch (ex->type->kind) {
            case AST_TYPE_STRUCTURE:
                if (ex->is_new) {
                    out(string)(" = malloc(sizeof(");
                    cg(type)(ex->type);
                    out(string)("));\n");

                    iterate_array(i, ex->type->u_structure->member_list.size) {
                        out(tabs)();
                        out(format)("%s->%s = ", ex->u_variable_name, ex->type->u_structure->member_list.data[i].name);
                        cg(ex_expression_data)(&ex->argument_list.data[i]->data);
                        out(string)(";\n");
                    }
                } else {
                    out(string)(" = { ");
                    iterate_array(i, ex->argument_list.size) {
                        cg(ex_expression_data)(&ex->argument_list.data[i]->data);
                        if (i != ex->argument_list.size - 1) {
                            out(char)(',');
                        }
                        out(char)(' ');
                    }

                    out(string)("};\n");
                }
            break;

            case AST_TYPE_PRIMITIVE:
                if (ex->is_new) {
                    out(string)(" = malloc(sizeof(");
                    cg(type)(ex->type);
                    out(string)("));\n");

                    out(tabs)();
                    out(format)("*%s = ", ex->u_variable_name);
                } else {
                    out(string)(" = ");
                }

                cg(ex_expression_data)(&ex->argument_list.data[0]->data);
                out(string)(";\n");
            break;

            otherwise_error
        }
    }
}


    /** STRUCTURE **/

cgd_dc(structure_member) {
    out(tabs)();

    cg(type)(&dc->type);
    out(format)(" %s;\n", dc->name);
}

cgd_dc(structure) {
    out(format)("typedef struct %s {\n", dc->name);
    
    tabs++;
    iterate_array(i, dc->member_list.size) {
        cg(structure_member)(&dc->member_list.data[i]);
    }
    out(format)("} %s;\n\n", dc->name);
}

    /** TYPE **/

cgd_type() {
    switch (type->kind) {
        case AST_TYPE_PRIMITIVE:
            out(string)(type->u_primitive->code_name);
            break;

        case AST_TYPE_STRUCTURE:
            if (type->u_structure->name != NULL) {
                out(string)(type->u_structure->name);
            }
            break;

        case AST_TYPE_ENUM:
            if (type->u_enum->name != NULL) {
                out(string)(type->u_enum->name);
            }
            break;

        case AST_TYPE_FUNCTION:
            error_internal("function types are not supported yet");
            break;

        otherwise_error
    }
    iterate_array(i, type->level_list.size) {
        switch (type->level_list.data[i].kind) {
            case AT_LEVEL_POINTER:
            case AT_LEVEL_ARRAY:
                out(char)('*');
                break;

            otherwise_error
        }
    }
}


    /** NUMBER EXPRESSION **/

cgd_ex(number_data) {
    switch (ex->kind) {
        case EX_N_DOUBLE:
            out(format)("%g", ex->u_double);
            break;

        case EX_N_LONG:
            out(format)("%lld", ex->u_long);
            break;

        otherwise_error
    }
}

    /** ENUM MEMBER **/

cgd_dc(enum_member) {
    out(string)("_cst_enum__");
    out(string)(dc->parent->name);
    out(string)("__member__");
    out(string)(dc->name);
}


    /** ENUM **/

cgd_dc(enum) {
    out(format)("typedef enum %s {\n", dc->name);
    
    tabs++;
    if (!arraylist_is_empty(dc->member_list)) {
        index_t last = dc->member_list.size - 1;
        dc_enum_member* member;

        iterate_array(i, last) {
            out(tabs)();
            member = &dc->member_list.data[i];
            cg(enum_member)(member);
            out(string)(" = ");
            cg(ex_constant)(&member->value);
            out(string)(",\n");
        }

        out(tabs)();
        member = &dc->member_list.data[last];
        cg(enum_member)(member);
        out(string)(" = ");
        cg(ex_constant)(&member->value);
        out(char)('\n');
    }
    out(format)("} %s;\n\n", dc->name);
}


    /** EXPRESSION BLOCK **/

cgd(ex_block_constructors, ex_block* ex) {
    iterate_array(i, ex->constructors.size) {
        cg(ex_constructor)(ex->constructors.data[i]);
    }
}
cgd_ex(block) {
    //todo store data, move properties into ex_block!
    cg(ex_expression_data)(&ex->value->data);
}


    /** CONSTANT EXPRESSION */
cgd_ex(constant) {
    switch (ex->kind) {
        case EX_C_BOOLEAN:
            out(string)(ex->u_boolean ? "true" : "false");
            break;

        case EX_C_BYTE:
        //todo use conversion functions
            out(format)("%d", ex->u_byte);
            break;

        case EX_C_SHORT:
            out(format)("%d", ex->u_short);
            break;

        case EX_C_INT:
            out(format)("%d", ex->u_int);
            break;

        case EX_C_LONG:
            out(format)("%zu", ex->u_long);
            break;
        
        case EX_C_FLOAT:
            out(format)("%g", ex->u_float);
            break;

        case EX_C_DOUBLE:
            out(format)("%g", ex->u_double);
            break;

        case EX_C_ARRAY:
            logfe("code generation for array constant expressions is not supported");
            break;

        case EX_C_STRUCTURE:
            logfe("code generation for structure constant expressions is not supported");
            break;

        case EX_C_DYNAMIC:
            logfe("code generation for dynamic constant expressions is not supported");
            break;

        otherwise_error
    }
}


    /** BASIC EXPRESSION **/

cgd_ex(basic_data) {
    switch (ex->kind) {
        case EX_B_FUNCTION:
            out(string)(ex->u_function->name);
            break;

        case EX_B_VARIABLE:
            out(string)(ex->u_variable->name);
            break;

        case EX_B_NUMBER:
            cg(ex_number_data)(&ex->u_number);
            break;

        case EX_B_BOOLEAN:
            out(string)(ex->u_boolean == true ? "true" : "false");
            break;

        case EX_B_CHARACTER:
            out(char)('\'');
            out(char)(ex->u_character);
            out(char)('\'');
            break;

        case EX_B_EX_CONSTRUCTOR:
            out(string)(ex->u_ex_constructor->u_variable_name);
            break;

        case EX_B_ENUM_MEMBER:
            cg(enum_member)(ex->u_enum_member);
            break;

        case EX_B_STRING:
            out(string)(ex->u_string);
            break;

        case EX_B_FUNCTION_PARAMETER:
            out(string)(ex->u_function_parameter->name);
            break;

        case EX_B_EXPRESSION:
            out(char)('(');
            cg(ex_expression_data)(ex->u_expression);
            out(char)(')');
            break;

        otherwise_error
    }
}


    /** POSTFIX EXPRESSION **/

cgd_ex(postfix_data) {
    cg(ex_basic_data)(&ex->value);
    iterate_array(i, ex->level_list.size) {
        ex_postfix_level level = ex->level_list.data[i];
        switch (level.kind) {
            case EX_PL_INDEX:
                out(char)('[');
                cg(ex_expression_data)(level.u_index);
                out(char)(']');
                break;

            case EX_PL_PROPERTY:
                out(char)('.');
                out(string)(level.u_property->name);
                break;

            case EX_PL_POINTER_PROPERTY:
                out(string)("->");
                out(string)(level.u_property->name);
                break;

            case EX_PL_INVOCATION:
                out(char)('(');
                if (!arraylist_is_empty(level.u_invocation)) {
                    index_t last = level.u_invocation.size - 1;

                    iterate_array(j, last) {
                        cg(ex_expression_data)(&level.u_invocation.data[j]->data);
                        out(string)(", ");
                    }

                    cg(ex_expression_data)(&level.u_invocation.data[last]->data);
                }
                out(char)(')');
                break;

            otherwise_error
        }
    }
    switch (ex->kind) {
        case EX_P_INCREMENT:
            out(string)("++");
            break;

        case EX_P_DECREMENT:
            out(string)("--");
            break;

        case EX_P_PLAIN:
            break;

        otherwise_error
    }
}


    /** UNARY EXPRESSION **/

cgd_ex(unary_data) {
    switch (ex->kind) {
        case EX_U_INCREMENT:
            out(string)("++");
            break;

        case EX_U_DECREMENT:
            out(string)("--");
            break;

        case EX_U_MINUS:
            out(char)('-');
            break;

        case EX_U_PLUS:
            out(char)('+');
            break;

        case EX_U_PLAIN:
            break;
        
        otherwise_error
    }
    iterate_array(i, ex->op_list.size) {
        out(char)(op_unary_chars[ex->op_list.data[i]]);
    }
    cg(ex_postfix_data)(&ex->value);
}


    /** CAST EXPRESSION **/

cgd_ex(cast_data) {
    iterate_array(i, ex->cast_list.size) {
        out(char)('(');
        cg(type)(&ex->cast_list.data[i]);
        out(string)(") ");
    }
    cg(ex_unary_data)(&ex->value);
}


    /** BINARY EXPRESSION **/

cgd_ex(binary_data) {
    if (!ex->has_operation) {
        cg(ex_cast_data)(&ex->value);
    } else {
        cg(ex_binary_data)(ex->a);
        out(char)(' ');
        out(string)(op_binary_strings[ex->operator]);
        out(char)(' ');
        cg(ex_binary_data)(ex->b);
    }
}


    /** CONDITION EXPRESSION **/

cgd_ex(condition_data) {
    cg(ex_binary_data)(&ex->value);
    if (ex->has_condition) {
        out(string)(" ? ");
        cg(ex_expression_data)(ex->u_if);
        out(string)(" : ");
        cg(ex_condition_data)(ex->u_else);
    }
}


    /** EXPRESSION **/

cgd_ex(expression_data) {
    if (ex->has_assignment) {
        cg(ex_unary_data)(&ex->u_assignment.assignee);
        out(format)(" %s ", op_assign_strings[ex->u_assignment.operator]); 
        cg(ex_expression_data)(ex->u_assignment.value);
    } else {
        cg(ex_condition_data)(&ex->u_value);
    }
}


    /** IF STATEMENT **/

cgd_st(if) {
    cg(ex_block_constructors)(&st->condition);

    out(tabs)();
    out(string)("if (");
    cg(ex_block)(&st->condition);
    out(string)(") ");

    cg(statement)(st->body);

    if (st->has_else) {
        out(tabs)();
        out(string)("else ");
        cg(statement)(st->u_else);
    }
}


    /** WHILE STATEMENT **/

cgd_st(while) {
    cg(ex_block_constructors)(&st->condition);

    out(tabs)();
    out(string)("while (");
    cg(ex_block)(&st->condition);
    out(string)(") ");

    cg(statement)(st->body);
}


    /** JUMP STATEMENT **/

cgd_st(jump) {
    switch (st->kind) {
        case ST_J_BREAK:
            out(tabs)();
            out(string)("break;\n");
            break;

        case ST_J_CONTINUE:
            out(tabs)();
            out(string)("continue;\n");
            break;

        case ST_J_RETURN:
            cg(ex_block_constructors)(&st->u_return_value);

            out(tabs)();
            out(string)("return ");
            cg(ex_block)(&st->u_return_value);
            out(string(";\n"));
            break;

        case ST_J_RETURN_VOID:
            out(tabs)();
            out(string)("return;\n");
            break;

        otherwise_error
    }
}


    /** VARIABLE STATEMENT DECLARATION **/

cgd(st_variable_declaration, dc_st_variable* st) {
    out(tabs)();
    cg(type)(&st->type);

    out(string)(";\n");
}


    /** VARIABLE STATEMENT **/

cgd_dc_st_variable() {
    cg(ex_block_constructors)(&st->value);

    out(tabs)();
    cg(type)(&st->type);
    out(format)(" %s = ", st->name);
    cg(ex_block)(&st->value);

    out(string)(";\n");
}


    /** STATEMENT **/

cgd_statement() {
    switch (st->kind) {
        case ST_COMPOUND:
            cg(st_compound)(&st->u_st_compound);
            break;

        case ST_IF:
            cg(st_if)(&st->u_st_if);
            break;

        case ST_JUMP:
            cg(st_jump)(&st->u_st_jump);
            break;

        case ST_WHILE:
            cg(st_while)(&st->u_st_while);
            break;

        case ST_EXPRESSION:
            if (st->u_st_expression.value != NULL) {
                cg(ex_block_constructors)(&st->u_st_expression);

                out(tabs)();
                cg(ex_block)(&st->u_st_expression);
            }
            out(string)(";\n");
            break;

        otherwise_error
    }
}


    /** COMPOUND STATEMENT **/

cgd_st(compound) {
    out(string)("{\n");
    tabs++;
    iterate_array(i, st->size) {
        switch (st->data[i].kind) {
            case ST_C_ST_VARIABLE:
                cg(dc_st_variable)(st->data[i].u_variable);
                break;

            case ST_C_STATEMENT:
                cg(statement)(st->data[i].u_statement);
                break;

            otherwise_error
        }
    }
    tabs--;
    out(tabs)();
    out(string)("}\n");
}


    /** FUNCTION PARAMETERS **/

cgd(function_parameters, dc_function_parameters* parameters) {
    out(char)('(');
    if (!arraylist_is_empty(parameters->value)) {
        index_t last = parameters->value.size - 1;

        iterate_array(i, last) {
            cg(type)(&parameters->value.data[i].type);
            out(format)(" %s, ", parameters->value.data[i].name);
        }

        cg(type)(&parameters->value.data[last].type);
        out(format)(" %s", parameters->value.data[last].name);
    }
    if (parameters->is_c_vararg) {
        out(string)(", ...");
    }
    out(char)(')');
}


    /** FUNCTION DECLARATION **/

cgd(function_declaration, dc_function* dc) {
    /* handle function declarations */
    if (dc->is_extern) {
        return;
    }

    cg(type)(&dc->return_type);

    out(char)(' ');
    out(string)(dc->name);
    cg(function_parameters)(&dc->parameters);

    out(string)(";\n\n");
}

    /** FUNCTION **/

cgd_dc(function) {
    /* handle function declarations */
    if (dc->is_extern || !dc->is_full) {
        return;
    }

    cg(type)(&dc->return_type);

    out(char)(' ');
    out(string)(dc->name);
    cg(function_parameters)(&dc->parameters);

    cg(st_compound)(&dc->body);
    out(char)('\n');
}


    /** IMPORT **/

cgd_dc(import) {
    if (dc->is_native) {
        out(string)("#include <");
        iterate_array(i, dc->path.size) {
            out(string)(dc->path.data[i]);
        }
        out(string)(".h>\n");
    }
}

    /* code generation tasks */
void codegen_task_source_prefix(FILE* file) {
    out(string)("/* This source file was generated by the CARBONSTEEL compiler */\n");
    out(string)("/* Prefix end */\n\n");
}

cgtask_declare(header_prefix) {
    out(string)("/* This header was generated by the CARBONSTEEL compiler */\n");
    out(string)("#include <stdint.h>\n");
    out(string)("#include <stdbool.h>\n");
    out(string)("#include <stdlib.h>\n");
    out(string)("/* Prefix end */\n\n");
}

cgtask_declare_ast(definitions) {
    int tabs = 0;
    int tmp_value = 0;
    int* tmp = &tmp_value;

    iterate_array(i, ast->declaration_list.size) {
        declaration dc = *ast->declaration_list.data[i];
        switch (dc.kind) {
            case DC_IMPORT:
            case DC_STRUCTURE:
            case DC_ALIAS:
            case DC_ENUM:
                break;

            case DC_FUNCTION:
                cg(function)(dc.u_function);
                break;

            case DC_ST_VARIABLE:
                cg(dc_st_variable)(dc.u_variable);
                break;

            otherwise_error
        }
    }
}

cgtask_declare_ast(declarations) {
    int tabs = 0;
    int tmp_value = 0;
    int* tmp = &tmp_value;

    iterate_array(i, ast->declaration_list.size) {
        declaration dc = *ast->declaration_list.data[i];
        switch (dc.kind) {
            case DC_IMPORT:
                cg(import)(dc.u_import);
                break;

            case DC_STRUCTURE:
                cg(structure)(dc.u_structure);
                break;

            case DC_ALIAS:
                break;

            case DC_ENUM:
                cg(enum)(dc.u_enum);
                break;

            case DC_FUNCTION:
            case DC_ST_VARIABLE:
                break;

            otherwise_error
        }
    }

    /* functions and variables are declared last */
    iterate_array(i, ast->declaration_list.size) {
        declaration dc = *ast->declaration_list.data[i];
        switch (dc.kind) {
            case DC_IMPORT:
            case DC_STRUCTURE:
            case DC_ALIAS:
            case DC_ENUM:
                break;

            case DC_FUNCTION:
                cg(function_declaration)(dc.u_function);
                break;
            
            case DC_ST_VARIABLE:
                out(string)("extern ");
                cg(st_variable_declaration)(dc.u_variable);
                break;

            otherwise_error
        }
    }
}


    /* functions */
/**
 * Does code generation into the
 * specified output file
 * 
 * @param ast The abstract syntax tree
 * @param output The output source code file
 */
void codegen(ast_root* ast, FILE* file) {
    cgtask(header_prefix);
    cgtask_ast(declarations);

    codegen_task_source_prefix(file);
    cgtask_ast(definitions);
}