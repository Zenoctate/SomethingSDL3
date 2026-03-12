#include "Gameplay.h"

void Character_Tick(Character *character, double delta_time) {
    if(character->health > 0 || 1) {
        Entity_Tick(&character->entity_struct, delta_time);
        character->fire_cooldown -= delta_time;
    }
}

void Character_Draw(SDL_Renderer *renderer, Character *character,  Vector2D *camera) {
    SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0xff, 0xff);
    SDL_FRect rect;
    rect.x = character->entity_struct.pos.x - camera->x + (INIT_WIDTH / 2) - 9.5;
    rect.y = -character->entity_struct.pos.y + camera->y + (INIT_HEIGHT / 2) - 9.5;
    rect.w = 20;
    rect.h = 20;
    SDL_RenderFillRect(renderer, &rect);
}

void Bullet_Tick(Bullet *bullet, double delta_time) {
    if(bullet->time_alive > 0) {
        Entity_Tick(&bullet->entity_struct, delta_time);
        bullet->time_alive -= delta_time;
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

void Fire_Bullet(Bullet *bullet, Character *by_character, double time_alive) {
    bullet->entity_struct.vel = by_character->entity_struct.vel;
    bullet->entity_struct.pos = by_character->entity_struct.pos;
    bullet->time_alive = time_alive;
    bullet->spawned_by = by_character;
}
