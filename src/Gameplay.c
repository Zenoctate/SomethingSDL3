#include "Gameplay.h"
#include "lib/API.h"
#include <stdlib.h>

void Entity_Tick(Entity *entity, double delta_time) {
    entity->pos.x += entity->vel.x * delta_time;
    entity->pos.y += entity->vel.y * delta_time;
}

void Character_Tick(Character *character, double delta_time) {
    if(character->health > 0 || 1) {
        Entity_Tick(&character->entity_struct, delta_time);
        character->fire_cooldown -= delta_time;
    }
}

void Character_Draw(Character *character,  Vector2D *camera) {
    Set_Color(0x00ffffff);
    Draw_Rect(
        character->entity_struct.pos.x - camera->x + (INIT_WIDTH / 2) - 9.5,
        -character->entity_struct.pos.y + camera->y + (INIT_HEIGHT / 2) - 9.5,
        20,
        20
    );
}

void Bullet_Tick(Bullet *bullet, double delta_time) {
    Entity_Tick(&bullet->entity_struct, delta_time);
    bullet->time_alive -= delta_time;
}

void Bullet_Draw(Bullet *bullet, Vector2D *camera) {
    Set_Color(0xffff55ff);
    Draw_Rect(
        bullet->entity_struct.pos.x - camera->x + (INIT_WIDTH / 2) - 9.5,
        -bullet->entity_struct.pos.y + camera->y + (INIT_HEIGHT / 2) - 9.5,
        2,
        2
    );
}

void Fire_Bullet(Bullet **bullet, Character *by_character, double time_alive) {
    (*bullet) = Spawn_Bullet();
    (*bullet)->entity_struct.pos = by_character->entity_struct.pos;
    (*bullet)->entity_struct.vel = by_character->entity_struct.vel;
    (*bullet)->time_alive = time_alive;
    (*bullet)->spawned_by = by_character;
}

bool Check_Collision(Entity *e1, Entity *e2) {
    double e1x1 = e1->square_hitbox_cornerpos.x + e1->pos.x;
    double e1x2 = e1->square_hitbox_cornerpos.x + e1->pos.x + e1->hitbox_dimensions.x;
    
    double e1y1 = e1->square_hitbox_cornerpos.y + e1->pos.y;
    double e1y2 = e1->square_hitbox_cornerpos.y + e1->pos.y + e1->hitbox_dimensions.y;

    double e2x1 = e2->square_hitbox_cornerpos.x + e2->pos.x;
    double e2x2 = e2->square_hitbox_cornerpos.x + e2->pos.x + e2->hitbox_dimensions.x;

    double e2y1 = e2->square_hitbox_cornerpos.y + e2->pos.y;
    double e2y2 = e2->square_hitbox_cornerpos.y + e2->pos.y + e2->hitbox_dimensions.y;

    if(((e1x1 <= e2x1 && e1x2 >= e2x1) || (e1x1 <= e2x2 && e1x2 >= e2x2))
    && ((e1y1 <= e2y1 && e1y2 >= e2y1) || (e1y1 <= e2y2 && e1y2 >= e2y2))) {
        return true;
    }

    return false;
}

// Elastic Collision Only
bool Translational_Collision(Entity *to, Entity *from) {
    if(Check_Collision(to, from)) {
        double angle = SDL_atan2(to->pos.y - from->pos.y, to->pos.x - from->pos.x);
    
        double tmpx = from->square_hitbox_cornerpos.x + from->hitbox_dimensions.x
            + to->square_hitbox_cornerpos.x + to->hitbox_dimensions.x;
        double tmpy = from->square_hitbox_cornerpos.y + from->hitbox_dimensions.y
            + to->square_hitbox_cornerpos.y + to->hitbox_dimensions.y;

        double mass_sum = to->mass + from->mass;
        double diff_to_from_mass = to->mass - from->mass;

        #define VERTICAL 1
        #define HORIZONTAL 2
        int collision_axis = 0;

        if(angle >= DEGTORAD * -45 && angle <= DEGTORAD * 45) {
            to->pos.x = from->pos.x + (tmpx + 0.05);
            from->pos.x = to->pos.x - (tmpx + 0.1);
            collision_axis = HORIZONTAL;
        } else if(angle > DEGTORAD * 45 && angle < DEGTORAD * 135) {
            to->pos.y = from->pos.y + (tmpy + 0.05);
            from->pos.y = to->pos.y - (tmpy + 0.1);
           collision_axis = VERTICAL;
        } else if(angle >= DEGTORAD * 135 || angle <= DEGTORAD * -135) {
            to->pos.x = from->pos.x - (tmpx + 0.05);
            from->pos.x = to->pos.x + (tmpx + 0.1);
            collision_axis = HORIZONTAL;
        } else if(angle > DEGTORAD * -135 && angle < DEGTORAD * -45) {
            to->pos.y = from->pos.y - (tmpy + 0.05);
            from->pos.y = to->pos.y + (tmpy + 0.1);
            collision_axis = VERTICAL;
        }

        if(collision_axis == HORIZONTAL) {
            double initial_to_velx = to->vel.x;
            to->vel.x = ((to->vel.x * diff_to_from_mass) + (2 * from->mass * from->vel.x)) / mass_sum;
            from->vel.x = ((from->vel.x * -diff_to_from_mass) + (2 * to->mass * initial_to_velx)) / mass_sum;
        } else if(collision_axis == VERTICAL) {
            double initial_to_vely = to->vel.y;
            to->vel.y = ((to->vel.y * diff_to_from_mass) + (2 * from->mass * from->vel.y)) / mass_sum;
            from->vel.y = ((from->vel.y * -diff_to_from_mass) + (2 * to->mass * initial_to_vely)) / mass_sum;
        }

        return true;
    }
    return false;
}

Character *Spawn_Character() {
    return (Character *)malloc(sizeof(Character));
}

Bullet *Spawn_Bullet() {
    return (Bullet *)malloc(sizeof(Bullet));
}

StaticObj *Spawn_StaticObj() {
    return (StaticObj *)malloc(sizeof(StaticObj));
}

bool Despawn_Character(Character *character) {
    free(character);
}

bool Despawn_Bullet(Bullet *bullet) {
    free(bullet);
}

bool Despawn_StaticObj(StaticObj *staticobj) {
    free(staticobj);
}

