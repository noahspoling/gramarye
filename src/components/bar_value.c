#include "bar_value.h"

#include "stddef.h" 
#include "stdbool.h"

#include "raylib.h"

#include "except.h"

const Except_T BarValue_Invalid_Error = { EXCEPT_BAR_VALUE_INVALID };

void update_bar_value(BarValue* bar, float change, bool isNegativeBar) {
    if (bar == NULL) {
        Except_raise(&BarValue_Invalid_Error, __FILE__, __LINE__);
    }

    if (isNegativeBar) {
        if (change < 0) {
            if (change + bar->value < -bar->maxValue) {
                bar->value = -bar->maxValue;
            } else {
                bar->value += change;   
            }
        } 
        if (change > 0) {
            if (change + bar->value > bar->maxValue) {
                bar->value = bar->maxValue;
            } else {
                bar->value += change;   
            }
        }
        else {
            TraceLog(LOG_WARNING, "Bar value change is zero; no update performed.");
        }
    } else {

    }
}
void set_bar_value(BarValue* bar, float newValue, bool isNegativeBar) {
    if (bar == NULL) {
        Except_raise(&BarValue_Invalid_Error, __FILE__, __LINE__);
    }

    if (isNegativeBar) {
        
    } else {

    }
}
void get_bar_value(const BarValue* bar, float* percentage);