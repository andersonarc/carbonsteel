/**
 * @file codegen.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.2
 * @date 2021-08-06
 * 
 *  Code generation macro for easier
 *  implementation of functions and
 *  the main code generation interface
 */
    /* header guard */
#ifndef CARBONSTEEL_CODEGEN_CODEGEN_H
#define CARBONSTEEL_CODEGEN_CODEGEN_H

    /* includes */
#include <stdio.h> /* file functions */

#include "ctool/macro.h" /* macro concatenation */
#include "ast/root.h" /* ast root */


    /* defines */
/**
 * Declares a code generation function
 * for an expression/statement/declaration
 * 
 * @param[in] name Name of the element
 */
#define cgd(name, argument) void macro_concatenate(codegen_, name)(argument, FILE* file, int tabs, int* tmp)
/* --- */
#define cgd_ex(name)            cgd(macro_concatenate(ex_, name), macro_concatenate(ex_, name)* ex)
/* --- */
#define cgd_st(name)            cgd(macro_concatenate(st_, name), macro_concatenate(st_, name)* st)
#define cgd_statement()         cgd(statement, statement* st)
#define cgd_dc_st_variable()    cgd(dc_st_variable, dc_st_variable* st)
/* --- */
#define cgd_dc(name)            cgd(name, macro_concatenate(dc_, name)* dc)
/* --- */
#define cgd_type(name)          cgd(type, ast_type* type)


/**
 * Invokes a code generation function
 * of specified type
 * 
 * @param[in] x The code generation function type
 */
#define cg(x) macro_concatenate(codegen_, x) _codegen_call
#define _codegen_call(x) (x, file, tabs, tmp)


/**
 * Emits a specified (tabs/char/string/format)
 * data into the code generation function file
 * 
 * @param[in] x One of "tabs", "char", "string" or "format"
 */
#define out(x) macro_concatenate(_codegen_out_, x)
#define _codegen_out_tabs() iterate_array(t, tabs) { out(char)('\t'); }
#define _codegen_out_char(character) fputc(character, file); fflush(file)
#define _codegen_out_string(string) fputs(string, file); fflush(file)
#define _codegen_out_format(string, ...) fprintf(file, string, __VA_ARGS__); fflush(file)


    /* functions */
/**
 * Does code generation into the
 * specified output file
 * 
 * @param ast The abstract syntax tree
 * @param output The output source code file
 */
void codegen(ast_root* ast, FILE* output);

#endif /* CARBONSTEEL_CODEGEN_CODEGEN_H */