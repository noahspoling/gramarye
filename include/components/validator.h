#ifndef VALIDATOR_H
#define VALIDATOR_H

typedef enum Operation {
    OPERATION_IS_EQUAL_TO = 0,
    OPERATION_IS_NOT_EQUAL_TO = 1,
    OPERATION_IS_GREATER_THAN = 2,
    OPERATION_IS_GREATER_THAN_OR_EQUAL_TO = 3,
    OPERATION_IS_LESS_THAN = 4,
    OPERATION_IS_LESS_THAN_OR_EQUAL_TO = 5,
    OPERATION_IS_BETWEEN = 6,
    OPERATION_IS_NOT_BETWEEN = 7,
} Operation;

typedef enum ValueType {
    VALUE_TYPE_INT = 0,
    VALUE_TYPE_FLOAT = 1,
    VALUE_TYPE_STRING = 2,
    VALUE_TYPE_BOOL = 3,
    VALUE_TYPE_ARRAY = 4,
    VALUE_TYPE_OBJECT = 5,
} ValueType;

typedef struct Value {
    void* value;
    size_t size;
} Value;

#endif // VALIDATOR_H