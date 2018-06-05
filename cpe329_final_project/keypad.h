#include "msp.h"
#include "LCD.h"
#include "delay.h"

static void nibble() {
    P3->OUT |= E;
    delay_us(1,FREQ_24MHz);
    P3->OUT &= ~E;
}

void LCD_CMD(unsigned char command) {
    P3->OUT &= ~(RS|RW);
    P5->OUT = command;
    nibble();
    P5->OUT = command << 4;
    nibble();
    if (command < 4) {
        delay_ms(2,FREQ_6MHz);
    }
    else {
        delay_ms(1,FREQ_6MHz);
    }
}

void LCD_INIT() {
    P3->SEL0 &= ~(RS|RW|E);
    P3->SEL1 &= ~(RS|RW|E);
    P5->SEL0 = 0x0F;
    P5->SEL1 = 0x0F;
    P3->DIR |= (RS|RW|E);
    P5->DIR = 0xF0;
    P3->OUT &= ~E;
    delay_ms(50,FREQ_6MHz);
    LCD_CMD(FSDATA);
    LCD_CMD(DISPLAY_ON);
    LCD_CMD(CLR_DISP);
}

void LCD_WRITE(unsigned char letter) {
    P3->OUT |= RS;
    P3->OUT &= ~RW;
    P5->OUT = letter;
    nibble();
    P5->OUT = letter << 4;
    nibble();
    delay_ms(1,FREQ_6MHz);
}

void Clear_LCD() {
    LCD_CMD(CLR_DISP);
}

void Home_LCD() {
    LCD_CMD(HOME);
}

void Write_char_LCD(unsigned char character) {
    P3->OUT |= RS;
    P3->OUT &= ~RW;
    P5->OUT = character;
    nibble();
    P5->OUT = character << 4;
    nibble();
    delay_ms(1,FREQ_24MHz);
}

void Write_string_LCD(char* string) {
    while (*string != (void*)0) {
        Write_char_LCD(*string);
        string++;
    }
}
