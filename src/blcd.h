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

#ifndef BLCD_H
#define BLCD_H

#include <stm32f4xx.h>

//	===================================================
//	--- Constantes ---

#define BLCD_OP_COMMAND		0x0
#define BLCD_OP_DATA		0x1

//	===================================================
//	--- Objetos ---

typedef struct {

	GPIO_TypeDef *gpio;

	uint16_t rs_pin;
	uint16_t enable_pin;

	uint16_t data_pins[4];

} BLCD_TypeDef;

//	===================================================
//	--- Protótipos de funções ---

void BLCD_init(BLCD_TypeDef lcd);

void BLCD_create_char(BLCD_TypeDef lcd, int pos, char data[8]);
void BLCD_print_char(BLCD_TypeDef lcd, int pos);

void BLCD_clear(BLCD_TypeDef lcd);
void BLCD_gotoxy(BLCD_TypeDef lcd, int X, int Y);
void BLCD_print(BLCD_TypeDef lcd, char *str);
void BLCD_putch(BLCD_TypeDef lcd, char c);

void BLCD_send_data(BLCD_TypeDef lcd, int op, char data);
void BLCD_send_nibble(BLCD_TypeDef lcd, char nibble);

void BLCD_delay_us(long t);

#endif
