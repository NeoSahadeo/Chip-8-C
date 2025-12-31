#ifndef CPU_H
#define CPU_H
#include "display.h"

#include <stdint.h>
typedef struct {
  uint8_t memory[4096];
  uint8_t keys[16];
  uint8_t stack[16];

  uint8_t sound_timer;
  uint8_t delay_timer;

  uint8_t reg_gen[16];
  uint16_t reg_OPCODE;
  uint16_t reg_I;
  uint16_t reg_SP;
  uint16_t reg_PC;

  Display* display;
} CPU;

void cycle(CPU* cpu);
void inc_pc(CPU* cpu);
void render_display(DisplayCtx* ctx, CPU* cpu);

#endif
