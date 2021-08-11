/**
 * @file codegen.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-08-06
 * 
 *  Code generation from the lexical structures
 */
    /* header guard */
#ifndef CARBONSTEEL_CODEGEN_H
#define CARBONSTEEL_CODEGEN_H

    /* includes */
#include "ast.h" /* syntax tree */
#include "expression.h" /* expressions */
#include "statement.h" /* statements */

#include <stdio.h> /* file functions */

    /* defines */
/**
 * Declares a code generation function
 * for an expression/statement/AST element
 * 
 * @param[in] name Name of the element
 */
#define _codegen_declare(name, argument) void codegen_##name(argument, FILE* file, int tabs, int* tmp)
#define cg_ex(name) _codegen_declare(ex_##name, ex_##name* ex)
#define cg_expression() _codegen_declare(expression, expression* ex)
#define cg_st(name) _codegen_declare(st_##name, st_##name* st)
#define cg_statement() _codegen_declare(statement, statement* st)
#define cg_ast(name) _codegen_declare(name, ast_##name* name)

/**
 * Invokes a code generation function
 * of specified type
 * 
 * @param[in] x The code generation function type
 */
#define cg(x) codegen_##x _codegen_call
#define _codegen_call(x) (x, file, tabs, tmp)

/**
 * Emits a specified (tabs/char/string/format)
 * data into the code generation function file
 * 
 * @param[in] x One of tabs, char, string or format
 */
#define out(x) _codegen_out_##x
#define _codegen_out_tabs() iterate_array(t, tabs) { out(char)('\t'); }
#define _codegen_out_char(character) fputc(character, file)
#define _codegen_out_string(string) fputs(string, file)
#define _codegen_out_format(string, ...) fprintf(file, string, __VA_ARGS__)

    /* functions */
/**
 * Does code generation into the
 * specified output file
 * 
 * @param file The file
 */
void codegen(FILE* file);

#endif /* CARBONSTEEL_CODEGEN_H */