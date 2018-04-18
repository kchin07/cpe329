#include "msp.h"
#include "LCD.h"
#include "delay.h"
#include <stdio.h>

char get_key() {
    int row_select = BIT3;
    int col,row;

    P4->DIR |= 0x78;
    P4->OUT &= ~0x78;
    delay_ms(1,FREQ_3MHz);
    col = P4->IN & 0x78;
    P4->OUT |= 0x78;
    P4->DIR &= ~0x78;
    if (col == 0x07) return 0;

    for (row = 0; row < 4; row++) {
        P4->DIR &= ~0x78;//disable rows
        P4->DIR |= row_select;
        P4->OUT &= ~row_select;
        delay_ms(1,FREQ_3MHz);
        col = P4->IN & 0x07;
        P4->OUT |= row_select;
        if (col != 0x07) break;
        row_select <<= 1;
    }
    P4->OUT |= 0x78;
    P4->DIR &= ~0x78;
    if (row == 4) return 0;
    if (col == 0x06) {
        return row * 3 + 1;
    }
    else if (col == 0x05) {
        return row * 3 + 2;
    }
    else if (col == 0x03) {
        return row * 3 + 3;
    }
    return 0;
}

void main(void) {
    int col,row,row_select=BIT3;
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

    while (1) {
        index = get_key();
        if (index == 1) {
            P2->OUT |= BIT0;
            LCD_WRITE('1');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 2) {
            P2->OUT |= BIT1;
            LCD_WRITE('2');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 3) {
            P2->OUT |= BIT2;
            LCD_WRITE('3');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 4) {
            P2->OUT |= BIT0|BIT1;
            LCD_WRITE('4');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 5) {
            P2->OUT |= BIT0|BIT2;
            LCD_WRITE('5');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 6) {
            P2->OUT |= BIT1|BIT2;
            LCD_WRITE('6');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 7) {
            P2->OUT |= BIT0|BIT1|BIT2;
            LCD_WRITE('7');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 8) {
            P2->OUT |= BIT0|BIT1|BIT2;
            LCD_WRITE('8');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 9) {
            P2->OUT |= BIT0|BIT1|BIT2;
            LCD_WRITE('9');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 10) {
            P2->OUT |= BIT0|BIT1|BIT2;
            LCD_WRITE('*');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 11) {
            P2->OUT |= BIT0|BIT1|BIT2;
            LCD_WRITE('0');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else if (index == 12) {
            P2->OUT |= BIT0|BIT1|BIT2;
            LCD_WRITE('#');
            delay_ms(100,FREQ_3MHz);
            Clear_LCD();
        }
        else {
            P2->OUT &= ~(BIT0|BIT1|BIT2);
        }

    }

/*
    LCD_INIT();
    LCD_CMD(0x80);
    LCD_WRITE('A');
    while(1);*/
}
