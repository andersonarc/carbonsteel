/**
 * @file codegen.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-08-06
 * 
 *  Code generation implementation
 */
    /* includes */
#include "codegen.h" /* this */

#include <string.h> /* string functions */
#include "convert.h" /* converstion functions */

    /* defines */
/**
 * Code generation task (e.g. structures, functions)
 * declaration and invocation macro
 */
#define cgtask_declare(name) void codegen_task_##name(FILE* file)
#define cgtask(name) codegen_task_##name(file)

    /* forward declarations */
cg_dc(type);
cg_statement();
cg_st(compound);
cg_expression();
cg_ex(condition);

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
    char* name = malloc(name_size);
    snprintf(name, name_size, "_cst_tmpvar_%d_", *tmp);
    (*tmp)++;
    return name;
}
#define get_tmp_name() get_tmp_name_from(tmp)

        /* code generation functions */

    /** CONSTRUCTOR EXPRESSION **/

cg_ex(constructor) {
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

            //todo generate size checker

            iterate_array(i, ex->arguments.size) {
                out(format)("%s[%zu] = ", ex->u_variable_name, i);
                cg(expression)(&ex->arguments.data[i]);
                out(string)(";\n");
            }
        } else {
            out(char)('[');
            cg(expression)(ex->u_array_size); 
            
            //todo constexpr size checking

            out(string)("] = { ");

            iterate_array(i, ex->arguments.size) {
                cg(expression)(&ex->arguments.data[i]);
                if (i != ex->arguments.size - 1) {
                    out(char)(',');
                }
                out(char)(' ');
            }

            out(string)("};\n");
        }
    } else {
        start:
        switch (ex->type->kind) {
            case DC_TK_STRUCTURE:
                if (ex->type->u_structure->member_list.size != ex->arguments.size) {
                    error_syntax("invalid argument count for a constructor");
                }

                if (ex->is_new) {
                    out(string)(" = malloc(sizeof(");
                    cg(type)(ex->type);
                    out(string)("));");

                    iterate_array(i, ex->type->u_structure->member_list.size) {
                        out(tabs)();
                        out(format)("%s->%s = ", ex->u_variable_name, ex->type->u_structure->member_list.data[i].name);
                        cg(expression)(&ex->arguments.data[i]);
                        out(string)(";\n");
                    }
                } else {
                    out(string)(" = { ");
                    iterate_array(i, ex->arguments.size) {
                        cg(expression)(&ex->arguments.data[i]);
                        if (i != ex->arguments.size - 1) {
                            out(char)(',');
                        }
                        out(char)(' ');
                    }

                    out(string)("};\n");
                }
                
                break;

            case DC_TK_ALIAS:
                ex->type = ex->type->u_alias->target;
                goto start;
                break;

            case DC_TK_PRIMITIVE:
                if (ex->arguments.size != 1) {
                    error_syntax("invalid argument count for a constructor");
                }

                if (ex->is_new) {
                    out(string)(" = malloc(sizeof(");
                    cg(type)(ex->type);
                    out(string)("));\n");

                    out(tabs)();
                    out(format)("*%s = ", ex->u_variable_name);
                } else {
                    out(string)(" = ");
                }

                cg(expression)(&ex->arguments.data[0]);
                out(string)(";\n");
                
                break;

            case DC_TK_ENUM:
                error_syntax("cannot construct an enum");
                break;

            otherwise_error
        }
    }
}


    /** STRUCTURE **/

cg_dc(structure_member) {
    out(tabs)();

    cg(type)(&dc->type);
    out(format)(" %s;\n", dc->name);
}

cg_dc(structure) {
    out(format)("typedef struct %s {\n", dc->name);
    
    tabs++;
    iterate_array(i, dc->member_list.size) {
        cg(structure_member)(&dc->member_list.data[i]);
    }
    out(format)("} %s;\n\n", dc->name);
}


    /** TYPE **/

cg_dc(type) {
    switch (dc->kind) {
        case DC_TK_PRIMITIVE:
            out(string)(dc->u_primitive->code_name);
            break;

        case DC_TK_STRUCTURE:
            if (dc->u_structure->name != NULL) {
                out(string)(dc->u_structure->name);
            }
            break;

        case DC_TK_ENUM:
            if (dc->u_enum->name != NULL) {
                out(string)(dc->u_enum->name);
            }
            break;

        case DC_TK_ALIAS:
            out(string)(dc->u_alias->name);
            break;

        otherwise_error
    }
    iterate_array(i, dc->level_list.size) {
        switch (dc->level_list.data[i].kind) {
            case DC_TL_POINTER:
            case DC_TL_ARRAY:
                out(char)('*');
                break;

            otherwise_error
        }
    }
}


    /** ENUM **/

cg_dc(enum) {
    out(format)("typedef enum %s {\n", dc->name);
    
    tabs++;
    iterate_array(i, dc->member_list.size) {
        out(string)(dc->member_list.data[i].name);
        //todo enum value generation
    }
    out(format)("} %s;\n\n", dc->name);
}


    /** EXPRESSION CONSTRUCTORS **/

_codegen_declare(expression_constructors, expression* ex) {
    iterate_array(i, ex->constructors.size) {
        cg(ex_constructor)(ex->constructors.data[i]);
    }
}


    /** BASIC EXPRESSION **/

cg_ex(basic) {
    switch (ex->kind) {
        case EX_B_EXPRESSION:
            out(char)('(');
            cg(expression)(ex->u_expression);
            out(char)(')');
            break;

        case EX_B_FUNCTION:
            out(string)(ex->u_function->name);
            break;

        case EX_B_VARIABLE:
            out(string)(ex->u_variable->name);
            break;

        case EX_B_NUMBER:
            out(string)(ex->u_number.value);
            break;

        case EX_B_BOOLEAN:
            out(string)(ex->u_boolean.value);
            break;

        case EX_B_FUNCTION_PARAMETER:
            out(string)(ex->u_function_parameter->name);
            break;

        case EX_B_CONSTRUCTOR:
            out(string)(ex->u_constructor->u_variable_name);
            break;

        otherwise_error
    }
}


    /** POSTFIX EXPRESSION **/

cg_ex(postfix) {
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
                out(string)(level.u_property);
                break;

            case EX_PL_INVOCATION:
                out(char)('(');
                iterate_array(j, level.u_arguments.size) {
                    cg(expression)(&level.u_arguments.data[j]);
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

cg_ex(unary) {
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
        out(string)(op_unary_strings[ex->op_list.data[i]]);
    }
    cg(ex_postfix)(&ex->value);
}


    /** CAST EXPRESSION **/

cg_ex(cast) {
    iterate_array(i, ex->cast_list.size) {
        out(char)('(');
        cg(type)(&ex->cast_list.data[i]);
        out(string)(") ");
    }
    cg(ex_unary)(&ex->value);
}


    /** CONDITION EXPRESSION **/

cg_ex(condition) {
    cg(ex_or)(&ex->a);
    if (ex->has_condition) {
        out(string)(" ? ");
        cg(expression)(ex->u_if);
        out(string)(" : ");
        cg(ex_condition)(ex->u_else);
    }
}


    /** EXPRESSION **/

cg_expression() {
    if (ex->has_assignment) {
        cg(ex_unary)(&ex->u_assignment.a);
        out(format)(" %s ", op_assign_strings[ex->u_assignment.op]); 
        cg(expression)(ex->u_assignment.b);
    } else {
        cg(ex_condition)(&ex->u_plain);
    }
}


    /** IF STATEMENT **/

cg_st(if) {
    cg(expression_constructors)(st->condition);

    out(tabs)();
    out(string)("if (");
    cg(expression)(st->condition);
    out(string)(") ");

    cg(statement)(st->body);

    if (st->has_else) {
        out(tabs)();
        out(string)("else ");
        cg(statement)(st->u_else);
    }
}


    /** WHILE STATEMENT **/

cg_st(while) {
    cg(expression_constructors)(st->condition);

    out(tabs)();
    out(string)("while (");
    cg(expression)(st->condition);
    out(string)(") ");

    cg(statement)(st->body);
}


    /** JUMP STATEMENT **/

cg_st(jump) {
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
            cg(expression_constructors)(st->u_return_value);

            out(tabs)();
            out(string)("return ");
            cg(expression)(st->u_return_value);
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

_codegen_declare(st_variable_declaration, st_variable* st) {
    out(tabs)();
    cg(type)(&st->type);

    out(string)(";\n");
}


    /** VARIABLE STATEMENT **/

cg_st(variable) {
    cg(expression_constructors)(st->value);

    out(tabs)();
    cg(type)(&st->type);
    out(format)(" %s = ", st->name);
    cg(expression)(st->value);

    out(string)(";\n");
}


    /** STATEMENT **/

//todo store declarations in AST and preserve order

cg_statement() {
    switch (st->kind) {
        case ST_COMPOUND:
            cg(st_compound)(st->u_st_compound);
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
            out(tabs)();
            cg(expression_constructors)(st->u_st_expression);
            cg(expression)(st->u_st_expression);
            out(string)(";\n");
            break;

        otherwise_error
    }
}


    /** COMPOUND STATEMENT **/

cg_st(compound) {
    out(string)("{\n");
    tabs++;
    iterate_array(i, st->size) {
        switch (st->data[i].kind) {
            case ST_C_PROPERTY:
                cg(st_variable)(&st->data[i].u_st_variable);
                break;

            case ST_C_STATEMENT:
                cg(statement)(&st->data[i].u_statement);
                break;

            default:
                error_internal("unexpected");
                break;
        }
    }
    tabs--;
    out(tabs)();
    out(string)("}\n");
}


    /** FUNCTION DECLARATION **/

_codegen_declare(function_declaration, dc_function* dc) {
    cg(type)(&dc->return_type);

    out(format)(" %s(", dc->name);
    if (!arraylist_is_empty(dc->parameter_list)) {
        index_t last = dc->parameter_list.size - 1;

        iterate_array(i, last) {
            cg(type)(&dc->parameter_list.data[i].type);
            out(format)(" %s, ", dc->parameter_list.data[i].name);
        }

        cg(type)(&dc->parameter_list.data[last].type);
        out(format)(" %s", dc->parameter_list.data[last].name);
    }

    out(string)(");\n\n");
}

    /** FUNCTION **/

cg_dc(function) {
    cg(type)(&dc->return_type);

    out(format)(" %s(", dc->name);
    if (!arraylist_is_empty(dc->parameter_list)) {
        index_t last = dc->parameter_list.size - 1;

        iterate_array(i, last) {
            cg(type)(&dc->parameter_list.data[i].type);
            out(format)(" %s, ", dc->parameter_list.data[i].name);
        }

        cg(type)(&dc->parameter_list.data[last].type);
        out(format)(" %s", dc->parameter_list.data[last].name);
    }
    out(string)(") ");
    cg(st_compound)(&dc->body);
    out(char)('\n');
}


    /** IMPORT **/

cg_dc(import) {
    out(string)("#include <");
    iterate_array(i, dc->size) {
        out(string)(dc->data[i]);
    }
    out(string)(".h>\n");
}

    
    /** ALIASES **/

cg_dc(alias) {
    out(string)("typedef ");
    cg(type)(dc->target);
    out(char)(' ');
    out(string)(dc->name);
    out(string)(";\n\n");
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
    out(string)("/* Prefix end */\n\n");
}

cgtask_declare(definitions) {
    int tabs = 0;
    int tmp_value = 0;
    int* tmp = &tmp_value;

    iterate_array(i, ast.declaration_list.size) {
        declaration dc = ast.declaration_list.data[i];
        switch (dc.kind) {
            case DC_IMPORT:
            case DC_STRUCTURE:
            case DC_ALIAS:
            case DC_ENUM:
                break;

            case DC_FUNCTION:
                cg(function)(&dc.u_function);
                break;

            case DC_ST_VARIABLE:
                cg(st_variable)(dc.u_st_variable);
                break;

            otherwise_error
        }
    }
}

cgtask_declare(declarations) {
    int tabs = 0;
    int tmp_value = 0;
    int* tmp = &tmp_value;

    iterate_array(i, ast.declaration_list.size) {
        declaration dc = ast.declaration_list.data[i];
        switch (dc.kind) {
            case DC_IMPORT:
                cg(import)(&dc.u_import);
                break;

            case DC_STRUCTURE:
                cg(structure)(&dc.u_structure);
                break;

            case DC_ALIAS:
                cg(alias)(&dc.u_alias);
                break;

            case DC_ENUM:
                cg(enum)(&dc.u_enum);
                break;

            case DC_FUNCTION:
                cg(function_declaration)(&dc.u_function);
                break;
            
            case DC_ST_VARIABLE:
                out(string)("extern ");
                cg(st_variable_declaration)(dc.u_st_variable);
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
 * @param source The source file
 * @param header The header file
 * @param header_name The header file name
 */
void codegen(FILE* source, FILE* header, char* header_name) {
    FILE* file = header;
    cgtask(header_prefix);
    cgtask(declarations);

    file = source;
    codegen_task_source_prefix(file, header_name);
    cgtask(definitions);
}