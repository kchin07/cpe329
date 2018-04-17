#include "msp.h"
#include "delay.h"
#include "LCD.h"

void main(void) {
    LCD_INIT();
    LCD_CMD(0x80);
    LCD_WRITE('A');
    while(1);
}
