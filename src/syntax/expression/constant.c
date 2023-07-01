/**
 * @file constant.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-10-22
 * 
 *  Constant expression initialization, printing, 
 *  cloning and type checking function implementations
 */
    /* includes */
#include "syntax/expression/constant/core.h" /* this */

#include <ctype.h> /* isdigit and isalpha */

    /* global variables */
/**
 * Union size table for constant expressions
 * with kind unknown at compile time
 */
size_t ex_constant_size_table[] = {
    ex_constant_size_static(EX_C_BOOLEAN),
    ex_constant_size_static(EX_C_BYTE),  ex_constant_size_static(EX_C_SHORT),
    ex_constant_size_static(EX_C_INT),   ex_constant_size_static(EX_C_LONG),
    ex_constant_size_static(EX_C_UBYTE), ex_constant_size_static(EX_C_USHORT),
    ex_constant_size_static(EX_C_UINT),  ex_constant_size_static(EX_C_ULONG),
    ex_constant_size_static(EX_C_FLOAT), ex_constant_size_static(EX_C_DOUBLE),

    ex_constant_size_static(EX_C_ARRAY), ex_constant_size_static(EX_C_STRUCTURE)
};

//todo small_size_t type for optimization

    /* functions */
/**
 * Initializes a dynamic constant expression
 * that has no value
 * 
 * @param[out] this The constant expression
 */
void ex_constant_dynamic(ex_constant* this) {
    this->kind = EX_C_DYNAMIC;
    this->origin = NULL;
}

/**
 * Initializes a constant expression
 * from an array of bytes while interpreting
 * each N bytes as a constant expression of
 * specified kind until the end of the array
 * 
 * @param[out] this   The constant expression
 * @param[in]  kind   Kind of the array elements
 * @param[in]  buffer Byte array to be interpreted as a list of constant expressions
 * @param[in]  size   Size of the array
 */
void ex_constant_from_array(ex_constant* this, ex_constant_kind kind, char* buffer, size_t size) {
    this->kind = EX_C_ARRAY;
    this->origin = NULL;
    li_init(ex_constant, this->u_array, size);
    iterate_array(i, size) {
        this->kind = kind;
        memcpy(&this->_union_offset, &this->u_array.data[i], ex_constant_size_table[kind]);
    }
}

/**
 * Copies a constant expression value
 * and references the source in the origin field
 * 
 * @param[out] this The constant expression
 * @param[in]  origin The constant expression to be cloned
 */
void ex_constant_reference(ex_constant* this, ex_constant* origin) {
    this->kind = origin->kind;
    memcpy(&this->_union_offset, &origin->_union_offset, ex_constant_size_table[origin->kind]);
    this->origin = origin;
}

/**
 * Copies a constant expression value
 * *without* referencing the source
 * 
 * @param[out] this The constant expression
 * @param[in]  source The constant expression to be cloned
 */
void ex_constant_clone(ex_constant* this, ex_constant* source) {
    this->kind = source->kind;
    memcpy(&this->_union_offset, &source->_union_offset, ex_constant_size_table[source->kind]);
    this->origin = NULL;
}

/**
 * Prints a constant expression value
 * to the standard output stream
 * 
 * @param[in] this The constant expression
 */
void ex_constant_print(ex_constant* this) {
    switch (this->kind) {
        case EX_C_BOOLEAN:
            puts(this->u_boolean ? "true" : "false");
            break;

        case EX_C_BYTE:
            if (isalpha(this->u_byte) || isdigit(this->u_byte)) {
                printf("<byte> %d (%c)", this->u_byte, this->u_byte);
            } else {
                printf("<byte> %d", this->u_byte);
            }
            break;

        case EX_C_SHORT:
            printf("<short> %d", this->u_short);
            break;

        case EX_C_INT:
            printf("<int> %d", this->u_int);
            break;

        case EX_C_LONG:
            printf("<long> %ld", this->u_long);
            break;

        case EX_C_UBYTE:
            if (isalpha(this->u_byte) || isdigit(this->u_byte)) {
                printf("<byte> %u (%c)", this->u_byte, this->u_byte);
            } else {
                printf("<byte> %u", this->u_byte);
            }
            break;

        case EX_C_USHORT:
            printf("<short> %u", this->u_short);
            break;

        case EX_C_UINT:
            printf("<int> %u", this->u_int);
            break;

        case EX_C_ULONG:
            printf("<long> %lu", this->u_long);
            break;
        
        case EX_C_FLOAT:
            printf("<float> %g", this->u_float);
            break;

        case EX_C_DOUBLE:
            printf("<double> %g", this->u_double);
            break;

        case EX_C_ARRAY:
            printf("<array> [");

            if (this->u_array.size > 1) {
                size_t without_last = this->u_array.size - 1;
                iterate_array(i, without_last) {
                    ex_constant_print(&this->u_array.data[i]);
                    puts(", ");
                }
            }

            ex_constant_print(&arraylist_last(this->u_array));
            putc(']', stdout);
            break;

        case EX_C_STRUCTURE:
            //todo
            break;

        case EX_C_DYNAMIC:
            puts("<dynamic>");
            break;

        otherwise_error
    }
}

/**
 * Constant expression type check functions
 * 
 *  ex_constant_is_number   - integer or a floating-point number
 *  ex_constant_is_integer  - integer number
 *  ex_constant_is_floating - floating-point number
 *  ex_constant_is_signed   - signed integer
 *  ex_constant_is_unsigned - unsigned integer
 * 
 * @param[in] this The constant expression
 */
#define _ex_constant_in_range(name, from, to)                        \
    bool ex_constant_is_##name(ex_constant* this) {                  \
        return this->kind >= EX_C_##from && this->kind <= EX_C_##to; \
    }

_ex_constant_in_range(number,   BYTE,  DOUBLE);
_ex_constant_in_range(integer,  BYTE,  ULONG);
_ex_constant_in_range(floating, FLOAT, DOUBLE);
_ex_constant_in_range(signed,   BYTE,  LONG);
_ex_constant_in_range(unsigned, UBYTE, ULONG);

#undef _ex_constant_in_range