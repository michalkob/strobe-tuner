#ifndef MENU_H

#define MENU_H

#define MIN_POT_STEP 4

#include "knobs_buttons.h"
#include "lcd.h"
#include "tuner.h"
#include "vfd.h"

#define NOTE_SELECTOR_POS_NUMBER	25
#define	HOLD_DELAY					500
#define HOLD_INTERVAL				50

typedef enum
{
	WELCOME,
	MAIN_SCREEN,
	SETTINGS,
	SET_ROOT_NOTE,
	SET_TEMPERAMENT,
	SET_A_FREQ,
	SET_SCREEN,
	SET_BRIGHTNESS,
	SET_CONTRAST
} menu_screens;

typedef enum
{
	SEL_ROOT,
	SEL_TEMPERAMENT,
	SEL_A_FREQ,
	SEL_SCREEN,
	SEL_EXIT
} settings_scr_sel;

typedef enum
{
	SEL_SCR_BRIGHT,
	SEL_SCR_CON,
	SEL_SCR_EXIT
} screen_settings_sel;

typedef struct
{
	settings_scr_sel	selection;
} settings_screen_data;

typedef struct
{
	screen_settings_sel	selection;
} lcd_settings_data;

typedef struct
{
	menu_screens			screen;
	settings_screen_data	settings_scr;
	lcd_settings_data		lcd_settings_scr;
} menu_items;

void init_menu_items();

void show_menu();

void button1_press_event();

void button2_press_event();

void button3_press_event();

void button1_hold_event();

void button3_hold_event();

void pot1_change_event(uint16_t value);

void pot2_change_event(uint16_t value);

#endif
