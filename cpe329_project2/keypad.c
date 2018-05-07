#include "msp.h"
#include "delay.h"
#include "keypad.h"

char get_pin() {//function to identify which pin is being pressed on the keypad
    int row_select = BIT3;
    int col,row;

    P4->DIR |= 0x78;
    P4->OUT &= ~0x78;
    delay_us(1,FREQ_3MHz);
    col = P4->IN & 0x78;
    P4->OUT |= 0x78;
    P4->DIR &= ~0x78;
    if (col == 0x07) return 0;

    for (row = 0; row < 4; row++) {
        P4->DIR &= ~0x78;
        P4->DIR |= row_select;
        P4->OUT &= ~row_select;
        delay_us(1,FREQ_3MHz);
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

void displayAndLight(char pinId) {
    if (pinId == 1) {
        P2->OUT |= BIT0;
    }
    else if (pinId == 2) {
        P2->OUT |= BIT1;
    }
    else if (pinId == 3) {
        P2->OUT |= BIT2;
    }
    else if (pinId == 4) {
        P2->OUT |= BIT0|BIT1;
    }
    else if (pinId == 5) {
        P2->OUT |= BIT0|BIT2;
    }
    else if (pinId == 6) {
        P2->OUT |= BIT1|BIT2;
    }
    else if (pinId == 7) {
        P2->OUT |= BIT0|BIT1|BIT2;
    }
    else if (pinId == 8) {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
    }
    else if (pinId == 9) {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
    }
    else if (pinId == 10) {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
    }
    else if (pinId == 11) {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
    }
    else if (pinId == 12) {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
    }
    else {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
    }
}
