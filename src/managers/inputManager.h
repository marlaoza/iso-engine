#pragma once
#include <SDL_rect.h>
#include <SDL_keycode.h>
#include <SDL_events.h>

extern SDL_FPoint INPUT_AXIS;
extern SDL_FPoint MOUSE_POS;

void onKeyDown(SDL_Keycode key);
void onKeyUp(SDL_Keycode key);

void onMouseMotion(SDL_MouseMotionEvent motion);