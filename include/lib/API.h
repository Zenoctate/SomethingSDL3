#ifndef API
#define API

#include "defs.h"
#include <stdbool.h>

// typedef char bool;

#define TYPE_QUITEVENT 1
#define TYPE_KEYDOWNEVENT 2
#define TYPE_KEYUPEVENT 3

#define STATE_KEYDOWN 1
#define STATE_KEYUP 0

typedef int TypeEvent;

typedef struct {
    int keycode;
    int state;
} Key_MyEvent;

bool Initialize_Game();
bool Set_Color(int hexcode);
bool Draw_Rect(int x, int y, int w, int h);
void Destroy_Game();

bool Poll_Events();
TypeEvent Get_EventType();
Key_MyEvent Get_KeyEvent();

extern double SDL_atan2(double, double);

#endif