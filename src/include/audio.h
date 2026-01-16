#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3/SDL_audio.h>
#include <stdint.h>

SDL_AudioStream* init_audio(uint8_t* sound_timer);
void destroy_audio(SDL_AudioStream* stream);

#endif
