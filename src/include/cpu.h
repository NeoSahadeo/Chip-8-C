#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct {
  uint8_t memory[4096];
  uint8_t vram[64 * 32];
  uint8_t keys[16];
  uint16_t stack[16];

  uint8_t sound_timer;
  uint8_t delay_timer;

  uint8_t SP;
  uint8_t V[16];
  uint16_t PC;
  uint16_t I;
} CPU;

void cycle(CPU* cpu);
void inc_pc(CPU* cpu);
CPU* init_cpu();
void load_rom(CPU* cpu, char* dir);

#endif
