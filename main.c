#include "msp.h"
#include "LCD.h"
#include "delay.h"
#include <stdio.h>

void main(void) {
    int col,row,row_select=1;

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

    while (1) {
        int i;
        int num;
        for (i = 0; i < 4; i++){
                P4->DIR &= 0x78; /* disable all rows */
                P4->DIR |= 0x08 << i; /* enable one row at a time */
                P4->OUT &= ~(0x08 << i); /* drive the active row low */
                delay_ms(1, FREQ_3MHz); /* wait for signal to settle */
                col = P4->IN & 0x07; /* read all columns */
                P4->OUT |= 0x08 << i; /* drive the active row high */
                if (col != 0xF0) break; /* if one of the input is low, some key is pressed. */
        }

        col = P4->IN & 0x07;
        if (col == 0x06 || col == 0x05 || col == 0x03) {
            if (i % 2 == 0) {//even
                P2->OUT |= BIT0;//red
            }
            else {
                P2->OUT |= BIT2;
            }
        }
        else {
            P2->OUT &= ~(BIT0|BIT1|BIT2);

        }
        /*if (i == 4) {
            P2->OUT &= ~(BIT0|BIT1|BIT2);
        }
        else if(col == 0x06){
            num = 3*i + 1;
                P2->OUT |= num;

        }
        else if (col == 0x05){
            num = 3*i + 2;
                P2->OUT |= num;
        }
        else if (col == 0x03){
            num = 3*i + 2;
                P2->OUT |= num;
        }
        else {
                P2->OUT &= ~(BIT0|BIT1|BIT2);
        }

        /*col = P4->IN & 0x07; // read all columns

        if (col == 0x06){// 4.0 column pressed
            P2->OUT |= BIT0;
            LCD_WRITE('A');
        }
        else if (col == 0x05){// 4.1 column pressed
            P2->OUT |= BIT1;
            LCD_WRITE('B');
        }
        else if (col == 0x03){// 4.2 column pressed
            P2->OUT |= BIT2;
            LCD_WRITE('C');
        }
        else {
            P2->OUT &= ~(BIT0|BIT1|BIT2);
        }*/

        /*if (col == 0x06 || col == 0x05 || col == 0x03) {
            P4->DIR &= 0x78;
        }*/

    }


    LCD_INIT();
    LCD_CMD(0x80);
    LCD_WRITE('A');
    while(1);
}
