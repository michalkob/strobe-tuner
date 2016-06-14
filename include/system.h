#ifndef SYSTEM_H
#define SYSTEM_H

#include "stm32f10x.h"
#include <stddef.h>
#include <diag/Trace.h>

#define MAX_SYS_TASKS	10

typedef struct
{
	void 		(*task)();
	uint64_t	next_call;
	uint32_t	interval;
	uint8_t		is_repeating;
} system_task_struct;

typedef struct
{
	system_task_struct 	task[MAX_SYS_TASKS];
	uint16_t delay;
	uint64_t sys_ms;
	uint8_t sys_ms_mod_flag;
} system_data_struct;

void SysTick_Handler();

void delay_ms(uint16_t time);

void init_system_struct();

int8_t submit_system_task(void (*task)(), uint32_t interval, uint8_t is_repeating);

uint64_t get_sys_ms();

uint64_t ms_into_future(uint32_t ms);

uint8_t has_expired(uint64_t * timeout);

void sys_task();

#endif
