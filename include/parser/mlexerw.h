/**
 * @file mlexerw.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-09-17
 * 
 *  Wrapper for the lexer header include
 */
    /* header guard */
#ifndef CARBONSTEEL_PARSER_MLEXERW_H
#define CARBONSTEEL_PARSER_MLEXERW_H

    /* defines */
#define YYSTYPE MPSTYPE
#define YYLTYPE MPLTYPE
#define YY_DECL int mplex(MPSTYPE* yylval_param, MPLTYPE* yylloc_param, void* yyscanner, se_context* context)

    /* includes */
#include "mlexer.h"

    /* undefines */
#undef YYSTYPE
#undef YYLTYPE
#undef YY_DECL

#endif /* CARBONSTEEL_PARSER_MLEXERW_H */