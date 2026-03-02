#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "Entity.h"
#include "Control.h"
#include "Vector.h"

#define MAIN_TITLE "SDL Game"
#define INIT_WIDTH 960
#define INIT_HEIGHT 530
#define INIT_BACKCOLOR 0x000000ff

#define SQRT2 1.4142135623730951
#define BYSQRT2 1 / 1.4142135623730951

#define ENTITY_LIMIT 500
#define PLAYABLE_WIDTH 1400
#define PLAYABLE_HEIGHT 1400
#define STARS_LIMIT 1000

SDL_Window *main_window;
SDL_Renderer *main_renderer;
SDL_Event main_event;

Vector2D camera = {0};
Entity player;
Entity opponent_test;
Player_Input player_input;

bool MySDL_SetDrawHexColor(int hexcode);
void ToOnScreenCoordinate(Vector2D *out, Vector2D *ingame, Vector2D *camera);
void LimitPos(Vector2D *vec, int Xmin, int Ymin, int Xmax, int Ymax);

bool is_running = false;
double do_frame_delta = 0.008;              // For consistent frame rate (set 0 to remove function)
double delta_time = 0;                      // Time between game ticks

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
    player.pos.x = 0;
    player.pos.y = 0;
    player.rotation = 0;

    player.type = PLAYER;

    player.square_hitbox_cornerpos.x = -10;
    player.square_hitbox_cornerpos.y = -10;
    player.hitbox_dimensions.x = 20;
    player.hitbox_dimensions.y = 20;

    opponent_test.pos.x = 40;
    opponent_test.pos.y = 0;
    opponent_test.vel.x = 0;
    opponent_test.vel.y = 0;

    opponent_test.square_hitbox_cornerpos.x = -10;
    opponent_test.square_hitbox_cornerpos.y = -10;
    opponent_test.hitbox_dimensions.x = 20;
    opponent_test.hitbox_dimensions.y = 20;

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
    
    player.vel.x += (player_input.right - player_input.left) * 5.0f * diag_1byroot2;
    player.vel.y += (player_input.up - player_input.down) * 5.0f * diag_1byroot2;
    
    if(player.pos.x > (PLAYABLE_WIDTH / 2) - 10) {
        player.vel.x = -SDL_fabs(player.vel.x) * 0.8;
    } else if(player.pos.x < (-PLAYABLE_WIDTH / 2) + 10) {
        player.vel.x = SDL_fabs(player.vel.x) * 0.8;
    }
    
    if(player.pos.y > (PLAYABLE_HEIGHT / 2) - 10) {
        player.vel.y = -SDL_fabs(player.vel.y) * 0.8;
    } else if(player.pos.y < (-PLAYABLE_HEIGHT / 2) + 10) {
        player.vel.y = SDL_fabs(player.vel.y) * 0.8;
    }

    Entity_Tick(&player, delta_time);
    // ##############################################

    if(Bounce_Collision(&player, &opponent_test, 0.5)) {
        SDL_Log("Collision Tested!");
    }
    LimitPos(&camera, player.pos.x - 100, player.pos.y - 100, player.pos.x + 100, player.pos.y + 100);

    return true;
}

bool Game_Draw() {
    Vector2D onScreen, tmpvec;
    SDL_FRect rect;

    // PLAYER #######################################
    ToOnScreenCoordinate(&onScreen, &player.pos, &camera);
    rect.x = onScreen.x - 10;
    rect.y = onScreen.y - 10;
    rect.w = 20;
    rect.h = 20;
    MySDL_SetDrawHexColor(0x00ffffff);
    SDL_RenderFillRect(main_renderer, &rect);
    // ##############################################

    // OPPONENT_TEST ################################
    ToOnScreenCoordinate(&onScreen, &opponent_test.pos, &camera);
    rect.x = onScreen.x - 10;
    rect.y = onScreen.y - 10;
    rect.w = 20;
    rect.h = 20;
    MySDL_SetDrawHexColor(0x0000ffff);
    SDL_RenderFillRect(main_renderer, &rect);
    // ##############################################

    // PLAY BORDER ##################################
    tmpvec.x = (-PLAYABLE_WIDTH / 2);
    tmpvec.y = (PLAYABLE_HEIGHT / 2);
    ToOnScreenCoordinate(&onScreen, &tmpvec, &camera);
    rect.x = onScreen.x;
    rect.y = onScreen.y;
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