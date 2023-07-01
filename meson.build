# carbonsteel build file

# project definition
project('carbonsteel', 'c', 
    version: '0.2')

# find required programs
flex = find_program('flex')
bison = find_program('bison')

# generated sources and headers
main_lexer = custom_target('main_lexer', 
    build_by_default: true,
    input: 'src/language/lexer.l', 
    output: ['main_lexer.c', 'main_lexer.h'],
    command: [flex, '@INPUT@'])
main_parser = custom_target('main_parser', 
    build_by_default: true,
    input: 'src/language/parser.y', 
    output: ['main_parser.c', 'main_parser.h'],
    command: [bison, '--defines=main_parser.h', '-o', 'main_parser.c', '@INPUT@'])

# c compiler arguments
c_args = ['-Wno-unused-function']

# get dependencies
math = meson.get_compiler('c').find_library('m', 
    required: false)
ctool = dependency('ctool', 
    fallback: ['ctool', 'libctool_dep'],
    default_options: 'default_library=static')

# prepare build files
src = files('src/main.c', 
            'src/ast/lookup.c',
            'src/ast/root.c', 
            'src/ast/search.c', 
            'src/ast/type/type.c', 
            'src/ast/type/check.c', 
            'src/ast/type/resolve.c', 
            'src/ast/type/primitive.c', 
            'src/codegen/codegen.c',
            'src/misc/convert.c',
            'src/misc/generic.c', 
            'src/language/context.c', 
            'src/syntax/expression/basic.c',
            'src/syntax/expression/binary.c',
            'src/syntax/expression/cast.c',
            'src/syntax/expression/complex.c',
            'src/syntax/expression/constant.c',
            'src/syntax/expression/constructor.c',
            'src/syntax/expression/number.c',
            'src/syntax/expression/operator.c',
            'src/syntax/expression/postfix.c',
            'src/syntax/expression/unary.c')
include = include_directories('include')

# compile executable
executable('carbonsteel', [src, main_parser, main_lexer],
    include_directories: include,
    dependencies: [ctool, math],
    c_args: c_args)