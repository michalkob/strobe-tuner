#include "menu.h"

volatile menu_items menu;

void init_menu_items()
{
	menu.screen = WELCOME;
	menu.settings_scr.selection = 		SEL_ROOT;
	menu.lcd_settings_scr.selection = 	SEL_SCR_BRIGHT;

	set_button_onPress_callback(0, button1_press_event);
	set_button_onPress_callback(1, button2_press_event);
	set_button_onPress_callback(2, button3_press_event);
	set_button_onHold_callback(0, button1_hold_event, HOLD_DELAY, HOLD_INTERVAL);
	set_button_onHold_callback(2, button3_hold_event, HOLD_DELAY, HOLD_INTERVAL);
	set_pot_onChange_callback(0, pot1_change_event, MIN_POT_STEP);
	set_pot_onChange_callback(1, pot2_change_event, MIN_POT_STEP);
}

void show_menu()
{
	LcdClearBuf();

	switch(menu.screen)
	{
		case WELCOME:
		{
			LcdSetXY(0,0);
			LcdPutStr("StrobeTuner", NORMAL);
			LcdSetXY(0,10);
			LcdPutStr("(C) MK 2016", NORMAL);
			LcdSetXY(0,35);
			LcdPutStr("Press any key.", NORMAL);
		}
		break;

		case MAIN_SCREEN:
		{
			LcdSetXY(0,0);
			switch(get_vfd_status())
			{
				case VFD_STOPPED:
				LcdPutStr("Stopped", NORMAL);
				break;

				case VFD_SPIN_UP:
				LcdPutStr("SpinUp", NORMAL);
				break;

				case VFD_SPIN_DOWN:
				LcdPutStr("SpinDown", NORMAL);
				break;

				case VFD_IN_SYNC:
				LcdPutStr("Running", NORMAL);
				break;
			}
			LcdSetXY(0, 9);
			print_pitch_name(tuner_cfg()->resultant_note, NORMAL);
			LcdPutInt(tuner_cfg()->octave_index, 0, NORMAL);
			LcdSetXY(47, 9);
			if(tuner_cfg()->cent_offset > 0)
			{
				LcdPutStr("+ ", NORMAL);
				LcdPutInt(tuner_cfg()->cent_offset, 0, NORMAL);
			}
			else if (tuner_cfg()->cent_offset < 0)
			{
				LcdPutStr("- ", NORMAL);
				LcdPutInt(-tuner_cfg()->cent_offset, 0, NORMAL);
			}
			else
			{
				LcdPutStr("+/- 0", NORMAL);
			}
			LcdPutStr("c", NORMAL);
			LcdSetXY(0, 18);
			print_temperament_name(tuner_cfg()->temperament, NORMAL);
			LcdSetXY(0, 27);
			LcdPutStr("A4= ", NORMAL);
			LcdPutInt(tuner_cfg()->A_ref_freq/10, 0, NORMAL);
			LcdPutStr(".", NORMAL);
			LcdPutInt(tuner_cfg()->A_ref_freq%10, 0, NORMAL);
			LcdPutStr("Hz", NORMAL);

			LcdSetXY(0, 42);
			LcdPutStr("F= ", NORMAL);
			LcdPutInt(tuner_cfg()->final_freq/1000, 0, NORMAL);
			LcdPutStr(".", NORMAL);
			LcdPutInt(tuner_cfg()->final_freq%1000, 3, NORMAL);
			LcdPutStr("Hz", NORMAL);
		}
		break;

		case SETTINGS:
		{
			LcdSetXY(7,0);
			LcdPutStr("-SETTINGS-", NORMAL);
			LcdSetXY(0,8);
			LcdPutStr(" root     ", (menu.settings_scr.selection == SEL_ROOT ? INVERTED : NORMAL));
			print_pitch_name(tuner_cfg()->root_note, (menu.settings_scr.selection == SEL_ROOT ? INVERTED : NORMAL));
			LcdSetXY(0,16);
			LcdPutStr(" temperament ", (menu.settings_scr.selection == SEL_TEMPERAMENT ? INVERTED : NORMAL));
			LcdSetXY(0,24);
			LcdPutStr(" A frequency ", (menu.settings_scr.selection == SEL_A_FREQ ? INVERTED : NORMAL));
			LcdSetXY(0,32);
			LcdPutStr(" screen setup", (menu.settings_scr.selection == SEL_SCREEN ? INVERTED : NORMAL));
			LcdSetXY(0,40);
			LcdPutStr(" exit ", (menu.settings_scr.selection == SEL_EXIT ? INVERTED : NORMAL));
		}
		break;

		case SET_ROOT_NOTE:
		{
			LcdSetXY(3, 0);
			LcdPutStr("-ROOT NOTE-", NORMAL);
			LcdSetXY(10, 15);
			LcdPutStr("< ", NORMAL);
			print_pitch_name(tuner_cfg()->root_note, NORMAL);
			LcdSetXY(53, 15);
			LcdPutStr(">", NORMAL);
			LcdSetXY(0,30);
			LcdPutStr(" exit ", INVERTED);
		}
		break;

		case SET_TEMPERAMENT:
		{
			LcdSetXY(3, 0);
			LcdPutStr("-TEMPERAM.-", NORMAL);
			LcdSetXY(0,15);
			print_temperament_name(tuner_cfg()->temperament, NORMAL);
			LcdSetXY(1, 24);
			LcdPutStr("< ", NORMAL);
			LcdSetXY(79, 24);
			LcdPutStr(">", NORMAL);
			LcdSetXY(0,36);
			LcdPutStr(" exit ", INVERTED);
		}
		break;

		case SET_A_FREQ:
		{
			LcdSetXY(6, 0);
			LcdPutStr("-REF. PITCH-", NORMAL);
			LcdSetXY(10, 15);
			LcdPutStr("< ", NORMAL);
			LcdPutInt(tuner_cfg()->A_ref_freq/10, 0, NORMAL);
			LcdPutStr(".", NORMAL);
			LcdPutInt(tuner_cfg()->A_ref_freq%10, 0, NORMAL);
			LcdPutStr("Hz", NORMAL);
			LcdSetXY(70, 15);
			LcdPutStr(">", NORMAL);
			LcdSetXY(0,30);
			LcdPutStr(" exit ", INVERTED);
		}
		break;

		case SET_SCREEN:
		{
			LcdSetXY(2, 0);
			LcdPutStr("-LCD SETUP-", NORMAL);
			LcdSetXY(0,10);
			LcdPutStr(" backlight ", (menu.lcd_settings_scr.selection == SEL_SCR_BRIGHT ? INVERTED : NORMAL));
			LcdSetXY(0,20);
			LcdPutStr(" contrast ", (menu.lcd_settings_scr.selection == SEL_SCR_CON ? INVERTED : NORMAL));
			LcdSetXY(0,30);
			LcdPutStr(" exit ", (menu.lcd_settings_scr.selection == SEL_SCR_EXIT ? INVERTED : NORMAL));
		}
		break;

		case SET_CONTRAST:
		{
			LcdSetXY(4, 0);
			LcdPutStr("-CONTRAST-", NORMAL);
			LcdSetXY(32, 15);
			LcdPutStr("< ", NORMAL);
			LcdPutInt(lcd_cfg()->contrast, 0, NORMAL);
			LcdSetXY(50, 15);
			LcdPutStr(">", NORMAL);
			LcdSetXY(0,30);
			LcdPutStr(" exit ", INVERTED);
		}
		break;

		case SET_BRIGHTNESS:
		{
			LcdSetXY(3, 0);
			LcdPutStr("-BACKLIGHT-", NORMAL);
			LcdSetXY(32, 15);
			LcdPutStr("< ", NORMAL);
			LcdPutInt(lcd_cfg()->brightness, 0, NORMAL);
			LcdSetXY(50, 15);
			LcdPutStr(">", NORMAL);
			LcdSetXY(0,30);
			LcdPutStr(" exit ", INVERTED);
		}
		break;

		default:
		break;
	}

	LcdRefresh();
}

void button1_press_event()	//DOWN
{
	switch(menu.screen)
	{
		case WELCOME:
		{
			menu.screen = MAIN_SCREEN;
		}
		break;

		case MAIN_SCREEN:
		{
			tuner_cfg()->drive_enabled = 0;
		}
		break;

		case SETTINGS:
		{
			menu.settings_scr.selection--;
			if (menu.settings_scr.selection > SEL_EXIT)
			{
				menu.settings_scr.selection = SEL_EXIT;
			}
		}
		break;

		case SET_ROOT_NOTE:
		{
			tuner_cfg()->root_note--;
			if (tuner_cfg()->root_note > STEP_B)
			{
				tuner_cfg()->root_note = STEP_B;
			}
		}
		break;

		case SET_A_FREQ:
		{
			tuner_cfg()->A_ref_freq--;
			if (tuner_cfg()->A_ref_freq < REFERENCE_A_FREQ_MIN * 10)
			{
				tuner_cfg()->A_ref_freq = REFERENCE_A_FREQ_MIN * 10;
			}
		}
		break;

		case SET_TEMPERAMENT:
		{
			tuner_cfg()->temperament--;
			if(tuner_cfg()->temperament > GALILEI)
			{
				tuner_cfg()->temperament = GALILEI;
			}
		}
		break;

		case SET_SCREEN:
		{
			menu.lcd_settings_scr.selection--;
			if (menu.lcd_settings_scr.selection > SEL_SCR_EXIT)
			{
				menu.lcd_settings_scr.selection = SEL_SCR_EXIT;
			}
		}
		break;

		case SET_BRIGHTNESS:
		{
			lcd_cfg()->brightness--;
			if (lcd_cfg()->brightness >= LCD_BACKLIGHT_STEPS)
			{
				lcd_cfg()->brightness = LCD_BACKLIGHT_STEPS -1;
			}
			LcdApplyCfg();
		}
		break;

		case SET_CONTRAST:
		{
			lcd_cfg()->contrast--;
			if (lcd_cfg()->contrast >= LCD_CONTRAST_STEPS)
			{
				lcd_cfg()->contrast = LCD_CONTRAST_STEPS -1;
			}
			LcdApplyCfg();
		}
		break;

		default:
		break;
	}
	apply_tuner_cfg();
}

void button2_press_event()	//SELECT
{
	switch(menu.screen)
	{
		case WELCOME:
		{
			menu.screen = MAIN_SCREEN;
		}
		break;

		case MAIN_SCREEN:
		{
			menu.screen = SETTINGS;
		}
		break;

		case SETTINGS:
		{
			switch(menu.settings_scr.selection)
			{
				case SEL_ROOT:
				{
					menu.screen = SET_ROOT_NOTE;
				}
				break;

				case SEL_A_FREQ:
				{
					menu.screen = SET_A_FREQ;
				}
				break;

				case SEL_TEMPERAMENT:
				{
					menu.screen = SET_TEMPERAMENT;
				}
				break;

				case SEL_SCREEN:
				{
					menu.screen = SET_SCREEN;
				}
				break;

				case SEL_EXIT:
				{
					menu.screen = MAIN_SCREEN;
				}
				break;

				default:
				break;
			}
		}
		break;

		case SET_ROOT_NOTE:
		{
			menu.screen = SETTINGS;
		}
		break;

		case SET_A_FREQ:
		{
			menu.screen = SETTINGS;
		}
		break;

		case SET_TEMPERAMENT:
		{
			menu.screen = SETTINGS;
		}
		break;

		case SET_SCREEN:
		{
			switch(menu.lcd_settings_scr.selection)
			{
				case SEL_SCR_BRIGHT:
				{
					menu.screen = SET_BRIGHTNESS;
				}
				break;

				case SEL_SCR_CON:
				{
					menu.screen = SET_CONTRAST;
				}
				break;

				case SEL_SCR_EXIT:
				{
					menu.screen = SETTINGS;
				}
				break;
			}
		}
		break;

		case SET_BRIGHTNESS:
		{
			menu.screen = SET_SCREEN;
		}
		break;

		case SET_CONTRAST:
		{
			menu.screen = SET_SCREEN;
		}
		break;

		default:
		break;
	}
	apply_tuner_cfg();
}

void button3_press_event()	//UP
{
	switch(menu.screen)
	{
		case WELCOME:
		{
			menu.screen = MAIN_SCREEN;
		}
		break;

		case MAIN_SCREEN:
		{
			tuner_cfg()->drive_enabled = 1;
		}
		break;

		case SETTINGS:
		{
			menu.settings_scr.selection++;
			if (menu.settings_scr.selection > SEL_EXIT)
			{
				menu.settings_scr.selection = SEL_ROOT;
			}
		}
		break;

		case SET_ROOT_NOTE:
		{
			tuner_cfg()->root_note++;
			if (tuner_cfg()->root_note > STEP_B)
			{
				tuner_cfg()->root_note = STEP_C;
			}
		}
		break;

		case SET_A_FREQ:
		{
			tuner_cfg()->A_ref_freq++;
			if (tuner_cfg()->A_ref_freq > REFERENCE_A_FREQ_MAX * 10)
			{
				tuner_cfg()->A_ref_freq = REFERENCE_A_FREQ_MAX * 10;
			}
		}
		break;

		case SET_TEMPERAMENT:
		{
			tuner_cfg()->temperament++;
			if(tuner_cfg()->temperament > GALILEI)
			{
				tuner_cfg()->temperament = EQUAL;
			}
		}
		break;

		case SET_SCREEN:
		{
			menu.lcd_settings_scr.selection++;
			if (menu.lcd_settings_scr.selection > SEL_SCR_EXIT)
			{
				menu.lcd_settings_scr.selection = SEL_SCR_BRIGHT;
			}
		}
		break;

		case SET_BRIGHTNESS:
		{
			lcd_cfg()->brightness++;
			if (lcd_cfg()->brightness >= LCD_BACKLIGHT_STEPS)
			{
				lcd_cfg()->brightness = 0;
			}
			LcdApplyCfg();
		}
		break;

		case SET_CONTRAST:
		{
			lcd_cfg()->contrast++;
			if (lcd_cfg()->contrast >= LCD_CONTRAST_STEPS)
			{
				lcd_cfg()->contrast = 0;
			}
			LcdApplyCfg();
		}
		break;

		default:
		break;
	}
	apply_tuner_cfg();
}

void button1_hold_event()
{
	switch(menu.screen)
	{
		case SET_A_FREQ:
		{
			tuner_cfg()->A_ref_freq--;
			if (tuner_cfg()->A_ref_freq < REFERENCE_A_FREQ_MIN * 10)
			{
				tuner_cfg()->A_ref_freq = REFERENCE_A_FREQ_MIN * 10;
			}
		}
		break;

		default:
		break;
	}
	apply_tuner_cfg();
}

void button3_hold_event()
{
	switch(menu.screen)
	{
		case SET_A_FREQ:
		{
			tuner_cfg()->A_ref_freq++;
			if (tuner_cfg()->A_ref_freq > REFERENCE_A_FREQ_MAX * 10)
			{
				tuner_cfg()->A_ref_freq = REFERENCE_A_FREQ_MAX * 10;
			}
		}
		break;

		default:
		break;
	}
	apply_tuner_cfg();
}

void pot1_change_event(uint16_t value)
{
	switch(menu.screen)
	{
		case MAIN_SCREEN:
		{
			value <<= 4;	//Scale value up to full 16bit range
			uint16_t step_size = (ADC_MAX << 4) / NOTE_SELECTOR_POS_NUMBER;
			tuner_cfg()->tuned_note = value / step_size;
			if (tuner_cfg()->tuned_note >= NOTE_SELECTOR_POS_NUMBER)
			{
				tuner_cfg()->tuned_note = NOTE_SELECTOR_POS_NUMBER - 1;
			}
		}
		break;

		default:
		break;
	}
	apply_tuner_cfg();
}

void pot2_change_event(uint16_t value)
{
	switch(menu.screen)
	{
		case MAIN_SCREEN:
		{
			value <<= 4;	//Scale value up to full 16bit range
			uint16_t step_size = (ADC_MAX << 4) / 100;
			tuner_cfg()->cent_offset = ((value / step_size) - 50);
		}
		break;

		default:
		break;
	}
	apply_tuner_cfg();
}

