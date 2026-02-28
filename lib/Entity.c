#include "Entity.h"

void updateEntity(Entity *entity) {
    entity->pos.x += entity->vel.x;
    entity->pos.y += entity->vel.y;
}