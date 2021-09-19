/**
 * @file lexer.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-17
 * 
 *  Wrapper for the main lexer header file
 */
    /* header guard */
#ifndef CARBONSTEEL_LANGUAGE_LEXER_H
#define CARBONSTEEL_LANGUAGE_LEXER_H

    /* defines */
#define YYSTYPE MYYSTYPE
#define YYLTYPE MYYLTYPE
#define YY_DECL int myylex(MYYSTYPE* yylval_param, MYYLTYPE* yylloc_param, void* yyscanner, se_context* context)

    /* includes */
#include "main_lexer.h"

    /* undefines */
#undef YYSTYPE
#undef YYLTYPE
#undef YY_DECL

#endif /* CARBONSTEEL_LANGUAGE_LEXER_H */