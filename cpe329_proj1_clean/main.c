#include "msp.h"
#include "LCD.h"
#include "delay.h"
#include "keypad.h"

void main(void) {
    char index;

    LCD_INIT();//initializes the LCD screen
    LCD_CMD(0x80); //sets LCD to output to top row of screen

    /*initializes P4.0-P4.6 to general purpose I/O*/
    P4->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);
    P4->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);

    P4->DIR = 0x78;//MAKE P4.3-P4.6 outputs, rest are inputs
    P4->REN = 0x07;//make P4.0-P4.2 resistors enabled
    P4->OUT = 0x07;//make P4.0-P4.2 resistors set as pull-up

    delay_ms(1,FREQ_3MHz);

    char* string1 = "LOCKED";
    char* string2 = "ENTER KEY: ";
    char* string3 = "HELLO WORLD";

    Write_string_LCD(string1); /*prints "LOCKED" on LCD*/
    LCD_CMD(0xC0); /*switches LCD to output to bottom of screen*/
    Write_string_LCD(string2);  /*prints "ENTER KEY" on LCD*/

    int code[4] = {1,2,3,4}; /*initializes password array*/
    int* input = malloc(4*sizeof(int)); /*creates input array to store keypad inputs*/
    int inputIndex = 0;

    while (1) { /*continuously takes in inputs from keypad*/
        index = get_pin(); /*gets single input number*/
        if (index == 11){ /* 0 input converted into 20 to pass condition that int > 0*/
            index = 20;
        }
        if (index == 10){ /*reset process, clear input array, reinitialize screen*/
            input[0] = 0;
            input[1] = 0;
            input[2] = 0;
            input[3] = 0;
            Clear_LCD();
            LCD_CMD(0x80);
            Write_string_LCD(string1);
            LCD_CMD(0xC0);
            Write_string_LCD(string2);
            inputIndex = 0;
        }
        else{
            if(index > 0) { /*input on keypad*/
                if(index == 20){ /*int 20 (which was a 0 input) converted back to a 0 to be inserted into the input array*/
                    index = 0;
                }
                input[inputIndex] = index; /*add input number into input array*/
                Clear_LCD(); /*clear screen, display input prompt*/
                LCD_CMD(0x80);
                Write_string_LCD(string1);
                LCD_CMD(0xC0);
                Write_string_LCD(string2);
                inputIndex ++;
                int i;
                for(i = 0; i < inputIndex; i++){ /*display all input numbers*/
                    Write_char_LCD(input[i]+48);
                }
                delay_ms(100, FREQ_3MHz);
                index = 0;
            }
            if(inputIndex == 4){ /*checks for set of 4 inputs*/
                int i;
                int notSameFlag = 1;
                for (i = 0; i < 4; i++){ /*compare input numbers with password numbers*/
                    if(input[i] != code[i]){
                        notSameFlag = 0;
                        break;
                    }
                }
                if (notSameFlag){ /*correct combo is inputed*/
                    Clear_LCD();
                    LCD_CMD(0x80); /*display "HELLO WORLD" on LCD*/
                    Write_string_LCD(string3);
                    delay_ms(100, FREQ_3MHz);
                    code[0] = (65535^ code[0]*7) % 10; /*shuffle code*/
                    code[1] = (65535^ code[1]*7) % 10;
                    code[2] = (65535^ code[2]*7) % 10;
                    code[3] = (65535^ code[3]*7) % 10;
                }
                input[0] = 0;/*reset input code to all 0's*/
                input[1] = 0;
                input[2] = 0;
                input[3] = 0;
                inputIndex = 0;
            }
        }
    }
}
