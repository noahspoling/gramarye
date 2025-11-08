#ifndef BUTTON_H
#define BUTTON_H

#include "clay.h"
#include "theme.h"
#include "props.h"

typedef struct ButtonProps {
    Clay_ElementId id;
    SharedProps shared;
    Theme theme;
    Clay_StringSlice text;
    void (*onClick)(void);
} ButtonProps;

void Button_Render(ButtonProps props);

#endif // BUTTON_H