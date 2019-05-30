//	===================================================
//
//	BLCD
//
//	Biblioteca para controle de displays LCD.
//
// 	Autor: Álvaro Duquia
//	Data: 02/04/2019
//
//	===================================================

#include <blcd.h>

void BLCD_init(BLCD_TypeDef lcd) {

	lcd.gpio->ODR &= ~(lcd.rs_pin | lcd.enable_pin | lcd.data_pins[0]
			| lcd.data_pins[1] | lcd.data_pins[2] | lcd.data_pins[3]);

	BLCD_delay_us(15 * 1000);

	BLCD_send_nibble(lcd, 0x03);
	BLCD_delay_us(5 * 1000);
	BLCD_send_nibble(lcd, 0x03);
	BLCD_delay_us(5 * 1000);
	BLCD_send_nibble(lcd, 0x03);
	BLCD_delay_us(5 * 1000);
	BLCD_send_nibble(lcd, 0x02);
	BLCD_delay_us(1 * 1000);
	BLCD_send_data(lcd, BLCD_OP_COMMAND, 0x28);
	BLCD_send_data(lcd, BLCD_OP_COMMAND, 0x0C);
	BLCD_clear(lcd);
	BLCD_send_data(lcd, BLCD_OP_COMMAND, 0x06);

}

void BLCD_create_char(BLCD_TypeDef lcd, int pos, char data[8]) {

	if (pos < 0 || pos > 7) {
		return;
	}

	BLCD_send_data(lcd, BLCD_OP_COMMAND, 0x40 | pos);

	for (int i = 0; i < 8; i++) {
		BLCD_send_data(lcd, BLCD_OP_DATA, data[i]);
	}

	BLCD_clear(lcd);

}

void BLCD_print_char(BLCD_TypeDef lcd, int pos) {

	if (pos < 0 || pos > 7) {
		return;
	}

	BLCD_send_data(lcd, BLCD_OP_DATA, pos);

}

void BLCD_clear(BLCD_TypeDef lcd) {

	BLCD_send_data(lcd, BLCD_OP_COMMAND, 0x1);

	BLCD_delay_us(20 * 1000);

}

void BLCD_gotoxy(BLCD_TypeDef lcd, int X, int Y) {

	char offset[4] = { 0x0, 0x40, 0x14, 0x54 };

	BLCD_send_data(lcd, BLCD_OP_COMMAND, 0x80 | (X + offset[Y]));

}

void BLCD_print(BLCD_TypeDef lcd, char *str) {

	while (*str) {
		BLCD_putch(lcd, *str++);
	}

}

void BLCD_putch(BLCD_TypeDef lcd, char c) {

	BLCD_send_data(lcd, BLCD_OP_DATA, c);

}

void BLCD_send_data(BLCD_TypeDef lcd, int op, char data) {

	if(op) {
		lcd.gpio->ODR |= lcd.rs_pin;
	} else {
		lcd.gpio->ODR &= ~lcd.rs_pin;
	}

	BLCD_delay_us(100);

	lcd.gpio->ODR &= ~lcd.enable_pin;

	BLCD_send_nibble(lcd, (data >> 4) & 0xF);
	BLCD_send_nibble(lcd, data & 0xF);

	BLCD_delay_us(40);

}

void BLCD_send_nibble(BLCD_TypeDef lcd, char nibble) {

	for (int i = 0; i < 4; i++) {
		if((nibble >> i) & 0x1) {
			lcd.gpio->ODR |= lcd.data_pins[i];
		} else {
			lcd.gpio->ODR &= ~lcd.data_pins[i];
		}
	}

	lcd.gpio->ODR |= lcd.enable_pin;
	BLCD_delay_us(1);
	lcd.gpio->ODR &= ~lcd.enable_pin;

}

void BLCD_delay_us(long t) {

	t *= 1.6;

	for(int i = 0; i < t; i++) {
		__asm("nop");
	}

}
