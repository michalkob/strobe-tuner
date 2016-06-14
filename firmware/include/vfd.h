#ifndef _VFD_H

#define _VFD_H

#include <stddef.h>
#include <stm32f10x_gpio.h>

#define	VFD_PHASE1_PIN	GPIO_Pin_8
#define	VFD_PHASE2_PIN	GPIO_Pin_9
#define	VFD_PHASE3_PIN	GPIO_Pin_10

#define	SINE_PP_VALUE	250
#define	DDS_REG_CAP		24000000	//24E6 counts per 1 cycle @ 24kHz = 0.001Hz resolution
#define DDS_DIVISOR		93750		//24.000.000 / 93750 = 256
#define	SINE_ARR_SZ		256

#define	DDS_PH1_INIT	((DDS_REG_CAP / 3) * 0)	// 0 deg
#define	DDS_PH2_INIT	((DDS_REG_CAP / 3) * 1)	// 120 deg
#define	DDS_PH3_INIT	((DDS_REG_CAP / 3) * 2)	// 240 deg

#define	DDS_AMPL_MAX	8	//Must be 2^n
#define DDS_AMPL_SHIFT	3

#define	VFD_RAMP_SPEED	1	//1 unit = 12Hz/s

const uint16_t	sine_table[256];

typedef enum
{
	VFD_STOPPED,
	VFD_SPIN_UP,
	VFD_SPIN_DOWN,
	VFD_IN_SYNC
} vfd_states;

typedef struct
{
	uint32_t 	reg[3];
	uint16_t	out[3];
	uint32_t	curr_speed_mHz;
	uint32_t	target_speed_mHz;
	uint8_t		ampl;
	uint8_t		ramp_speed;
	vfd_states	status;
} DDS_data;

void init_vfd();

void set_vfd_outputs();

void TIM1_UP_IRQHandler();

void NVIC_config();

vfd_states get_vfd_status();

void set_vfd_target_speed(uint32_t speed_mHz);

void vfd_set_amplitude(uint8_t ampl);

#endif
