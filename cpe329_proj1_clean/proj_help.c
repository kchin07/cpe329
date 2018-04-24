#include "msp.h"
#include "delay.h"
#include "LCD.h"
#include "keypad.h"

void display(char index) {
    if (index == 1) {
        LCD_WRITE('1');
        delay_ms(100,FREQ_3MHz);
    }
    else if (index == 2) {
        LCD_WRITE('2');
        delay_ms(100,FREQ_3MHz);
    }
    else if (index == 3) {
        LCD_WRITE('3');
        delay_ms(100,FREQ_3MHz);
    }
    else if (index == 4) {
        LCD_WRITE('4');
        delay_ms(100,FREQ_3MHz);
    }
    else if (index == 5) {
        LCD_WRITE('5');
        delay_ms(100,FREQ_3MHz);

    }
    else if (index == 6) {
        LCD_WRITE('6');
        delay_ms(100,FREQ_3MHz);

    }
    else if (index == 7) {
        LCD_WRITE('7');
        delay_ms(100,FREQ_3MHz);

    }
    else if (index == 8) {
        LCD_WRITE('8');
        delay_ms(100,FREQ_3MHz);

    }
    else if (index == 9) {
        LCD_WRITE('9');
        delay_ms(100,FREQ_3MHz);

    }
    else if (index == 10) {
        LCD_WRITE('*');
        delay_ms(100,FREQ_3MHz);

    }
    else if (index == 11) {
        LCD_WRITE('0');
        delay_ms(100,FREQ_3MHz);

    }
    else if (index == 12) {
        LCD_WRITE('#');
        delay_ms(100,FREQ_3MHz);
    }
    else {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
    }
}
