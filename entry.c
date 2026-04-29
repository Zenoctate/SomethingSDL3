#include <stdio.h>

#include "defs.h"
#include "Control.h"
#include "Vector.h"
#include "Gameplay.h"
#include "lib/API.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

extern SDL_Window *main_window;
extern SDL_Renderer *main_renderer;
extern SDL_Event main_event;

Vector2D camera = {0};
Player_Input player_input;

int num_characters = 0;
Character *characters[ENTITY_LIMIT];
#define PLAYER_CHARACTER characters[0]           // First entity is always the PLAYER_CHARACTER

int num_bullets = 0;
Bullet *bullets[BULLET_LIMIT];

bool is_running = false;

double delta_time = 0;                      // Time between game ticks
double time_passed = 0;                     // Seconds passed since running

void ToOnScreenCoordinate(Vector2D *out, Vector2D *ingame, Vector2D *camera);
bool LimitPos(Vector2D *vec, int Xmin, int Ymin, int Xmax, int Ymax);

bool Init_Game();
bool Init_FirstTick();
void Game_EHandle();                        // Event handling (Inputs mostly)
bool Game_Tick();                           // Game Updates
bool Game_Draw();
void Free_Game();
void Destroy_Game();

int main(int argc, char* argv[]) {
    Initialize_Game();
    Init_FirstTick();

    is_running = true;
    while(is_running) {
        unsigned long counter = SDL_GetPerformanceCounter();

        Set_Color(0x000000ff);
        SDL_RenderClear(main_renderer);
        
        Game_EHandle();
        Game_Tick();
        Game_Draw();
        
        SDL_RenderPresent(main_renderer);

        // ################ For Consistent Frame Rate ################
        delta_time = (SDL_GetPerformanceCounter() - counter) / (double)SDL_GetPerformanceFrequency();
        if(DO_FRAME_DELTA && delta_time < DO_FRAME_DELTA) {
            SDL_Delay((int)((DO_FRAME_DELTA - delta_time + 0.0005) * 1000));
            delta_time = DO_FRAME_DELTA;
            time_passed += delta_time;
        }
        // ###########################################################
    }

    Free_Game();
    Destroy_Game();
    return 0;
}

bool Init_FirstTick() {
    // PLAYER_CHARACTER->entity_struct.pos.x = 0;
    // PLAYER_CHARACTER->entity_struct.pos.y = 0;
    // PLAYER_CHARACTER->entity_struct.vel.y = 0;
    // PLAYER_CHARACTER->entity_struct.vel.y = 0;

    for(int i = 0; i < 5; i++) {
        characters[i] = Spawn_Character();
        characters[i]->entity_struct.pos.x = i * 25;
        characters[i]->entity_struct.pos.y = i * 30;
        characters[i]->health = 100;
        characters[i]->entity_struct.mass = 10;

        characters[i]->entity_struct.square_hitbox_cornerpos.x = -10;
        characters[i]->entity_struct.square_hitbox_cornerpos.y = -10;
        characters[i]->entity_struct.hitbox_dimensions.x = 20;
        characters[i]->entity_struct.hitbox_dimensions.y = 20;
    }
    num_characters = 5;

    return true;
}

void Game_EHandle() {
    while(Poll_Events()) {
        switch(Get_EventType()) {
            case TYPE_QUITEVENT: {
                is_running = false;
            } break;
            case TYPE_KEYDOWNEVENT:
            case TYPE_KEYUPEVENT: {
                Key_MyEvent ke = Get_KeyEvent();
                switch(ke.keycode) {
                    case SDL_SCANCODE_ESCAPE: {
                        is_running = false;
                    } break;
                    case SDL_SCANCODE_W: {
                        player_input.up = ke.state;
                    } break;
                    case SDL_SCANCODE_A: {
                        player_input.left = ke.state;
                    } break;
                    case SDL_SCANCODE_S: {
                        player_input.down = ke.state;
                    } break;
                    case SDL_SCANCODE_D: {
                        player_input.right = ke.state;
                    } break;
                    case SDL_SCANCODE_SPACE: {
                        player_input.fire = ke.state;
                    } break;
                }
            } break;
        }
    }
}

bool Game_Tick() {
    // PLAYER #######################################
    float diag_1byroot2 = 1;
    if((player_input.up || player_input.down) && (player_input.right || player_input.left)) {
        diag_1byroot2 = BYSQRT2;
    }
    
    PLAYER_CHARACTER->entity_struct.vel.x += (player_input.right - player_input.left) * 5.0f * diag_1byroot2;
    PLAYER_CHARACTER->entity_struct.vel.y += (player_input.up - player_input.down) * 5.0f * diag_1byroot2;

    // if(player_input.fire && PLAYER_CHARACTER.fire_cooldown <= 0) {
    //     PLAYER_CHARACTER.fire_cooldown = FIRE_COOLDOWN_TIME;
    //     Fire_Bullet(&bullets[0], PLAYER_CHARACTER, 2.0);
    //     num_bullets++;
    // }
    // // ##############################################
    
    // Character_Tick(characters[0], delta_time);
    for(int i = 0; i < num_characters; i++) {
        Character_Tick(characters[i], delta_time);

        if(characters[i]->entity_struct.pos.x > (PLAYABLE_WIDTH / 2) - 10) {
            characters[i]->entity_struct.vel.x = -SDL_fabs(characters[i]->entity_struct.vel.x) * 0.8;
        } else if(characters[i]->entity_struct.pos.x < (-PLAYABLE_WIDTH / 2) + 10) {
            characters[i]->entity_struct.vel.x = SDL_fabs(characters[i]->entity_struct.vel.x) * 0.8;
        }
        
        if(characters[i]->entity_struct.pos.y > (PLAYABLE_HEIGHT / 2) - 10) {
            characters[i]->entity_struct.vel.y = -SDL_fabs(characters[i]->entity_struct.vel.y) * 0.8;
        } else if(characters[i]->entity_struct.pos.y < (-PLAYABLE_HEIGHT / 2) + 10) {
            characters[i]->entity_struct.vel.y = SDL_fabs(characters[i]->entity_struct.vel.y) * 0.8;
        }
        LimitPos(&characters[i]->entity_struct.pos, (-PLAYABLE_WIDTH / 2) + 10, (-PLAYABLE_HEIGHT / 2) + 10, (PLAYABLE_WIDTH / 2) - 10, (PLAYABLE_HEIGHT / 2) - 10);

        for(int j = i + 1; j < num_characters; j++) {
            Translational_Collision(&characters[i]->entity_struct, &characters[j]->entity_struct);
        }
    }

    // for(int i = 0; i < num_bullets; i++) {
    //     if(LimitPos(&bullets[i].entity_struct.pos, (-PLAYABLE_WIDTH / 2) + 10, (-PLAYABLE_HEIGHT / 2) - 10, (PLAYABLE_WIDTH / 2) - 10, (PLAYABLE_HEIGHT / 2) - 10)) {
    //         bullets[i].time_alive = -1;
    //     } else if(bullets[i].time_alive > 0) {
    //         Bullet_Tick(&bullets[i], delta_time);
    //     }
    // }

    LimitPos(&camera, PLAYER_CHARACTER->entity_struct.pos.x - 100, PLAYER_CHARACTER->entity_struct.pos.y - 100, PLAYER_CHARACTER->entity_struct.pos.x + 100, PLAYER_CHARACTER->entity_struct.pos.y + 100);

    return true;
}

bool Game_Draw() {
    Vector2D onScreen, tmpvec;
    SDL_FRect rect;

    // // BULLETS ######################################
    // for(int i = 0; i < 1; i++) {
    //     if(bullets[i].time_alive > 0) {
    //         Bullet_Draw(main_renderer, bullets[i], &camera);
    //     }
    // }
    // // ##############################################
    
    // // ENTITES ######################################
    for(int i = 0; i < num_characters; i++) {
        Character_Draw(characters[i], &camera);
    }
    // // ##############################################

    // PLAY BORDER ##################################
    tmpvec.x = (-PLAYABLE_WIDTH / 2);
    tmpvec.y = (PLAYABLE_HEIGHT / 2);
    ToOnScreenCoordinate(&onScreen, &tmpvec, &camera);
    rect.x = onScreen.x + 0.5;
    rect.y = onScreen.y + 0.5;
    rect.w = PLAYABLE_WIDTH;
    rect.h = PLAYABLE_HEIGHT;
    Set_Color(0xffffffff);
    SDL_RenderRect(main_renderer, &rect);
    // ##############################################

    return true;
}

void Free_Game() {
    for(int i = 0; i < num_characters; i++) {
        Despawn_Character(characters[i]);
        characters[i] = 0;
    }
}

void ToOnScreenCoordinate(Vector2D *out, Vector2D *ingame, Vector2D *camera) {
    out->x = ingame->x - camera->x + (INIT_WIDTH / 2);
    out->y = -ingame->y + camera->y + (INIT_HEIGHT / 2);
}

bool LimitPos(Vector2D *vec, int Xmin, int Ymin, int Xmax, int Ymax) {
    bool did_limit = false;
    if(vec->x > Xmax) {
        did_limit = true;
        vec->x = Xmax;
    } else if (vec->x < Xmin) {
        did_limit = true;
        vec->x = Xmin;
    }
    
    if(vec->y > Ymax) {
        did_limit = true;
        vec->y = Ymax;
    } else if (vec->y < Ymin) {
        did_limit = true;
        vec->y = Ymin;
    }

    return did_limit;
}