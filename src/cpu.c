#include "include/cpu.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

void inc_pc(CPU* cpu) {
  cpu->reg_PC += 2;
}

void cycle(CPU* cpu) {
  cpu->reg_OPCODE =
      (cpu->memory[cpu->reg_PC] << 8) | cpu->memory[cpu->reg_PC + 1];

  // printf("%d\n", cpu->reg_OPCODE);
  switch (cpu->reg_OPCODE >> 12) {
    case 0x0: {
      if (cpu->reg_OPCODE == 0x00E0) {  // clear screen
        cpu->display->clear(cpu->display);
      } else if (cpu->reg_OPCODE == 0x00EE) {  // return from subroutine
        cpu->reg_SP--;
        cpu->reg_PC = cpu->stack[cpu->reg_SP];
      }
      inc_pc(cpu);
      break;
    };
    case 0x1: {  // jump to address
      cpu->reg_PC = cpu->reg_OPCODE & 0x0FFF;
      break;
    };
    case 0x2: {  // execute subroutine
      cpu->stack[cpu->reg_SP] = cpu->reg_PC;
      cpu->reg_SP++;
      cpu->reg_PC = cpu->reg_OPCODE & 0x0fff;
      break;
    };
    case 0x3: {  // skip instruction in vX for nn
      if (cpu->reg_gen[(cpu->reg_OPCODE & 0x0F00) >> 8] ==
          (cpu->reg_OPCODE & 0x00FF)) {
        inc_pc(cpu);
      };
      inc_pc(cpu);
      break;
    };
    case 0x4: {  // skip instruction in vX for not nn
      if (cpu->reg_gen[(cpu->reg_OPCODE & 0x0F00) >> 8] !=
          (cpu->reg_OPCODE & 0x00FF)) {
        inc_pc(cpu);
      };
      inc_pc(cpu);
      break;
    };
    case 0x5: {  // skip instruction if value in vX for vY
      if (cpu->reg_gen[(cpu->reg_OPCODE & 0x0F00) >> 8] ==
          cpu->reg_gen[(cpu->reg_OPCODE & 0x00F0) >> 4]) {
        inc_pc(cpu);
      };
      inc_pc(cpu);
      break;
    };
    case 0x6: {  // Assign vX nn
      cpu->reg_gen[(cpu->reg_OPCODE & 0x0F00) >> 8] =
          (cpu->reg_OPCODE & 0x00FF);
      inc_pc(cpu);
      break;
    };
    case 0x7: {  // Add nn to vX
      cpu->reg_gen[(cpu->reg_OPCODE & 0x0F00) >> 8] += cpu->reg_OPCODE & 0x00FF;
      inc_pc(cpu);
      break;
    };
    case 0x8: {
      uint8_t reg_x = (cpu->reg_OPCODE & 0x0F00) >> 8;
      uint8_t reg_y = (cpu->reg_OPCODE & 0x00F0) >> 4;

      switch (cpu->reg_OPCODE & 0x000F) {
        case 0x0: {
          cpu->reg_gen[reg_x] = cpu->reg_gen[reg_y];
          break;
        }
        case 0x1: {
          cpu->reg_gen[reg_x] |= cpu->reg_gen[reg_y];
          break;
        }
        case 0x2: {
          cpu->reg_gen[reg_x] &= cpu->reg_gen[reg_y];
          break;
        }
        case 0x3: {
          cpu->reg_gen[reg_x] ^= cpu->reg_gen[reg_y];
          break;
        }
        case 0x4: {
          const uint8_t sum = cpu->reg_gen[reg_x] + cpu->reg_gen[reg_y];
          cpu->reg_gen[0xf] = sum > 0xff ? 1 : 0;
          cpu->reg_gen[reg_x] = sum;
          break;
        }
        case 0x5: {
          cpu->reg_gen[0xf] = cpu->reg_gen[reg_y] < cpu->reg_gen[reg_x];
          cpu->reg_gen[reg_x] = cpu->reg_gen[reg_x] - cpu->reg_gen[reg_y];
          break;
        }
        case 0x6: {
          cpu->reg_gen[0xf] = cpu->reg_gen[reg_y] & 0x01;
          cpu->reg_gen[reg_x] = cpu->reg_gen[reg_y] >> 1;
          break;
        }
        case 0x7: {
          cpu->reg_gen[0xf] = cpu->reg_gen[reg_y] < cpu->reg_gen[reg_x];
          cpu->reg_gen[reg_x] = cpu->reg_gen[reg_y] - cpu->reg_gen[reg_x];
          break;
        }
        case 0xE: {
          cpu->reg_gen[0xf] = cpu->reg_gen[reg_y] >> 7;
          cpu->reg_gen[reg_x] = cpu->reg_gen[reg_y] << 1;
          break;
        }
      }
      inc_pc(cpu);
      break;
    };
    case 0x9: {
      uint8_t reg_x = (cpu->reg_OPCODE & 0x0F00) >> 8;
      uint8_t reg_y = (cpu->reg_OPCODE & 0x00F0) >> 4;
      if (reg_x != reg_y) {
        inc_pc(cpu);
      }
      inc_pc(cpu);
      break;
    };
    case 0xA: {
      cpu->reg_I = cpu->reg_OPCODE & 0x0FFF;
      inc_pc(cpu);
      break;
    };
    case 0xB: {
      cpu->reg_PC = cpu->reg_OPCODE & 0x0FFF + cpu->reg_gen[0x0];
      inc_pc(cpu);
      break;
    };
    case 0xC: {  // set vX to a random number with bitmask nn
      cpu->reg_gen[(cpu->reg_OPCODE & 0x0f00) >> 8] =
          rand() & (cpu->reg_OPCODE & 0x0ff);
      inc_pc(cpu);
      break;
    };
    case 0xD: {  // graphics
      cpu->reg_gen[0xf] = 0;
      const uint8_t reg_x = (cpu->reg_OPCODE & 0x0F00) >> 8;
      const uint8_t reg_y = (cpu->reg_OPCODE & 0x00F0) >> 4;
      const uint8_t rows = (cpu->reg_OPCODE & 0x000F);

      for (uint8_t p = 0; p < rows; p++) {
        const uint8_t data = cpu->memory[cpu->reg_I + p];

        for (uint8_t q = 0; q < 8; q++) {
          uint8_t bit = (data >> (7 - q)) & 0x01;
          if (bit) {
            uint8_t px = (reg_y + q) % SCREEN_WIDTH;
            uint8_t py = (reg_x + p) % SCREEN_HEIGHT;
            uint8_t index = py * SCREEN_WIDTH + px;

            uint8_t _pixel = cpu->display->get_pixel(cpu->display, index);
            cpu->display->set_pixel(cpu->display, index, _pixel ^= 1);

            if (_pixel) {
              cpu->reg_gen[0xf] = 1;
            }
          }
        }
      }

      inc_pc(cpu);
      break;
    };
    case 0xE: {
      uint8_t reg_x = (cpu->reg_OPCODE & 0x0F00) >> 8;

      switch (cpu->reg_OPCODE & 0x00FF) {
        case 0x9E: {
          if (cpu->keys[cpu->reg_gen[reg_x]])
            inc_pc(cpu);
          break;
        }
        case 0xA1: {
          if (!cpu->keys[cpu->reg_gen[reg_x]])
            inc_pc(cpu);
          break;
        }
      }
      inc_pc(cpu);
      break;
    };
    case 0xF: {
      uint8_t reg_x = (cpu->reg_OPCODE & 0x0F00) >> 8;

      switch (cpu->reg_OPCODE & 0x00FF) {
        case 0x07: {
          cpu->reg_gen[reg_x] = cpu->delay_timer;
          break;
        }
        case 0x0A: {  // keyboard input
          break;
        }
        case 0x15: {
          cpu->delay_timer = cpu->reg_gen[reg_x];
          break;
        }
        case 0x18: {
          cpu->sound_timer = cpu->reg_gen[reg_x];
          break;
        }
        case 0x1E: {
          cpu->reg_I += cpu->reg_gen[reg_x];
          break;
        }
        case 0x29: {  // sprites?
          cpu->reg_I = cpu->reg_gen[reg_x];
          break;
        }
        case 0x33: {
          cpu->memory[cpu->reg_I] = cpu->reg_gen[reg_x] / 100;
          cpu->memory[cpu->reg_I + 1] = (cpu->reg_gen[reg_x] / 10) % 10;
          cpu->memory[cpu->reg_I + 2] = cpu->reg_gen[reg_x] % 10;
          break;
        }
        case 0x55: {  // store values from v0 to vX
          for (uint8_t p = 0; p < reg_x; p++) {
            cpu->memory[cpu->reg_I + p] = cpu->reg_gen[p];
          }
          cpu->reg_I += reg_x + 1;
          break;
        }
        case 0x65: {  // load values from v0 to vX
          for (uint8_t p = 0; p < reg_x; p++) {
            cpu->reg_gen[p] = cpu->memory[cpu->reg_I + p];
          }
          cpu->reg_I += reg_x + 1;
          break;
        }
      }
      inc_pc(cpu);
      break;
    };
  }
}
