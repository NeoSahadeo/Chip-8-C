#include "cpu.h"
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

void load_rom(char *filename, CPU *cpu) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    perror("Error opening file!");
    exit(EXIT_FAILURE);
  }

  int count = 0;
  int c;
  while ((c = fgetc(file)) != EOF) {
    cpu->memory[count++ + 0x200] = (unsigned char)c;
  }
  fclose(file);
}
