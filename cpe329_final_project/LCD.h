#ifndef LCD_H_
#define LCD_H_

#define RS BIT5
#define RW BIT6
#define E  BIT7

#define FSDATA     0x2F
#define DISPLAY_ON 0x0C
#define CLR_DISP   0x01
#define HOME       0x02

void LCD_CMD(unsigned char command);
void LCD_INIT();
void LCD_WRITE(unsigned char letter);
void Clear_LCD();
void Home_LCD();
void Write_char_LCD(unsigned char character);
void Write_string_LCD(char* string);

#endif
