#ifndef PROPS_H
#define PROPS_H

#include "clay.h"

typedef struct SharedProps {
    Clay_FloatingElementConfig floating;
    unsigned int level;
} SharedProps;

#endif // PROPS_H