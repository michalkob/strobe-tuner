#ifndef KNOBS_BUTTONS
#define KNOBS_BUTTONS

#include <stm32f10x_adc.h>
#include <stddef.h>
#include "system.h"

#define POT_PORT	GPIOA
#define POT1_PIN	GPIO_Pin_0
#define POT2_PIN	GPIO_Pin_1

#define BUTTON_PORT	GPIOB
#define SW1_PIN		GPIO_Pin_5
#define SW2_PIN		GPIO_Pin_4
#define SW3_PIN		GPIO_Pin_2

#define ADC_STEPS	4096
#define ADC_MAX		(ADC_STEPS-1)

typedef struct
{
	void (*onPress)();
	void (*onHold)();
	uint16_t holdDelay;
	uint16_t holdInterval;
	uint64_t holdTimeout;
	uint8_t state;
} button;

typedef struct
{
	void (*onChange)(uint16_t potValue);
	uint16_t value;
	uint8_t	min_step;
} potentiometer;

typedef struct
{
	button 			ui_button[3];
	potentiometer	ui_pot[2];
} control_struct;

void init_clock();

void init_controls();

uint16_t get_adc(int channel);

void refresh_controls();

control_struct * get_controls_status();

void set_button_onPress_callback(uint8_t button, void(*callback)());

void set_button_onHold_callback(uint8_t button, void(*callback)(), uint16_t holdDelay, uint16_t holdInterval);

void set_pot_onChange_callback(uint8_t pot, void(*callback)(uint16_t potValue), uint8_t min_step);

#endif
