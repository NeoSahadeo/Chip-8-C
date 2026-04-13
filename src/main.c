#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_timer.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/audio.h"
#include "include/cpu.h"
#include "include/data.h"
#include "include/display.h"

const int SCREEN_WIDTH = 64 * 10;
const int SCREEN_HEIGHT = 32 * 10;

void render_display(DisplayCtx* ctx, CPU* cpu);

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Requires a ROM!\n");
    return 0;
  }

  CPU* cpu = init_cpu();
  load_rom(cpu, argv[1]);

  SDL_AudioStream* audiostream = init_audio(&cpu->sound_timer);
  SDL_ResumeAudioStreamDevice(audiostream);

  DisplayCtx* ctx = create_display();

  uint IPS = 500;
  uint FPS = 60;
  double FRAME_TIME = 1.0 / FPS;
  uint CYCLES_PER_FRAME = (IPS / FPS);

  SDL_Event event;
  bool running = true;

  while (running) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_QUIT: {
          running = false;
          break;
        }
        case SDL_EVENT_KEY_DOWN: {
          uint8_t chip8_key = sdl_to_chip8(event.key.key);
          if (chip8_key < 16) {
            cpu->keys[chip8_key] = 1;
          }
          break;
        }
        case SDL_EVENT_KEY_UP: {
          uint8_t chip8_key = sdl_to_chip8(event.key.key);
          if (chip8_key < 16) {
            cpu->keys[chip8_key] = 0;
          }
          break;
        }
        default:
          break;
      }
    }

    for (uint x = 0; x < CYCLES_PER_FRAME; x++) {
      cycle(cpu);
    }

    if (cpu->delay_timer > 0)
      cpu->delay_timer--;
    if (cpu->sound_timer > 0) {
      cpu->sound_timer--;
    }

    render_display(ctx, cpu);

    SDL_Delay(16);
  }

  destroy_audio(audiostream);
  destroy_display(ctx);
  free(cpu);
  return 0;
}
