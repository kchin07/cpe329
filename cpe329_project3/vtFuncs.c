#include "vtFuncs.h"
#include "adc.h"

char hexTable[16] = {'0','1','2','3','4','5','6','7','8','9',
                     'a','b','c','d','e','f'};

void set_cursor(uint8_t row, uint8_t col) {
  uint8_t character;
  write_to_terminal(27);//ESC
  write_to_terminal('[');

  character = row / 100;
  write_to_terminal(character + 48);
  row -= (row/100) * 100;
  character = row / 10;
  write_to_terminal(character + 48);
  row -= (row/10) * 10;
  character = row / 1;
  write_to_terminal(character + 48);

  write_to_terminal(';');

  character = col / 100;
  write_to_terminal(character + 48);
  col -= (col/100) * 100;
  character = col / 10;
  write_to_terminal(character + 48);
  col -= (col/10) * 10;
  character = col / 1;
  write_to_terminal(character + 48);

  write_to_terminal('H');
}

void clear_screen() {
  write_to_terminal(27);
  write_to_terminal('[');
  write_to_terminal('2');
  write_to_terminal('J');

  write_to_terminal(27);
  write_to_terminal('[');
  write_to_terminal('H');
}

void print_string(char* s) {
  while (*s != 0) {
    write_to_terminal((uint8_t)*s);
    s++;
  }
}

void print_int32(uint32_t i) {
  uint32_t highestPowerOf10 = 1000000000;

  if (i == 0) {
    write_to_terminal((uint8_t)hexTable[i]);
    return;
  }

  while (i / highestPowerOf10 == 0) {//find highestPowerOf10
    highestPowerOf10 /= 10;
  }

  while (highestPowerOf10 > 0) {
    write_to_terminal((uint8_t)hexTable[i/highestPowerOf10]);
    i -= (i/highestPowerOf10) * highestPowerOf10;
    highestPowerOf10 /= 10;
  }
}


