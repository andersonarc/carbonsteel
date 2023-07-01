/**
 * @file binary.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2022-01-26
 * 
 *  Binary constant expression system
 * 
 *  To emulate binary expressions at compilation time,
 *  an O(n^2) complexity algorithm is used, where each
 *  pair of primitive types is processed disretely.
 * 
 *  The main API macro is ex_constant_inherit_binary,
 *  which accepts both parent constant expression and the operation.
 * 
 *  Then the first constant expression type is 'switched'
 *  in the _ex_constant_inherit_binary_case macro.
 * 
 *  Consequently, the second constant expression type is 'switched'
 *  in the _ex_constant_inherit_binary_case_inner macro.
 * 
 *  This is where things get complicated. The merge result' type
 *  should be computed. It has been implemented using a constant conversion
 *  lookup table and therefore does not affect performance (only the code readability)
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_CONSTANT_BINARY
#define CARBONSTEEL_SYNTAX_EXPRESSION_CONSTANT_BINARY

#define ex_constant_inherit_binary_boolean(this, parent1, operation, parent2) \
switch (parent1.kind) { \
	case EX_C_BOOLEAN: switch (parent2.kind) { \
		case EX_C_BOOLEAN: this.u_boolean = (parent1.u_boolean) operation (parent2.u_boolean); break; \
		otherwise_error \
	} \
	otherwise_error \
}


#define ex_constant_inherit_binary_all(this, parent1, operation, parent2) \
switch (parent1.kind) { \
	case EX_C_BOOLEAN: switch (parent2.kind) { \
		case EX_C_BOOLEAN: this.u_boolean = (parent1.u_boolean) operation (parent2.u_boolean); break; \
		otherwise_error \
	} \
	case EX_C_BYTE: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_byte = (parent1.u_byte) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_short = (parent1.u_byte) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_byte) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_byte) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_short = (parent1.u_byte) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_int = (parent1.u_byte) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_byte) operation (parent2.u_uint); break; \
		case EX_C_FLOAT: this.u_float = (parent1.u_byte) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_float = (parent1.u_byte) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_SHORT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_short = (parent1.u_short) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_short = (parent1.u_short) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_short) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_short) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_short = (parent1.u_short) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_int = (parent1.u_short) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_short) operation (parent2.u_uint); break; \
		case EX_C_FLOAT: this.u_float = (parent1.u_short) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_float = (parent1.u_short) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_INT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_int = (parent1.u_int) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_int = (parent1.u_int) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_int) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_int) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_int = (parent1.u_int) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_int = (parent1.u_int) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_int) operation (parent2.u_uint); break; \
		case EX_C_FLOAT: this.u_double = (parent1.u_int) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_double = (parent1.u_int) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_LONG: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_long = (parent1.u_long) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_long = (parent1.u_long) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_long = (parent1.u_long) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_long) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_long = (parent1.u_long) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_long = (parent1.u_long) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_long) operation (parent2.u_uint); break; \
		otherwise_error \
	} \
	case EX_C_UBYTE: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_short = (parent1.u_ubyte) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_short = (parent1.u_ubyte) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_short = (parent1.u_ubyte) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_short = (parent1.u_ubyte) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_ubyte = (parent1.u_ubyte) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_ushort = (parent1.u_ubyte) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_uint = (parent1.u_ubyte) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_ubyte) operation (parent2.u_ulong); break; \
		case EX_C_FLOAT: this.u_float = (parent1.u_ubyte) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_float = (parent1.u_ubyte) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_USHORT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_int = (parent1.u_ushort) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_int = (parent1.u_ushort) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_ushort) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_int = (parent1.u_ushort) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_ushort = (parent1.u_ushort) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_ushort = (parent1.u_ushort) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_uint = (parent1.u_ushort) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_ushort) operation (parent2.u_ulong); break; \
		case EX_C_FLOAT: this.u_float = (parent1.u_ushort) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_float = (parent1.u_ushort) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_UINT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_long = (parent1.u_uint) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_long = (parent1.u_uint) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_long = (parent1.u_uint) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_uint) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_uint = (parent1.u_uint) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_uint = (parent1.u_uint) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_uint = (parent1.u_uint) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_uint) operation (parent2.u_ulong); break; \
		case EX_C_FLOAT: this.u_double = (parent1.u_uint) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_double = (parent1.u_uint) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_ULONG: switch (parent2.kind) { \
		case EX_C_UBYTE: this.u_ulong = (parent1.u_ulong) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_ulong = (parent1.u_ulong) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_ulong = (parent1.u_ulong) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_ulong) operation (parent2.u_ulong); break; \
		otherwise_error \
	} \
	case EX_C_FLOAT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_float = (parent1.u_float) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_float = (parent1.u_float) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_double = (parent1.u_float) operation (parent2.u_int); break; \
		case EX_C_UBYTE: this.u_float = (parent1.u_float) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_float = (parent1.u_float) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_double = (parent1.u_float) operation (parent2.u_uint); break; \
		case EX_C_FLOAT: this.u_float = (parent1.u_float) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_double = (parent1.u_float) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_DOUBLE: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_double = (parent1.u_double) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_double = (parent1.u_double) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_double = (parent1.u_double) operation (parent2.u_int); break; \
		case EX_C_UBYTE: this.u_double = (parent1.u_double) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_double = (parent1.u_double) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_double = (parent1.u_double) operation (parent2.u_uint); break; \
		case EX_C_FLOAT: this.u_double = (parent1.u_double) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_double = (parent1.u_double) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	otherwise_error \
}


#define ex_constant_inherit_binary_numerical(this, parent1, operation, parent2) \
switch (parent1.kind) { \
	case EX_C_BYTE: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_byte = (parent1.u_byte) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_short = (parent1.u_byte) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_byte) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_byte) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_short = (parent1.u_byte) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_int = (parent1.u_byte) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_byte) operation (parent2.u_uint); break; \
		case EX_C_FLOAT: this.u_float = (parent1.u_byte) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_float = (parent1.u_byte) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_SHORT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_short = (parent1.u_short) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_short = (parent1.u_short) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_short) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_short) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_short = (parent1.u_short) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_int = (parent1.u_short) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_short) operation (parent2.u_uint); break; \
		case EX_C_FLOAT: this.u_float = (parent1.u_short) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_float = (parent1.u_short) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_INT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_int = (parent1.u_int) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_int = (parent1.u_int) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_int) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_int) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_int = (parent1.u_int) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_int = (parent1.u_int) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_int) operation (parent2.u_uint); break; \
		case EX_C_FLOAT: this.u_double = (parent1.u_int) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_double = (parent1.u_int) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_LONG: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_long = (parent1.u_long) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_long = (parent1.u_long) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_long = (parent1.u_long) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_long) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_long = (parent1.u_long) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_long = (parent1.u_long) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_long) operation (parent2.u_uint); break; \
		otherwise_error \
	} \
	case EX_C_UBYTE: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_short = (parent1.u_ubyte) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_short = (parent1.u_ubyte) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_short = (parent1.u_ubyte) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_short = (parent1.u_ubyte) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_ubyte = (parent1.u_ubyte) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_ushort = (parent1.u_ubyte) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_uint = (parent1.u_ubyte) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_ubyte) operation (parent2.u_ulong); break; \
		case EX_C_FLOAT: this.u_float = (parent1.u_ubyte) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_float = (parent1.u_ubyte) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_USHORT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_int = (parent1.u_ushort) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_int = (parent1.u_ushort) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_ushort) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_int = (parent1.u_ushort) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_ushort = (parent1.u_ushort) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_ushort = (parent1.u_ushort) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_uint = (parent1.u_ushort) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_ushort) operation (parent2.u_ulong); break; \
		case EX_C_FLOAT: this.u_float = (parent1.u_ushort) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_float = (parent1.u_ushort) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_UINT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_long = (parent1.u_uint) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_long = (parent1.u_uint) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_long = (parent1.u_uint) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_uint) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_uint = (parent1.u_uint) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_uint = (parent1.u_uint) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_uint = (parent1.u_uint) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_uint) operation (parent2.u_ulong); break; \
		case EX_C_FLOAT: this.u_double = (parent1.u_uint) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_double = (parent1.u_uint) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_ULONG: switch (parent2.kind) { \
		case EX_C_UBYTE: this.u_ulong = (parent1.u_ulong) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_ulong = (parent1.u_ulong) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_ulong = (parent1.u_ulong) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_ulong) operation (parent2.u_ulong); break; \
		otherwise_error \
	} \
	case EX_C_FLOAT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_float = (parent1.u_float) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_float = (parent1.u_float) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_double = (parent1.u_float) operation (parent2.u_int); break; \
		case EX_C_UBYTE: this.u_float = (parent1.u_float) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_float = (parent1.u_float) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_double = (parent1.u_float) operation (parent2.u_uint); break; \
		case EX_C_FLOAT: this.u_float = (parent1.u_float) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_double = (parent1.u_float) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	case EX_C_DOUBLE: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_double = (parent1.u_double) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_double = (parent1.u_double) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_double = (parent1.u_double) operation (parent2.u_int); break; \
		case EX_C_UBYTE: this.u_double = (parent1.u_double) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_double = (parent1.u_double) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_double = (parent1.u_double) operation (parent2.u_uint); break; \
		case EX_C_FLOAT: this.u_double = (parent1.u_double) operation (parent2.u_float); break; \
		case EX_C_DOUBLE: this.u_double = (parent1.u_double) operation (parent2.u_double); break; \
		otherwise_error \
	} \
	otherwise_error \
}


#define ex_constant_inherit_binary_integer(this, parent1, operation, parent2) \
switch (parent1.kind) { \
	case EX_C_BYTE: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_byte = (parent1.u_byte) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_short = (parent1.u_byte) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_byte) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_byte) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_short = (parent1.u_byte) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_int = (parent1.u_byte) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_byte) operation (parent2.u_uint); break; \
		otherwise_error \
	} \
	case EX_C_SHORT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_short = (parent1.u_short) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_short = (parent1.u_short) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_short) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_short) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_short = (parent1.u_short) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_int = (parent1.u_short) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_short) operation (parent2.u_uint); break; \
		otherwise_error \
	} \
	case EX_C_INT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_int = (parent1.u_int) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_int = (parent1.u_int) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_int) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_int) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_int = (parent1.u_int) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_int = (parent1.u_int) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_int) operation (parent2.u_uint); break; \
		otherwise_error \
	} \
	case EX_C_LONG: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_long = (parent1.u_long) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_long = (parent1.u_long) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_long = (parent1.u_long) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_long) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_long = (parent1.u_long) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_long = (parent1.u_long) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_long = (parent1.u_long) operation (parent2.u_uint); break; \
		otherwise_error \
	} \
	case EX_C_UBYTE: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_short = (parent1.u_ubyte) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_short = (parent1.u_ubyte) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_short = (parent1.u_ubyte) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_short = (parent1.u_ubyte) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_ubyte = (parent1.u_ubyte) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_ushort = (parent1.u_ubyte) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_uint = (parent1.u_ubyte) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_ubyte) operation (parent2.u_ulong); break; \
		otherwise_error \
	} \
	case EX_C_USHORT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_int = (parent1.u_ushort) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_int = (parent1.u_ushort) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_int = (parent1.u_ushort) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_int = (parent1.u_ushort) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_ushort = (parent1.u_ushort) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_ushort = (parent1.u_ushort) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_uint = (parent1.u_ushort) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_ushort) operation (parent2.u_ulong); break; \
		otherwise_error \
	} \
	case EX_C_UINT: switch (parent2.kind) { \
		case EX_C_BYTE: this.u_long = (parent1.u_uint) operation (parent2.u_byte); break; \
		case EX_C_SHORT: this.u_long = (parent1.u_uint) operation (parent2.u_short); break; \
		case EX_C_INT: this.u_long = (parent1.u_uint) operation (parent2.u_int); break; \
		case EX_C_LONG: this.u_long = (parent1.u_uint) operation (parent2.u_long); break; \
		case EX_C_UBYTE: this.u_uint = (parent1.u_uint) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_uint = (parent1.u_uint) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_uint = (parent1.u_uint) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_uint) operation (parent2.u_ulong); break; \
		otherwise_error \
	} \
	case EX_C_ULONG: switch (parent2.kind) { \
		case EX_C_UBYTE: this.u_ulong = (parent1.u_ulong) operation (parent2.u_ubyte); break; \
		case EX_C_USHORT: this.u_ulong = (parent1.u_ulong) operation (parent2.u_ushort); break; \
		case EX_C_UINT: this.u_ulong = (parent1.u_ulong) operation (parent2.u_uint); break; \
		case EX_C_ULONG: this.u_ulong = (parent1.u_ulong) operation (parent2.u_ulong); break; \
		otherwise_error \
	} \
	otherwise_error \
}


#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_CONSTANT_BINARY */