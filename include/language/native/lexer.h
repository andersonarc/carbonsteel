/**
 * @file lexer.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-17
 * 
 *  Wrapper for the c-native lexer header file
 */
    /* header guard */
#ifndef CARBONSTEEL_LANGUAGE_NATIVE_LEXER_H
#define CARBONSTEEL_LANGUAGE_NATIVE_LEXER_H

    /* defines */
#define YYSTYPE CYYSTYPE
#define YYLTYPE CYYLTYPE
#define YY_DECL int cyylex(CYYSTYPE* yylval_param, CYYLTYPE* yylloc_param, void* yyscanner, se_context* context)

    /* includes */
#include "native_lexer.h"

    /* undefines */
#undef YYSTYPE
#undef YYLTYPE
#undef YY_DECL

#endif /* CARBONSTEEL_LANGUAGE_NATIVE_LEXER_H */