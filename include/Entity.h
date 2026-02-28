#ifndef ENTITY
#define ENTITY

#include "Vector.h"

typedef struct {
    int health;
    Vector2D pos;
    Vector2D vel;
} Entity;

void updateEntity(Entity *entity);

#endif