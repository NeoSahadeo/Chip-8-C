#include <assert.h>
#include "include/cpu.h"

void test_opcode() {
  CPU cpu;
  cpu.reg_PC = 0;
  cpu.memory[0] = 0x01;
  cpu.memory[1] = 0x0A;

  // 0000000000000000
  // 0000000100000000
  // 0000000100001010
  cpu.reg_OPCODE = (cpu.memory[cpu.reg_PC] << 8) | cpu.memory[cpu.reg_PC + 1];
  assert(cpu.reg_OPCODE == 266 && "Failed OPCODE load");
  assert(cpu.memory[0] == 0x01 && "Value mutated");
}

void test_all() {
  test_opcode();
}
