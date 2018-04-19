#include "msp.h"
#include "LCD.h"
#include "delay.h"
#include "keypad.h"

void main(void) {
    char index;

    LCD_INIT();
    LCD_CMD(0x80);

    P4->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);
    P4->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);

    P4->DIR = 0x78;//MAKE P4.3-P4.6 outputs, rest are inputs
    P4->REN = 0x07;//make P4.0-P4.2 resistors enabled
    P4->OUT = 0x07;//make P4.0-P4.2 resistors set as pull-up

    P2->SEL1 &= ~(BIT0|BIT1|BIT2);  /* configure P2.0-P2.2 as simple I/O */
    P2->SEL0 &= ~(BIT0|BIT1|BIT2);
    P2->DIR |= BIT0|BIT1|BIT2;          /* P2.0-P2.2 set as output pin */
    delay_ms(1,FREQ_3MHz);

    while (1) {//this is for turning on the correct LED color as well as
               //displaying the correct character on the LCD
        index = get_pin();
        displayAndLight(index);
    }
}
