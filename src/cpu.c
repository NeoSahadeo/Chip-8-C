#include "cpu.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void init_cpu(CPU *cpu) {
  cpu->PC = 0x200;
  cpu->opcode = 0;
  cpu->I = 0;
  cpu->SP = 0;

  memset(cpu->memory, 0, sizeof(uint8_t));
  memset(cpu->graphics, 0, sizeof(uint8_t));
  memset(cpu->registers, 0, sizeof(uint8_t));
  memset(cpu->stack, 0, sizeof(uint8_t));
  memset(cpu->keys, 0, sizeof(uint8_t));

  srand(time(NULL));

  const uint8_t fonts[] = {
      // 0
      0xf0,
      0x90,
      0x90,
      0x90,
      0xf0,
      // 1
      0x20,
      0x60,
      0x20,
      0x20,
      0x70,
      // 2
      0xf0,
      0x10,
      0xf0,
      0x80,
      0xf0,
      // 3
      0xf0,
      0x10,
      0xf0,
      0x10,
      0xf0,
      // 4
      0x90,
      0x90,
      0xf0,
      0x10,
      0x10,
      // 5
      0xf0,
      0x80,
      0xf0,
      0x10,
      0xf0,
      // 6
      0xf0,
      0x80,
      0xf0,
      0x90,
      0xf0,
      // 7
      0xf0,
      0x10,
      0x20,
      0x40,
      0x40,
      // 8
      0xf0,
      0x90,
      0xf0,
      0x90,
      0xf0,
      // 9
      0xf0,
      0x90,
      0xf0,
      0x10,
      0xf0,
      // A
      0xf0,
      0x90,
      0xf0,
      0x90,
      0x90,
      // B
      0xe0,
      0x90,
      0xe0,
      0x90,
      0xe0,
      // C
      0xf0,
      0x80,
      0x80,
      0x80,
      0xf0,
      // D
      0xe0,
      0x90,
      0x90,
      0x90,
      0xe0,
      // E
      0xf0,
      0x80,
      0xf0,
      0x80,
      0xf0,
      // F
      0xf0,
      0x80,
      0xf0,
      0x80,
      0x80,
  };

  for (int i = 0; i < 80; i++) {
    cpu->memory[i] = fonts[i];
  }
};

void inc_pc(CPU *cpu) { cpu->PC += 2; }

void cycle(CPU *cpu) {
  cpu->opcode = cpu->memory[cpu->PC] << 8 | cpu->memory[cpu->PC + 1];
  printf("%04x\n", cpu->opcode);

  switch (cpu->opcode >> 15) {
  case 0x0:
    if (cpu->opcode == 0x00E0) {
      memset(cpu->graphics, 0, sizeof(uint8_t));
    } else if (cpu->opcode == 0x00EE) {
      cpu->SP--;
      cpu->PC = cpu->stack[cpu->SP];
    }
    inc_pc(cpu);
    break;
  case 0x1:
    cpu->PC = cpu->opcode & 0x0FFF;
    break;
  case 0x2:
    cpu->stack[cpu->SP] = cpu->PC;
    cpu->SP++;
    cpu->PC = cpu->opcode & 0x0FFF;
    break;
  case 0x3:
    if (cpu->registers[(cpu->opcode & 0x0F00) >> 8] == (cpu->opcode & 0x00FF)) {
      inc_pc(cpu);
    };
    inc_pc(cpu);
    break;
  case 0x4:
    if (cpu->registers[(cpu->opcode & 0x0F00) >> 8] != (cpu->opcode & 0x00FF)) {
      inc_pc(cpu);
    };
    inc_pc(cpu);
    break;
  case 0x5:
    if (cpu->registers[(cpu->opcode & 0x0F00) >> 8] ==
        cpu->registers[(cpu->opcode & 0x00F0) >> 4]) {
      inc_pc(cpu);
    }
    inc_pc(cpu);
    break;
  case 0x6:
    cpu->registers[(cpu->opcode & 0x0F00) >> 8] =
        (uint8_t)(cpu->opcode & 0x00FF);
    inc_pc(cpu);
    break;
  case 0x7:
    cpu->registers[(cpu->opcode & 0x0F00) >> 8] +=
        (uint8_t)(cpu->opcode & 0x00FF);
    inc_pc(cpu);
    break;
  case 0x8: {
    uint8_t const x_index = (cpu->opcode & 0x0F00) >> 8;
    uint8_t const y_index = (cpu->opcode & 0x00F0) >> 4;
    uint8_t const mode = cpu->opcode & 0x000F;

    switch (mode) {
    case 0x0:
      cpu->registers[x_index] = cpu->registers[y_index];
      break;
    case 0x1:
      cpu->registers[x_index] |= cpu->registers[y_index];
      break;
    case 0x2:
      cpu->registers[x_index] &= cpu->registers[y_index];
      break;
    case 0x3:
      cpu->registers[x_index] ^= cpu->registers[y_index];
      break;
    case 0x4: {
      uint16_t const sum = cpu->registers[x_index] + cpu->registers[y_index];
      cpu->registers[0xF] = (sum > 0xFF) == 1 ? 1 : 0;
      cpu->registers[x_index] = (uint8_t)sum;
      break;
    }
    case 0x5:
      cpu->registers[0xF] =
          (cpu->registers[y_index] > cpu->registers[x_index]) == 1 ? 0 : 1;
      cpu->registers[x_index] -= cpu->registers[y_index];
      break;
    case 0x6:
      cpu->registers[0xF] = cpu->registers[y_index] & 1;
      cpu->registers[x_index] >>= cpu->registers[y_index];
      break;
    case 0x7:
      cpu->registers[0xF] =
          (cpu->registers[x_index] > cpu->registers[y_index]) == 1 ? 0 : 1;
      cpu->registers[x_index] =
          cpu->registers[y_index] - cpu->registers[x_index];
      break;
    case 0xE:
      cpu->registers[0xF] = cpu->registers[y_index] >> 7;
      cpu->registers[x_index] <<= cpu->registers[y_index];
      break;
    }
    inc_pc(cpu);
    break;
  }
  case 0x9:
    if (cpu->registers[(cpu->opcode & 0x0F00) >> 8] !=
        cpu->registers[(cpu->opcode & 0x00F0) >> 4]) {
      inc_pc(cpu);
    }
    inc_pc(cpu);
    break;
  case 0xA:
    cpu->I = cpu->opcode & 0x0FFF;
    inc_pc(cpu);
    break;
  case 0xB:
    cpu->PC = (cpu->opcode & 0x0FFF) + cpu->registers[0];
    break;
  case 0xC:
    cpu->registers[(cpu->opcode & 0x0F00) >> 8] =
        rand() & (uint8_t)(cpu->opcode & 0x00FF);
    inc_pc(cpu);
    break;
  case 0xD:
    cpu->registers[0xF] = 0;
    uint8_t const x_reg = cpu->registers[(cpu->opcode & 0x0F00) >> 8];
    uint8_t const y_reg = cpu->registers[(cpu->opcode & 0x00F0) >> 4];
    uint8_t const height = cpu->opcode & 0x000F;

    for (uint8_t p = 0; p < height; p++) {
      uint8_t const pixel = cpu->memory[cpu->I + p];

      for (uint8_t q = 0; q < 8; q++) {
        if (pixel & (0x80 >> q)) {
          uint8_t const ty = (y_reg + p) % 32;
          uint8_t const tx = (x_reg + q) % 64;

          uint8_t const idx = tx + ty * 64;
          cpu->graphics[idx] ^= 1;

          if (!cpu->graphics[idx]) {
            cpu->registers[0xF] = 1;
          }
        }
      }
    }

    inc_pc(cpu);
    break;
  case 0xE: {
    uint8_t const x_index = (cpu->opcode & 0x0F00) >> 8;
    uint8_t const mode = cpu->opcode & 0x00FF;

    switch (mode) {
    case 0x9E:
      if (cpu->keys[cpu->registers[x_index]]) {
        inc_pc(cpu);
      }
      break;
    case 0xA1:
      if (!cpu->keys[cpu->registers[x_index]]) {
        inc_pc(cpu);
      }
      break;
    }
    inc_pc(cpu);
    break;
  }
  case 0xF: {
    uint8_t const x_index = (cpu->opcode & 0x0F00) >> 8;
    uint8_t const mode = cpu->opcode & 0x00FF;
    switch (mode) {
    case 0x07:
      cpu->registers[x_index] = cpu->delay_timer;
      break;
    case 0x0A:
      break;
    case 0x15:
      cpu->delay_timer = cpu->registers[x_index];
      break;
    case 0x18:
      cpu->sound_timer = cpu->registers[x_index];
      break;
    case 0x1E: {
      cpu->I += cpu->registers[x_index];
    } break;
    case 0x29:
      cpu->I = cpu->registers[x_index] * 5;
      break;
    case 0x33:
      cpu->memory[cpu->I] = cpu->registers[x_index] / 100;
      cpu->memory[cpu->I + 1] = (cpu->registers[x_index] / 10) % 10;
      cpu->memory[cpu->I + 2] = cpu->registers[x_index] % 10;
      break;
    case 0x55:
      for (uint8_t i = 0; i < x_index; i++) {
        cpu->memory[cpu->I + i] = cpu->registers[i];
      }
      break;
    case 0x65:
      for (uint8_t i = 0; i < x_index; i++) {
        cpu->registers[i] = cpu->memory[cpu->I + i];
      }
      break;
    }

    inc_pc(cpu);
    break;
  }
  }

  if (cpu->delay_timer > 0)
    cpu->delay_timer--;

  if (cpu->sound_timer > 0)
    cpu->sound_timer--;
}
