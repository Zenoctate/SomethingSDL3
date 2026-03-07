#ifndef ENTITY
#define ENTITY

#include <SDL3/SDL.h>
#include <stdbool.h>
#include "Vector.h"

#define PI 3.14159265
#define DEGTORAD (PI / 180)

typedef enum {
    PLAYER,
    OPPONENT,
    HEAL,
    DAMAGE
} Entity_Type;

typedef struct {
    int health;
    Entity_Type type;

    Vector2D pos;
    Vector2D vel;
    double mass;

    Vector2D square_hitbox_cornerpos;    // Square hitbox
    Vector2D hitbox_dimensions;
} Entity;

void Entity_Tick(Entity *entity, double delta_time);

#endif