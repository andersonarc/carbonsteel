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
#include "misc/convert.h" /* conversion functions */
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
cgd_expression_block();
cgd_expression();
cgd_ex(condition);

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
            cg(expression)(ex->u_array_size);
            out(string)(");\n");

            iterate_array(i, ex->argument_list.size) {
                out(tabs)();
                out(format)("%s[%zu] = ", ex->u_variable_name, i);
                cg(expression)(ex->argument_list.data[i]);
                out(string)(";\n");
            }
        } else {
            out(char)('[');
            cg(expression)(ex->u_array_size);

            out(string)("] = { ");

            iterate_array(i, ex->argument_list.size) {
                cg(expression)(ex->argument_list.data[i]);
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
                        cg(expression)(ex->argument_list.data[i]);
                        out(string)(";\n");
                    }
                } else {
                    out(string)(" = { ");
                    iterate_array(i, ex->argument_list.size) {
                        cg(expression)(ex->argument_list.data[i]);
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

                cg(expression)(ex->argument_list.data[0]);
                out(string)(";\n");
                
                break;

            case AST_TYPE_ENUM:
                error_syntax("cannot construct an enum type");
                break;

            case AST_TYPE_FUNCTION:
                error_syntax("cannot construct a function type");
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

cgd_ex(number) {
    switch (ex->kind) {
        case EX_N_FLOAT:
            out(format)("%f", ex->u_float_constant);
            break;

        case EX_N_INT:
            out(format)("%d", ex->u_int_constant);
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
            cg(ex_number)(&member->value);
            out(string)(",\n");
        }

        out(tabs)();
        member = &dc->member_list.data[last];
        cg(enum_member)(member);
        out(string)(" = ");
        cg(ex_number)(&member->value);
        out(char)('\n');
    }
    out(format)("} %s;\n\n", dc->name);
}


    /** EXPRESSION BLOCK **/

cgd(expression_block_constructors, expression_block* ex) {
    iterate_array(i, ex->constructors.size) {
        cg(ex_constructor)(ex->constructors.data[i]);
    }
}
cgd_expression_block() {
    cg(expression)(ex->value);
}


    /** BASIC EXPRESSION **/

cgd_ex(basic) {
    switch (ex->kind) {
        case EX_B_FUNCTION:
            out(string)(ex->u_function->name);
            break;

        case EX_B_VARIABLE:
            out(string)(ex->u_variable->name);
            break;

        case EX_B_EX_NUMBER:
            cg(ex_number)(&ex->u_ex_number);
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
            cg(expression)(ex->u_expression);
            out(char)(')');
            break;

        otherwise_error
    }
}


    /** POSTFIX EXPRESSION **/

cgd_ex(postfix) {
    cg(ex_basic)(&ex->value);
    iterate_array(i, ex->level_list.size) {
        ex_postfix_level level = ex->level_list.data[i];
        switch (level.kind) {
            case EX_PL_INDEX:
                out(char)('[');
                cg(expression)(level.u_index);
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
                if (!arraylist_is_empty(level.u_argument_list)) {
                    index_t last = level.u_argument_list.size - 1;

                    iterate_array(j, last) {
                        cg(expression)(level.u_argument_list.data[j]);
                        out(string)(", ");
                    }

                    cg(expression)(level.u_argument_list.data[last]);
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

cgd_ex(unary) {
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
    cg(ex_postfix)(&ex->value);
}


    /** CAST EXPRESSION **/

cgd_ex(cast) {
    iterate_array(i, ex->cast_list.size) {
        out(char)('(');
        cg(type)(&ex->cast_list.data[i]);
        out(string)(") ");
    }
    cg(ex_unary)(&ex->value);
}


    /** BINARY EXPRESSION **/

cgd_ex(binary_inline) {
    if (!ex->has_operation) {
        cg(ex_cast)(&ex->value);
    } else {
        cg(ex_binary_inline)(ex->a);
        out(char)(' ');
        out(string)(op_binary_strings[ex->operator]);
        out(char)(' ');
        cg(ex_binary_inline)(ex->b);
    }
}
cgd_ex(binary) { cg(ex_binary_inline)(&ex->v); }


    /** CONDITION EXPRESSION **/

cgd_ex(condition) {
    cg(ex_binary)(&ex->value);
    if (ex->has_condition) {
        out(string)(" ? ");
        cg(expression)(ex->u_if);
        out(string)(" : ");
        cg(ex_condition)(ex->u_else);
    }
}


    /** EXPRESSION **/

cgd_expression() {
    if (ex->has_assignment) {
        cg(ex_unary)(&ex->u_assignment.assignee);
        out(format)(" %s ", op_assign_strings[ex->u_assignment.operator]); 
        cg(expression)(ex->u_assignment.value);
    } else {
        cg(ex_condition)(&ex->u_value);
    }
}


    /** IF STATEMENT **/

cgd_st(if) {
    cg(expression_block_constructors)(&st->condition);

    out(tabs)();
    out(string)("if (");
    cg(expression_block)(&st->condition);
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
    cg(expression_block_constructors)(&st->condition);

    out(tabs)();
    out(string)("while (");
    cg(expression_block)(&st->condition);
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
            cg(expression_block_constructors)(&st->u_return_value);

            out(tabs)();
            out(string)("return ");
            cg(expression_block)(&st->u_return_value);
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
    cg(expression_block_constructors)(&st->value);

    out(tabs)();
    cg(type)(&st->type);
    out(format)(" %s = ", st->name);
    cg(expression_block)(&st->value);

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
                cg(expression_block_constructors)(&st->u_st_expression);

                out(tabs)();
                cg(expression_block)(&st->u_st_expression);
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
    cg(type)(&dc->return_type);

    out(char)(' ');
    out(string)(dc->name);
    cg(function_parameters)(&dc->parameters);

    out(string)(";\n\n");
}

    /** FUNCTION **/

cgd_dc(function) {
    /* handle function declarations */
    if (dc->is_extern) {
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
    out(string)("#include <");
    iterate_array(i, dc->size) {
        out(string)(dc->data[i]);
    }
    out(string)(".h>\n");
}

    /* code generation tasks */
void codegen_task_source_prefix(FILE* file, char* header_name) {
    out(string)("/* This source file was generated by the CARBONSTEEL compiler */\n");

    out(string)("#include \"");
    out(string)(header_name);
    out(string)("\"\n");

    out(string)("/* Prefix end */\n\n");
}

cgtask_declare(header_prefix) {
    out(string)("/* This header file was generated by the CARBONSTEEL compiler */\n");
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
        declaration dc = ast->declaration_list.data[i];
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
        declaration dc = ast->declaration_list.data[i];
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
 * specified output source and header files
 * 
 * @param ast The abstract syntax tree
 * @param source The source file
 * @param header The header file
 * @param header_name The header file name
 */
void codegen(ast_root* ast, FILE* source, FILE* header, char* header_name) {
    FILE* file = header;
    cgtask(header_prefix);
    cgtask_ast(declarations);

    file = source;
    codegen_task_source_prefix(file, header_name);
    cgtask_ast(definitions);
}