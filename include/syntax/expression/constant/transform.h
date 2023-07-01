/**
 * @file transform.h
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2021-11-17
 * 
 *  Constant expression transformation macro
 * 
 *  Transformations are used to mutate a constant
 *  expression value by applying a generic operation to it.
 * 
 *  There are 3 types of transformations:
 *      - ex_constant_*_assign_*
 *          Operation result type: Constant expression
 *          Operation applied to: This
 *          Result receiver: This
 *      
 *      - ex_constant_*_apply_*
 *          Operation result type: Constant expression VALUE
 *          Operation applied to: This
 *          Result receiver: This
 * 
 *      - ex_constant_*_inherit_*
 *          Operation result type: Constant expression VALUE
 *          Operation applied to: Parent
 *          Result receiver: This
 * 
 *  Apart from types, transformations have a scope and an operation type. 
 *  The scope can be numerical, integer, unsigned, signed or floating-point. 
 *  The operation type is either postfix, prefix or both.
 */
    /* header guard */
#ifndef CARBONSTEEL_SYNTAX_EXPRESSION_CONSTANT_TRANSFORM_H
#define CARBONSTEEL_SYNTAX_EXPRESSION_CONSTANT_TRANSFORM_H

    /* includes */
#include "syntax/expression/constant/binary.h" /* binary expression transformations */

    /* defines */
/**
 * Internal macro which applies a generic
 * numerical transformation to the value
 * of the constant expression
 * 
 * @param[in] dest    Initialized constant expression
 * @param[in] src     Source constant expression
 * @param[in] prefix  Prefix of the operation
 * @param[in] postfix Postfix of the operation
 * 
 * @param[in] case Generic transformation which must conform to a template of
 *                  (dest, src, KIND, kind, prefix, postfix)
 */
#define _ex_constant_generic_transform(dest, src, prefix, postfix, case)    \
    switch (src.kind) {                                                     \
       case(dest, src, BYTE,   byte,   prefix, postfix)                     \
       case(dest, src, SHORT,  short,  prefix, postfix)                     \
       case(dest, src, INT,    int,    prefix, postfix)                     \
       case(dest, src, LONG,   long,   prefix, postfix)                     \
       case(dest, src, UBYTE,  ubyte,  prefix, postfix)                     \
       case(dest, src, USHORT, ushort, prefix, postfix)                     \
       case(dest, src, UINT,   uint,   prefix, postfix)                     \
       case(dest, src, ULONG,  ulong,  prefix, postfix)                     \
       case(dest, src, FLOAT,  float,  prefix, postfix)                     \
       case(dest, src, DOUBLE, double, prefix, postfix)                     \
       otherwise_error                                                      \
    }

#define _ex_constant_generic_unsigned_transform(dest, src, prefix, postfix, case)   \
    switch (src.kind) {                                                             \
       case(dest, src, UBYTE,  ubyte,  prefix, postfix)                             \
       case(dest, src, USHORT, ushort, prefix, postfix)                             \
       case(dest, src, UINT,   uint,   prefix, postfix)                             \
       case(dest, src, ULONG,  ulong,  prefix, postfix)                             \
       otherwise_error                                                              \
    }


/**
 * Assignment transformation which replaces the
 * source constant expression with the 
 * resulting constant expression
 * after executing the operation
 * 
 * @param[in] this    The constant expression
 * @param[in] prefix  Prefix operation to apply to the number
 * @param[in] postfix Postfix operation to apply to the number
 */
#define _ex_constant_assign_case(this, _, upper, lower, prefix, postfix) \
    case EX_C_##upper: this = (prefix this.u_##lower postfix); break;

    /* numerical scope */
#define ex_constant_assign_prefix(this, prefix)   ex_constant_assign(this, prefix,  );
#define ex_constant_assign_postfix(this, postfix) ex_constant_assign(this, , postfix);
#define ex_constant_assign(this, prefix, postfix) \
    _ex_constant_generic_transform(this, this, prefix, postfix, _ex_constant_assign_case)

    /* unsigned scope */
#define ex_constant_unsigned_assign_prefix(this, prefix)   ex_constant_unsigned_assign(this, prefix,  );
#define ex_constant_unsigned_assign_postfix(this, postfix) ex_constant_unsigned_assign(this, , postfix);
#define ex_constant_unsigned_assign(this, prefix, postfix) \
    _ex_constant_generic_unsigned_transform(this, this, prefix, postfix, _ex_constant_assign_case)


/**
 * Macro for assigning a mutated value of a constant expression
 * to another constant expression
 * 
 * @param[in] this The constant expression
 * @param[in] parent The parent constant expression
 * @param[in] prefix Prefix operation to apply to the number
 * @param[in] postfix Postfix operation to apply to the number
 */
#define _ex_constant_inherit_case(this, parent, upper, lower, prefix, postfix) \
    case EX_C_##upper: this.u_##lower = (prefix parent.u_##lower postfix); break;

    /* numerical scope */
#define ex_constant_inherit_prefix(this, parent,  prefix)  ex_constant_inherit(this, parent, prefix,  );
#define ex_constant_inherit_postfix(this, parent, postfix) ex_constant_inherit(this, parent, , postfix);
#define ex_constant_inherit(this, parent, prefix, postfix) \
     _ex_constant_generic_transform(this, parent, prefix, postfix, _ex_constant_inherit_case);


/**
 * Application transformation which mutates the value
 * of a constant expression in-place.
 * 
 * It is virually self-inheritance, so the actual
 * implementation is just an alias for ex_constant_inherit.
 * 
 * @param[in] this    The constant expression
 * @param[in] prefix  Prefix operation to apply to the number
 * @param[in] postfix Postfix operation to apply to the number
 */
    /* numerical scope */
#define ex_constant_apply_prefix(this,  prefix)  ex_constant_inherit(this, this, prefix,        );
#define ex_constant_apply_postfix(this, postfix) ex_constant_inherit(this, this, ,       postfix);
#define ex_constant_apply(this, prefix, postfix) ex_constant_inherit(this, this, prefix, postfix);

#endif /* CARBONSTEEL_SYNTAX_EXPRESSION_CONSTANT_TRANSFORM_H */