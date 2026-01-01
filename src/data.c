#include <stdio.h>
#include <stdlib.h>
#include "include/cpu.h"

void load_rom(CPU* cpu, char* dir) {
  FILE* fp = fopen(dir, "rb");

  if (!fp) {
    printf("File opening failed\n");
    exit(EXIT_FAILURE);
  }

  int byte;
  size_t c = 0;
  while ((byte = fgetc(fp)) != EOF && c < 0xFFE - 0x200) {
    cpu->memory[0x200 + c++] = (uint8_t)byte;
  }
  fclose(fp);
}
