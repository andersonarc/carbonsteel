# carbonsteel build file

# project definition
project('carbonsteel', 'c', 
    version: '0.2')

# find required programs
flex = find_program('flex')
bison = find_program('bison')

# generated sources and headers
lexer = custom_target('lexer', 
    build_by_default: true,
    input: 'src/parser/lexer.l', 
    output: ['lexer.c', 'lexer.h'],
    command: [flex, '-o', 'lexer.c', '@INPUT@'])
parser = custom_target('parser', 
    build_by_default: true,
    input: 'src/parser/parser.y', 
    output: ['parser.c', 'parser.h'],
    command: [bison, '--defines=parser.h', '-o', 'parser.c', '@INPUT@'])

# c compiler arguments
c_args = ['-Wno-unused-function']

# get dependencies
math = meson.get_compiler('c').find_library('m', 
    required: false)
ctool = dependency('ctool', 
    fallback: ['ctool', 'libctool_dep'])

# prepare build files
src = files('src/main.c', 
            'src/ast/root.c', 
            'src/ast/type.c', 
            'src/codegen/codegen.c',
            'src/misc/convert.c',
            'src/misc/generic.c', 
            'src/parser/context.c', 
            'src/syntax/declaration/declaration.c',
            'src/syntax/expression/binary.c', 
            'src/syntax/expression/operator.c',
            'src/syntax/expression/unary.c')
include = include_directories('include')

# compile executable
executable('carbonsteel', [src, parser, lexer],
    include_directories: include,
    dependencies: [ctool, math],
    c_args: c_args)