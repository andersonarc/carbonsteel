/**
 * @file declaration.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2023-07-13
 * 
 *  Functions for working with C-native declarations
 */
    /* includes */
#include "language/native/declaration.h"
#include "ast/type/type.h"
#include "language/parser.h"
#include "language/native/parser.h"
#include "ast/type/check.h"

    /* functions */
/**
 * Translates a C-native declaration specifier list
 * into a Carbonsteel type, listing all
 * non-anonymous declarations into the provided arraylist
 * 
 * @param context The parser context
 * @param in The C-native declaration specifiers
 * @param declarations The arraylist to append any inner declarations into
 * 
 * @return The type
 */
ast_type cst_native_declspecs_translate(se_context* context, c_declaration_specifiers in, arraylist(declaration)* declarations) {
    // ignore: storage class specifiers
    // ignore: function specifiers
    // ignore: type qualifiers

    ast_type type;
    bool has_been_set = false;
    bool is_unsigned = false;
    for (int i = in.type_specs.size - 1; i >= 0; i-- /* reverse iteration*/) {
        switch (in.type_specs.data[i].kind) {
            case C_TS_SIGNED:
                if (is_unsigned) {
                    logw("signedness redefined for a type, possible error");
                }
                is_unsigned = false;
                break;

            case C_TS_UNSIGNED:
                if (is_unsigned) {
                    logw("double unsigned type, possible error");
                }
                is_unsigned = true;
                break;
            
            case C_TS_COMPLEX:
            case C_TS_IMAGINARY:
            case C_TS_ATOMIC:
                logw("ignoring unsupported C type specifier");
                break;

            case C_TS_TYPE:
                ast_type* current = &in.type_specs.data[i].u_type;
                logd("walk: <%s>", ast_type_to_string(current));

                /* handle types like "long int" and "long long int", etc */
                if (has_been_set) {
                    logd("complex type! processing...");

                    if (ast_type_is_pp(current)) {
                        index_t index = ast_type_primitive_get_index(current->u_primitive);
                        switch (index) {
                            case PRIMITIVE_INDEX_INT:
                            case PRIMITIVE_INDEX_LONG:
                                logd("discarding an <int/long> flag");
                                current = NULL;
                                break;

                            case PRIMITIVE_INDEX_DOUBLE:
                                logd("overriding the type with a <double>");
                                logw("long doubles are not supported yet, resolving as double");
                                break;

                            default:
                                logfe("this type is not allowed in complex sequences!");
                                break;
                        }
                    }
                }
    
                /* no more type info needed */
                if (current == NULL) break;
                
                /* assign the type */
                type = *current;
                has_been_set = true;

                /* handle unsigned types */
                if (is_unsigned == true) {
                    if (ast_type_is_pp_integer(&type)) {
                        type.u_primitive = primitive_signed_to_unsigned(type.u_primitive);
                    } else {
                        logfe("only primitive types can be unsigned");
                    }
                }
                
                /* handle inner structure and enum declarations */
                declaration dc;
                dc.u__any = NULL;
                dc.is_native = true;
                switch (type.kind) {
                    case AST_TYPE_STRUCTURE:
                        dc_structure* st = type.u_structure;
                        if (st->name != NULL) {
                            declaration* dc_ex = ast_declaration_lookup(&context->ast, st->name);
                            if (dc_ex != NULL) {
                                logd("[native-ignore] struct %s", st->name);
                            } else {
                                st->name = cst_native_struct_name(st->name);
                                dc.name = st->name;
                                logd("[native] struct %s", dc.name);
                                dc.is_full = st->is_full;
                                dc.kind = DC_STRUCTURE;
                                dc.token = TOKEN_STRUCTURE_NAME;
                                dc.ctoken = CTOKEN_STRUCTURE_NAME;
                                dc.u_structure = st;
                            }
                        }
                        break;

                    case AST_TYPE_ENUM:
                        dc_enum* en = type.u_enum;
                        if (en->name != NULL) {
                            declaration* dc_ex = ast_declaration_lookup(&context->ast, en->name);
                            if (dc_ex != NULL) {
                                logd("[native-ignore] enum %s", en->name);
                            } else {
                                en->name = cst_native_enum_name(en->name);
                                dc.name = en->name;
                                logd("[native] enum %s", dc.name);
                                dc.is_full = en->is_full;
                                dc.kind = DC_ENUM;
                                dc.token = TOKEN_ENUM_NAME;
                                dc.ctoken = CTOKEN_ENUM_NAME;
                                dc.u_enum = en;
                            }
                        }
                        break;

                    case AST_TYPE_FUNCTION:
                    case AST_TYPE_PRIMITIVE:
                        break;
                }
                if (dc.u__any != NULL) {
                    arraylist_add(declaration)(declarations, dc);
                }
                break;

            otherwise_error
        }
    }

    if (!has_been_set) {
        logfe("no type found in sequence!");
    }
    return type;
}

/**
 * Translates a C-native declaration into a list of
 * Carbonsteel declarations
 * 
 * @param context The parser context
 * @param in The C-native declaration
 * 
 * @return A list of processed declarations 
 */
arraylist(declaration) cst_native_declaration_translate(se_context* context, c_declaration in) {
    arraylist(declaration) result;
    arl_init(declaration, result);

    /* process storage flags */
    bool is_typedef = false;
    for (int i = 0; i < in.specs.storage_specs.size; i++) {
        if (in.specs.storage_specs.data[i] == C_SCLS_TYPEDEF) {
            is_typedef = true;
        }
    }

    /* process inner declarations */
    for (int i = 0; i < in.declarations.size; i++) {
        arraylist(declaration) current_result = cst_native_declaration_translate(context, in.declarations.data[i]);
        for (int j = 0; j < current_result.size; j++) {
            arraylist_add(declaration)(&result, current_result.data[j]);
        }
        arraylist_free(declaration)(&current_result);
    }

    /* extract the raw type */
    ast_type raw_type = cst_native_declspecs_translate(context, in.specs, &result);

    /* process the declarators */
    for (int i = 0; i < in.declarators.size; i++) {
        c_declarator this = in.declarators.data[i];

        /* create an alias for the type */
        dc_alias* al = allocate(dc_alias);
        al->is_full = true;
        al->name = this.name;
        ast_type_clone_to(&al->target, raw_type);

        /* append the levels set in the declarator */
        for (int j = 0; j < this.level_list.size; j++) {
            arraylist_add(ast_type_level)(&al->target.level_list, this.level_list.data[j]);
        }

        /* path char* as char[] */
        if (ast_type_is_single_pointer(&al->target)
            && al->target.kind == AST_TYPE_PRIMITIVE
            && ast_type_primitive_get_index(al->target.u_primitive) == PRIMITIVE_INDEX_CHAR) {
                logd("patching char* as char[]");
                al->target.level_list.data[0].kind = AT_LEVEL_ARRAY;
                al->target.level_list.data[0].u_array_size = 0;
            }

        declaration dc;
        if (this.is_function) {
            /* wrap it into a function */
            dc_function* fn = allocate(dc_function);
            fn->is_extern = true;
            fn->is_full = true;
            fn->name = this.name;
            fn->parameters = this.u_parameters;
            fn->return_type = al->target;
            free(al);

            if (is_typedef) {
                /* then wrap it into another alias */
                al = allocate(dc_alias);
                al->is_full = true;
                al->name = this.name;
                ast_type_init(&al->target, AST_TYPE_PRIMITIVE, &primitive_list.data[PRIMITIVE_INDEX_VOID]);
                ast_type_pointer_wrap(&al->target);
                logw("function types are not supported yet!");

                /* and wrap it into a declaration */
                dc.is_full = true;
                dc.is_native = true;
                dc.name = al->name;
                dc.kind = DC_ALIAS;
                dc.token = TOKEN_ALIAS_NAME;
                dc.ctoken = CTOKEN_ALIAS_NAME;
                dc.u_alias = al;

                logd("[native] %s = <%s(...)>", ast_type_to_string(&fn->return_type), this.name);
            } else {
                /* or wrap it into a declaration straightaway*/
                dc.is_full = true;
                dc.is_native = true;
                dc.name = fn->name;
                dc.kind = DC_FUNCTION;
                dc.token = TOKEN_FUNCTION_NAME;
                dc.ctoken = CTOKEN_FUNCTION_NAME;
                dc.u_function = fn;

                logd("[native] %s %s(...)", ast_type_to_string(&fn->return_type), this.name);
            }
        } else {
            /* wrap it into a declaration */
            dc.is_full = true;
            dc.is_native = true;
            dc.name = al->name;
            dc.kind = DC_ALIAS;
            dc.token = TOKEN_ALIAS_NAME;
            dc.ctoken = CTOKEN_ALIAS_NAME;
            dc.u_alias = al;
            
            logd("[native] %s = <%s>", this.name, ast_type_to_string(&al->target));
        }

        /* add it to the result list */
        arraylist_add(declaration)(&result, dc);
    }

    /* special case - abstract declaration */
    if (in.declarators.size == 0) {
        /* create an alias for the type */
        dc_alias* al = allocate(dc_alias);
        al->is_full = true;
        al->name = NULL;
        ast_type_clone_to(&al->target, raw_type);

        /* wrap it into a declaration */
        declaration dc;
        dc.is_full = true;
        dc.is_native = true;
        dc.name = al->name;
        dc.kind = DC_ALIAS;
        dc.token = TOKEN_ALIAS_NAME;
        dc.ctoken = CTOKEN_ALIAS_NAME;
        dc.u_alias = al;
        
        logd("[native] <abstract> <%s>;", ast_type_to_string(&al->target));

        /* add it to the result list */
        arraylist_add(declaration)(&result, dc);
    }

    return result;
}