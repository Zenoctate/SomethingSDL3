#ifndef STATICS
#define STATICS

#include "Vector.h"

typedef struct {
    Vector2D pos;
    Vector2D square_hitbox_cornerpos;
    Vector2D hitbox_dimensions;
} Statics;

#endif