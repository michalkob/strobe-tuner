#include "knobs_buttons.h"

control_struct controls;

void init_clock()
{
	RCC_DeInit();
	ErrorStatus HSE_Startup_Status;
	RCC_HSEConfig(RCC_HSE_ON);								//Uruchom oscylator HSE
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);	//Czekaj na wystartowanie HSE
	HSE_Startup_Status = RCC_WaitForHSEStartUp();
	if (HSE_Startup_Status == SUCCESS)
	{
		FLASH_SetLatency(FLASH_ACR_LATENCY_2);					//Konfiguracja Flash: 2x wait-state
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	//Bufor odczytu FLASH aktywny
		RCC_HCLKConfig(RCC_SYSCLK_Div1);						//HCLK (zegar AHB) = SYSCLK = 72MHz
		RCC_PCLK1Config(RCC_HCLK_Div2);							//PCLK1 = 36MHz
		RCC_PCLK2Config(RCC_HCLK_Div2);							//PCLK2 = 36MHz
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);	//PLLCLK = 12MHz (kwarc) / 1 * 6 = 72MHz
		RCC_PLLCmd(ENABLE);										//PLL uruchomiona
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);	//Oczekiwanie na gotowoœc PLL
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);				//Prze³¹czenie zegara systemowego na PLLCLK (72MHz)
		while (RCC_GetSYSCLKSource() != 0x08);					//Oczekiwanie na pomyœlne prze³¹czenie
	}
   	if (SysTick_Config(72000))  while (1);	//Konfiguruje SysTick tak, aby jedno "przekrêcenie" nastêpowa³o co 1 ms
   	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}

void init_controls()
{
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitTypeDef adc;
	GPIO_InitTypeDef gpio;

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = POT1_PIN | POT2_PIN;
	gpio.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(POT_PORT, &gpio);

	gpio.GPIO_Pin = SW1_PIN | SW2_PIN | SW3_PIN;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTTON_PORT, &gpio);

	ADC_StructInit(&adc);
	adc.ADC_ContinuousConvMode = DISABLE;
	adc.ADC_NbrOfChannel = 1;
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_Init(ADC1, &adc);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1));

	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1));

	for(uint8_t i=0; i<3; i++)
	{
		controls.ui_button[i].state	=			0;
		controls.ui_button[i].onHold = 			NULL;
		controls.ui_button[i].onPress = 		NULL;
		controls.ui_button[i].holdDelay =		0;
		controls.ui_button[i].holdInterval =	0;
		controls.ui_button[i].holdTimeout =		0;
	}
	for(uint8_t i=0; i<2; i++)
	{
		controls.ui_pot[i].onChange = 			NULL;
		controls.ui_pot[i].value = 				0;
		controls.ui_pot[i].min_step =			0;
	}
}

void refresh_controls()
{
	uint8_t new_button_state[3];
	new_button_state[0] = (GPIO_ReadInputDataBit(BUTTON_PORT, SW1_PIN) ? 0 : 1);
	new_button_state[1] = (GPIO_ReadInputDataBit(BUTTON_PORT, SW2_PIN) ? 0 : 1);
	new_button_state[2] = (GPIO_ReadInputDataBit(BUTTON_PORT, SW3_PIN) ? 0 : 1);
	uint16_t new_pot_value[2];
	new_pot_value[0] = get_adc(0);
	new_pot_value[1] = get_adc(1);
	for(uint8_t i=0; i<3; i++)
	{
		if(new_button_state[i])
		{
			if (controls.ui_button[i].state == 0)
			{
				if (controls.ui_button[i].onPress != NULL)
				{
					controls.ui_button[i].onPress();
					controls.ui_button[i].holdTimeout = ms_into_future(controls.ui_button[i].holdDelay);
				}
			}
			if (controls.ui_button[i].onHold != NULL)
			{
				if(has_expired(&controls.ui_button[i].holdTimeout))
				{
					controls.ui_button[i].onHold();
					controls.ui_button[i].holdTimeout = ms_into_future(controls.ui_button[i].holdInterval);
				}
			}
		}
		controls.ui_button[i].state = new_button_state[i];
	}
	for(uint8_t i=0; i<2; i++)
	{
		if(controls.ui_pot[i].onChange != NULL)
		{
			if(new_pot_value[i] >= controls.ui_pot[i].value)
			{
				if((new_pot_value[i] - controls.ui_pot[i].value) >= (uint16_t) controls.ui_pot[i].min_step)
				{
					controls.ui_pot[i].onChange(new_pot_value[i]);
					controls.ui_pot[i].value = new_pot_value[i];
				}
			}
			else
			{
				if((controls.ui_pot[i].value - new_pot_value[i]) >= (uint16_t) controls.ui_pot[i].min_step)
				{
					controls.ui_pot[i].onChange(new_pot_value[i]);
					controls.ui_pot[i].value = new_pot_value[i];
				}
			}
		}
	}
}

control_struct * get_controls_status()
{
	return &controls;
}

void set_button_onPress_callback(uint8_t button, void(*callback)())
{
	if (button > 2) return;
	controls.ui_button[button].onPress = callback;
}

void set_button_onHold_callback(uint8_t button, void(*callback)(), uint16_t holdDelay, uint16_t holdInterval)
{
	if (button > 2) return;
	controls.ui_button[button].onHold = callback;
	controls.ui_button[button].holdInterval = holdInterval;
	controls.ui_button[button].holdDelay = holdDelay;
}

void set_pot_onChange_callback(uint8_t pot, void(*callback)(uint16_t potValue), uint8_t min_step)
{
	if (pot > 1) return;
	controls.ui_pot[pot].onChange = callback;
	controls.ui_pot[pot].min_step = min_step;
}

uint16_t get_adc(int channel)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_71Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

    return ADC_GetConversionValue(ADC1);
}
