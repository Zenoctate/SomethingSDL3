#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "Entity.h"
#include "Control.h"

#define MAIN_TITLE "SDL Game"
#define INIT_WIDTH 800
#define INIT_HEIGHT 500
#define INIT_BACKCOLOR 0x000000ff

SDL_Window *main_window;
SDL_Renderer *main_renderer;
SDL_Event main_event;

bool MySDL_SetDrawHexColor(int hexcode);

bool is_running = false;
float deltaTime;         // Time between game ticks

bool Init_Game();
bool Init_FirstTick();
bool Game_EHandle();     // Event handling (Inputs mostly)
bool Game_Tick();        // Game Updates
bool Destroy_Game();

Entity player;
Player_Input player_input;

int main(int argc, char* argv[]) {
    Init_Game();
    Init_FirstTick();

    is_running = true;
    // float ticks
    while(is_running) {
        // deltaTime = ticks - 
        SDL_RenderClear(main_renderer);
        
        Game_EHandle();
        Game_Tick();SDL_Log("%f %f\n", player.vel.x, player.vel.y);
        
        SDL_RenderPresent(main_renderer);
        // ticks = 
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
    player.vel.x += (player_input.right - player_input.left) * 2.0f;
    player.vel.y += (player_input.up - player_input.down) * 2.0f;

    return true;
}

bool Destroy_Game() {
    SDL_DestroyRenderer(main_renderer);
    SDL_DestroyWindow(main_window);
    
    main_window = 0;
    main_renderer = 0;
    
    SDL_Quit();
    return true;
}

bool MySDL_SetDrawHexColor(int hexcode) {
    return SDL_SetRenderDrawColor(main_renderer, 
            (hexcode & 0xff000000)  >> 24,
            (hexcode & 0xff0000)    >> 16,
            (hexcode & 0xff00)      >> 8,
             hexcode & 0xff);
}