//	===================================================
//
//	Relógio
//
//	Trabalho para a disciplina de Sistema Microprocessados II.
//
// 	Autor: Álvaro Duquia
//	Data: 29/05/2019
//
//	===================================================

#include <stm32f4xx.h>

#include <hardware_setup.h>
#include <blcd.h>
#include <timers.h>

//	===================================================
//	--- Constantes ---

//	===================================================
//	--- Objetos ---

typedef enum {

	BOOT_SCREEN, CLOCK_ADJUST_SCREEN, MAIN_SCREEN, WARNING_SCREEN

} SCREEN_Typedef;

typedef enum {

	COMMAND_UP,
	COMMAND_DOWN,
	COMMAND_PRESS,
	COMMAND_LONG_PRESS,
	COMMAND_SW1,
	COMMAND_SW2

} COMMAND_Typedef;

typedef struct {

	int hour;
	int minute;
	int second;

} TIME_Typedef;

BLCD_TypeDef lcd;

TIME_Typedef clock;
TIME_Typedef schedule_on, schedule_off;

//	===================================================
//	--- Variáveis ---

char clock_adjusted = 0x0, schedule_adjusted = 0x0;

int adjust_field = 0;
int adjusting_schedule = 0;

char last_encoder_state = 0x0;
char press_flag = 0x0, sw1_flag = 0x0, sw2_flag = 0x0;
char arnaldo = 0x0;
uint32_t galvao = 0;

char load = 0x0;
char warning = 0x0, warning_state = 0x0;

uint32_t tick = 0; // T = 50ms

//	===================================================
//	--- Protótipos de funções ---

void screen(SCREEN_Typedef screen);
void update_screen(SCREEN_Typedef screen);
void command(COMMAND_Typedef command);

//	===================================================
//	--- Interrupções ---

int main(void) {

	hardware_setup();

	clock.hour = 12;
	clock.minute = 30;
	clock.second = 0;

	schedule_on.hour = 13;
	schedule_on.minute = 30;
	schedule_on.second = 0;

	schedule_off.hour = 18;
	schedule_off.minute = 45;
	schedule_off.second = 0;

	lcd.gpio = LCD_GPIO;

	lcd.rs_pin = LCD_RS;
	lcd.enable_pin = LCD_E;

	lcd.data_pins[0] = LCD_DB4;
	lcd.data_pins[1] = LCD_DB5;
	lcd.data_pins[2] = LCD_DB6;
	lcd.data_pins[3] = LCD_DB7;

	BLCD_init(lcd);

	screen(BOOT_SCREEN);

	BLCD_delay_us(4 * 1000 * 1000);

	screen(CLOCK_ADJUST_SCREEN);
	update_screen(CLOCK_ADJUST_SCREEN);

	timers();

	while (1) {
		if (TIM9->SR & TIM_SR_UIF) {
			TIM9->SR &= ~TIM_SR_UIF;

			//	--- PRESS DEBOUNCE ---

			if (!(ENCODER_GPIO->IDR & ENCODER_PRESS)) {
				if (!press_flag) {
					press_flag = 0x1;

					command(COMMAND_PRESS);

					galvao = tick;
				}

				if ((tick - galvao) >= 60) {
					if (!arnaldo) {
						command(COMMAND_LONG_PRESS);

						arnaldo = 0x1;
					}
				}
			} else {
				press_flag = 0x0;

				arnaldo = 0x0;
			}

			//	--- SW1 DEBOUNCE ---

			if (!(SW_GPIO->IDR & SW_SW1)) {
				if (!sw1_flag) {
					sw1_flag = 0x1;

					command(COMMAND_SW1);
				}
			} else {
				sw1_flag = 0x0;
			}

			//	--- SW2 DEBOUNCE ---

			if (!(SW_GPIO->IDR & SW_SW2)) {
				if (!sw2_flag) {
					sw2_flag = 0x1;

					command(COMMAND_SW2);
				}
			} else {
				sw2_flag = 0x0;
			}

			tick++;
		}

		if (TIM10->SR & TIM_SR_UIF) {
			TIM10->SR &= ~TIM_SR_UIF;

			if (clock_adjusted) {
				if (++clock.second > 59) {
					clock.second = 0;

					if (++clock.minute > 59) {
						clock.minute = 0;

						if (++clock.hour > 23) {
							clock.hour = 0;
						}
					}
				}

				if (schedule_adjusted && adjusting_schedule == 0) {
					if (clock.hour == schedule_on.hour
							&& clock.minute == schedule_on.minute
							&& clock.second == 0) {
						OUTPUT_GPIO->ODR |= OUTPUT;

						load = 0x1;

						update_screen(MAIN_SCREEN);
					} else if (clock.hour == schedule_off.hour
							&& clock.minute == schedule_off.minute
							&& clock.second == 0) {
						OUTPUT_GPIO->ODR &= ~OUTPUT;

						load = 0x0;

						warning = 0x1;
						warning_state = 0x0;

						schedule_adjusted = 0x0;

						screen(WARNING_SCREEN);
						update_screen(WARNING_SCREEN);
					} else {
						update_screen(MAIN_SCREEN);
					}
				} else {
					if (!warning) {
						update_screen(MAIN_SCREEN);
					}
				}
			}
		}

		// --- ENCODER DEBOUNCE ---

		char encoder_state = (ENCODER_GPIO->IDR & ENCODER_A) ? 0x1 : 0x0;

		if (encoder_state != last_encoder_state) {
			char encoder_direction =
					(ENCODER_GPIO->IDR & ENCODER_B) ? 0x1 : 0x0;

			if (encoder_state != encoder_direction) {
				command(COMMAND_UP);
			} else {
				command(COMMAND_DOWN);
			}
		}

		last_encoder_state = encoder_state;
	}

}

void screen(SCREEN_Typedef screen) {

	BLCD_clear(lcd);

	switch (screen) {
	case BOOT_SCREEN: {

		BLCD_gotoxy(lcd, 2, 0);
		BLCD_print(lcd, "Trabalho Relogio");
		BLCD_gotoxy(lcd, 2, 2);
		BLCD_print(lcd, "Alvaro C. Duquia");
		BLCD_gotoxy(lcd, 8, 3);
		BLCD_print(lcd, "4423");

		break;

	}
	case CLOCK_ADJUST_SCREEN: {

		BLCD_gotoxy(lcd, 3, 0);
		BLCD_print(lcd, "Ajuste horario");

		break;

	}
	case MAIN_SCREEN: {

		BLCD_gotoxy(lcd, 3, 2);
		BLCD_putch(lcd, 'L');
		BLCD_gotoxy(lcd, 16, 2);
		BLCD_putch(lcd, 'D');

		break;

	}
	case WARNING_SCREEN: {

		BLCD_gotoxy(lcd, 2, 0);
		BLCD_print(lcd, "Manter horarios?");

		break;

	}
	default:
		break;
	}

}

void update_screen(SCREEN_Typedef screen) {

	switch (screen) {
	case CLOCK_ADJUST_SCREEN: {

		BLCD_gotoxy(lcd, 6, 2);

		char str[20];
		sprintf(str, "%02d:%02d:%02d", clock.hour, clock.minute, clock.second);
		BLCD_print(lcd, str);

		switch (adjust_field) { // ajusta o cursor piscante
		case 0:
			BLCD_gotoxy(lcd, 7, 2);
			break;
		case 1:
			BLCD_gotoxy(lcd, 10, 2);
			break;
		case 2:
			BLCD_gotoxy(lcd, 13, 2);
			break;
		default:
			break;
		}

		BLCD_send_data(lcd, BLCD_OP_COMMAND, 0x0F); // habilita cursor

		break;

	}
	case MAIN_SCREEN: {

		BLCD_gotoxy(lcd, 6, 0);

		char str[20];
		sprintf(str, "%02d:%02d:%02d", clock.hour, clock.minute, clock.second);
		BLCD_print(lcd, str);

		BLCD_gotoxy(lcd, 1, 3);

		if (schedule_adjusted || adjusting_schedule > 0) {
			if (load) {
				BLCD_print(lcd, "LIGAD");
			} else {
				char str2[20];
				sprintf(str2, "%02d:%02d", schedule_on.hour, schedule_on.minute,
						schedule_on.second);
				BLCD_print(lcd, str2);
			}
		} else {
			BLCD_print(lcd, "--:--");
		}

		BLCD_gotoxy(lcd, 14, 3);

		if (schedule_adjusted || adjusting_schedule > 0) {
			char str3[20];
			sprintf(str3, "%02d:%02d", schedule_off.hour, schedule_off.minute,
					schedule_off.second);
			BLCD_print(lcd, str3);
		} else {
			BLCD_print(lcd, "--:--");
		}

		if (adjusting_schedule > 0) {
			if (adjusting_schedule == 1) {
				switch (adjust_field) { // ajusta o cursor piscante (on)
				case 0:
					BLCD_gotoxy(lcd, 2, 3);
					break;
				case 1:
					BLCD_gotoxy(lcd, 5, 3);
					break;
				default:
					break;
				}
			} else if (adjusting_schedule == 2) {
				switch (adjust_field) { // ajusta o cursor piscante (off)
				case 0:
					BLCD_gotoxy(lcd, 15, 3);
					break;
				case 1:
					BLCD_gotoxy(lcd, 18, 3);
					break;
				default:
					break;
				}
			}

			BLCD_send_data(lcd, BLCD_OP_COMMAND, 0x0F); // habilita cursor
		} else {
			BLCD_send_data(lcd, BLCD_OP_COMMAND, 0x0C); // desabilita cursor
		}

		break;

	}
	case WARNING_SCREEN: {

		BLCD_gotoxy(lcd, 8, 2);

		if (warning_state) {
			BLCD_print(lcd, "Yep!");
		} else {
			BLCD_print(lcd, "Nop.");
		}

		BLCD_send_data(lcd, BLCD_OP_COMMAND, 0x0C); // desabilita cursor

		break;

	}
	default:
		break;
	}

}

void command(COMMAND_Typedef command) {

	if(command == COMMAND_SW2) {
		NVIC_SystemReset();

		return;
	}

	if (!clock_adjusted) { // ajustando o relógio
		if (command == COMMAND_UP) { // subiu encoder na hora de ajustar o relógio: incrementa campo atual
			if (adjust_field == 0) {	// ajustando a hora
				if (++clock.hour > 23) {
					clock.hour = 0;
				}
			} else if (adjust_field == 1) { // ajustando o minuto
				if (++clock.minute > 59) {
					clock.minute = 0;
				}
			} else if (adjust_field == 2) { // ajustando os segundos
				if (++clock.second > 59) {
					clock.second = 0;
				}
			}

			update_screen(CLOCK_ADJUST_SCREEN);
		} else if (command == COMMAND_DOWN) { // desceu encoder na hora de ajustar o relógio: decrementa campo atual
			if (adjust_field == 0) { // ajustando a hora
				if (--clock.hour < 0) {
					clock.hour = 23;
				}
			} else if (adjust_field == 1) { // ajustando o minuto
				if (--clock.minute < 0) {
					clock.minute = 59;
				}
			} else if (adjust_field == 2) { // ajustando os segundos
				if (--clock.second < 0) {
					clock.second = 59;
				}
			}

			update_screen(CLOCK_ADJUST_SCREEN);
		} else if (command == COMMAND_PRESS) { // pressionou o encoder na hora de ajustar o relógio: pula de campo
			if (++adjust_field > 2) {
				adjust_field = 0;
			}

			update_screen(CLOCK_ADJUST_SCREEN);
		} else if (command == COMMAND_LONG_PRESS) { // pressionou longamente o encoder na hora de ajustar o relógio: confirma ajuste
			screen(MAIN_SCREEN);
			update_screen(MAIN_SCREEN);

			clock_adjusted = 0x1;
		}
	} else {
		if (command == COMMAND_SW1) {
			if (!warning) {
				if (adjusting_schedule == 0) {
					adjusting_schedule = (!load) ? 1 : 2; // se a carga estiver ligada, ajusta só o tempo de desligamento
					adjust_field = 0;

					update_screen(MAIN_SCREEN);
				} else if (adjusting_schedule == 1) {
					adjusting_schedule = 2;
					adjust_field = 0;

					update_screen(MAIN_SCREEN);
				} else if (adjusting_schedule == 2) {
					adjusting_schedule = 0;

					schedule_adjusted = 0x1;

					update_screen(MAIN_SCREEN);
				}
			}
		} else if (command == COMMAND_UP) {
			if (warning) {
				warning_state = 0x1;

				update_screen(WARNING_SCREEN);
			} else {
				if (adjusting_schedule == 1) {
					if (adjust_field == 0) {	// ajustando a hora
						if (++schedule_on.hour > 23) {
							schedule_on.hour = 0;
						}
					} else if (adjust_field == 1) { // ajustando o minuto
						if (++schedule_on.minute > 59) {
							schedule_on.minute = 0;
						}
					}

					update_screen(MAIN_SCREEN);
				} else if (adjusting_schedule == 2) {
					if (adjust_field == 0) {	// ajustando a hora
						if (++schedule_off.hour > 23) {
							schedule_off.hour = 0;
						}
					} else if (adjust_field == 1) { // ajustando o minuto
						if (++schedule_off.minute > 59) {
							schedule_off.minute = 0;
						}
					}

					update_screen(MAIN_SCREEN);
				}
			}
		} else if (command == COMMAND_DOWN) {
			if (warning) {
				warning_state = 0x0;

				update_screen(WARNING_SCREEN);
			} else {
				if (adjusting_schedule == 1) {
					if (adjust_field == 0) {	// ajustando a hora
						if (--schedule_on.hour < 0) {
							schedule_on.hour = 23;
						}
					} else if (adjust_field == 1) { // ajustando o minuto
						if (--schedule_on.minute < 0) {
							schedule_on.minute = 59;
						}
					}

					update_screen(MAIN_SCREEN);
				} else if (adjusting_schedule == 2) {
					if (adjust_field == 0) {	// ajustando a hora
						if (--schedule_off.hour < 0) {
							schedule_off.hour = 23;
						}
					} else if (adjust_field == 1) { // ajustando o minuto
						if (--schedule_off.minute < 0) {
							schedule_off.minute = 59;
						}
					}

					update_screen(MAIN_SCREEN);
				}
			}
		} else if (command == COMMAND_PRESS) {
			if (warning) {
				if (warning_state) {
					schedule_adjusted = 0x1;
				} else {
					schedule_on.hour = 13;
					schedule_on.minute = 30;
					schedule_on.second = 0;

					schedule_off.hour = 18;
					schedule_off.minute = 45;
					schedule_off.second = 0;
				}

				warning = 0x0;

				screen(MAIN_SCREEN);
				update_screen(MAIN_SCREEN);
			} else {
				if (adjusting_schedule > 0) {
					if (++adjust_field > 1) {
						adjust_field = 0;
					}

					update_screen(MAIN_SCREEN);
				}
			}
		}
	}

}
