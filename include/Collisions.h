#ifndef COLLISIONS
#define COLLISIONS

#include "Entity.h"
#include "Statics.h"

// Between ENTITIES
bool Check_Collision(Entity *e1, Entity *e2);
bool Translational_Collision(Entity *e1, Entity *e2);

// Between ENTITY and STATIC
bool Check_Collision_Statics(Entity *e1, Statics *s1);
bool Translational_Collision_Statics(Entity *e1, Statics *s1);

#endif