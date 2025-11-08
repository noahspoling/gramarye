#ifndef THEME_H
#define THEME_H

#include "clay.h"

typedef struct Theme {
    Clay_Color backgroundColor;
    Clay_Color textColor;
    float borderRadius;
} Theme;

#endif // THEME_H