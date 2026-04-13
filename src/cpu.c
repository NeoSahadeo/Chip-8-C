#include "include/cpu.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/data.h"

CPU* init_cpu() {
  CPU* cpu = malloc(sizeof(CPU));
  for (int x = 0; x < 80; x++) {
    cpu->memory[x] = font[x];
  }

  cpu->PC = 0x200;
  cpu->key_pressed = 0xFF;
  return cpu;
}

void inc_pc(CPU* cpu) {
  cpu->PC += 2;
}

void cycle(CPU* cpu) {
  uint16_t opcode = cpu->memory[cpu->PC] << 8 | cpu->memory[cpu->PC + 1];
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  uint8_t N = opcode & 0x000F;
  uint8_t NN = opcode & 0x00FF;
  uint16_t NNN = opcode & 0x0FFF;
  inc_pc(cpu);

  // printf("%04X\n", opcode);

  switch (opcode >> 12) {
    case 0x0: {
      if (NN == 0xEE) {
        if (cpu->SP > 0)
          cpu->PC = cpu->stack[--cpu->SP];
      } else if (NN == 0xE0) {
        memset(cpu->vram, 0, 64 * 32);
      } else {
        // SKIPPED THIS INSTRUCTION
        // cpu->PC = opcode & 0x0FFF;
      }
      break;
    }
    case 0x1: {
      cpu->PC = NNN;
      break;
    }
    case 0x2: {
      if (cpu->SP < 16) {
        cpu->stack[cpu->SP++] = cpu->PC;
      }
      cpu->PC = NNN;
      break;
    }
    case 0x3: {
      if (cpu->V[x] == NN)
        inc_pc(cpu);
      break;
    }
    case 0x4: {
      if (cpu->V[x] != NN)
        inc_pc(cpu);
      break;
    }
    case 0x5: {
      if (cpu->V[x] == cpu->V[y])
        inc_pc(cpu);
      break;
    }
    case 0x6: {
      cpu->V[x] = NN;
      break;
    }
    case 0x7: {
      cpu->V[x] += NN;
      break;
    }
    case 0x8: {
      uint8_t oldx = cpu->V[x];
      uint8_t oldy = cpu->V[y];

      switch (N) {
        case 0: {
          cpu->V[x] = cpu->V[y];
          break;
        }
        case 1: {
          cpu->V[x] |= cpu->V[y];
          cpu->V[0xF] = 0;
          break;
        }
        case 2: {
          cpu->V[x] &= cpu->V[y];
          cpu->V[0xF] = 0;
          break;
        }
        case 3: {
          cpu->V[x] ^= cpu->V[y];
          cpu->V[0xF] = 0;
          break;
        }
        case 4: {
          cpu->V[x] += cpu->V[y];
          cpu->V[0xF] = cpu->V[y] > cpu->V[x];
          break;
        }
        case 5: {
          cpu->V[x] = oldx - oldy;
          cpu->V[0xF] = oldx >= oldy;
          break;
        }
        case 6: {
          cpu->V[x] = cpu->V[y] >> 1;
          cpu->V[0xF] = oldy & 1;
          break;
        }
        case 7: {
          cpu->V[x] = oldy - oldx;
          cpu->V[0xF] = oldy >= oldx;
          break;
        }
        case 0xE: {
          cpu->V[x] = cpu->V[y] << 1;
          cpu->V[0xF] = oldy >> 7;
          break;
        }
      }
      break;
    }
    case 0x9: {
      if (cpu->V[x] != cpu->V[y])
        inc_pc(cpu);
      break;
    }
    case 0xA: {
      cpu->I = NNN;
      break;
    }
    case 0xB: {
      cpu->PC = NNN + cpu->V[0];
      break;
    }
    case 0xC: {
      cpu->V[x] = rand() & NN;
      break;
    }
    case 0xD: {
      uint8_t x0 = cpu->V[x] % 64;
      uint8_t y0 = cpu->V[y] % 32;
      cpu->V[0xF] = 0;

      for (uint8_t row = 0; row < N; row++) {
        uint8_t sprite_byte = cpu->memory[cpu->I + row];
        uint8_t y = (y0 + row) % 32;  // wrap vertically
        uint16_t base = y * 64;

        for (uint8_t col = 0; col < 8; col++) {
          if (sprite_byte & (0x80 >> col)) {
            uint8_t x = (x0 + col) % 64;  // wrap horizontally
            uint16_t idx = base + x;

            if (cpu->vram[idx]) {
              cpu->V[0xF] = 1;
            }
            cpu->vram[idx] ^= 1;  // XOR toggle
          }
        }
      }
      break;
    }
    case 0xE: {
      if (NN == 0x9E && cpu->keys[cpu->V[x]]) {
        inc_pc(cpu);
      } else if (NN == 0xA1 && !cpu->keys[cpu->V[x]]) {
        inc_pc(cpu);
      }
      break;
    }
    case 0xF: {
      switch (NN) {
        case 0x7: {
          cpu->V[x] = cpu->delay_timer;
          break;
        }

        case 0xA: {
          int8_t key_pressed = 0;
          for (uint8_t i = 0; i < 16; i++) {
            if (cpu->keys[i]) {
              cpu->V[x] = i;
              key_pressed = i;
              break;
            }
          }

          if (!key_pressed) {
            cpu->PC -= 2;
          }
          break;
        }
        case 0x15: {
          cpu->delay_timer = cpu->V[x];
          break;
        }
        case 0x18: {
          cpu->sound_timer = cpu->V[x];
          break;
        }
        case 0x1E: {
          cpu->I += cpu->V[x];
          break;
        }
        case 0x29: {
          cpu->I = cpu->V[x] * 5;
          break;
        }
        case 0x33: {
          cpu->memory[cpu->I] = cpu->V[x] / 100;
          cpu->memory[cpu->I + 1] = (cpu->V[x] / 10) % 10;
          cpu->memory[cpu->I + 2] = cpu->V[x] % 10;
          break;
        }
        case 0x55: {
          for (uint8_t p = 0; p < x + 1; p++) {
            cpu->memory[cpu->I + p] = cpu->V[p];
          };
          cpu->I += x + 1;
          break;
        }
        case 0x65: {
          for (uint8_t p = 0; p < x + 1; p++) {
            cpu->V[p] = cpu->memory[cpu->I + p];
          };
          cpu->I += x + 1;
          break;
        }
      }
      break;
    }
  }
}
