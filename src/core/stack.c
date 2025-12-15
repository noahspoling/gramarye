static char rcsid[] = "$Id$";
#include <stddef.h>
#include "assert.h"
#include "mem.h"
#include "stack.h"
#define T Stack_T
struct T {
    int count;
    struct elem {
        void *x;
        struct elem *link;
    } *head;
};
extern T Stack_new(void) {
    T stack;
    NEW(stack);
    stack->count = 0;
    stack->head = NULL;
    return stack;
}
extern int Stack_empty(T stack) {
    assert(stack);
    return stack->count == 0;
}
extern void Stack_push(T stack, void *x) {
    struct elem *t;
    assert(stack);
    NEW(t);
    t->x = x;
    t->link = stack->head;
    stack->head = t;
    stack->count++;
}
extern int *Stack_pop(T stack) {
    void *x;
    struct elem *t;
    assert(stack);
    assert(stack->count > 0);
    t = stack->head;
    stack->head = t->link;
    stack->count--;
    x = t->x;
    FREE(t);
    return x;
}
extern void Stack_free(T *stack) {
    struct elem *t, *u;
    assert(stack && *stack);
    for(t = (*stack)->head; t; t = u) {
        u = t->link;
        FREE(t);
    }
    FREE(*stack);
}