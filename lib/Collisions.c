#include "Collisions.h"

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