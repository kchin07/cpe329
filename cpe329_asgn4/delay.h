#ifndef DELAY_H_
#define DELAY_H_

#define FREQ_1P5MHz 1.5
#define FREQ_3MHz 3
#define FREQ_6MHz 6
#define FREQ_12MHz 12
#define FREQ_24MHz 24
#define FREQ_48MHz 48

int set_dco(int input);
void delay_ms(int numSec, int sysFreq);
void delay_us(int numSec, int sysFreq);

#endif
