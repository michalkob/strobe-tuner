//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "system.h"
#include "knobs_buttons.h"
#include "lcd.h"
#include "menu.h"
#include "tuner.h"
#include "vfd.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main()
{
	init_system_struct();
	init_clock();
	init_controls();
	lcd_init();
	init_vfd();
	init_tuner();
	init_menu_items();

	vfd_set_amplitude(2);

	submit_system_task(show_menu, 			50,		1);		//20FPS
	submit_system_task(refresh_controls, 	20, 	1);

	while(1)
    {
		sys_task();
    }
}

#pragma GCC diagnostic pop
