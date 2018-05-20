#include <avr/io.h>
#include "serial.h"

char hexTable[16] = {'0','1','2','3','4','5','6','7','8','9',
                     'a','b','c','d','e','f'};

/*
 * Initialize the serial port.
 */
void serial_init() {
   uint16_t baud_setting;

   UCSR0A = _BV(U2X0);
   baud_setting = 16; //115200 baud

   // assign the baud_setting
   UBRR0H = baud_setting >> 8;
   UBRR0L = baud_setting;

   // enable transmit and receive
   UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
}

/*
 * Return 1 if a character is available else return 0.
 */
uint8_t byte_available() {
   return (UCSR0A & (1 << RXC0)) ? 1 : 0;
}

/*
 * Unbuffered read
 * Return 255 if no character is available otherwise return available character.
 */
uint8_t read_byte() {
   if (UCSR0A & (1 << RXC0)) return UDR0;
   return 255;
}

/*
 * Unbuffered write
 *
 * b byte to write.
 */
uint8_t write_byte(uint8_t b) {
   //loop until the send buffer is empty
   while (((1 << UDRIE0) & UCSR0B) || !(UCSR0A & (1 << UDRE0))) {}

   //write out the byte
   UDR0 = b;
   return 1;
}

void print_string(char* s) {
  while (*s != 0) {
    write_byte((uint8_t)*s);
    s++;
  }
}

void print_int(uint16_t i) {//print an 8-bit or 16-bit unsigned integer
  uint32_t highestPowerOf10 = 10000;
  
  if (i == 0) {
    write_byte((uint8_t)hexTable[i]);
    return;
  }

  while (i / highestPowerOf10 == 0) {//find highestPowerOf10
    highestPowerOf10 /= 10;
  }

  while (highestPowerOf10 > 0) {
    write_byte((uint8_t)hexTable[i/highestPowerOf10]);
    i -= (i/highestPowerOf10) * highestPowerOf10;
    highestPowerOf10 /= 10;
  }
}

void print_int32(uint32_t i) {//print an 8-bit or 16-bit unsigned integer
  uint32_t highestPowerOf10 = 1000000000;
  
  if (i == 0) {
    write_byte((uint8_t)hexTable[i]);
    return;
  }

  while (i / highestPowerOf10 == 0) {//find highestPowerOf10
    highestPowerOf10 /= 10;
  }

  while (highestPowerOf10 > 0) {
    write_byte((uint8_t)hexTable[i/highestPowerOf10]);
    i -= (i/highestPowerOf10) * highestPowerOf10;
    highestPowerOf10 /= 10;
  }
}

void print_hex(uint16_t i) {
  uint32_t highestPowerOf16 = 4096;
  print_string("0x");
  if (i == 0) {
    write_byte((uint8_t)hexTable[i]);
    return;
  }

  while (i / highestPowerOf16 == 0) {//find highestPowerOf16
    highestPowerOf16 /= 16;
  }

  while (highestPowerOf16 > 0) {
    write_byte((uint8_t)hexTable[i/highestPowerOf16]);
    i -= (i/highestPowerOf16) * highestPowerOf16;
    highestPowerOf16 /= 16;
  }
}

void print_hex32(uint32_t i) {
  uint32_t highestPowerOf16 = 268435456;
  if (i == 0) {
    write_byte((uint8_t)hexTable[i]);
    return;
  }

  while (i / highestPowerOf16 == 0) {//find highestPowerOf16
    highestPowerOf16 /= 16;
  }

  while (highestPowerOf16 > 0) {
    write_byte((uint8_t)hexTable[i/highestPowerOf16]);
    i -= (i/highestPowerOf16) * highestPowerOf16;
    highestPowerOf16 /= 16;
  }
}

void set_cursor(uint8_t row, uint8_t col) {
  uint8_t character;
  write_byte(27);//ESC
  write_byte('[');

  character = row / 100;
  write_byte(character + 48);
  row -= (row/100) * 100;
  character = row / 10;
  write_byte(character + 48);
  row -= (row/10) * 10;
  character = row / 1;
  write_byte(character + 48);

  write_byte(';');

  character = col / 100;
  write_byte(character + 48);
  col -= (col/100) * 100;
  character = col / 10;
  write_byte(character + 48);
  col -= (col/10) * 10;
  character = col / 1;
  write_byte(character + 48);

  write_byte('H');
}

void set_color(uint8_t color) {
  uint8_t character;
  write_byte(27);
  write_byte('[');

  character = color / 10;
  write_byte(character+48);
  character = color - ((color/10) * 10);
  write_byte(character+48);

  write_byte('m');
}

void clear_screen() {
  write_byte(27);
  write_byte('[');
  write_byte('2');
  write_byte('J');

  write_byte(27);
  write_byte('[');
  write_byte('H');
  
}

void erase_line() {
  write_byte(27);
  write_byte('[');
  write_byte('2');
  write_byte('K');
}
