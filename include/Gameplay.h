#ifndef GAMEPLAY
#define GAMEPLAY

#include <SDL3/SDL.h>
#include "Entity.h"
#include "Control.h"

typedef struct {
    Entity entity_struct;
    double time_alive;               // Max time it can exist in seconds
    Entity* spawned_by;
} Bullet;

void Create_Bullet(Bullet *bullet, Entity *entity, double time_alive);
void Bullet_Tick(Bullet *bullet, double delta_time);
void Bullet_Draw(SDL_Renderer *renderer, Bullet *bullet,  Vector2D *camera);

void Player_Input_Handle(Player_Input *pi);

#endif