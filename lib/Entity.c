#include "Entity.h"

void Entity_Tick(Entity *entity, double delta_time) {
    entity->pos.x += entity->vel.x * delta_time;
    entity->pos.y += entity->vel.y * delta_time;
}

// Unefficient
bool Check_Collision(Entity *e1, Entity *e2) {
    double e1x1 = e1->square_hitbox_cornerpos.x + e1->pos.x;
    double e1x2 = e1->square_hitbox_cornerpos.x + e1->pos.x + e1->hitbox_dimensions.x;
    
    double e1y1 = e1->square_hitbox_cornerpos.y + e1->pos.y;
    double e1y2 = e1->square_hitbox_cornerpos.y + e1->pos.y + e1->hitbox_dimensions.y;

    double e2x1 = e2->square_hitbox_cornerpos.x + e2->pos.x;
    double e2x2 = e2->square_hitbox_cornerpos.x + e2->pos.x + e2->hitbox_dimensions.x;

    double e2y1 = e2->square_hitbox_cornerpos.y + e2->pos.y;
    double e2y2 = e2->square_hitbox_cornerpos.y + e2->pos.y + e2->hitbox_dimensions.y;

    if((e1x1 - e2x1 <= 0 && e1x1 - e2x2 >= 0 || e1x1 - e2x1 >= 0 && e1x1 - e2x2 <= 0
    || e1x2 - e2x1 <= 0 && e1x2 - e2x2 >= 0 || e1x2 - e2x1 >= 0 && e1x2 - e2x2 <= 0)
    && (e1y1 - e2y1 <= 0 && e1y1 - e2y2 >= 0 || e1y1 - e2y1 >= 0 && e1y1 - e2y2 <= 0
    || e1y2 - e2y1 <= 0 && e1y2 - e2y2 >= 0 || e1y2 - e2y1 >= 0 && e1y2 - e2y2 <= 0)) {
        return true;
    }

    return false;
}

bool Bounce_Collision(Entity *to, Entity *from, double factor) {
    
    if(SDL_fabs(to->pos.y - from->pos.y) <= 20 && SDL_fabs(to->pos.x - from->pos.x) <= 20) {
        double angle = SDL_atan2(to->pos.y - from->pos.y, to->pos.x - from->pos.x);
        if(angle >= DEGTORAD * -45 && angle <= DEGTORAD * 45) {
            to->vel.x = SDL_fabs(to->vel.x) * factor;
            to->pos.x = from->pos.x + (from->square_hitbox_cornerpos.x + from->hitbox_dimensions.x
                + to->square_hitbox_cornerpos.x + to->hitbox_dimensions.x);
        } else if(angle > DEGTORAD * 45 && angle < DEGTORAD * 135) {
            to->vel.y = SDL_fabs(to->vel.y) * factor;
            to->pos.y = from->pos.y + (from->square_hitbox_cornerpos.y + from->hitbox_dimensions.y
                + to->square_hitbox_cornerpos.y + to->hitbox_dimensions.y);
        } else if(angle >= DEGTORAD * 135 || angle <= DEGTORAD * -135) {
            to->vel.x = -SDL_fabs(to->vel.x) * factor;
            to->pos.x = from->pos.x - (from->square_hitbox_cornerpos.x + from->hitbox_dimensions.x
                + to->square_hitbox_cornerpos.x + to->hitbox_dimensions.x);
        } else if(angle > DEGTORAD * -135 && angle < DEGTORAD * -45) {
            to->vel.y = -SDL_fabs(to->vel.y) * factor;
            to->pos.y = from->pos.y - (from->square_hitbox_cornerpos.y + from->hitbox_dimensions.y
                + to->square_hitbox_cornerpos.y + to->hitbox_dimensions.y);
        }
        
        return true;
    }
    return false;
}