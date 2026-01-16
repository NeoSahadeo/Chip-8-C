#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_stdinc.h>
#include <stdint.h>
#include <stdlib.h>

void audio_callback(void* userdata,
                    SDL_AudioStream* stream,
                    int additional_amount,
                    int total_amount) {
  uint8_t timer = *(uint8_t*)userdata;

  Uint16* samples = SDL_malloc(total_amount);

  const Uint16 amplitude = 16000;
  const int half_period = (44100 / 1000.0f) / 2;
  static int sample_index = 0;

  int num_samples = total_amount / sizeof(Uint16);

  for (int i = 0; i < num_samples; i++) {
    if (timer > 0) {
      samples[i] =
          ((sample_index / half_period) % 2) ? (65535 - amplitude) : amplitude;
    } else {
      samples[i] = 32768;
    }
    sample_index++;
  }

  SDL_PutAudioStreamData(stream, samples, total_amount);
  SDL_free(samples);
}

SDL_AudioStream* init_audio(uint8_t* sound_timer) {
  SDL_AudioSpec spec;

  spec.channels = 1;
  spec.format = SDL_AUDIO_S16LE;
  spec.freq = 44100;

  if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Couldn't initialize SDL AUDIO: %s", SDL_GetError());
    exit(20);
  }

  SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(
      SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, &audio_callback, sound_timer);

  if (!stream) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open stream: %s",
                 SDL_GetError());
    return NULL;
  }

  return stream;
}

void destroy_audio(SDL_AudioStream* stream) {
  if (stream) {
    SDL_ClearAudioStream(stream);
  }
}
