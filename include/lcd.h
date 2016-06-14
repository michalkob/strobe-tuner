
//SPI1
// D/C 		PA4
// CLK		PA5
// RST 		PA6
// DIN		PA7
// BL_PWM	PA15 TIM2 CH1

#ifndef LCD_H_
#define LCD_H_

#include "lcd_font.h"

#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "system.h"

#define LCD_PIN_RESET       GPIO_Pin_6
#define LCD_PIN_COMMAND     GPIO_Pin_4
#define LCD_PIN_DATA        GPIO_Pin_7
#define LCD_PIN_CLOCK       GPIO_Pin_5
#define	LCD_PIN_BACKLIGHT	GPIO_Pin_15

#define LCD_CMD             0
#define LCD_DATA            1

#define LCD_DISPLAY_BLANK   0x08
#define LCD_DISPLAY_FULL    0x09
#define LCD_DISPLAY_NORMAL  0x0C
#define LCD_DISPLAY_INVERT  0x0D

#define	LCD_BACKLIGHT_STEPS	10
#define	LCD_BACKLIGHT_MUL	200

#define	LCD_CONTRAST_STEPS	10
#define	LCD_CONTRAST_MAX	62
#define	LCD_CONTRAST_MIN	42

#define	LCD_BACKLIGHT_DEF	3
#define	LCD_CONTRAST_DEF	6

typedef struct
{
	uint8_t brightness;
	uint8_t	contrast;
} lcd_settings;

typedef enum
{
	NORMAL,
	INVERTED
} lcd_diplay_mode;

typedef struct
{
	uint8_t frameBuffer[84][6];
	uint8_t x, y;
} lcd_data_struct;

void lcd_init();

void LcdSetBacklight(uint8_t backlight);

void LcdSetContrast(uint8_t contrast);

void LcdApplyCfg();

lcd_settings * lcd_cfg();

void LcdWrite(uint8_t type, uint8_t value);

void LcdRefresh();

void LcdSetXY(uint8_t x, uint8_t y);

void LcdPutSymbol(uint8_t symbol, lcd_diplay_mode mode);

void LcdClearBuf();

void LcdPutInt(uint32_t integer, uint8_t numOfDigits, lcd_diplay_mode mode);

void LcdPutStr(char * string, lcd_diplay_mode mode);

#endif
