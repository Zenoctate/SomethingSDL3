#include "Entity.h"

void Entity_Tick(Entity *entity, double delta_time) {
    entity->pos.x += entity->vel.x * delta_time;
    entity->pos.y += entity->vel.y * delta_time;
}

bool Entity_Draw(SDL_Renderer *renderer, Entity *entity, Vector2D *camera) {
    if(entity->type == PLAYER) {
        SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0xff, 0xff);
    } else {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xff, 0xff);
    }

    SDL_FRect rect;
    rect.x = entity->pos.x - camera->x + (INIT_WIDTH / 2) - 9.5;
    rect.y = -entity->pos.y + camera->y + (INIT_HEIGHT / 2) - 9.5;
    rect.w = 20;
    rect.h = 20;
    SDL_RenderFillRect(renderer, &rect);
}