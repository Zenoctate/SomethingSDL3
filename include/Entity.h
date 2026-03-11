#ifndef ENTITY
#define ENTITY

#include <SDL3/SDL.h>
#include <stdbool.h>
#include "Vector.h"
#include "defs.h"

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

    Vector2D square_hitbox_cornerpos;       // Square hitbox
    Vector2D hitbox_dimensions;
} Entity;

void Entity_Tick(Entity *entity, double delta_time);
bool Entity_Draw(SDL_Renderer *renderer, Entity *entity, Vector2D *camera);

#endif