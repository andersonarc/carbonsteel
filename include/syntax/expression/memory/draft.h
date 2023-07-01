/**
 * Each expression has a MemoryUnit reference in its properties
 * apart from its type and ConstantValue 
 * 
 * - both of them do not track pointer/array levels
 * - MemoryUnits are monolithic and are not affected by such operations 
 * 
 * MemoryUnit {
 *      Kind: [HEAP, STACK, STATIC, FOREIGN],
 *      HeapUnit?: {
 *          ast_type type;
 *      }
 * }
 */
