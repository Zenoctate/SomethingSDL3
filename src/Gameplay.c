#include "Gameplay.h"

void Create_Bullet(Bullet *bullet, Entity *entity, double time_alive) {
    bullet->entity_struct.vel = entity->vel;
    bullet->entity_struct.pos = entity->pos;
    bullet->entity_struct.type = DAMAGE;
    bullet->time_alive = time_alive;
    bullet->spawned_by = entity;
}

void Bullet_Tick(Bullet *bullet, double delta_time) {
    if(bullet->time_alive > 0) {
        Entity_Tick(&bullet->entity_struct, delta_time);
        bullet->time_alive -= delta_time;
    } else if(bullet->time_alive <= 0) {
        bullet->time_alive = -1;
    }
}

void Bullet_Draw(SDL_Renderer *renderer, Bullet *bullet, Vector2D *camera) {
    SDL_SetRenderDrawColor(renderer, 0xff, 0x55, 0x55, 0xff);
    SDL_FRect rect;
    rect.x = bullet->entity_struct.pos.x - camera->x + (INIT_WIDTH / 2) - 9.5;
    rect.y = -bullet->entity_struct.pos.y + camera->y + (INIT_HEIGHT / 2) - 9.5;
    rect.w = 2;
    rect.h = 2;
    SDL_RenderFillRect(renderer, &rect);
}