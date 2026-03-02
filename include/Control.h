#ifndef CONTROL
#define CONTROL

#include <stdbool.h>

typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;
    bool fire;
} Player_Input;

#endif