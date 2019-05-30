//	===================================================
//
//	Relógio: Timers
//
//	Configuração dos temporizadores.
//
// 	Autor: Álvaro Duquia
//	Data: 29/05/2019
//
//
//	Configuração:
//
//		TIM9 	(50ms): Debounce and tick w/ interrupt
//		TIM10 	  (1s):	Clock increment w/ interrupt
//
//	===================================================

#ifndef TIMERS
#define TIMERS

#include <stm32f4xx.h>

//	===================================================
//	--- Protótipos de funções ---

void timers();

void timers() {

	//	--- TIM9 ---

	RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;

	TIM9->PSC = 999;
	TIM9->ARR = 799;
	TIM9->CR1 |= TIM_CR1_CEN;

	//	--- TIM10 ---

	RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;

	TIM10->PSC = 9999;
	TIM10->ARR = 1599;
	TIM10->CR1 |= TIM_CR1_CEN;

}

#endif
