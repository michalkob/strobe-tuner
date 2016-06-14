#include "lcd.h"

GPIO_InitTypeDef 	GPIO_InitStructure;
SPI_InitTypeDef		SPI_InitStructure;

lcd_data_struct lcd_data;

lcd_settings lcd_config;

void lcd_delay()
{
	for(volatile uint32_t i=0; i<10000; i++);
}

void lcd_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//Aktywacja zegara alternatywnych funkcji GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LCD_PIN_RESET | LCD_PIN_COMMAND;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_PIN_BACKLIGHT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_PIN_DATA | LCD_PIN_CLOCK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_BaudRatePrescaler =  SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_CPHA 				=  SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPOL				=  SPI_CPOL_Low;
    SPI_InitStructure.SPI_CRCPolynomial 	=  7;
    SPI_InitStructure.SPI_DataSize 			=  SPI_DataSize_8b;
    SPI_InitStructure.SPI_Direction 		=  SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_FirstBit 			=  SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Mode 				=  SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS 				=  SPI_NSS_Soft;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_CalculateCRC(SPI1, DISABLE);
    SPI_Cmd(SPI1, ENABLE);

  	TIM_DeInit(TIM2);
    TIM_TimeBaseInitTypeDef Timebase_Config;
    Timebase_Config.TIM_ClockDivision		= TIM_CKD_DIV2;			// APB1 / 2 = 18MHz. Mo¿na prze³¹czyc na DIV1 dla PWM 200Hz
	Timebase_Config.TIM_Prescaler			= 35;					// Zegar timera to 2MHz
	Timebase_Config.TIM_CounterMode			= TIM_CounterMode_Up;
	Timebase_Config.TIM_Period				= LCD_BACKLIGHT_STEPS * LCD_BACKLIGHT_MUL;	//Okres timera to 10000, co daje 100Hz PWM
	Timebase_Config.TIM_RepetitionCounter	= 0;
	TIM_TimeBaseInit(TIM2, &Timebase_Config);
   	TIM_Cmd(TIM2, ENABLE);

   	lcd_config.brightness	= LCD_BACKLIGHT_DEF;
   	lcd_config.contrast		= LCD_CONTRAST_DEF;

	LcdSetBacklight(LCD_BACKLIGHT_DEF);
	LcdSetContrast(LCD_CONTRAST_DEF);
}

void LcdSetBacklight(uint8_t backlight)
{
	if (backlight >= LCD_BACKLIGHT_STEPS) backlight = LCD_BACKLIGHT_STEPS - 1;
	TIM_OCInitTypeDef outChannel_cfg;
	outChannel_cfg.TIM_OCIdleState		= TIM_OCIdleState_Reset;
	outChannel_cfg.TIM_OCMode			= TIM_OCMode_PWM1;
	outChannel_cfg.TIM_OCNIdleState		= TIM_OCNIdleState_Reset;
	outChannel_cfg.TIM_OCPolarity		= TIM_OCPolarity_High;
	outChannel_cfg.TIM_OCNPolarity		= TIM_OCNPolarity_High;
	outChannel_cfg.TIM_OutputNState		= TIM_OutputNState_Disable;
	outChannel_cfg.TIM_OutputState		= TIM_OutputState_Enable;

	outChannel_cfg.TIM_Pulse			= (uint16_t) LCD_BACKLIGHT_MUL * backlight;
	TIM_OC1Init(TIM2, &outChannel_cfg);
}

void LcdSetContrast(uint8_t contrast)
{
	if (contrast >= LCD_CONTRAST_STEPS) contrast = LCD_CONTRAST_STEPS - 1;
	contrast = ((LCD_CONTRAST_MAX - LCD_CONTRAST_MIN)/LCD_CONTRAST_STEPS) * contrast + LCD_CONTRAST_MIN;
	GPIO_SetBits(GPIOA, LCD_PIN_RESET);
	lcd_delay();
	GPIO_ResetBits(GPIOA, LCD_PIN_RESET);
	lcd_delay();
	GPIO_SetBits(GPIOA, LCD_PIN_RESET);

	LcdWrite(LCD_CMD, 0x21); // FUNC_SET | EXT_INSTR
	LcdWrite(LCD_CMD, 0x80 | contrast); // Vop
	LcdWrite(LCD_CMD, 0x04); // temp coeff.
	LcdWrite(LCD_CMD, 0x15); // bias

	LcdWrite(LCD_CMD, 0x20); // basic cmd
	LcdWrite(LCD_CMD, LCD_DISPLAY_BLANK);
	LcdWrite(LCD_CMD, LCD_DISPLAY_NORMAL);
}

void LcdApplyCfg()
{
	LcdSetBacklight(lcd_config.brightness);
	LcdSetContrast(lcd_config.contrast);
}

lcd_settings * lcd_cfg()
{
	return &lcd_config;
}

void LcdWrite(uint8_t type, uint8_t value)
{
    if( type == LCD_DATA )
        GPIO_SetBits(GPIOA, LCD_PIN_COMMAND);
    else
        GPIO_ResetBits(GPIOA, LCD_PIN_COMMAND);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET );
    SPI_I2S_SendData(SPI1, value);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET );
}

void LcdRefresh()
{
	LcdWrite(LCD_CMD, 0x80);
	LcdWrite(LCD_CMD, 0x40);
	for(uint8_t y = 0; y < 6; y++)
	{
		for(uint8_t x = 0; x < 84; x++)
		{
			LcdWrite(LCD_DATA, lcd_data.frameBuffer[x][y]);
		}
	}
	LcdWrite(LCD_DATA, 0);	//Dummy
}

void LcdSetXY(uint8_t x, uint8_t y)
{
	if (x > 75) x = 75;
	if (y > 41) y = 41;
	lcd_data.x = x;
	lcd_data.y = y;
}

void LcdClearBuf()
{
	for(uint8_t y = 0; y < 6; y++)
	{
		for(uint8_t x = 0; x < 84; x++)
		{
			lcd_data.frameBuffer[x][y] = 0;
		}
	}
}

void LcdPutSymbol(uint8_t symbol, lcd_diplay_mode mode)
{
	uint8_t mask = ((mode == NORMAL) ? 0x00 : 0xFF);
	uint8_t width = font_8px[symbol][0];
	if (width>(84 - lcd_data.x))
	{
		lcd_data.y+=8;
		lcd_data.x=0;
	}
	for(uint8_t i=0; i<width; i++)
	{
		lcd_data.frameBuffer[lcd_data.x][lcd_data.y / 8] |= (uint8_t)((font_8px[symbol][i+1] ^ mask) << (lcd_data.y % 8));
		if (lcd_data.y % 8)
		{
			lcd_data.frameBuffer[lcd_data.x][1 + lcd_data.y / 8] |= (uint8_t)((font_8px[symbol][i+1] ^ mask) >> (8 - lcd_data.y % 8));
		}
		lcd_data.x++;
	}
}

void LcdPutInt(uint32_t integer, uint8_t numOfDigits, lcd_diplay_mode mode)
{
	//dla numOfDigits = 0 wypisuje liczbê z pominiêciem zer
	//dla numOfDigits 1-10 wypisuje tyle cyfr ile kazano
	if (numOfDigits > 10) return;
	uint8_t supressZeros = 0;
	uint32_t	dividend;
	if (integer == 0)
	{
		if (numOfDigits == 0) numOfDigits = 1;
		for (uint8_t i=0; i<numOfDigits; i++)
		{
			LcdPutSymbol(0, mode);
		}
		return;
	}
	if (numOfDigits == 0)
	{
		dividend		= 1000000000;
		supressZeros	= 1;
	}
	else
	{
		dividend = 1;
		for (uint8_t i=0; i<numOfDigits - 1; i++) dividend *= 10;
	}
	do
	{
		uint32_t divResult = integer / dividend;
		if (divResult > 9) return;
		if (divResult == 0)
		{
			if (supressZeros == 0) LcdPutSymbol(0, mode);
		}
		else
		{
			supressZeros = 0;
			LcdPutSymbol((uint8_t)divResult, mode);
		}
		divResult *= dividend;
		integer -= divResult;
		dividend /= 10;
	} while (dividend);
}

void LcdPutStr(char * string, lcd_diplay_mode mode)
{
	while(*string)
	{
		if ((*string >= '0') && (*string <= '9')) LcdPutSymbol(*string - '0', mode);
		else if ((*string >= 'A') && (*string <= 'Z')) LcdPutSymbol(*string - 'A' + 10, mode);
		else if ((*string >= 'a') && (*string <= 'z')) LcdPutSymbol(*string - 'a' + 36, mode);
		else if (*string == ' ') LcdPutSymbol(62, mode);
		else if (*string == '#') LcdPutSymbol(63, mode);
		else if (*string == '@') LcdPutSymbol(64, mode);
		else if (*string == ')') LcdPutSymbol(65, mode);
		else if (*string == '(') LcdPutSymbol(66, mode);
		else if (*string == '-') LcdPutSymbol(67, mode);
		else if (*string == '+') LcdPutSymbol(68, mode);
		else if (*string == '/') LcdPutSymbol(69, mode);
		else if (*string == '>') LcdPutSymbol(70, mode);
		else if (*string == '<') LcdPutSymbol(71, mode);
		else if (*string == '.') LcdPutSymbol(72, mode);
		else if (*string == ':') LcdPutSymbol(73, mode);
		else if (*string == '=') LcdPutSymbol(74, mode);
		string++;
	}
}
