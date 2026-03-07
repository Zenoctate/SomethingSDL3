#include "Vector.h"

void AddVector(Vector2D *to, Vector2D *from) {
    to->x += from->x;
    to->y += from->y;
}