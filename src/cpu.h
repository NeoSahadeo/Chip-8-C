#pragma once
#include <stdint.h>

typedef struct {
  uint8_t memory[4096];
  uint8_t graphics[64 * 32];
  uint8_t registers[16];

  uint8_t delay_timer;
  uint8_t sound_timer;
  uint8_t keys[16];

  uint16_t stack[16];
  uint16_t I;
  uint16_t opcode;
  uint16_t PC;
  uint16_t SP;
} CPU;

void init_cpu(CPU *cpu);
void cycle(CPU *cpu);
