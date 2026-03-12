#ifndef GAMEPLAY
#define GAMEPLAY

#include <SDL3/SDL.h>
#include "Entity.h"
#include "Control.h"

typedef enum {
    PLAYER,
    OPPONENT,
    ALLY
} Character_Type;

typedef struct {
    Entity entity_struct;
    Character_Type type;

    double fire_cooldown;            // Time left it can fire bullet again in seconds
    int health;
} Character;

typedef struct {
    Entity entity_struct;
    double time_alive;               // Max time it can exist in seconds
    Character* spawned_by;
} Bullet;

void Character_Tick(Character *character, double delta_time);
void Character_Draw(SDL_Renderer *renderer, Character *character,  Vector2D *camera);

void Bullet_Tick(Bullet *bullet, double delta_time);
void Bullet_Draw(SDL_Renderer *renderer, Bullet *bullet,  Vector2D *camera);

void Fire_Bullet(Bullet *bullet, Character *by_character, double time_alive);

#endif