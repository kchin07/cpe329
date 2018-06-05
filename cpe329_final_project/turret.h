#define SERVOCENTER 16800
#define MAXLEFT 17600
#define MAXRIGHT 16000
#define TENDEGREES 88
#define DEGREES180 18
#define LEFT45DEGREES 16450
#define RIGHT45DEGREES 17200
#define FOURTYDEGREES 355
#define ASCII_OFFSET 48

void pull_trigger();
void motor_on();
void motor_off();
void turret_init();
void LCD_Keypad_init();
void LCD_ammo_prompt();
int convert_ammo_string_to_int(char* ammoCountString, int ammoCountNums);
int reload_turret();
void sensor_init();
uint32_t getDistance();
