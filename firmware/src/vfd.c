
#include "vfd.h"

const uint16_t sine_table[256] =
{
	500, 512, 525, 537, 549, 561, 573, 585, 598, 610, 621, 633, 645, 657, 668, 680,
	691, 703, 714, 725, 736, 746, 757, 767, 778, 788, 798, 808, 817, 827, 836, 845,
	854, 862, 870, 879, 887, 894, 902, 909, 916, 922, 929, 935, 941, 947, 952, 957,
	962, 966, 971, 975, 978, 982, 985, 988, 990, 993, 995, 996, 998, 999, 999, 1000,
	1000, 1000, 999, 999, 998, 996, 995, 993, 990, 988, 985, 982, 978, 975, 971, 966,
	962, 957, 952, 947, 941, 935, 929, 922, 916, 909, 902, 894, 887, 879, 870, 862,
	854, 845, 836, 827, 817, 808, 798, 788, 778, 767, 757, 746, 736, 725, 714, 703,
	691, 680, 668, 657, 645, 633, 621, 610, 598, 585, 573, 561, 549, 537, 525, 512,
	500, 488, 475, 463, 451, 439, 427, 415, 402, 390, 379, 367, 355, 343, 332, 320,
	309, 297, 286, 275, 264, 254, 243, 233, 222, 212, 202, 192, 183, 173, 164, 155,
	146, 138, 130, 121, 113, 106,  98,  91,  84,  78,  71,  65,  59,  53,  48,  43,
	 38,  34,  29,  25,  22,  18,  15,  12,  10,   7,   5,   4,   2,   1,   1,   0,
	  0,   0,   1,   1,   2,   4,   5,   7,  10,  12,  15,  18,  22,  25,  29,  34,
	 38,  43,  48,  53,  59,  65,  71,  78,  84,  91,  98, 106, 113, 121, 130, 138,
	146, 155, 164, 173, 183, 192, 202, 212, 222, 233, 243, 254, 264, 275, 286, 297,
	309, 320, 332, 343, 355, 367, 379, 390, 402, 415, 427, 439, 451, 463, 475, 488
};

DDS_data dds;

void init_vfd()
{
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_TIM1, ENABLE);	//Aktywacja zegara dla Timera 1
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);	//Aktywacja zegara alternatywnych funkcji GPIO

	GPIO_InitTypeDef GPIO_cfg;
	GPIO_cfg.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_cfg.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_cfg.GPIO_Pin   = VFD_PHASE1_PIN | VFD_PHASE2_PIN | VFD_PHASE3_PIN;
	GPIO_Init(GPIOA, &GPIO_cfg);

	TIM_DeInit(TIM1);

	TIM_TimeBaseInitTypeDef Timebase_Config;
	Timebase_Config.TIM_ClockDivision		= TIM_CKD_DIV1;			// APB1 / 2 = 18MHz. Mo¿na prze³¹czyc na DIV1 dla PWM 200Hz
	Timebase_Config.TIM_Prescaler			= 2;					// Zegar timera to 24MHz
	Timebase_Config.TIM_CounterMode			= TIM_CounterMode_Up;	// Do poeksperymentowania
	Timebase_Config.TIM_Period				= 1000;					// 24kHz PWM
	Timebase_Config.TIM_RepetitionCounter	= 0;
	TIM_TimeBaseInit(TIM1, &Timebase_Config);

	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);			//Aktywuj przerwanie, ilekroc Timer1 przepe³ni siê

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);

	TIM_Cmd(TIM1, ENABLE);

	NVIC_config();

	TIM_OCInitTypeDef outChannel_cfg;
	outChannel_cfg.TIM_OCIdleState		= TIM_OCIdleState_Reset;
	outChannel_cfg.TIM_OCMode			= TIM_OCMode_PWM1;
	outChannel_cfg.TIM_OCNIdleState		= TIM_OCNIdleState_Reset;
	outChannel_cfg.TIM_OCPolarity		= TIM_OCPolarity_High;
	outChannel_cfg.TIM_OCNPolarity		= TIM_OCNPolarity_High;
	outChannel_cfg.TIM_OutputNState		= TIM_OutputNState_Disable;
	outChannel_cfg.TIM_OutputState		= TIM_OutputState_Enable;
	outChannel_cfg.TIM_Pulse			= (uint16_t) SINE_PP_VALUE / 2;
	TIM_OC1Init(TIM1, &outChannel_cfg);
	TIM_OC2Init(TIM1, &outChannel_cfg);
	TIM_OC3Init(TIM1, &outChannel_cfg);

	TIM_CtrlPWMOutputs(TIM1, ENABLE);

	dds.out[0] = SINE_PP_VALUE / 2;
	dds.out[1] = SINE_PP_VALUE / 2;
	dds.out[2] = SINE_PP_VALUE / 2;

	dds.reg[0] = DDS_PH1_INIT;
	dds.reg[1] = DDS_PH2_INIT;
	dds.reg[2] = DDS_PH3_INIT;

	dds.curr_speed_mHz 		= 0;
	dds.target_speed_mHz 	= 0;
	dds.ampl				= 0;	//MAX 8

	dds.ramp_speed			= VFD_RAMP_SPEED;

	dds.status				= VFD_STOPPED;
}

void NVIC_config()
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel						= TIM1_UP_IRQn;		//Przerwanie Timera1 12kHz
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 1;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_SetPriority(SysTick_IRQn, 2);
}

inline void set_vfd_outputs()
{
	for (uint8_t i=0; i<3; i++)
	{
		dds.reg[i] += dds.curr_speed_mHz;								//Add tuning word to each register
		dds.reg[i] %= DDS_REG_CAP;										//Wrap-around
		dds.out[i] = sine_table[dds.reg[i] / DDS_DIVISOR] * dds.ampl;	//Multiply by amplitude (0...8)
		dds.out[i] >>= 3;												//Divide by 8
	}
	if (dds.curr_speed_mHz)
	{
		TIM1->CCR1	= dds.out[0];											//Apply to PWMs
		TIM1->CCR2	= dds.out[1];
		TIM1->CCR3	= dds.out[2];
	}
	else
	{
		TIM1->CCR1	= 0;												//Disable drive if speed = 0
		TIM1->CCR2	= 0;
		TIM1->CCR3	= 0;
	}
	if (dds.curr_speed_mHz < dds.target_speed_mHz)						//Process smooth speed ramp up/down
	{
		dds.curr_speed_mHz += dds.ramp_speed;
		dds.status = VFD_SPIN_UP;
		if (dds.curr_speed_mHz > dds.target_speed_mHz)
		{
			dds.curr_speed_mHz = dds.target_speed_mHz;
		}
	}
	else if (dds.curr_speed_mHz > dds.target_speed_mHz)						//Process smooth speed ramp up/down
	{
		dds.status = VFD_SPIN_DOWN;
		dds.curr_speed_mHz -= dds.ramp_speed;
		if ((dds.curr_speed_mHz < dds.target_speed_mHz) || (dds.curr_speed_mHz > 0x7FFFFFFF))
		{
			dds.curr_speed_mHz = dds.target_speed_mHz;
		}
	}
	if (dds.curr_speed_mHz == 0) dds.status = VFD_STOPPED;
	else if (dds.curr_speed_mHz == dds.target_speed_mHz) dds.status = VFD_IN_SYNC;
}

void TIM1_UP_IRQHandler()
{
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	set_vfd_outputs();
}

vfd_states get_vfd_status()
{
	return dds.status;
}

void set_vfd_target_speed(uint32_t speed_mHz)
{
	dds.target_speed_mHz = speed_mHz;
}

void vfd_set_amplitude(uint8_t ampl)
{
	if (ampl > DDS_AMPL_MAX) ampl = DDS_AMPL_MAX;
	dds.ampl = ampl;
}
