#ifndef GAMEPLAY
#define GAMEPLAY

#include "Control.h"
#include "Vector.h"
#include "defs.h"

typedef struct {
    Vector2D pos;
    Vector2D vel;
    double mass;

    Vector2D square_hitbox_cornerpos;       // Square hitbox
    Vector2D hitbox_dimensions;
} Entity;

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

typedef Entity StaticObj;

// Ticking
void Entity_Tick(Entity *entity, double delta_time);
void Character_Tick(Character *character, double delta_time);
void Bullet_Tick(Bullet *bullet, double delta_time);

// Rendering
void Character_Draw(Character *character,  Vector2D *camera);
void Bullet_Draw(Bullet *bullet,  Vector2D *camera);

// Between ENTITIES
bool Check_Collision(Entity *e1, Entity *e2);
bool Translational_Collision(Entity *to, Entity *from);

// Spawning
Character *Spawn_Character();
Bullet *Spawn_Bullet();
StaticObj *Spawn_StaticObj();

// Despawn
bool Despawn_Character(Character *character);
bool Despawn_Bullet(Bullet *bullet);
bool Despawn_StaticObj(StaticObj *staticobj);

void Fire_Bullet(Bullet **bullet, Character *by_character, double time_alive);

#endif