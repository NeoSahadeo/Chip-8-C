#include <SDL3/SDL.h>
#include <stdint.h>

#ifndef DATA_H
#define DATA_H

extern const int font[80];
uint8_t sdl_to_chip8(SDL_Keycode key);
#endif
