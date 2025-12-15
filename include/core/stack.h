/* $Id$ */
#ifndef STACK_INCLUDED
#define STACK_INCLUDED
#define T Stack_T
typedef struct T *T;
/// Create a new stack
/// @return Pointer to new stack
extern T        Stack_new    (void);
/// Check if the stack is empty
/// @param stack Pointer to stack
/// @return 1 if empty, 0 otherwise
extern int      Stack_empty  (T stack);
/// Push an element onto the stack.
/// @param stack Pointer to stack.
/// @param x Pointer to element to push.
extern void     Stack_push   (T stack, void *x);
/// Pop an element from the stack.
/// @param stack Pointer to stack.
/// @return Pointer to popped element.
extern int      *Stack_pop   (T stack);
/// Free the stack.
/// @param stack Pointer to stack pointer.
extern void     Stack_free   (T *stack);
#undef T
#endif // STACK_INCLUDED