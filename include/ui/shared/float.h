#ifndef FLOAT_H
#define FLOAT_H

#include "clay.h"

typedef enum FloatType {
    FLOAT_TYPE_NONE,
    FLOAT_TYPE_PARENT,
    FLOAT_TYPE_GLOBAL
} FloatType;

typedef struct {
    FloatType type;
} FloatProps;

void* applyFloatBehavior(FloatProps floatProps);

#endif // FLOAT_H