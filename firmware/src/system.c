#include "system.h"

volatile system_data_struct system;

void init_system_struct()
{
	system.delay			=	0;
	system.sys_ms			=	0;
	system.sys_ms_mod_flag 	= 	0;
	for(uint16_t i=0; i<MAX_SYS_TASKS; i++)
	{
		system.task[i].interval		=	0;
		system.task[i].is_repeating	=	0;
		system.task[i].next_call	=	0;
		system.task[i].task			=	NULL;
	}
}

void delay_ms(uint16_t time)
{
    system.delay = time;
    while(system.delay);
}

uint64_t get_sys_ms()
{
	uint64_t sys_ms_copy;
	do
	{
		system.sys_ms_mod_flag = 0;
		sys_ms_copy = system.sys_ms;
	} while(system.sys_ms_mod_flag);
	return sys_ms_copy;
}

uint64_t ms_into_future(uint32_t ms)
{
	return get_sys_ms() + ms;
}

uint8_t has_expired(uint64_t * timeout)
{
	if (get_sys_ms() >= *timeout)
	{
		return 1;
	}
	return 0;
}

void SysTick_Handler()
{
	if (system.delay)
	{
		system.delay--;	//Dla delay_ms
	}
	system.sys_ms_mod_flag = 1;
	system.sys_ms++;
}

void sys_task()
{
	for(uint16_t i=0; i<MAX_SYS_TASKS; i++)
	{
		if(system.task[i].task)
		{
			void(*call)() = system.task[i].task;
			if (system.sys_ms >= system.task[i].next_call)
			{
				if (system.task[i].is_repeating)
				{
					system.task[i].next_call += system.task[i].interval;
				}
				else
				{
					system.task[i].task = NULL;
				}
				call();
			}
		}
	}
}

int8_t submit_system_task(void (*task)(), uint32_t interval, uint8_t is_repeating)
{
	for(uint16_t i=0; i<MAX_SYS_TASKS; i++)
	{
		if (system.task[i].task == NULL)
		{
			system.task[i].task 		= task;
			system.task[i].is_repeating	= is_repeating;
			system.task[i].interval		= interval;
			system.task[i].next_call	= system.sys_ms + (uint64_t) interval;
			return 0;
		}
	}
	trace_puts("\r\nERROR System Queue Overflown.");
	return -1;
}

