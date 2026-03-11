#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "defs.h"
#include "Entity.h"
#include "Control.h"
#include "Vector.h"
#include "Collisions.h"
#include "Gameplay.h"

SDL_Window *main_window;
SDL_Renderer *main_renderer;
SDL_Event main_event;

Vector2D camera = {0};
Player_Input player_input;

int num_entities = 0;
Entity entities[ENTITY_LIMIT];
#define PLAYER_ENTITY entities[0]           // First entity is always the PLAYER_ENTITY

int num_bullets = 0;
Bullet bullets[BULLET_LIMIT];

bool is_running = false;
double do_frame_delta = 0.008;              // For consistent frame rate (set 0 to remove function)
double delta_time = 0;                      // Time between game ticks
double time_passed = 0;                     // Seconds passed since running

bool MySDL_SetDrawHexColor(int hexcode);
void ToOnScreenCoordinate(Vector2D *out, Vector2D *ingame, Vector2D *camera);
void LimitPos(Vector2D *vec, int Xmin, int Ymin, int Xmax, int Ymax);

bool Init_Game();
bool Init_FirstTick();
bool Game_EHandle();                        // Event handling (Inputs mostly)
bool Game_Tick();                           // Game Updates
bool Game_Draw();
void Destroy_Game();

int main(int argc, char* argv[]) {
    Init_Game();
    Init_FirstTick();

    is_running = true;
    while(is_running) {
        unsigned long counter = SDL_GetPerformanceCounter();

        MySDL_SetDrawHexColor(0x000000ff);
        SDL_RenderClear(main_renderer);
        
        Game_EHandle();
        Game_Tick();
        Game_Draw();
        
        SDL_RenderPresent(main_renderer);

        // ################ For Consistent Frame Rate ################
        delta_time = (SDL_GetPerformanceCounter() - counter) / (double)SDL_GetPerformanceFrequency();
        if(do_frame_delta && delta_time < do_frame_delta) {
            SDL_Delay((int)((do_frame_delta - delta_time + 0.0005) * 1000));
            delta_time = do_frame_delta;
            time_passed += delta_time;
        }
        // ###########################################################
    }

    Destroy_Game();
    return 0;
}

bool Init_Game() {
    if(!SDL_Init(SDL_INIT_VIDEO)) {
        return false;
    }
    
    if(!(main_window = SDL_CreateWindow(MAIN_TITLE, INIT_WIDTH, INIT_HEIGHT, 0))) {
        return false;
    }
    
    if(!(main_renderer = SDL_CreateRenderer(main_window, 0))) {
        return false;
    }
    
    MySDL_SetDrawHexColor(INIT_BACKCOLOR);
    return true;
}

bool Init_FirstTick() {
    PLAYER_ENTITY.pos.x = 0;
    PLAYER_ENTITY.pos.y = 0;
    PLAYER_ENTITY.mass = 100;

    PLAYER_ENTITY.type = PLAYER;

    PLAYER_ENTITY.square_hitbox_cornerpos.x = -10;
    PLAYER_ENTITY.square_hitbox_cornerpos.y = -10;
    PLAYER_ENTITY.hitbox_dimensions.x = 20;
    PLAYER_ENTITY.hitbox_dimensions.y = 20;

    num_entities = 200;
    for(int i = 1; i < num_entities; i++) {
        entities[i].pos.x = (i * 25) % 600;
        entities[i].pos.y = (i * 27) % 600;
        entities[i].vel.x = (i % 10) - 5;
        entities[i].vel.y = (i % 11) - 5.5;
        entities[i].mass = 10;

        entities[i].type = OPPONENT;

        entities[i].square_hitbox_cornerpos.x = -10;
        entities[i].square_hitbox_cornerpos.y = -10;
        entities[i].hitbox_dimensions.x = 20;
        entities[i].hitbox_dimensions.y = 20;
    }

    return true;
}

bool Game_EHandle() {
    while(SDL_PollEvent(&main_event)) {
        switch(main_event.type) {
            case SDL_EVENT_QUIT: {
                is_running = false;
            } break;
            case SDL_EVENT_KEY_DOWN: {
                switch(main_event.key.scancode) {
                    case SDL_SCANCODE_ESCAPE: {
                        is_running = false;
                    } break;
                    case SDL_SCANCODE_W: {
                        player_input.up = true;
                    } break;
                    case SDL_SCANCODE_A: {
                        player_input.left = true;
                    } break;
                    case SDL_SCANCODE_S: {
                        player_input.down = true;
                    } break;
                    case SDL_SCANCODE_D: {
                        player_input.right = true;
                    } break;
                    case SDL_SCANCODE_SPACE: {
                        player_input.fire = true;
                    } break;
                }
            } break;
            case SDL_EVENT_KEY_UP: {
                switch(main_event.key.scancode) {
                    case SDL_SCANCODE_W: {
                        player_input.up = false;
                    } break;
                    case SDL_SCANCODE_A: {
                        player_input.left = false;
                    } break;
                    case SDL_SCANCODE_S: {
                        player_input.down = false;
                    } break;
                    case SDL_SCANCODE_D: {
                        player_input.right = false;
                    } break;
                    case SDL_SCANCODE_SPACE: {
                        player_input.fire = false;
                    } break;
                }
            } break;
        }
    }
    return true;
}

bool Game_Tick() {
    // PLAYER #######################################
    float diag_1byroot2 = 1;
    if((player_input.up || player_input.down) && (player_input.right || player_input.left)) {
        diag_1byroot2 = BYSQRT2;
    }
    
    PLAYER_ENTITY.vel.x += (player_input.right - player_input.left) * 5.0f * diag_1byroot2;
    PLAYER_ENTITY.vel.y += (player_input.up - player_input.down) * 5.0f * diag_1byroot2;
    // ##############################################
    
    for(int i = 0; i < num_entities; i++) {
        Entity_Tick(&entities[i], delta_time);

        if(entities[i].pos.x > (PLAYABLE_WIDTH / 2) - 10) {
            entities[i].vel.x = -SDL_fabs(entities[i].vel.x) * 0.8;
        } else if(entities[i].pos.x < (-PLAYABLE_WIDTH / 2) + 10) {
            entities[i].vel.x = SDL_fabs(entities[i].vel.x) * 0.8;
        }
        
        if(entities[i].pos.y > (PLAYABLE_HEIGHT / 2) - 10) {
            entities[i].vel.y = -SDL_fabs(entities[i].vel.y) * 0.8;
        } else if(entities[i].pos.y < (-PLAYABLE_HEIGHT / 2) + 10) {
            entities[i].vel.y = SDL_fabs(entities[i].vel.y) * 0.8;
        }
        LimitPos(&entities[i].pos, (-PLAYABLE_WIDTH / 2) + 10, (-PLAYABLE_HEIGHT / 2) + 10, (PLAYABLE_WIDTH / 2) - 10, (PLAYABLE_HEIGHT / 2) - 10);

        for(int j = i + 1; j < num_entities; j++) {
            Translational_Collision(&entities[i], &entities[j]);
        }
    }

    for(int i = 0; i < num_bullets; i++) {
        Bullet_Tick(&bullets[i], delta_time);
    }

    LimitPos(&camera, PLAYER_ENTITY.pos.x - 100, PLAYER_ENTITY.pos.y - 100, PLAYER_ENTITY.pos.x + 100, PLAYER_ENTITY.pos.y + 100);

    return true;
}

bool Game_Draw() {
    Vector2D onScreen, tmpvec;
    SDL_FRect rect;

    // BULLETS ######################################
    for(int i = 0; i < num_entities; i++) {
        Bullet_Draw(main_renderer, &bullets[i], &camera);
    }
    // ##############################################
    
    // ENTITES ######################################
    for(int i = 0; i < num_entities; i++) {
        Entity_Draw(main_renderer, &entities[i], &camera);
    }
    // ##############################################

    // PLAY BORDER ##################################
    tmpvec.x = (-PLAYABLE_WIDTH / 2);
    tmpvec.y = (PLAYABLE_HEIGHT / 2);
    ToOnScreenCoordinate(&onScreen, &tmpvec, &camera);
    rect.x = onScreen.x + 0.5;
    rect.y = onScreen.y + 0.5;
    rect.w = PLAYABLE_WIDTH;
    rect.h = PLAYABLE_HEIGHT;
    MySDL_SetDrawHexColor(0xffffffff);
    SDL_RenderRect(main_renderer, &rect);
    // ##############################################

    return true;
}

void Destroy_Game() {
    SDL_DestroyRenderer(main_renderer);
    SDL_DestroyWindow(main_window);
    
    main_window = 0;
    main_renderer = 0;
    
    SDL_Quit();
}

bool MySDL_SetDrawHexColor(int hexcode) {
    return SDL_SetRenderDrawColor(main_renderer, 
            (hexcode & 0xff000000)  >> 24,
            (hexcode & 0xff0000)    >> 16,
            (hexcode & 0xff00)      >> 8,
             hexcode & 0xff);
}

void ToOnScreenCoordinate(Vector2D *out, Vector2D *ingame, Vector2D *camera) {
    out->x = ingame->x - camera->x + (INIT_WIDTH / 2);
    out->y = -ingame->y + camera->y + (INIT_HEIGHT / 2);
}

void LimitPos(Vector2D *vec, int Xmin, int Ymin, int Xmax, int Ymax) {
    if(vec->x > Xmax) {
        vec->x = Xmax;
    } else if (vec->x < Xmin) {
        vec->x = Xmin;
    }

    if(vec->y > Ymax) {
        vec->y = Ymax;
    } else if (vec->y < Ymin) {
        vec->y = Ymin;
    }
}