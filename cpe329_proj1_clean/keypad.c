#include "msp.h"
#include "delay.h"
#include "LCD.h"
#include "keypad.h"

char get_pin() {//function to identify which pin is being pressed on the keypad
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
        P4->DIR &= ~0x78;
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
    if (col == 0x06) {//0th column
        return row * 3 + 1;
    }
    else if (col == 0x05) {//1th column
        return row * 3 + 2;
    }
    else if (col == 0x03) {//2nd column
        return row * 3 + 3;
    }
    return 0;
}

void displayAndLight(char index) {
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
        P2->OUT &= ~(BIT0|BIT1|BIT2);
        LCD_WRITE('8');
        delay_ms(100,FREQ_3MHz);
        Clear_LCD();
    }
    else if (index == 9) {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
        LCD_WRITE('9');
        delay_ms(100,FREQ_3MHz);
        Clear_LCD();
    }
    else if (index == 10) {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
        LCD_WRITE('*');
        delay_ms(100,FREQ_3MHz);
        Clear_LCD();
    }
    else if (index == 11) {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
        LCD_WRITE('0');
        delay_ms(100,FREQ_3MHz);
        Clear_LCD();
    }
    else if (index == 12) {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
        LCD_WRITE('#');
        delay_ms(100,FREQ_3MHz);
        Clear_LCD();
    }
    else {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
    }
}

