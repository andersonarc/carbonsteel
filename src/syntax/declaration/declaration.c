/**
 * @file declaration.c
 * @author andersonarc (e.andersonarc@gmail.com)
 * @version 0.1
 * @date 2023-07-16
 * 
 *  Functions for working with declarations
 */
#include "syntax/declaration/declaration.h"
#include "ast/type/check.h"
#include "misc/string.h"

    /* functions */
/**
 * Adds a generic implementation to a structure
 * 
 * @param this The structure
 * @param impl The list of types for the generic implementation
 * 
 * @return The index of the implementation
 */
index_t dc_structure_generic_add_impl(dc_structure* this, list(ast_type) impl) {
    /* check the size */
    if (impl.size != this->generics.size) {
        logfe("expected %zu generic implementations for type %s, got %zu",
            this->generics.size, this->name, impl.size);
    }

    /* check for duplicates */
    bool is_equal = false;
    for (size_t i = 0; i < this->_generic_impls.size; i++) {
        list(ast_type) impls = this->_generic_impls.data[i];
        is_equal = true;

        /* compare an implementation */
        for (size_t j = 0; j < impls.size; j++) {
            if (!ast_type_is_equal(&impls.data[j], &impl.data[j])) {
                is_equal = false;
            }
        }

        if (is_equal) return i;
    }

    /* add the generic implementation */
    arl_add(list(ast_type), this->_generic_impls, impl);
    return this->_generic_impls.size - 1;
}

/**
 * Applies a generic implementation to the structure's generics
 * for type checking or code generation purposes
 * 
 * @param this The structure
 * @param impl The list of types for the generic implementation
 */
void dc_structure_generic_apply_impl(dc_structure* this, list(ast_type) impl) {
    /* size check */
    if (impl.size != this->generics.size) {
        logfe("invalid generic implementation size for %s, expected %zu, got %zu", 
            this->name, this->generics.size, impl.size);
    }

    /* apply the implementation */
    for (size_t j = 0; j < this->generics.size; j++) {
        this->generics.data[j]->_impl = impl.data[j];
    }
}

/**
 * Returns the implementation of a generic type
 * 
 * @param this Pointer to the generic type
 * 
 * @return Pointer to an implementation of the generic type
 */
ast_type* dc_generic_get_impl(ast_type* this) {
    if (this->kind != AST_TYPE_GENERIC) {
        logfe("attempt to get an implementation of a non-generic type %s",
            ast_type_display_name(this));
    }

    ast_type* result = ast_type_clone(this->u_generic->_impl);

    /* ensure to copy the level list as well */
    for (size_t i = 0; i < this->level_list.size; i++) {
        arraylist_add(ast_type_level)(&result->level_list, this->level_list.data[i]);
    }

    return result;
}





/**
 * Pretty-prints a structure's contents to a string
 * 
 * @param[in] this The structure
 * 
 * @return The string, allocated by malloc
 */
char* dc_structure_contents_to_string(dc_structure* this) {
    /* the prefix */
    char* prefix = "type { ";
    size_t buffer_size = strlen(prefix);

    /* compute the members */
    list(dc_structure_member) members = this->member_list;
    char** member_names = allocate_array(char**, members.size);
    size_t* member_name_sizes = allocate_array(size_t, members.size);

    /* ... */
    for (int i = 0; i < members.size; i++) {
        char* internal_type = ast_type_display_name(&members.data[i].type);
        member_name_sizes[i] = strlen(internal_type);
        member_name_sizes[i] += 1;
        member_name_sizes[i] += strlen(members.data[i].name);
        if (i != members.size - 1) {
            member_name_sizes[i] += 2;
        }
        buffer_size += member_name_sizes[i];

        member_names[i] = allocate_array(char*, member_name_sizes[i] + 1);

        strncpy(member_names[i], internal_type, member_name_sizes[i]);
        member_name_sizes[i] = strlen(internal_type);
        free(internal_type);

        member_names[i][member_name_sizes[i]] = ' ';
        member_name_sizes[i] += 1;

        strcpy(member_names[i] + member_name_sizes[i], members.data[i].name);
        member_name_sizes[i] += strlen(members.data[i].name);

        if (i != members.size - 1) {
            member_names[i][member_name_sizes[i]] = ',';
            member_name_sizes[i] += 1;
            member_names[i][member_name_sizes[i]] = ' ';
            member_name_sizes[i] += 1;
        }

        member_names[i][member_name_sizes[i]] = 0;
    }



    /* the postfix */
    char* postfix = " };";
    buffer_size += strlen(postfix);

    /* allocate the buffer */
    char* buffer = allocate_array(char, buffer_size + 1);

    /* paste the prefix */
    strcpy(buffer, prefix);
    buffer_size = strlen(prefix);

    /* paste the members */
    for (int i = 0; i < members.size; i++) {
        strncpy(buffer + buffer_size, member_names[i], member_name_sizes[i]);
        buffer_size += member_name_sizes[i];
    }

    /* paste the postfix */
    strcpy(buffer + buffer_size, postfix);
    buffer_size += strlen(postfix);

    /* null terminator */
    buffer[buffer_size] = 0;

    return buffer;
}

/**
 * Pretty-prints a structure to show its display name,
 * or contents if it's anonymous
 * 
 * @param this The structure
 * @param index The generic impl index to use
 * 
 * @return Display name, allocated by a malloc,
 *          or the structure's actual name
 */
char* dc_structure_display_name(dc_structure* this, index_t impl_index) {
    char* name;
            
    /* find the actual name */
    if (this->name != NULL) {
        name = this->name;
    } else {
        /* handle anonymous structures */
        name = dc_structure_contents_to_string(this);
    }

    /* handle non-generic names */
    if (this->generics.size == 0) {
        return name;
    }

    /* else, the structure is generic so needs a mangled name */
    size_t name_size = strlen(name);
    size_t buffer_size = name_size;
    
    /* compute the generic names */
    char** impl_names = allocate_array(char*, this->generics.size);
    size_t* impl_sizes = allocate_array(size_t, this->generics.size);
    for (int i = 0; i < this->generics.size; i++) {
        impl_names[i] = this->generics.data[i]->name;
        impl_sizes[i] = strlen(this->generics.data[i]->name);
        buffer_size += impl_sizes[i] + strlen(", ");
    }



    /* allocate the buffer */
    char* buffer = allocate_array(char, buffer_size + 1);

    /* paste the name */
    strncpy(buffer, name, name_size);
    buffer_size = name_size;
    buffer[buffer_size] = '<';
    buffer_size++;

    /* paste the typenames */
    for (int i = 0; i < this->generics.size; i++) {
        strcpy(buffer + buffer_size, impl_names[i]);
        buffer_size += impl_sizes[i];

        strcpy(buffer + buffer_size, ", ");
        buffer_size += strlen(", ");
    }

    /* null terminator */
    buffer[buffer_size - 1] = '>';
    buffer[buffer_size] = 0;

    return buffer;
}

/**
 * Pretty-prints an enum to show its display name,
 * or contents if it's anonymous
 * 
 * @param this The enum
 * @param index The generic impl index to use
 * 
 * @return Display name, allocated by a malloc,
 *          or the enum's actual name
 */
char* dc_enum_display_name(dc_enum* this, index_t impl_index) {
    char* name;
            
    /* find the actual name */
    if (this->name != NULL) {
        name = this->name;
    } else {
        /* handle anonymous enums */
        name = "<anonymous enum>";
    }

    return name;
}

/**
 * Pretty-prints a function to show its display name,
 * or contents if it's anonymous
 * 
 * @param this The function
 * @param index The generic impl index to use
 * 
 * @return Display name, allocated by a malloc,
 *          or the function's actual name
 */
char* dc_function_display_name(dc_function* this, index_t impl_index) {
    char* name;
            
    /* find the actual name */
    if (this->name != NULL) {
        name = this->name;
    } else {
        /* handle anonymous functions */
        name = "<anonymous function>";
    }

    return name;
}





/**
 * Mangles a structure's name in case it has generics or is anonymous,
 * otherwise returns its original name
 * 
 * @param this The structure
 * @param index The generic impl index to use
 * 
 * @return Mangled name, allocated by a malloc,
 *          or the structure's actual name
 */
char* dc_structure_mangled_name(dc_structure* this, index_t impl_index) {
    char* name;
            
    /* find the actual name */
    if (this->name != NULL) {
        name = this->name;
    } else {
        /* handle anonymous structures */
        name = allocate_array(char, strlen(CST_ANON_STRUCT_PREFIX) + 20 + 1 /* max length of a 64 bit pointer */);
        strcpy(name, CST_ANON_STRUCT_PREFIX);
        sprintf(name + strlen(CST_ANON_STRUCT_PREFIX), "%zu", (size_t) this);
        name[strlen(CST_ANON_STRUCT_PREFIX) + 20] = 0;
    }

    /* handle non-generic names */
    if (this->generics.size == 0) {
        return name;
    }

    /* else, the structure is generic so needs a mangled name */
    size_t name_size = strlen(name);

    /* the prefix */
    char* prefix = CST_GENERIC_PREFIX;
    size_t prefix_size = strlen(prefix);
    
    /* compute the typenames */
    char** impl_names = allocate_array(char*, this->generics.size);
    size_t* impl_sizes = allocate_array(size_t, this->generics.size);
    list(ast_type) impls = this->_generic_impls.data[impl_index];
    for (int i = 0; i < impls.size; i++) {
        impl_names[i] = ast_type_mangled_name(&impls.data[i]);
        impl_sizes[i] = strlen(impl_names[i]);
        prefix_size += strlen(CST_GENERIC_START_PREFIX) + impl_sizes[i] + strlen(CST_GENERIC_END_PREFIX);
    }



    /* allocate the buffer */
    char* buffer = allocate_array(char, prefix_size + name_size + 1);

    /* paste the prefix */
    prefix_size = strlen(prefix);
    strncpy(buffer, prefix, prefix_size);

    /* paste the typenames */
    for (int i = 0; i < impls.size; i++) {
        strcpy(buffer + prefix_size, "__cst_startgeneric_");
        prefix_size += strlen("__cst_startgeneric_");

        strcpy(buffer + prefix_size, impl_names[i]);
        prefix_size += impl_sizes[i];

        strcpy(buffer + prefix_size, "__cst_endgeneric_");
        prefix_size += strlen("__cst_endgeneric_");
    }

    /* paste the name */
    strncpy(buffer + prefix_size, name, name_size);

    /* null terminator */
    buffer[prefix_size + name_size] = 0;

    return buffer;
}

/**
 * Mangles an enum's name in case it has generics or is anonymous,
 * otherwise returns its original name
 * 
 * @param this The enum
 * @param index The generic impl index to use
 * 
 * @return Mangled name, allocated by a malloc,
 *          or the enum's actual name
 */
char* dc_enum_mangled_name(dc_enum* this, index_t impl_index) {
    char* name;

    /* find the actual name */
    if (this->name != NULL) {
        name = this->name;
    } else {
        name = allocate_array(char, sizeof(CST_ANON_ENUM_PREFIX) + 20 /* max length of a 64 bit pointer */);
        strcpy(name, CST_ANON_ENUM_PREFIX);
        sprintf(name + sizeof(CST_ANON_ENUM_PREFIX) - 1, "%zu", (size_t) this);
        name[sizeof(CST_ANON_ENUM_PREFIX) + 19] = 0;
    }

    return name;
}

/**
 * Mangles a function's name in case it has generics or is anonymous,
 * otherwise returns its original name
 * 
 * @param this The function
 * @param index The generic impl index to use
 * 
 * @return Mangled name, allocated by a malloc,
 *          or the function's actual name
 */
char* dc_function_mangled_name(dc_function* this, index_t impl_index) {
    char* name;

    /* find the actual name */
    if (this->name != NULL) {
        name = this->name;
    } else {
        name = allocate_array(char, sizeof(CST_ANON_FUNC_PREFIX) + 20 /* max length of a 64 bit pointer */);
        strcpy(name, CST_ANON_FUNC_PREFIX);
        sprintf(name + sizeof(CST_ANON_FUNC_PREFIX) - 1, "%zu", (size_t) this);
        name[sizeof(CST_ANON_FUNC_PREFIX) + 19] = 0;
    }
    
    return name;
}