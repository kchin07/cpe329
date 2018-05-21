#include "msp.h"

#ifndef VTFUNCS_H_
#define VTFUNCS_H_

void set_cursor(uint8_t row, uint8_t col);
void clear_screen();
void print_int32(uint32_t i);
void print_string(char* s);
void erase_line();


#endif /* VTFUNCS_H_ */
