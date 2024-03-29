# Carbonsteel

![](https://raw.githubusercontent.com/andersonarc/carbonsteel/master/logo.svg)

Programming language transpiler designed to fix existing issues and introduce new features without changing the old-style C syntax.

## Overview

### Differences from C

- `#include` changed to `import`
- `typedef struct X X` changed to `type X`
- `typedef enum X X` changed to `enum X`
- `typedef A B` changed to `alias B = A`

*Actually, there are more, but they aren't listed yet.*

### Features

- Auto-generated inline constructors for every type
- Auto-generated headers for each source file
- More strict type checking
- Interoperability with C using the `import native` statement
- Generic types

### Planned features

- Generic methods
- Safer unions
- Expression statements `x = if (a) b else c` instead of conditional expressions `x = a ? b : c` 
- Auto-generated `free()` statements for allocated memory
- Support for C type qualifiers such as `static`, `extern`, `inline`

## Building Carbonsteel

See [BUILD.md](BUILD.md).