
#include "tuner.h"

tuner_configuration tuner;

const uint32_t temperament[TEMPERAMENT_NUMBER][12] =
{
	{-900000, 	-800000, 	-700000, 	-600000, 	-500000, 	-400000, 	-300000, 	-200000, 	-100000, 		0, 	100000, 	200000},	//equal
	{-889700, 	-813700, 	-696500, 	-579400, 	-503400, 	-386300, 	-310200, 	-192900, 	-117100, 		0, 	117100, 	193200},	//meantone
	{-888270, 	-798045, 	-696090, 	-594135, 	-498045, 	-390225, 	-300000, 	-192180, 	-96090, 		0, 	107820, 	203910},	//werckmeister
	{-893800, 	-799900, 	-698000, 	-596000, 	-502100, 	-393900, 	-301900, 	-195900, 	-98000, 		0, 	106000, 	198000},	//young
	{-905900, 	-792200, 	-702000, 	-611800, 	-498100, 	-407900, 	-294200, 	-203900, 	-113700, 		0, 	90200, 		203900},	//pythagorean
	{-891000, 	-792000, 	-693000, 	-594000, 	-495000, 	-396000, 	-297000, 	-198000, 	-99000, 		0, 	99000, 		198000}		//galilei
};


void init_tuner()
{
	tuner.A_ref_freq	= REFERENCE_A_FREQ_DEFAULT * 10;
	tuner.cent_offset	= 0;
	tuner.root_note		= STEP_C;
	tuner.tuned_note	= 12;
	tuner.temperament	= EQUAL;
	tuner.drive_enabled	= 0;
}

tuner_configuration * tuner_cfg()
{
	return &tuner;
}

void print_pitch_name(scale_steps step, lcd_diplay_mode mode)
{
	switch(step)
	{
		case STEP_C:
			LcdPutStr("C/B#", mode);
		break;

		case STEP_C_SHARP:
			LcdPutStr("C#/D@", mode);
		break;

		case STEP_D:
			LcdPutStr("D", mode);
		break;

		case STEP_D_SHARP:
			LcdPutStr("D#/E@", mode);
		break;

		case STEP_E:
			LcdPutStr("E/F@", mode);
		break;

		case STEP_F:
			LcdPutStr("F/E#", mode);
		break;

		case STEP_F_SHARP:
			LcdPutStr("F#/G@", mode);
		break;

		case STEP_G:
			LcdPutStr("G", mode);
		break;

		case STEP_G_SHARP:
			LcdPutStr("G#/A@", mode);
		break;

		case STEP_A:
			LcdPutStr("A", mode);
		break;

		case STEP_A_SHARP:
			LcdPutStr("A#/B@", mode);
		break;

		case STEP_B:
			LcdPutStr("B/C@", mode);
		break;

		default:
		break;
	}
}

void print_temperament_name(scale_temperaments temperament, lcd_diplay_mode mode)
{
	switch(temperament)
	{
		case EQUAL:
			LcdPutStr("Equal", mode);
		break;

		case MEANTONE:
			LcdPutStr("Meantone", mode);
		break;

		case WERCKMEISTER:
			LcdPutStr("Werckmeister", mode);
		break;

		case YOUNG:
			LcdPutStr("Young", mode);
		break;

		case PYTHAGOREAN:
			LcdPutStr("Pythagorean", mode);
		break;

		case GALILEI:
			LcdPutStr("Galilei", mode);
		break;
	}
}

uint32_t get_temperament_offset_mc(scale_temperaments temp_type, scale_steps step)
{
	return temperament[temp_type][step];
}

void apply_tuner_cfg()
{
	tuner.resultant_note 	= (tuner.root_note + tuner.tuned_note) % 12;
	tuner.octave_index		= (tuner.root_note + tuner.tuned_note) / 12 + BOTTOM_OCTAVE;
	tuner.octave_offset		= tuner.octave_index  - 4;	//Reference A is in the 4th octave
	tuner.octave_A_freq		= (uint32_t) tuner.A_ref_freq * 100000;
	if (tuner.octave_offset > 0)
	{
		tuner.octave_A_freq <<= tuner.octave_offset;
	}
	else if (tuner.octave_offset < 0)
	{
		tuner.octave_A_freq >>= (-tuner.octave_offset);
	}
	tuner.cents_from_oct_A = get_temperament_offset_mc(tuner.temperament, tuner.resultant_note) + (int32_t)(tuner.cent_offset * 1000);

	double	exponent = (double) tuner.cents_from_oct_A	/ 1200000;	//Calculate the exponent for 2^exp
	double	ratio	 = pow(2, exponent);							//Calculate the freq ratio
	double  result	 = ratio * (double) tuner.octave_A_freq;		//Calculate the final frequency in uHz
	result += 0.0005;
	result *= 0.001;												//Convert to mHz
	tuner.final_freq = (uint32_t) result;							//Downscale to integer mHz
	if (tuner.drive_enabled)	set_vfd_target_speed(tuner.final_freq);
	else						set_vfd_target_speed(0);
}
