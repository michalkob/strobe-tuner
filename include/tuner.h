#ifndef _TUNER_H

#define _TUNER_H

#include <stdint.h>
#include "tuner.h"
#include "lcd.h"
#include "vfd.h"
#include <math.h>

#define		REFERENCE_A_FREQ_DEFAULT	440
#define		REFERENCE_A_FREQ_MAX		500
#define		REFERENCE_A_FREQ_MIN		400

#define		BOTTOM_OCTAVE				2

typedef enum
{
	STEP_C			= 0,
	STEP_C_SHARP	= 1,
	STEP_D			= 2,
	STEP_D_SHARP	= 3,
	STEP_E			= 4,
	STEP_F			= 5,
	STEP_F_SHARP	= 6,
	STEP_G			= 7,
	STEP_G_SHARP	= 8,
	STEP_A			= 9,
	STEP_A_SHARP	= 10,
	STEP_B			= 11
} scale_steps;

typedef enum
{
	EQUAL,
	MEANTONE,
	WERCKMEISTER,
	YOUNG,
	PYTHAGOREAN,
	GALILEI
} scale_temperaments;

#define TEMPERAMENT_NUMBER	6

const uint32_t temperament[TEMPERAMENT_NUMBER][12];

typedef struct
{
	scale_steps			root_note;
	uint8_t				tuned_note;
	int8_t				cent_offset;
	uint16_t			A_ref_freq;		//Resolution: 0.1Hz
	scale_temperaments	temperament;
	uint8_t				drive_enabled;

	scale_steps		resultant_note;
	uint8_t			octave_index;
	int8_t			octave_offset;		//Offset in octaves from A4
	int32_t			cents_from_oct_A;	//Resolution: 0.001 cent
	uint32_t		octave_A_freq;		//Resolution: 1 uHz
	uint32_t		final_freq;			//Resolution: 1uHz
} tuner_configuration;

void init_tuner();

void apply_tuner_cfg();

tuner_configuration * tuner_cfg();

void print_pitch_name(scale_steps step, lcd_diplay_mode mode);

void print_temperament_name(scale_temperaments temperament, lcd_diplay_mode mode);

uint32_t get_temperament_offset_mc(scale_temperaments temp_type, scale_steps step);

#endif
