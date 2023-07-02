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

#define _ex_constant_upcast(resultant, resultant_upper, lower2, upper, lower1, this, parent1, operation, parent2) \
case EX_C_##upper:																\
	this.u_##resultant = (parent1.u_##lower1) operation (parent2.u_##lower2); 	\
	this.kind = EX_C_##resultant_upper;											\
	break;



/** NUMERICAL OPERATIONS **/
#define _ex_constant_intconvertcase(upper, lower, this, parent1, operation, parent2) 			\
case EX_C_##upper: switch (parent2.kind) {														\
		_ex_constant_upcast(int, INT, byte, BYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(int, INT, short, SHORT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(int, INT, int, INT, lower, this, parent1, operation, parent2); 			\
		_ex_constant_upcast(long, LONG, long, LONG, lower, this, parent1, operation, parent2); 		\
		\
		_ex_constant_upcast(uint, UINT, ubyte, UBYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(uint, UINT, ushort, USHORT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(uint, UINT, uint, UINT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(ulong, ULONG, ulong, ULONG, lower, this, parent1, operation, parent2);		\
		\
		_ex_constant_upcast(float, FLOAT, float, FLOAT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(double, DOUBLE, double, DOUBLE, lower, this, parent1, operation, parent2); 	\
		otherwise_error \
} break;

#define _ex_constant_uintconvertcase(upper, lower, this, parent1, operation, parent2) 			\
case EX_C_##upper: switch (parent2.kind) {														\
		_ex_constant_upcast(uint, UINT, byte, BYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(uint, UINT, short, SHORT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(uint, UINT, int, INT, lower, this, parent1, operation, parent2); 			\
		_ex_constant_upcast(ulong, ULONG, long, LONG, lower, this, parent1, operation, parent2); 		\
		\
		_ex_constant_upcast(uint, UINT, ubyte, UBYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(uint, UINT, ushort, USHORT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(uint, UINT, uint, UINT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(ulong, ULONG, ulong, ULONG, lower, this, parent1, operation, parent2);		\
		\
		_ex_constant_upcast(float, FLOAT, float, FLOAT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(double, DOUBLE, double, DOUBLE, lower, this, parent1, operation, parent2); 	\
		otherwise_error \
} break;

#define _ex_constant_floatconvertcase(upper, lower, this, parent1, operation, parent2) 			\
case EX_C_##upper: switch (parent2.kind) {														\
		_ex_constant_upcast(float, FLOAT, byte, BYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(float, FLOAT, short, SHORT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(float, FLOAT, int, INT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(float, FLOAT, long, LONG, lower, this, parent1, operation, parent2); 		\
		\
		_ex_constant_upcast(float, FLOAT, ubyte, UBYTE, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(float, FLOAT, ushort, USHORT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(float, FLOAT, uint, UINT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(float, FLOAT, ulong, ULONG, lower, this, parent1, operation, parent2);		\
		\
		_ex_constant_upcast(float, FLOAT, float, FLOAT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(double, DOUBLE, double, DOUBLE, lower, this, parent1, operation, parent2); 	\
		otherwise_error \
} break;

#define _ex_constant_doubleconvertcase(upper, lower, this, parent1, operation, parent2) 			\
case EX_C_##upper: switch (parent2.kind) {														\
		_ex_constant_upcast(double, DOUBLE, byte, BYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(double, DOUBLE, short, SHORT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(double, DOUBLE, int, INT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(double, DOUBLE, long, LONG, lower, this, parent1, operation, parent2); 		\
		\
		_ex_constant_upcast(double, DOUBLE, ubyte, UBYTE, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(double, DOUBLE, ushort, USHORT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(double, DOUBLE, uint, UINT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(double, DOUBLE, ulong, ULONG, lower, this, parent1, operation, parent2);	\
		\
		_ex_constant_upcast(double, DOUBLE, float, FLOAT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(double, DOUBLE, double, DOUBLE, lower, this, parent1, operation, parent2); 	\
		otherwise_error \
} break;

#define ex_constant_inherit_binary_numerical(this, parent1, operation, parent2) \
switch (parent1.kind) { \
	_ex_constant_intconvertcase(BYTE, byte, this, parent1, operation, parent2) 		\
	_ex_constant_intconvertcase(SHORT, short, this, parent1, operation, parent2) 	\
	_ex_constant_intconvertcase(INT, int, this, parent1, operation, parent2) 		\
	_ex_constant_intconvertcase(LONG, long, this, parent1, operation, parent2) 		\
	\
	_ex_constant_uintconvertcase(UBYTE, ubyte, this, parent1, operation, parent2)	\
	_ex_constant_uintconvertcase(USHORT, ushort, this, parent1, operation, parent2)	\
	_ex_constant_uintconvertcase(UINT, uint, this, parent1, operation, parent2)		\
	_ex_constant_uintconvertcase(ULONG, ulong, this, parent1, operation, parent2)	\
	\
	_ex_constant_floatconvertcase(FLOAT, float, this, parent1, operation, parent2)    \
	_ex_constant_doubleconvertcase(DOUBLE, double, this, parent1, operation, parent2) \
	\
	otherwise_error \
}





/** INTEGER-ONLY OPERATIONS **/

#define _ex_constant_intconvertcase_integer(upper, lower, this, parent1, operation, parent2) 			\
case EX_C_##upper: switch (parent2.kind) {														\
		_ex_constant_upcast(int, INT, byte, BYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(int, INT, short, SHORT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(int, INT, int, INT, lower, this, parent1, operation, parent2); 			\
		_ex_constant_upcast(long, LONG, long, LONG, lower, this, parent1, operation, parent2); 		\
		\
		_ex_constant_upcast(uint, UINT, ubyte, UBYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(uint, UINT, ushort, USHORT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(uint, UINT, uint, UINT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(ulong, ULONG, ulong, ULONG, lower, this, parent1, operation, parent2);		\
		\
		otherwise_error \
} break;

#define _ex_constant_uintconvertcase_integer(upper, lower, this, parent1, operation, parent2) 			\
case EX_C_##upper: switch (parent2.kind) {														\
		_ex_constant_upcast(uint, UINT, byte, BYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(uint, UINT, short, SHORT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(uint, UINT, int, INT, lower, this, parent1, operation, parent2); 			\
		_ex_constant_upcast(ulong, ULONG, long, LONG, lower, this, parent1, operation, parent2); 		\
		\
		_ex_constant_upcast(uint, UINT, ubyte, UBYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(uint, UINT, ushort, USHORT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_upcast(uint, UINT, uint, UINT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_upcast(ulong, ULONG, ulong, ULONG, lower, this, parent1, operation, parent2);		\
		\
		otherwise_error \
} break;

#define ex_constant_inherit_binary_integer(this, parent1, operation, parent2) \
switch (parent1.kind) { \
	case EX_C_BOOLEAN: switch (parent2.kind) { \
		case EX_C_BOOLEAN: this.u_boolean = (parent1.u_boolean) operation (parent2.u_boolean); break; \
		otherwise_error \
	} break; \
	\
	_ex_constant_intconvertcase_integer(BYTE, byte, this, parent1, operation, parent2) 		\
	_ex_constant_intconvertcase_integer(SHORT, short, this, parent1, operation, parent2) 	\
	_ex_constant_intconvertcase_integer(INT, int, this, parent1, operation, parent2) 		\
	_ex_constant_intconvertcase_integer(LONG, long, this, parent1, operation, parent2) 		\
	\
	_ex_constant_uintconvertcase_integer(UBYTE, ubyte, this, parent1, operation, parent2)	\
	_ex_constant_uintconvertcase_integer(USHORT, ushort, this, parent1, operation, parent2)	\
	_ex_constant_uintconvertcase_integer(UINT, uint, this, parent1, operation, parent2)		\
	_ex_constant_uintconvertcase_integer(ULONG, ulong, this, parent1, operation, parent2)	\
	\
	otherwise_error \
}





/** COMPARISON OPERATIONS **/

#define _ex_constant_booleancast(lower2, upper, lower1, this, parent1, operation, parent2) \
case EX_C_##upper: \
	this.u_boolean = (parent1.u_##lower1) operation (parent2.u_##lower2); 	\
	this.kind = EX_C_BOOLEAN;												\
	break;

#define _ex_constant_booleanconvertcase(upper, lower, this, parent1, operation, parent2) 		\
case EX_C_##upper: switch (parent2.kind) {														\
		_ex_constant_booleancast(byte, BYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_booleancast(short, SHORT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_booleancast(int, INT, lower, this, parent1, operation, parent2); 			\
		_ex_constant_booleancast(long, LONG, lower, this, parent1, operation, parent2); 		\
		\
		_ex_constant_booleancast(ubyte, UBYTE, lower, this, parent1, operation, parent2); 		\
		_ex_constant_booleancast(ushort, USHORT, lower, this, parent1, operation, parent2); 	\
		_ex_constant_booleancast(uint, UINT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_booleancast(ulong, ULONG, lower, this, parent1, operation, parent2);		\
		\
		_ex_constant_booleancast(float, FLOAT, lower, this, parent1, operation, parent2); 		\
		_ex_constant_booleancast(double, DOUBLE, lower, this, parent1, operation, parent2); 	\
		otherwise_error \
} break;

#define ex_constant_inherit_binary_number_comparison(this, parent1, operation, parent2) \
switch (parent1.kind) { \
	_ex_constant_booleanconvertcase(BYTE, byte, this, parent1, operation, parent2) 		\
	_ex_constant_booleanconvertcase(SHORT, short, this, parent1, operation, parent2) 	\
	_ex_constant_booleanconvertcase(INT, int, this, parent1, operation, parent2) 		\
	_ex_constant_booleanconvertcase(LONG, long, this, parent1, operation, parent2) 		\
	\
	_ex_constant_booleanconvertcase(UBYTE, ubyte, this, parent1, operation, parent2)	\
	_ex_constant_booleanconvertcase(USHORT, ushort, this, parent1, operation, parent2)	\
	_ex_constant_booleanconvertcase(UINT, uint, this, parent1, operation, parent2)		\
	_ex_constant_booleanconvertcase(ULONG, ulong, this, parent1, operation, parent2)	\
	\
	_ex_constant_booleanconvertcase(FLOAT, float, this, parent1, operation, parent2)   	\
	_ex_constant_booleanconvertcase(DOUBLE, double, this, parent1, operation, parent2) 	\
	\
	otherwise_error \
}

#define ex_constant_inherit_binary_comparison(this, parent1, operation, parent2) \
switch (parent1.kind) { \
	case EX_C_BOOLEAN: switch (parent2.kind) { \
		case EX_C_BOOLEAN: this.u_boolean = (parent1.u_boolean) operation (parent2.u_boolean); break; \
		otherwise_error \
	} break; \
	\
	_ex_constant_booleanconvertcase(BYTE, byte, this, parent1, operation, parent2) 		\
	_ex_constant_booleanconvertcase(SHORT, short, this, parent1, operation, parent2) 	\
	_ex_constant_booleanconvertcase(INT, int, this, parent1, operation, parent2) 		\
	_ex_constant_booleanconvertcase(LONG, long, this, parent1, operation, parent2) 		\
	\
	_ex_constant_booleanconvertcase(UBYTE, ubyte, this, parent1, operation, parent2)	\
	_ex_constant_booleanconvertcase(USHORT, ushort, this, parent1, operation, parent2)	\
	_ex_constant_booleanconvertcase(UINT, uint, this, parent1, operation, parent2)		\
	_ex_constant_booleanconvertcase(ULONG, ulong, this, parent1, operation, parent2)	\
	\
	_ex_constant_booleanconvertcase(FLOAT, float, this, parent1, operation, parent2)   \
	_ex_constant_booleanconvertcase(DOUBLE, double, this, parent1, operation, parent2) \
	\
	otherwise_error \
}

#define ex_constant_inherit_binary_logic(this, parent1, operation, parent2) \
switch (parent1.kind) { \
	case EX_C_BOOLEAN: switch (parent2.kind) { \
		case EX_C_BOOLEAN: this.u_boolean = (parent1.u_boolean) operation (parent2.u_boolean); break; \
		otherwise_error \
	} break; \
	otherwise_error \
}



#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_CONSTANT_BINARY */