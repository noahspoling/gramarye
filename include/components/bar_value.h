#ifndef COMPONENTS_BAR_VALUE_H
#define COMPONENTS_BAR_VALUE_H 

#include "stdbool.h"
#include "except.h"

#define BAR_VALUE_DEFAULT_MAX 100.0f
#define EXCEPT_BAR_VALUE_INVALID "BarValue: Invalid value"

typedef struct BarValue {
    float value;
    float maxValue;
} BarValue;

void update_bar_value(BarValue* bar, float change, bool isNegativeBar);
void set_bar_value(BarValue* bar, float newValue, bool isNegativeBar);
void get_bar_value(const BarValue* bar, float* percentage);

#endif //COMPONENTS_BAR_VALUE_H