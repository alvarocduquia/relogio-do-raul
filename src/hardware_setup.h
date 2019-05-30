//	===================================================
//
//	Relógio: Hardware Setup
//
//	Configuração e mapeamento do hardware.
//
// 	Autor: Álvaro Duquia
//	Data: 29/05/2019
//
//
//	Configuração:
//
//		LCD:
//			- LCD_RS 		(PC0): Digital output
//			- LCD_E 		(PC1): Digital output
//			- LCD_DB4 		(PC2): Digital output
//			- LCD_DB5 		(PC3): Digital output
//			- LCD_DB6 		(PC4): Digital output
//			- LCD_DB7 		(PC5): Digital output
//
//		ENCODER:
//			- ENCODER_A 	(PC6): Digital input w/ pull-up
//			- ENCODER_B 	(PC7): Digital input w/ pull-up
//			- ENCODER_PRESS	(PC8): Digital input w/ pull-up
//
//		SW:
//			- SW1 			(PC9): Digital input w/ pull-up
//			- SW2 		   (PC10): Digital input w/ pull-up
//
//		OUTPUT:
//			- OUTPUT 	   (PC11): Digital output
//
//	===================================================

#ifndef HARDWARE_SETUP
#define HARDWARE_SETUP

#include <stm32f4xx.h>

//	===================================================
//	--- Constantes ---

#define LCD_GPIO		GPIOC
#define LCD_RS			GPIO_ODR_ODR_0
#define LCD_E			GPIO_ODR_ODR_1
#define LCD_DB4			GPIO_ODR_ODR_2
#define LCD_DB5			GPIO_ODR_ODR_3
#define LCD_DB6			GPIO_ODR_ODR_4
#define LCD_DB7			GPIO_ODR_ODR_5

#define ENCODER_GPIO	GPIOC
#define ENCODER_A		GPIO_IDR_IDR_6
#define ENCODER_B		GPIO_IDR_IDR_7
#define ENCODER_PRESS	GPIO_IDR_IDR_8

#define SW_GPIO			GPIOC
#define SW_SW1			GPIO_IDR_IDR_9
#define SW_SW2			GPIO_IDR_IDR_10

#define OUTPUT_GPIO		GPIOC
#define OUTPUT			GPIO_ODR_ODR_11

//	===================================================
//	--- Protótipos de funções ---

void hardware_setup();

void hardware_setup() {

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	//	--- LCD ---

	GPIOC->MODER &= ~(GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1
			| GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1 | GPIO_MODER_MODER4_1
			| GPIO_MODER_MODER5_1);
	GPIOC->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0
			| GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0
			| GPIO_MODER_MODER5_0);

	//	--- ENCODER ---

	GPIOC->MODER &=
			~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7 | GPIO_MODER_MODER8);

	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR7_1
			| GPIO_PUPDR_PUPDR8_1);
	GPIOC->PUPDR |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0
			| GPIO_PUPDR_PUPDR8_0);

	//	--- SW ---

	GPIOC->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10);

	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR9_1 | GPIO_PUPDR_PUPDR10_1);
	GPIOC->PUPDR |= (GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0);

	//	--- OUTPUT ---

	GPIOC->MODER &= ~GPIO_MODER_MODER11_1;
	GPIOC->MODER |= GPIO_MODER_MODER11_0;

}

#endif
